#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

using std::cout;
using std::endl;
using std::cin;
using std::getline;
using std::string;
using std::vector;
using std::ostream;
using std::stringstream;
using std::pair;
using std::map;

/**
 * @todo: Implement event dispatcher. 
 * 
 * Abstract event listener class for when any pcb is going to be erased.
 * Simply loop through ALL the pcb items and delete them.
 */

class PCB;
typedef vector<PCB *>::iterator pcb_ptr_iter_t;
typedef vector<pair<int,PCB *>>::iterator pcb_ptr_int_iter_t;
typedef vector<PCB>::iterator pcb_iter_t;
class EventDispatcher;

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
	 * 
	 * @todo: change name to on_get_resource
	 */
	virtual EventListener* on_request(string name) = 0;
	virtual void has_deleted(string name) = 0;
};

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

EventListener* EventDispatcher::fire_on_delete(const string& name) {
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

void EventDispatcher::fire_has_deleted(string name) {
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

RCB::RCB() { 
	id = -1;
	name = "";
	status.status = FREE;
	total = allocated = 0;
}
RCB::RCB(int arg_id, string arg_name, int arg_allocated) {
	id = arg_id;
	name = arg_name;
	status.status = FREE;
	allocated = 0;
	total = arg_allocated;
}

ostream& operator<<(ostream& outs, RCB &obj) {
	outs << "Resource (" << obj.name << ") in status ";
	switch (obj.total - obj.allocated) {
		case 0:
			outs << "ALLOCATED";
			break;
		default:
			outs << "FREE(" << obj.total - obj.allocated << ")";
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
	PCB(int id, string name, int priority) : id(id), name(name), priority(priority), resource_to_amount_map() 
	{
		resource_to_amount_map["R1"] = 0;
		resource_to_amount_map["R2"] = 0;
		resource_to_amount_map["R3"] = 0;
		resource_to_amount_map["R4"] = 0;
	};
	PCB (const PCB& copy) : id(copy.id), name(copy.name), 
		resource_to_amount_map(copy.resource_to_amount_map),
		state(copy.state), creation_tree(copy.creation_tree),
		priority(copy.priority) { }
	int id;
	string name;
	map<string, int> resource_to_amount_map;
	vector<RCB *> resources;
	status_t state;
	vector<PCB *> creation_tree;
	int priority;
	virtual EventListener* on_delete(string arg_name) {
		return !name.compare(arg_name) ? this : nullptr;
	};

	virtual void has_deleted(string arg_name) {
		pcb_ptr_iter_t k = creation_tree.begin();
		while (k != creation_tree.end()) {
			PCB *i = *k;
			if (!i->name.compare(arg_name)) break;
			k++;
		}

		if (k != creation_tree.end()) creation_tree.erase(k);
	}

	virtual EventListener* on_request(string name) { return nullptr; };

	friend ostream& operator<<(ostream& outs, PCB &obj) {
		/*outs << "Process (" << obj.id << ") in status ";
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
		outs << " with " << obj.creation_tree.size() << " children";*/
		outs << obj.name;
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
	bool delete_pcb_from_list(PCB *to_delete);
	vector<PCB>* find_pcb_list(PCB *to_find);
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
 * ListManager::find_pcb_list
 *
 * Method to find a pcb in it's list.
 */
vector<PCB>* ListManager::find_pcb_list(PCB *to_delete) {
	vector<PCB> *ptr_to_list = nullptr;
	switch (to_delete->state.status) {
		case READY:
			return get_ready_list();
		break;
		case RUNNING:
			return get_running_list();
		break;
		case BLOCKED:
			return get_blocked_list();
		break;
		default:
			return nullptr;
		break;
	}
}

/**
 * ListManager::delete_pcb_from_list
 *
 * Method to find a pcb in it's list and delete it.
 */
bool ListManager::delete_pcb_from_list(PCB *to_delete) {
	vector<PCB> *list = find_pcb_list(to_delete);
	if (list == nullptr) return false;

	// find to_delete and erase it.
	pcb_iter_t iter = list->begin();
	while (iter != list->end()) {
		if (iter->id == to_delete->id)
			break;
		iter++;
	}
	if (iter != list->end()) {
		list->erase(iter);
		return true;
	} else {
		return false;
	}
}

/**
 * ShellManager
 * The actual manager of the shell. Will handle all actions related to processes and resources.
 */
class ShellManager {
public:
	ShellManager(ListManager *list_manager);
	PCB* get_current_process() { return delete_current_process ? nullptr : current_process; };
	bool create(PCB *parent_id, string name, int priority);
	bool destroy(string name);
	bool allocate_resource(PCB *process, string name, int amount);
	bool release_resource(string name, int amount);
	void list_processes_tree();
	void list_processes_tree(PCB *start, string spacer = "");
	void list_resources();
	bool timeout();
	bool init();
private:
	void schedule();
	void kill_tree(PCB *to_delete);
	void preempt_put_on_ready(PCB *px);
	void preempt_put_on_wait(PCB *px);
	void release_resource_enqueue_waiting_process(RCB *r);
	int processId; // strictly increasing.
	ListManager *list_manager;
	PCB *root_process;
	PCB *current_process;
	EventDispatcher event_dispatcher;
	const static int num_of_resources = 4;
	RCB *resources[num_of_resources];
	bool delete_current_process;
};

ShellManager::ShellManager(ListManager *list_manager)
	: processId(1), list_manager(list_manager), 
	root_process(nullptr), current_process(nullptr), 
	event_dispatcher(), resources(), delete_current_process(false) { 
	RCB *r1 = new RCB(0, "R1", 1), *r2 = new RCB(1, "R2", 2);
	RCB *r3 = new RCB(2, "R3", 3), *r4 = new RCB(3, "R4", 4);
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
bool ShellManager::create(PCB *parent, string name, int priority) {
	if (event_dispatcher.fire_on_delete(name) != nullptr) {
		return false;
	}
	PCB *new_process = new PCB(++processId, name, priority);

	// initialize states.
	new_process->state.status = READY;
	new_process->state.list = list_manager->get_ready_list();

	// Link the PCB to the creation tree of the parent.
	parent->creation_tree.push_back(new_process);
	list_manager->get_ready_list()->push_back(*new_process);

	event_dispatcher.register_listener(new_process);

	schedule();
	return true;
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
bool ShellManager::destroy(string name) {
	PCB *to_delete = (PCB*)event_dispatcher.fire_on_delete(name);
	if (to_delete == nullptr) {
		return false;
	}
	event_dispatcher.unregister_listener(to_delete);	
	pcb_iter_t iter;
	delete_current_process = to_delete->state.status == RUNNING;
	kill_tree(to_delete);
	event_dispatcher.fire_has_deleted(name);
	schedule();
	return true;
}

void ShellManager::kill_tree(PCB *item) {
	pcb_ptr_iter_t iter = item->creation_tree.begin();
	event_dispatcher.unregister_listener(item);
	list_manager->delete_pcb_from_list(item);
	for (auto r_iter = item->resources.begin(); r_iter != item->resources.end(); r_iter++) {
		(*r_iter)->allocated -= item->resource_to_amount_map[(*r_iter)->name];
		item->resource_to_amount_map[(*r_iter)->name] = 0;
		release_resource_enqueue_waiting_process(*r_iter);
	}
	for (; iter != item->creation_tree.end(); iter++)
		kill_tree(*iter);
	item->creation_tree.erase(item->creation_tree.begin(), item->creation_tree.end());
	delete item;
}

/**
 * ShellManager::release_resource_enqueue_waiting_process
 * Method will remove a waiting process from the wait queue on
 * resource release.
 */
void ShellManager::release_resource_enqueue_waiting_process(RCB *r) {
	pcb_ptr_int_iter_t waitlist_iter = r->waiting_list.begin();
	while (r->allocated < r->total && waitlist_iter != r->waiting_list.end()) {
		if ((r->allocated + waitlist_iter->first) <= r->total) {
			r->allocated+= waitlist_iter->first;
			waitlist_iter->second->resource_to_amount_map[r->name] += waitlist_iter->first;
			list_manager->delete_pcb_from_list(waitlist_iter->second);
			waitlist_iter->second->state.status = READY;
			waitlist_iter->second->state.list = list_manager->get_ready_list();
			list_manager->get_ready_list()->push_back(*waitlist_iter->second);
			waitlist_iter++;
		} else {
			break;
		}
	}
	r->waiting_list.erase(r->waiting_list.begin(), waitlist_iter);
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
bool ShellManager::release_resource(string name, int amount) {
	RCB *r = (RCB *)event_dispatcher.fire_on_request(name);
	if (r == nullptr || amount > current_process->resource_to_amount_map[name]) { 
		return false; 
	}
	r->allocated -= amount;
	current_process->resource_to_amount_map[name] -= amount;
	release_resource_enqueue_waiting_process(r);
	schedule();
	return true;
}

/**
 * ShellManager::allocate_resource
 * Method to allocate a resource for a process
 */
bool ShellManager::allocate_resource(PCB *process, string name, int amount) {
	RCB *r = (RCB *)event_dispatcher.fire_on_request(name);
	if (r == nullptr || r->total < amount) { 
		return false;
	}
	process->resources.push_back(r);
	if (r->allocated + amount <= r->total) {
		r->allocated += amount;
		process->resource_to_amount_map[name] += amount;
	} else {
		process->state.status = BLOCKED;
		process->state.list = list_manager->get_blocked_list();
		r->waiting_list.push_back(pair<int, PCB*>(amount, process));
	}
	schedule();
	return true;
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
	for (PCB *pcb : start->creation_tree) {
		list_processes_tree(pcb, spacer + "  ");
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
 * ShellManager::preempt
 */
void ShellManager::preempt_put_on_ready(PCB *px) {
	list_manager->get_ready_list()->push_back(*current_process);
	string t = (*px).name;
	current_process = (PCB *)event_dispatcher.fire_on_delete(t); 
	current_process->state.status = RUNNING;
	list_manager->get_running_list()->erase(list_manager->get_running_list()->begin());
	list_manager->get_running_list()->push_back(*current_process);
	// delete the current entry from the ready list.
	pcb_iter_t iter = list_manager->get_ready_list()->begin();
	while (iter->id != current_process->id) iter++;
	list_manager->get_ready_list()->erase(iter);
}

void ShellManager::preempt_put_on_wait(PCB *px) {
	list_manager->get_blocked_list()->push_back(*current_process);
	current_process = (PCB *)event_dispatcher.fire_on_delete(px->name); 
	current_process->state.status = RUNNING;
	list_manager->get_running_list()->erase(list_manager->get_running_list()->begin());
	list_manager->get_running_list()->push_back(*current_process);
	// delete the current entry from the ready list.
	pcb_iter_t iter = list_manager->get_ready_list()->begin();
	while (iter->id != current_process->id) iter++;
	list_manager->get_ready_list()->erase(iter);
}

/**
 * ShellManager::schedule
 * A preemptive priority scheduling algorithm.
 * Sudo code from the lecture:
 *
 * find highest priority process p
 * if (self->priority < p->priority || self->status.type != 'running' || self == null)
 *   preempt(p, self)
 */
void ShellManager::schedule() {
	// find the highest priority item in the ready list.
	int level_two = 0, level_one = 0, level_zero = 0;
	PCB *processes[3], *px = nullptr;
	pcb_iter_t p = list_manager->get_ready_list()->begin();
	for (; p != list_manager->get_ready_list()->end(); p++) {
		if (p->priority == 0 && level_zero < 1) {
			level_zero++;
			processes[2] = &(*p);
		} else if (p->priority == 1 && level_one < 1) {
			level_one++;
			processes[1] = &(*p);
		} else if (p->priority == 2 && level_two < 1) {
			level_two++;
			processes[0] = &(*p);
		}
	}
	
	if (level_two == 1) px = processes[0];
	else if (level_one == 1) px = processes[1];
	else if (level_zero == 1) px = processes[2];
	else { 
		// couldn't find a waiting process
		return;
	}
	if (get_current_process() == nullptr) { // dead process
		current_process = (PCB *)event_dispatcher.fire_on_delete(px->name); 
		list_manager->delete_pcb_from_list(current_process);
		current_process->state.status = RUNNING;
		list_manager->get_running_list()->push_back(*current_process);		
		delete_current_process = false;
	} else if (get_current_process()->priority < px->priority) { // higher priority
		// preempt
		current_process->state.status = READY;
		preempt_put_on_ready(px);
	}
	else if (get_current_process()->state.status != RUNNING) { // timeout
		preempt_put_on_wait(px);
	}
}

bool ShellManager::timeout() {
	int x = current_process->priority;
	PCB *y = current_process;
	current_process->priority = x - 1;
	schedule();
	y->priority = x;

	pcb_iter_t iter = list_manager->get_ready_list()->begin();
	while (iter != list_manager->get_ready_list()->end() && iter->id != y->id) {iter++;}
	if (list_manager->get_ready_list()->end() != iter) iter->priority = x;
	return true;
}

bool ShellManager::init() {
	// start over.
	resources[0]->allocated = 0;
	resources[1]->allocated = 0;
	resources[2]->allocated = 0;
	resources[3]->allocated = 0;
	if (root_process != nullptr) kill_tree(root_process);
	root_process = new PCB(1, "Init", 0);
	current_process = root_process;
	processId = 1;
	ListManager::get_instance()->get_running_list()->push_back(*current_process);
	event_dispatcher.register_listener(root_process);
	return true;
}

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

vector<string> prompt(PCB *current_process, bool init, bool error) {
	if (init && !error)
		cout << *current_process << " ";
	else if (error)
		cout << "error ";
	string input;
	getline(cin, input);
	return split_args(input);
}

int main() {
	// initialize the first process.
	ShellManager manager(ListManager::get_instance());	
	bool initialized = false, error = false;
	while (!ListManager::get_instance()->get_running_list()->empty() || !initialized) {
		vector<string> args = prompt(manager.get_current_process(), initialized, error);
		if (args.size() == 0) {
			if (!initialized) break;
			cout << endl;
			initialized = false;
			continue;
		}
		string input = args.at(0);
		error = false;

		if (!INIT_CMD.compare(input)) {
			initialized = manager.init();
		}
		else if (!initialized) {
			error = true;
		}
		else if (!CREATE_CMD.compare(input)) {
			error = !manager.create(manager.get_current_process(), args[1], args[2][0] - '0');
		}
		else if (!DESTORY_CMD.compare(input)) {
			error = !manager.destroy(args[1]);
		}
		else if (!REQUEST_CMD.compare(input)) {
			error = !manager.allocate_resource(manager.get_current_process(), args[1], args[2][0] - '0');
		}
		else if (!RELEASE_CMD.compare(input)) {
			error = !manager.release_resource(args[1], args[2][0] - '0');
		}
		else if (!TIMEOUT_CMD.compare(input)) {
			error = !manager.timeout();
		}
		else if (!LIST_PROCESSES_CMD.compare(input)) {
			manager.list_processes_tree();
		}
		else if (!LIST_RESOURCES_CMD.compare(input)) {
			manager.list_resources();
		}
		else if (!QUIT_CMD.compare(input)) {
			return 0;
		}
	}
	return 0;
}
