#include<iostream>
#include<memory>
#include<cstdint>
#include<list>
#include<vector>
#include<algorithm>

using namespace std;

class Sam
{
public:
	Sam(int v):val(v) {	}
	int32_t val;

	~Sam()
	{
		std::cout << "~sam()" << std::endl;
	}


};


class CB;
class CA;

class CA
{
public:
	CA() {}
	~CA() { std::cout << "~CA()" << std::endl; }

	void Register(const std::shared_ptr<CB>& sp)
	{
		m_sp = sp;
	}

private:
	std::shared_ptr<CB> m_sp;
};

class CB
{
public:
	CB() {};
	~CB() { std::cout << "~CB()" << std::endl; };

	void Register(const std::shared_ptr<CA>& sp)
	{
		m_sp = sp;
	}

private:
	std::shared_ptr<CA> m_sp;
};


class Y : public std::enable_shared_from_this<Y>
{
public:
	std::shared_ptr<Y> GetSharePtr()
	{
		return shared_from_this();
	}
};



template<typename T>
class Deleter
{
public:
	void operator () (T* x) const
	{
		if (x != NULL)
		{
			std::cout << __LINE__ << std::endl;
			delete x;
			x = NULL;
		}
	}
};


int main() {


	

	std::shared_ptr<Sam> sp(new Sam(10), Deleter<Sam> {});
	return 0;
}