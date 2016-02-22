// Ian schweer
// 22514022
#ifndef IO_SYSTEM_H
#define IO_SYSTEM_H
class IO_system {
public:
	virtual void read_block(int i, unsigned char *p) = 0;
	virtual void write_block(int i, unsigned char *p) = 0;	
	static IO_system* CreateIOSystem();
};
#endif