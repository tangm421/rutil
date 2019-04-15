#pragma once

//#include <algorithm>
#include <limits>
#include <iostream>

class Controler;

class Base {
public:
	virtual ~Base() {}
	virtual int GetValue() { return std::numeric_limits<int>::min(); }
	void process();
};

class Derive : public Base {
public:
	~Derive() {}
	int GetValue() { return std::numeric_limits<int>::max(); }
};


class Controler {
	Base& mBase;
public:
	~Controler() {}
	Controler(Base& base):mBase(base){}
	friend std::ostream& operator<<(std::ostream& os, const Controler* const c) {
		return os << c->mBase.GetValue() << std::endl;
	}
};