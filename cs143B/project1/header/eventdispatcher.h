#include <string>
#include <vector>
#include "EventListener.h"

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H
class EventDispatcher {
public:
	EventDispatcher() : listeners() {};
	EventListener* fire_on_delete(const string& name);
	void fire_has_deleted(string name);
	EventListener* fire_on_request(string name);
	void fire_on_release(int id);
	void register_listener(EventListener *listener);
	void unregister_listener(EventListener *listener);
private:
	vector<EventListener*> listeners;
};
#endif