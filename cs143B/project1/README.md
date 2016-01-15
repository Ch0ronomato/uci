__THREAD MANAGEMENT__

Simple thread manager with three states, and resources that can be released and requested.
Instead of using the actual machine hardware and interrupts, we will use a presentation shell
that represents both the current process and user.

Operation   |    old state              new state
Create      |    (none)        ->       ready
Request     |    running       ->       blocked
Release     |    blocked       ->       ready
Destroy     |    any           ->       (none)
==================================================
Scheduler   |    ready         ->       running
            |    running       ->       ready

**PCB**
Simple PCB data structure:
	id: Process id
	---------------
	Memory: Linked list to memory blocks
	---------------
	Other_Resources: Linked list to resources
	---------------
	Status: 
	---------------
	Creation_Tree: Parents and child proc
	---------------
	Priority: Running priority (0 init, 1, 2 highest)
	---------------
**RCB**
Simple RCB data structure:
	RID: ID
	------------
	Status: Status
	------------
	Waiting_list: blocked process

**Scheduling**
Preemptive multilevel priority schedule with fixed priority levels. FIFO order (queue). Termination ondition `self->priority < p->priority`. This can be satisifed two ways:
	1.) The scheduler is called at the end of a Release operation, the priority of the process unblocked as a result of the Release operation may be higher than the priority of the current process;
	2.) When the scheduler is called at the end of a Create operation, the priority of the new process may be higher than the priority of the current process.


