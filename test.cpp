#include <iostream>
#include "events.h"

using namespace maxy::control::events;

class e1 : public event
{
	public:
	e1 (std::string x) : payload{x} {};
	std::string name () { return "e1"; }
	std::string payload;
};

class e2 : public event
{
	public:
	e2 (int x) :payload{x} {};
	std::string name () { return "e2"; }
	int payload;
};

class e3 : public event
{
	public:
	e3 (float x) :payload{x} {};
	std::string name () { return "e3"; }
	float payload;
};

result L1 (e1 * e)
{
	std::cout << "L1 got " << e->name () << " with " << e->payload << "\n";
	return result::Ok;
}

result L2_FATAL (e2 * e)
{
	std::cout << "L2_FATAL got " << e->name () << " with " << e->payload << ", FATAL ERROR\n";
	return result::Fatal;
}

result L2_OK (e2 * e)
{
	std::cout << "L2_OK got " << e->name () << " with " << e->payload << "\n";
	return result::Ok;
}
result L2_MORE (e2 * e)
{
	std::cout << "L2_MORE got " << e->name () << " with " << e->payload << "\n";
	return result::Ok;
}



result L3 (e3 * e)
{
	std::cout << "L3 got " << e->name () << " with " << e->payload << "\n";
	return result::Ok;
}

result L3_2 (e3 * e)
{
	std::cout << "L3_2 got " << e->name () << " with " << e->payload << "\n";
	return result::Failed;
}

// this is global in order to test nested events
maxy::control::events::dispatcher d{};

result L3_nested (e3 * e)
{
	std::cout << "L3_nested got " << e->name () << " with " << e->payload << "\n";

	d (new e1 ("from L3_nested"));
	d (new e2 (1337));

	std::cout << "L3_nested dispatched two events (e1 and e2)\n";

	return result::Ok;
}

int main ()
{	
	d.register_listener (&L1);
	d.register_listener (&L3);
	d.register_listener (&L3_2);

	// e1 gets handled once
	std::cout << "E1->" << (int) d (new e1 ("OLOLO")) << "\n"; 

	// e2 is skipped
	std::cout << "E2->" << (int) d (new e2 (12345)) << "\n";

	// e3 is handled by two listeners with a failure
	std::cout << "E3->" << (int) d (new e3 (3.14159f)) << "\n";

	d.register_listener (&L2_FATAL);
	d.register_listener (&L2_OK);
	d.register_listener (&L2_MORE);
	// now e2 is handled by just the first listener producing a fatal error
	std::cout << "E2->" << (int) d (new e2 (0)) << "\n";

	d.unregister_listener (&L2_FATAL);
	// now the buggy listener is removed so e2 gets handled correctly by two remaining listeners
	std::cout << "E2->" << (int) d (new e2 (1)) << "\n";

	// now to the dispatching of events from inside listeners
	// reorder listeners first
	d.unregister_listener (&L3);
	d.register_listener (&L3_nested);
	d.register_listener (&L3);

	std::cout << "E3->" << (int) d (new e3 (666.f)) << "\n";
}
