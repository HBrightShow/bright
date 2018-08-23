#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <functional>
#include <cstdlib> //std::system
#include <utility>
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

typedef int KeyType;
typedef Item MappedType;
typedef std::pair<const int, Item> ValueType;

typedef allocator<ValueType, managed_mapped_file::segment_manager> ShmemAllocator;

typedef map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator> MyMap;

int main()
{
	try {
		// init
		managed_mapped_file segment(open_or_create, "SharedMemory", 65536);


		MyMap *mymap = segment.find<MyMap>("MyMap").first;
		if (mymap == NULL)
		{
			const ShmemAllocator alloc_inst(segment.get_segment_manager());

			mymap = segment.construct<MyMap>("MyMap") (std::less<int>(), alloc_inst);

		}

		Item v;
		for (int i = 0; i < 100; ++i) {
			v.id = i;
			mymap->insert(std::pair<const int, Item>(i, (Item)v));
		}

		for (MyMap::iterator it = mymap->begin(); it != mymap->end(); it++) {
			printf("%d ", it->second.id);
		}
		printf("\n");

		//file_mapping::remove("SharedMemory");
	}
	catch (const std::exception & e) {
		printf("Exception:%s\n", e.what());
		//file_mapping::remove("SharedMemory");
	}

	return 0;

}
