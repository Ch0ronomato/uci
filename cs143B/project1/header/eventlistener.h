#include <string>
using std::string;

#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H
class EventListener {
public:
	/**
	 * onDelete : PCB *
	 * Method returns a process control block that is being deleted
	 * to the event dispatcher, or null if it is not being deleted.
	 */
	virtual EventListener* on_delete(string name) = 0;
	/**
	 * on_request
	 * Method to get a resource of name ${name}
	 */
	virtual EventListener* on_request(string name) = 0;
	virtual void has_deleted(string name) = 0;
};
#endif