#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using std::cout;
using std::endl;
using std::cin;
using std::getline;
using std::string;
using std::vector;
using std::ostream;
using std::stringstream;

/**
 * @todo: Implement event dispatcher. 
 * 
 * Abstract event listener class for when any pcb is going to be erased.
 * Simply loop through ALL the pcb items and delete them.
 */

class PCB;
typedef vector<PCB>::iterator pcb_iter_t;
class EventDispatcher;

class EventListener {
public:
	/**
	 * onDelete : PCB *
	 * Method returns a process control block that is being deleted
	 * to the event dispatcher, or null if it is not being deleted.
	 */
	virtual EventListener* on_delete(char name) = 0;
	/**
	 * on_request
	 * Method to get a resource of name ${name}
	 * 
	 * @todo: change name to on_get_resource
	 */
	virtual EventListener* on_request(string name) = 0;
	virtual void has_deleted(char name) = 0;
};

class EventDispatcher {
public:
	EventDispatcher() : listeners() {};
	EventListener* fire_on_delete(char name);
	void fire_has_deleted(char name);
	EventListener* fire_on_request(string name);
	void register_listener(EventListener *listener);
	void unregister_listener(EventListener *listener);
private:
	vector<EventListener*> listeners;
};

EventListener* EventDispatcher::fire_on_delete(char name) {
	EventListener *to_delete = nullptr;
	for (EventListener *listener : listeners) {
		to_delete = listener->on_delete(name);
		if (to_delete != nullptr) break;
	}

	return to_delete;
}

EventListener* EventDispatcher::fire_on_request(string name) {
	EventListener *to_delete = nullptr;
	for (EventListener *listener : listeners) {
		to_delete = listener->on_request(name);
		if (to_delete != nullptr) break;
	}

	return to_delete;
}

void EventDispatcher::fire_has_deleted(char name) {
	for (EventListener *listener : listeners) {
		listener->has_deleted(name);
	}
}

void EventDispatcher::register_listener(EventListener *listener) {
	listeners.push_back(listener);
}

void EventDispatcher::unregister_listener(EventListener *listener) {
	vector<EventListener*>::iterator iter = listeners.begin();
	bool found = false;
	for (; iter != listeners.end(); iter++) {
		if (*iter == listener) {
			found = true;
			break;
		}
	}

	if (found)
		listeners.erase(iter);
}
/**
 * Status enum for a process.
 */
typedef enum statusType_enum {
	READY,
	RUNNING,
	BLOCKED,
	FREE,
	ALLOCATED
} statusType;


/** 
 * Status data structure for all processes waiting on this one.
 */
typedef struct status_s {
	statusType status;
	vector<PCB> *list;
} status_t;

/**
 * Resource control block. 
 * Contains data about a resource.
 */
 // @todo: Add code for resource allocation amount.
class RCB : public EventListener {
public:
	RCB();
	RCB(int id, string name);
	int id;
	string name;
	status_t status;
	vector<PCB *> waiting_list;
	friend ostream& operator<<(ostream& outs, RCB &obj);
	virtual EventListener* on_delete(char arg_name) { return nullptr; }
	virtual EventListener* on_request(string arg_name) {
		return !name.compare(arg_name) ? this : nullptr;
	}
	virtual void has_deleted(char name) { }
};

RCB::RCB() { 
	id = -1;
	name = "";
	status.status = FREE;
}
RCB::RCB(int arg_id, string arg_name) {
	id = arg_id;
	name = arg_name;
	status.status = FREE;
}

ostream& operator<<(ostream& outs, RCB &obj) {
	outs << "Resource (" << obj.name << ") in status ";
	switch (obj.status.status) {
		case FREE:
			outs << "FREE";
			break;
		case ALLOCATED:
			outs << "ALLOCATED";
			break;
		default:
			break;
	} 

	cout << " waiting on " << obj.waiting_list.size();
	return outs;
}

/**
 * Process control block.
 * Contains all data related to a running process. Include an id, all resources, a status
 * (@see status_t), a tree of all processes created and the priority of the process for the
 * scheduler.
 */
class PCB : public EventListener {
public:
	PCB(int id, char name, int priority) : id(id), name(name), priority(priority) {} ;
	int id;
	char name;
	vector<RCB *> resources;
	status_t state;
	vector<PCB> creation_tree;
	int priority;
	virtual EventListener* on_delete(char arg_name) {
		if (name != arg_name) return nullptr;
		else return this;
	};

	virtual void has_deleted(char arg_name) {
		vector<PCB>::iterator k = creation_tree.begin();
		while (k != creation_tree.end()) {
			if (k->name == arg_name) break;
			k++;
		}

		if (k != creation_tree.end()) creation_tree.erase(k);
	}

	virtual EventListener* on_request(string name) { return nullptr; };

