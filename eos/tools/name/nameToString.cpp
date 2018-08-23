#include<iostream>
#include<stdint.h>
#include<sstream>
#include <boost/algorithm/string.hpp>

//using namespace std;


const char* name_to_string(uint64_t name) {
     static const char* charmap = ".12345abcdefghijklmnopqrstuvwxyz";

      std::string str(13,'.');

      uint64_t tmp = name;
      for( uint32_t i = 0; i <= 12; ++i ) {
         char c = charmap[tmp & (i == 0 ? 0x0f : 0x1f)];
         str[12-i] = c;
         tmp >>= (i == 0 ? 4 : 5);
      }

      boost::algorithm::trim_right_if( str, []( char c ){ return c == '.'; } );

	std::cout << str.data() << std::endl;
      return str.data();
   }




int main(int argc, char* argv[]) {

	if (argc < 2)
		return 0;

	uint64_t val;
	std::stringstream ds;
	ds << argv[1];
	ds >> val;
	std::cout << val << std::endl;
	name_to_string(val);

	return 0;
}
