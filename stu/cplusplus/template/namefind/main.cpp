#include <iostream>

template <typename T>
struct Base
{
	Base() { std::cout << "Base::Base()" << std::endl; }
	void fun()
	{
		std::cout << "Base::fun" << std::endl;
	}
};

template <>
struct Base<int>
{
	Base() { std::cout << "Base::Base()  int" << std::endl; }
	void fun()
	{
		std::cout << "Base::fun() int" << std::endl;
	}
};

template <typename T>
struct Derived : Base<T>
{
	Derived() { std::cout << "Derived::Derived()" << std::endl; }
	void gun()
	{
		std::cout << "Derived::gun" << std::endl;
		this->fun();
		Base<T>::fun();
	}

	void fun()
	{
		std::cout << "Derived::fun() " << std::endl;
	}
};

void case1() {

	Derived<std::string> d;
	d.fun();
	d.gun();

	std::cout << std::endl;

	Derived<int> e;
	e.fun();
	e.gun();

}
int main() {

	case1();

	return 0;
}