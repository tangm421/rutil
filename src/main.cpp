
#include "header.h"
#include <memory>

void Base::process() {
	std::auto_ptr<Controler> ptr(new Controler(*this));
	std::cout << ptr.get() << std::endl;
}

int main()
{
	std::auto_ptr<Base> ptr(new Derive);
	ptr->process();
	return 0;
}