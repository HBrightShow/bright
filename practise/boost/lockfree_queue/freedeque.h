#pragma once#ifndef INCLUDED_UTILS_LFRINGQUEUE  
#define INCLUDED_UTILS_LFRINGQUEUE  

#define _ENABLE_ATOMIC_ALIGNMENT_FIX  
#define ATOMIC_FLAG_INIT 0  


#pragma once  


#include <vector>  
#include <mutex>  
#include <thread>  
#include <atomic>  
#include <chrono>  
#include <cstring>  
#include <iostream>  

// Lock free ring queue   

template < typename _TyData, long _uiCount = 100000 >
class lfringqueue
{
public:
	lfringqueue(long uiCount = _uiCount) : m_lTailIterator(0), m_lHeadIterator(0), m_uiCount(uiCount)
	{
		m_queue = new _TyData*[m_uiCount];
		memset(m_queue, 0, sizeof(_TyData*) * m_uiCount);
	}

	~lfringqueue()
	{
		if (m_queue)
			delete[] m_queue;
	}


	bool enqueue(_TyData *pdata, unsigned int uiRetries = 1000)
	{
		if (NULL == pdata)
		{
			// Null enqueues are not allowed  
			return false;
		}

		unsigned int uiCurrRetries = 0;
		while (uiCurrRetries < uiRetries)
		{
			// Release fence in order to prevent memory reordering   
			// of any read or write with following write  
			std::atomic_thread_fence(std::memory_order_release);

			long lHeadIterator = m_lHeadIterator;

			if (NULL == m_queue[lHeadIterator])
			{
				long lHeadIteratorOrig = lHeadIterator;

				++lHeadIterator;
				if (lHeadIterator >= m_uiCount)
					lHeadIterator = 0;

				// Don't worry if this CAS fails.  It only means some thread else has  
				// already inserted an item and set it.  
				if (std::atomic_compare_exchange_strong(&m_lHeadIterator, &lHeadIteratorOrig, lHeadIterator))
				{
					// void* are always atomic (you wont set a partial pointer).  
					m_queue[lHeadIteratorOrig] = pdata;

					if (m_lEventSet.test_and_set())
					{
						m_bHasItem.test_and_set();
					}
					return true;
				}
			}
			else
			{
				// The queue is full.  Spin a few times to check to see if an item is popped off.  
				++uiCurrRetries;
			}
		}
		return false;
	}

	bool dequeue(_TyData **ppdata)
	{
		if (!ppdata)
		{
			// Null dequeues are not allowed!  
			return false;
		}

		bool bDone = false;
		bool bCheckQueue = true;

		while (!bDone)
		{
			// Acquire fence in order to prevent memory reordering   
			// of any read or write with following read  
			std::atomic_thread_fence(std::memory_order_acquire);
			//MemoryBarrier();  
			long lTailIterator = m_lTailIterator;
			_TyData *pdata = m_queue[lTailIterator];
			//volatile _TyData *pdata = m_queue[lTailIterator];              
			if (NULL != pdata)
			{
				bCheckQueue = true;
				long lTailIteratorOrig = lTailIterator;

				++lTailIterator;
				if (lTailIterator >= m_uiCount)
					lTailIterator = 0;

				//if ( lTailIteratorOrig == atomic_cas( (volatile long*)&m_lTailIterator, lTailIterator, lTailIteratorOrig ))  
				if (std::atomic_compare_exchange_strong(&m_lTailIterator, &lTailIteratorOrig, lTailIterator))
				{
					// Sets of sizeof(void*) are always atomic (you wont set a partial pointer).  
					m_queue[lTailIteratorOrig] = NULL;

					// Gets of sizeof(void*) are always atomic (you wont get a partial pointer).  
					*ppdata = (_TyData*)pdata;

					return true;
				}
			}
			else
			{
				bDone = true;
				m_lEventSet.clear();
			}
		}
		*ppdata = NULL;
		return false;
	}


	long countguess() const
	{
		long lCount = trycount();

		if (0 != lCount)
			return lCount;

		// If the queue is full then the item right before the tail item will be valid.  If it  
		// is empty then the item should be set to NULL.  
		long lLastInsert = m_lTailIterator - 1;
		if (lLastInsert < 0)
			lLastInsert = m_uiCount - 1;

		_TyData *pdata = m_queue[lLastInsert];
		if (pdata != NULL)
			return m_uiCount;

		return 0;
	}

	long getmaxsize() const
	{
		return m_uiCount;
	}

	bool HasItem()
	{
		return m_bHasItem.test_and_set();
	}

	void SetItemFlagBack()
	{
		m_bHasItem.clear();
	}

private:
	long trycount() const
	{
		long lHeadIterator = m_lHeadIterator;
		long lTailIterator = m_lTailIterator;

		if (lTailIterator > lHeadIterator)
			return m_uiCount - lTailIterator + lHeadIterator;

		// This has a bug where it returns 0 if the queue is full.  
		return lHeadIterator - lTailIterator;
	}

private:
	std::atomic<long> m_lHeadIterator;  // enqueue index  
	std::atomic<long> m_lTailIterator;  // dequeue index  
	_TyData **m_queue;                  // array of pointers to the data  
	long m_uiCount;                     // size of the array  
	std::atomic_flag m_lEventSet = ATOMIC_FLAG_INIT;       // a flag to use whether we should change the item flag  
	std::atomic_flag m_bHasItem = ATOMIC_FLAG_INIT;        // a flag to indicate whether there is an item enqueued  
};

#endif //INCLUDED_UTILS_LFRINGQUEUE  
