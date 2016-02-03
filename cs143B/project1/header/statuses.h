#include <vector>
using std::vector;

#ifndef STATUSES_H
#define STATUSES_H
class PCB;
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
#endif