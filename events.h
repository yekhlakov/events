#pragma once
#ifndef _MAXY_CONTROL_EVENTS
#define _MAXY_CONTROL_EVENTS

#include <string>
#include <unordered_map>
#include <vector>

namespace maxy
{
	namespace control
	{
		namespace events
		{
			// Base for the events
			class event
			{
				public:
				virtual std::string name () { return ""; }
				virtual ~event () = default;
			};

			// Possible results of event handling
			enum class result : int
			{
				Void, // No listeners for this event
				Ok,  // All listeners behaved well
				Failed, // Some listeners failed
				Fatal  // There was a fatal error, some listeners were not invoked at all
			};

			// The dispatcher
			class dispatcher
			{
				using listener_type = result (*) (event *);
				std::unordered_map<std::string, std::vector<listener_type>> listeners;

				public:
				bool register_listener (std::string e, listener_type l);
				bool unregister_listener (std::string e, listener_type l);
				result operator() (event * e);
			};
		};
	}
}

#endif // _MAXY_CONTROL_EVENTS