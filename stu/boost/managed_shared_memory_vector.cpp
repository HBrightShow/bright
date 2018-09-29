#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <cstdlib> //std::system
#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include <sys/stat.h>        /*  For mode constants */
#include <fcntl.h>           /*  For O_* constants */
#include <string>

using namespace boost::interprocess;
using std::string;

class Item
{
public:
	Item() {}
	~Item() {}

	int id;
	int size;
	string name;
};

typedef allocator<Item, managed_shared_memory::segment_manager>  ShmemAllocator;

typedef vector<Item, ShmemAllocator> MyVector;

int main()
{
	try {
		// init
		managed_shared_memory segment(create_only, "SharedMemory", 65536);
		const ShmemAllocator alloc_inst(segment.get_segment_manager());
		MyVector *myvector = segment.construct<MyVector>("MyVector")(alloc_inst);

		// push_back
		Item v;
		for (int i = 0; i < 100; i++) {
			v.id = i;
			myvector->push_back(v);
			v.name = "hello";
		}

		// loop
		MyVector *myvector2 = segment.find<MyVector>("MyVector").first;
		printf("vector size:%d\n", myvector2->size());
		for (int j = 0; j < myvector2->size(); j++)
			printf("%d  ", (*myvector2)[j].id);

		printf("\n");

		shared_memory_object::remove("SharedMemory");
	}
	catch (const std::exception & e) {
		printf("Exception:%s\n", e.what());
		shared_memory_object::remove("SharedMemory");
	}

	return 0;

}
