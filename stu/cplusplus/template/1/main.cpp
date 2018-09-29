#include<iostream>



template<typename T>
int compare(const T& v1, const T& v2) {
	if (v1 < v2)	return -1;
	if (v1 > v2)	return 1;

	return 0;
}

template<>
int compare<const char*>(const char* const& v1, const char*  const& v2) {
	return strcmp(v1, v2);
}

//函数模板的特化
void case1() {

	std::cout << compare(9, 100) << std::endl;

	const char* p1 = "myname";
	const char* p2 = "myid";
	std::cout << compare(p1, p2) << std::endl;
}


template<typename T>
class CCompare {
public:
	static bool Equal(const T& lhs, const T& rhs) {
		return lhs == rhs;
	}
};


//类模板的特化
template<>
class CCompare<const char*> {
public:
	static bool Equal(const char*  lhs, const char*  rhs) {
		return strcmp(lhs, rhs);
	}
};

void case2() {

	CCompare<int> cmp;
	std::cout << cmp.Equal(300, 21) << std::endl;
	
	CCompare<const char*> cmp_ch;
	const char* p1 = "myname";
	const char* p2 = "myid";

	std::cout << cmp_ch.Equal(p1, p2) << std::endl;
}



int main() {
	//case1();
	case2();

	return 0;
}