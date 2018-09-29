#include<iostream>


template<typename T>
class List
{
public:
	template<T2>
	List(List<T2> const&);

};
template<typename T>
template<typename T2>
List<T>::List(List<T2> const& b){}

template<typename T>
int Length(List<T> const&);

template<typename T>
union AllocChunk
{

};