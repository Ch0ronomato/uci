#include <iostream>
#include <vector>
#include <string>

using std::cout;
using std::endl;
using std::cin;
using std::getline;
using std::vector;

typedef enum statusType_enum {
	READY,
	RUNNING,
	BLOCKED
} statusType;

class PCB;
typedef struct status_s {
	statusType status;
	vector<PCB> list;
} status_t;

class RCB {
public:
	RCB(int id);
	status_t status;
	vector<PCB> waiting_list;	
};

class PCB {
public:
	PCB(int id, int priority);
	vector<RCB> resources;
	status_t status;
	vector<PCB> creation_tree;
	int priority;
};

class ListManager {
public:
	vector<PCB> getReadyList() { return readyList; };
	vector<PCB> getRunningList() { return runningList; };
	vector<PCB> getBlockedList() { return blockedList; };
	static ListManager* getInstance() { return ListManager.instance };
private:
	ListManager() { };
	static ListManager* instance;
	vector<PCB> readyList;
	vector<PCB> runningList;	
	vector<PCB> blockedList;
};

int main() {
	return 0;
}
