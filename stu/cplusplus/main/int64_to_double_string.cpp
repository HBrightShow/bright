#include<iostream>
#include <sstream>
#include<vector>
#include <iomanip>

using namespace std;

struct vote_producer {
	string  producer;
	int64_t voted;

};

typedef std::vector<vote_producer>::iterator iterVoteProducer;

typedef std::vector<vote_producer>::iterator iterVoteProducer;


inline void int64_to_double_string(const int64_t amount, std::string& result);


int main() {

#if 0
	std::string strTemp;
	int64_t  arr[] = { 0, 8, 88, 888, 8888, 88888 };

	for (int i = 0; i < sizeof(arr) / sizeof(int64_t); ++i) {
		int64_to_double_string(arr[i],strTemp);
		std::cout << strTemp << std::endl;
	}
#endif


	constexpr size_t indent_size = 5;
	const string indent(indent_size, ' ');


	std::vector<vote_producer> voteproducer;
	int64_t totol_voted = 0;
	int64_t voted = 0;
	string strVoted;
	std::string strTemp;

	string strDan = "UOS";



	vote_producer vpr;
	vpr.producer = "hml";
	vpr.voted = 502.021;
	voteproducer.push_back(vpr);

	vpr.producer = "lx";
	vpr.voted = 1201452;
	voteproducer.push_back(vpr);

	vpr.producer = "bright";
	vpr.voted = 20;
	voteproducer.push_back(vpr);



	for (auto vp : voteproducer) {
		voted = vp.voted;
		totol_voted += voted;
		int64_to_double_string(voted, strTemp);

		std::cout << indent << std::left <<std::setw(11)
			<< vp.producer << std::left << ": " << std::right << std::setw(24)
			<< strTemp + " " + strDan << std::endl;
	}

	return 0;
}


inline void int64_to_double_string(const int64_t amount, std::string& result) {
	result.clear();
	const int64_t base = 10000;
	int64_t left = amount / base;
	int64_t right = amount % base;
	std::stringstream ss;

	if (left > 0) {
		ss << left << ".";
	}
	else
	{
		ss << "0.";
	}


	uint8_t bit = 0;
	for (int64_t temp = right; temp > 0; temp = temp / 10) {
		++bit;
	}

	switch (bit) {
	case 0: {
		ss << "0000";
		break;
	}
	case 1: {
		ss << "000" << right;
		break;
	}
	case 2: {
		ss << "00" << right;
		break;
	}
	case 3: {
		ss << "0" << right;
		break;
	}
	case 4: {
		ss << right;
		break;
	}
	default: {
		
		break;
	}	
	}
	result = ss.str();
}