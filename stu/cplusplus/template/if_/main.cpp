#include<iostream>




template<bool C, typename A, typename B> struct if_ {};

template<typename A, typename B> struct if_<true, A, B> { typedef A type; };
template<typename A, typename B> struct if_<false, A, B> { typedef B type; };


int main(int argc, char* argv[]) {

	if_<true, char, double>::type bb;
	std::cout << sizeof(bb) << std::endl;

	if_<false, char, double>::type cc;
	std::cout << sizeof(cc) << std::endl;

	
	//if_<char, int>::type dd;			//error use




	return 0;
}