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
 * Status enum for a process.
 */
typedef enum statusType_enum {
	READY,
	RUNNING,
	BLOCKED
} statusType;

class PCB;

/** 
 * Status data structure for all processes waiting on this one.
 */
typedef struct status_s {
	statusType status;
	vector<PCB> list;
} status_t;

/**
 * Resource control block. 
 * Contains data about a resource.
 */
 // @todo: Add code for resource allocation amount.
class RCB {
public:
	RCB(int id);
	int id;
	status_t status;
	vector<PCB> waiting_list;
	
};

/**
 * Process control block.
 * Contains all data related to a running process. Include an id, all resources, a status
 * (@see status_t), a tree of all processes created and the priority of the process for the
 * scheduler.
 */
class PCB {
public:
	PCB(int id, char name, int priority) : id(id), name(name), priority(priority) {} ;
	int id;
	char name;
	vector<RCB> resources;
	status_t status;
	vector<PCB> creation_tree;
	int priority;
	friend ostream& operator<<(ostream& outs, PCB &obj) {
		outs << "Process (" << obj.id << ") in status ";
		switch (obj.status.status) {
			case READY:
				outs << "READY";
				break;
			case BLOCKED:
				outs << "BLOCKED";
				break;
			case RUNNING:
				outs << "RUNNING";
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
	ShellManager(ListManager *list_manager, PCB *root_process) 
		: processId(1), list_manager(list_manager), root_process(root_process) { };
	void create(PCB *parent_id, char name, int priority);
	void destroy(char name);
	void list_processes_tree();
	void list_processes_tree(PCB *start, string spacer = "");
private:
	void schedule() {} ;
	void kill_tree(PCB *to_delete);
	int processId; // strictly increasing.
	ListManager *list_manager;
	PCB *root_process;
};


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
	PCB newProcess(++processId, name, priority);

	// Link the PCB to the creation tree of the parent.
	parent->creation_tree.push_back(newProcess);
	list_manager->get_ready_list()->push_back(newProcess);

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

int main() {
	// initialize the first process.
	PCB current_process(1, 'a', 0);
	ListManager::get_instance()->get_running_list()->push_back(current_process);
	ShellManager manager(ListManager::get_instance(), &current_process);
	while (!ListManager::get_instance()->get_running_list()->empty()) {
		cout << current_process << endl;
		string input;
		cout << ">>> ";
		getline(cin, input);
		vector<string> args = split_args(input);
		input = args.at(0);

		if (!INIT_CMD.compare(input)) {
			cout << "Init" << endl;
		}
		if (!CREATE_CMD.compare(input)) {
			manager.create(&current_process, args[1][0], args[2][0] - '0');
		}
		if (!DESTORY_CMD.compare(input)) {
			cout << "Destory" << endl;
		}
		if (!REQUEST_CMD.compare(input)) {
			cout << "Request" << endl;
		}
		if (!RELEASE_CMD.compare(input)) {
			cout << "Release" << endl;
		}
		if (!TIMEOUT_CMD.compare(input)) {
			cout << "Timeout" << endl;
		}
		if (!LIST_PROCESSES_CMD.compare(input)) {
			manager.list_processes_tree();
		}
		if (!LIST_RESOURCES_CMD.compare(input)) {
			cout << "lsr" << endl;
		}
		if (!HELP_CMD.compare(input)) {
			cout << "Help" << endl;
		}
		if (!QUIT_CMD.compare(input)) {
			cout << "Quit" << endl;
		}
	}
	return 0;
}
