class 
class PCB : public EventListener {
public:
	PCB(int id, string name, int priority);
	PCB (const PCB& copy);
	int id;
	string name;
	map<string, int> resource_to_amount_map;
	vector<RCB *> resources;
	status_t state;
	vector<PCB *> creation_tree;
	int priority;
	virtual EventListener* on_delete(string arg_name);
	virtual void has_deleted(string arg_name);
	virtual EventListener* on_request(string name);
	friend ostream& operator<<(ostream& outs, PCB &obj);
}