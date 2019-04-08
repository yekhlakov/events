# events - a simple event dispatcher

* Include `events.h`
* Create your event classes like `class my_event : event { ... }`
* Instantiate the dispatcher.
* Define a listener like `result my_event_handler_function (my_event * e) { ... }`
* Register (or unregister) your listeners like `my_dispatcher.register_listener (&my_event_handler_function);`
* Send events to dispatcher and watch them flow like `my_dispatcher (new my_event{});`
* `test.cpp` is here to provide useful examples

