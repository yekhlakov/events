# events - a simple event dispatcher

* Include `events.h` (now it is header-only).
* Create your event classes deriving from `event` class.
* Instantiate the dispatcher.
* Register (or unregister) your listeners like `my_dispatcher.register_listener<my_event> (my_event_handler_function);`.
* Send events to dispatcher and watch them flow like `my_dispatcher (new my_event{});`.
* `test.cpp` is here to provide useful examples.

