#include<iostream>



//（1）模板类的非模板友元函数(类模板具体化：非模板友元 == n：1)
template<typename T>
class HashFriend {
public:
	HashFriend(const T& s):base(s) {}

	template<typename T>
	friend void report(HashFriend<T> &);

private:
	T base;

};

template<typename T>
void report(HashFriend<T>& hash) {
	std::cout << hash.base << std::endl;

}

template<>
class HashFriend<const char*> {
public:
	HashFriend(const char* s) :base(s) {}

	friend void report(HashFriend<const char* > &);

private:
	const char* base;

};

void report(HashFriend<const char* >& hash) {
	std::cout << hash.base << std::endl;

}


template<>
class HashFriend<std::string> {
public:
	HashFriend(std::string s) :base(s) {}

	friend void report(HashFriend<std::string> &);

private:
	std::string base;

};

void report(HashFriend<std::string >& hash) {
	std::cout << hash.base.c_str() << std::endl;

}

void case1() {
	HashFriend<const char*> aa("bright");
	report(aa);

	HashFriend<const double> bb(900.900);
	report(bb);

	std::string s = "good evening";
	HashFriend<std::string> cc(s);
	report(cc);
}


//模板类的约束模板友元函数(类模板具体化：非模板友元 == 1：1)
template<typename T> void counts();
template<typename T> void report2(T &);

template<typename type2>
class HashFrinde2 {

public:

	friend void counts<type2>();

	friend void report2<>(HashFrinde2<type2> &);

private:
	type2  base;
};

template<typename type2>
void counts<type2>() {
	std::cout << "counts"  << std::endl;
}

template<typename type2>
void report2(HashFrinde2<type2> &hash){
	std::cout << hash.base << std::endl;
}


void case2(){
	

	HashFriend<const double> bb(900.900);
	counts<const double>();
	report(bb);

}

//模板类的非约束模板友元函数(类模板具体化：非模板友元 == 1：n)

template<typename T>
class ManyFriend {
private:
	T item;
public:
	ManyFriend(const T& t):item(t){}
	template<typename C, typename D> friend void show(C& c, D& d);
};

template<typename C, typename D>
void show(C& c, D& d) {
	std::cout << c.item << "  " << d.item << std::endl;
}

void case3() {

	ManyFriend<int> hfi1(10);
	ManyFriend<int> hfi2(20);
	ManyFriend<double> hfdb(10.5);
	
	show(hfi1, hfi2);
	
	show(hfdb, hfi2);
}
int main(){
	//case1();
	//case2();
	case3();
	return 0;
}