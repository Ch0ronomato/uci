#include <iostream>
#include "pm.h"

void print_va_t(pm::va_t *v) {
	std::cout << "{ p: " << v->p << ", s: " << v->s << ", w: " << v->w;
	std::cout << ", addr: " << v->addr << " }" << std::endl;
}

int main() {
	pm p;
	p.set_frame(1);
	p.set_frame(3);
	std::cout << p.get_free_frame(2) << std::endl;
	return 0;
}