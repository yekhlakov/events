#pragma once
#ifndef _MAXY_CONTROL_EVENTS
#define _MAXY_CONTROL_EVENTS

#include <string>
#include <unordered_map>
#include <vector>
#include <deque>
#include <typeinfo>
#include <typeindex>

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
				virtual ~event () = default;
			};

			// Possible results of event handling
			enum class result : int
			{
				Void, // No listeners for this event
				Ok,  // All listeners behaved well
				Failed, // Some listeners failed
				Fatal,  // There was a fatal error, some listeners were not invoked at all
				Queued  // We're currently processing an event, so this got queued
			};

			// The dispatcher
			class dispatcher
			{
				// basic listener type for internal storage
				using base_listener_type = result (*) (event *);

				// listener type for outside world
				template<class E>using listener_type = result (*) (E *);

				// a list of sets of listeners
				std::unordered_map<std::type_index, std::vector<base_listener_type>> listeners;

				// are we currently processing an event
				bool is_busy = false;

				// event queue
				std::deque<event *> queue;

				/**
				 * Process the event from the queue front
				 */
				result dispatch ()
				{
					if (!queue.size ())
					{
						return result::Void;
					}

					auto e = queue.front ();
					queue.pop_front ();

					auto pool = listeners.find (std::type_index (typeid (*e)));
					if (pool == listeners.end ())
					{
						// No handlers for this event -> void result
						delete e;
						return result::Void;
					}

					auto res = result::Ok;

					// Iterate over the registered listeners
					for (auto listener : pool->second)
					{
						auto partial_res = listener (e);

						if (partial_res == result::Fatal)
						{
							// a fatal error terminates the iteration
							res = partial_res;
							break;
						}
						else if (partial_res == result::Failed)
						{
							// a regular error just sets the result
							res = partial_res;
						}
					}
					delete e;
					return res;
				}

				public:
				// Add a listener
				// Return false if such a listener is already registered for the event
				// Return true if added a new listener
				template<class T>
				bool register_listener (listener_type<T> l)
				{
					auto x = std::type_index (typeid (T));
					auto ll = reinterpret_cast<base_listener_type>(l);

					auto & pool = listeners[x];
					if (pool.size ()
						&& std::find (pool.begin (), pool.end (), ll) != pool.end ())
					{
						return false;
					}
					pool.push_back (ll);
					return true;
				}

				// Remove a listener
				// Return false if no such listener is registered
				// Return true if the listener was removed
				template<class T>
				bool unregister_listener (listener_type<T> l)
				{
					auto pool = listeners.find (std::type_index (typeid (T)));
					if (pool == listeners.end ()
						|| !pool->second.size ())
					{
						return false;
					}
					auto ll = reinterpret_cast<base_listener_type>(l);
					auto old = std::find (pool->second.begin (), pool->second.end (), ll);
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
					queue.push_back (e);

					if (is_busy) return result::Queued;

					is_busy = true;

					result r = result::Void;

					while (queue.size ()) r = dispatch ();

					is_busy = false;

					return r;
				}
			};
		}
	}
}

#endif // _MAXY_CONTROL_EVENTS