	friend ostream& operator<<(ostream& outs, PCB &obj) {
		outs << "Process (" << obj.id << ") in status ";
		switch (obj.state.status) {
			case READY:
				outs << "READY";
				break;
			case BLOCKED:
				outs << "BLOCKED";
				break;
			case RUNNING:
				outs << "RUNNING";
				break;
			default:
				break;
		} 
		outs << " with priority " << obj.priority;
		outs << " with " << obj.creation_tree.size() << " children";
		return outs;
	}
};

/**
 * ListManager
 * A singleton design for a list manager. This class holds the ready, running and block list of 
 * processes. Each call must be routed through the getInstance call to ensure these lists are
 * not created.
 */
class ListManager {
public:
	vector<PCB>* get_ready_list() { return &ready_list; };
	vector<PCB>* get_running_list() { return &running_list; };
	vector<PCB>* get_blocked_list() { return &blocked_list; };
	static ListManager* get_instance();
private:
	static ListManager *instance;
	vector<PCB> ready_list;
	vector<PCB> running_list;	
	vector<PCB> blocked_list;
	ListManager() : ready_list(), running_list(), blocked_list() { };
};

// Singleton pattern for lists.
ListManager *ListManager::instance = nullptr;
ListManager* ListManager::get_instance() {
	if (ListManager::instance == nullptr)
		ListManager::instance = new ListManager();
	return ListManager::instance;
}

/**
 * ShellManager
 * The actual manager of the shell. Will handle all actions related to processes and resources.
 */
class ShellManager {
public:
	ShellManager(ListManager *list_manager, PCB *root_process);
	PCB* get_current_process() { return current_process; };
	void create(PCB *parent_id, char name, int priority);
	void destroy(char name);
	void allocate_resource(PCB *process, string name);
	void release_resource(string name);
	void list_processes_tree();
	void list_processes_tree(PCB *start, string spacer = "");
	void list_resources();
private:
	void schedule() {} ;
	void kill_tree(PCB *to_delete);
	int processId; // strictly increasing.
	ListManager *list_manager;
	PCB *root_process;
	PCB *current_process;
	EventDispatcher event_dispatcher;
	const static int num_of_resources = 4;
	RCB *resources[num_of_resources];
};

ShellManager::ShellManager(ListManager *list_manager, PCB *root_process)
	: processId(1), list_manager(list_manager), 
	root_process(root_process), current_process(root_process), 
	event_dispatcher(), resources() { 
	event_dispatcher.register_listener(root_process); 

	RCB *r1 = new RCB(0, "R1"), *r2 = new RCB(1, "R2");
	RCB *r3 = new RCB(2, "R3"), *r4 = new RCB(3, "R4");
	event_dispatcher.register_listener(r1);
	event_dispatcher.register_listener(r2);
	event_dispatcher.register_listener(r3);
	event_dispatcher.register_listener(r4);
	resources[0] = r1; 
	resources[1] = r2;
	resources[2] = r3; 
	resources[3] = r4;
}

/**
 * ShellManager::Create
 * Method to create a new process with @prop {processId} id using @param parent and @param name. 
 *
 * Sudo code from slides:
 *	Create PCB data structure
 *	Init PCB using parameters
 *	Link PCB to creation tree
 *	Insert into Ready List
 *	Schedule
 */
void ShellManager::create(PCB *parent, char name, int priority) {
	PCB *new_process = new PCB(++processId, name, priority);

	// initialize states.
	new_process->state.status = READY;
	new_process->state.list = list_manager->get_ready_list();

	// Link the PCB to the creation tree of the parent.
	parent->creation_tree.push_back(*new_process);
	list_manager->get_ready_list()->push_back(*new_process);

	event_dispatcher.register_listener(new_process);

	schedule();
}

/**
 * ShellManager::Destroy
 * Method to destroy a @param process.
 * Each PCB block destroyed will also destroy all of it's children, as well as release the resources.
 * A recursive destory design will allow us to do this efficently. 
 *
 * Sudo code from slides:
 *	Get pointer p to PCB to deleted
 *	Kill subtree
 *	Schedule
 */
void ShellManager::destroy(char name) {
	PCB *to_delete = (PCB*)event_dispatcher.fire_on_delete(name);
	event_dispatcher.unregister_listener(to_delete);	
	if (to_delete == nullptr)
		return;
	else {
		switch (to_delete->state.status) {
			case READY:

			break;
			case RUNNING:

			break;
			case BLOCKED:

			break;
			default:

			break;
		}
		kill_tree(to_delete);
		event_dispatcher.fire_has_deleted(name);
	}
}

void ShellManager::kill_tree(PCB *item) {
	pcb_iter_t iter = item->creation_tree.begin();
	for (; iter != item->creation_tree.end(); iter++) {
		kill_tree(&(*iter));
		item->creation_tree.erase(iter);
	}
	delete item;
}

