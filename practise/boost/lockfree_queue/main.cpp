/*
* File:   main.cpp
* Author: Peng
*
* Created on February 22, 2014, 9:55 PM
*/

#include <cstdlib>  
#include "freedeque.h"  
#include <mutex>  
#include <stdio.h>  
#include <string>  
#include <set>  
#include <random>  
#include <chrono>  
#include <iostream>  
#include <ctime>  
#include <atomic>  
#include <sstream>  
#include <boost/thread/thread.hpp>  
#include <boost/lockfree/queue.hpp>  
#include <iostream>  
#include <boost/atomic.hpp>  

const long NUM_DATA = 10;
const int NUM_ENQUEUE_THREAD = 1;
const int NUM_DEQUEUE_THREAD = 1;
const long NUM_ITEM = 1000000;

using namespace std;
class Data
{
public:
	Data(int i = 0) : m_iData(i)
	{
		stringstream ss;
		ss << i;
		m_szDataString = ss.str();
		//sprintf( m_szDataString, "%l-d", i);      
	}

	bool operator< (const Data & aData) const
	{
		if (m_iData < aData.m_iData)
			return true;
		else
			return false;
	}

	int& GetData()
	{
		return m_iData;
	}
private:
	int m_iData;
	string m_szDataString;
	//char m_szDataString[MAX_DATA_SIZE];  
};

Data DataArray[NUM_DATA];

constexpr long size = 0.5 * NUM_DATA;
lfringqueue < Data, 1000> LockFreeQueue;
boost::lockfree::queue<Data*> BoostQueue(1000);

// Since there is a chance that the searched number cannot be found, so the function should return boolean  
bool BinarySearchNumberInSortedArray(Data datas[], int iStart, int iEnd, int SearchedNum, int &iFound)
{
	if (iEnd - iStart <= 1)
	{
		if (datas[iStart].GetData() == SearchedNum)
		{
			iFound = iStart;
			return true;
		}
		else if (datas[iEnd].GetData() == SearchedNum)
		{
			iFound = iEnd;
			return true;
		}
		else
			return false;
	}

	int mid = 0.5 * (iStart + iEnd);

	if (datas[mid].GetData() == SearchedNum)
	{
		iFound = mid;
		return true;
	}

	if (datas[mid].GetData() > SearchedNum)
	{
		if (mid - 1 >= 0)
			return BinarySearchNumberInSortedArray(datas, iStart, mid - 1, SearchedNum, iFound);
		else
			return false;
	}
	else
	{
		if (mid + 1 <= iEnd)
			return BinarySearchNumberInSortedArray(datas, mid + 1, iEnd, SearchedNum, iFound);
		else
			return false;
	}
}
bool GenerateRandomNumber_FindPointerToTheNumber_EnQueue()
{
	std::uniform_int_distribution<int> dis(1, NUM_DATA);
	default_random_engine engine{};

	for (long i = 0; i < NUM_ITEM; i++)
	{
		int x = dis(engine);

		int iFoundIndex;
		if (BinarySearchNumberInSortedArray(DataArray, 0, NUM_DATA - 1, x, iFoundIndex))
		{
			Data* pData = &DataArray[iFoundIndex];
			LockFreeQueue.enqueue(pData);
			//BoostQueue.push( pData );  
		}
	}

	return true;
}
bool Dequeue()
{
	Data *pData;

	for (long i = 0; i < NUM_ITEM; i++)
	{
		while (LockFreeQueue.dequeue(&pData));
		//while (  BoostQueue.pop( pData ) ) ;        
	}

	return true;
}

int main(int argc, char** argv)
{
	for (int i = 1; i < NUM_DATA + 1; i++)
	{
		Data data(i);
		DataArray[i - 1] = data;
	}

	std::thread PublishThread[NUM_ENQUEUE_THREAD];
	std::thread ConsumerThread[NUM_DEQUEUE_THREAD];
	std::chrono::duration<double> elapsed_seconds;

	for (int i = 0; i < NUM_ENQUEUE_THREAD; i++)
	{
		PublishThread[i] = std::thread(GenerateRandomNumber_FindPointerToTheNumber_EnQueue);
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < NUM_DEQUEUE_THREAD; i++)
	{
		ConsumerThread[i] = std::thread{ Dequeue };
	}

	for (int i = 0; i < NUM_DEQUEUE_THREAD; i++)
	{
		ConsumerThread[i].join();
	}

	auto end = std::chrono::high_resolution_clock::now();
	elapsed_seconds = end - start;
	std::cout << "Enqueue and Dequeue 1 million item in:" << elapsed_seconds.count() << std::endl;


	for (int i = 0; i < NUM_ENQUEUE_THREAD; i++)
	{
		PublishThread[i].join();
	}

	return 0;
}