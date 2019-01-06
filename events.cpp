#include "events.h"

namespace maxy
{
	namespace control
	{
		namespace events
		{
			// Add a listener
			// Return false if such a listener is already registered for the event
			// Return true if added a new listener
			bool dispatcher::register_listener (std::string e, listener_type l)
			{
				auto & pool = listeners[e];
				if (pool.size ()
					&& std::find (pool.begin (), pool.end (), l) != pool.end ())
				{
					return false;
				}
				pool.push_back (l);
				return true;
			}

			// Remove a listener
			// Return false if no such listener is registered
			// Return true if the listener was removed
			bool dispatcher::unregister_listener (std::string e, listener_type l)
			{
				auto pool = listeners.find (e);
				if (pool == listeners.end ()
					|| !pool->second.size ())
				{
					return false;
				}
				auto old = std::find (pool->second.begin (), pool->second.end (), l);
				if (old == pool->second.end ())
				{
					return false;
				}
				pool->second.erase (old);
				return true;
			}

			// Handle the event
			// Return the result of the event handling
			// The event is destroyed after handling
			result dispatcher::operator() (event * e)
			{
				auto pool = listeners.find (e->name ());
				if (pool == listeners.end ())
				{
					delete e;
					return result::Void;
				}

				auto res = result::Ok;

				for (auto listener : pool->second)
				{
					auto partial_res = listener (e);
					if (partial_res == result::Fatal)
					{
						res = partial_res;
						break;
					}
					else if (partial_res == result::Failed)
					{
						res = partial_res;
					}
				}

				delete e;

				return res;
			}
		}
	}
}
