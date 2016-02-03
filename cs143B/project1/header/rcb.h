#include "eventlistener.h"
#include "statuses.h"
#include <vector>
#include <pair>

using std::vector;
using std::pair;
/**
 * Resource control block. 
 * Contains data about a resource.
 */
#ifndef RCB_H
#define RCB_H
class PCB;
class RCB : public EventListener {
public:
	RCB();
	RCB(int arg_id, string arg_name, int arg_allocated);
	int id;
	int total;
	int allocated;
	string name;
	status_t status;
	vector<pair<int, PCB *>> waiting_list;
	friend ostream& operator<<(ostream& outs, RCB &obj);
	virtual EventListener* on_delete(string arg_name) { return nullptr; }
	virtual EventListener* on_request(string arg_name) {
		return !name.compare(arg_name) ? this : nullptr;
	}
	virtual void has_deleted(string name) { }
};
#endif