#include<iostream>
#include<boost/any.hpp>
#include<vector>


std::vector<boost::any> vc;

template<typename T>
void print(T val) {
	std::cout << val << std::endl;
}

template<typename Head,typename... Rail>
void print(Head head, Rail... rail) {
	//std::cout << head << ",";
	
	vc.push_back(head);

	print(rail...);
}


int main() {
	print(20);
	print(5, 'a');
	print("bright", "is", 5, "years old");



	for(auto var : vc)
	{
		
	}
	return 0;
}