/**
 * ShellManager::release_resource
 * Method to release a resource to the process world.
 * 
 * After this process the scheduler is called. 
 *
 * If there is process waiting on the resource (aka in the processes wait list)
 * run it.
 */
void ShellManager::release_resource(string name) {
	RCB *r = (RCB *)event_dispatcher.fire_on_request(name);
	if (r == nullptr) cout << "Error" << endl;
	if (r->waiting_list.empty()) {
		r->status.status = FREE;
	} else {
		PCB *p = r->waiting_list[0];
		p->state.status = READY;
		p->state.list = list_manager->get_ready_list();
		list_manager->get_ready_list()->push_back(*p);
		r->waiting_list.erase(r->waiting_list.begin());
	}

	schedule();
}

/**
 * ShellManager::allocate_resource
 * Method to allocate a resource for a process
 */
void ShellManager::allocate_resource(PCB *process, string name) {
	RCB *r = (RCB *)event_dispatcher.fire_on_request(name);
	if (r == nullptr) cout << "Error" << endl;
	if (r->status.status == FREE) {
		r->status.status = ALLOCATED;
		process->resources.push_back(r);
	} else {
		process->state.status = BLOCKED;
		process->state.list = list_manager->get_blocked_list();
		r->waiting_list.push_back(process);
		list_manager->get_blocked_list()->push_back(*process);
		auto i = list_manager->get_ready_list()->begin();
		while (i != list_manager->get_ready_list()->end()) {
			if (i->name == process->name)
				break;
			i++;
		}

		if (i != list_manager->get_ready_list()->end())
			list_manager->get_ready_list()->erase(i);
	}
}
/**
 * ShellManager::list_process
 * Calls list_process(PCB)
 */
void ShellManager::list_processes_tree() {
	list_processes_tree(root_process);
}

/**
 * ShellManager::list_processes
 * Does a depth first transversal of the processes. Simply prints them.
 */
void ShellManager::list_processes_tree(PCB *start, string spacer) {
	cout << spacer << *start << endl;
	for (auto& pcb : start->creation_tree) {
		list_processes_tree(&pcb, spacer + "  ");
	}
}

/**
 * ShellManager::list_resources
 * Doesn't do antyhing fancy, just printing resources
 */
void ShellManager::list_resources() {
	for (int i = 0; i < num_of_resources; i++) {
		cout << *resources[i] << endl;
	}
}

/**
 * ShellManager::schedule
 * Preemptive priority scheduling.
 * Sudo code from text:
 *
 */
const string INIT_CMD = "init";
const string CREATE_CMD = "cr";
const string DESTORY_CMD = "de";
const string REQUEST_CMD = "req";
const string RELEASE_CMD = "rel";
const string TIMEOUT_CMD = "to";
const string LIST_PROCESSES_CMD = "lsp";
const string LIST_RESOURCES_CMD = "lsr";
const string HELP_CMD = "help"; 
const string QUIT_CMD = "quit"; 

vector<string> split_args(string input_args) {
	string buf;
	stringstream ss(input_args);
	vector<string> tokens;
	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}

vector<string> prompt(PCB *current_process) {
	cout << *current_process << endl;
	string input;
	cout << ">>> ";
	getline(cin, input);
	return split_args(input);
}

int main() {
	// initialize the first process.
	PCB current_process(1, '_', 0);
	ListManager::get_instance()->get_running_list()->push_back(current_process);
	ShellManager manager(ListManager::get_instance(), &current_process);	
	while (!ListManager::get_instance()->get_running_list()->empty()) {
		vector<string> args = prompt(manager.get_current_process());
		string input = args.at(0);

		if (!INIT_CMD.compare(input)) {
			cout << "Init" << endl;
		}
		if (!CREATE_CMD.compare(input)) {
			manager.create(&current_process, args[1][0], args[2][0] - '0');
		}
		if (!DESTORY_CMD.compare(input)) {
			manager.destroy(args[1][0]);
		}
		if (!REQUEST_CMD.compare(input)) {
			manager.allocate_resource(manager.get_current_process(), args[1]);
		}
		if (!RELEASE_CMD.compare(input)) {
			manager.release_resource(args[1]);
		}
		if (!TIMEOUT_CMD.compare(input)) {
			cout << "Timeout" << endl;
		}
		if (!LIST_PROCESSES_CMD.compare(input)) {
			manager.list_processes_tree();
		}
		if (!LIST_RESOURCES_CMD.compare(input)) {
			manager.list_resources();
		}
		if (!HELP_CMD.compare(input)) {
			cout << "Help" << endl;
		}
		if (!QUIT_CMD.compare(input)) {
			return 0;
		}
	}
	return 0;
}