#include<iostream>
#include<boost/filesystem.hpp>
#include<boost/filesystem/path.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/program_options.hpp>
#include<cstring>
#include<vector>
#include<boost/thread.hpp>
#include<boost/container/flat_map.hpp>
#include<algorithm>


namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

void CountWords(std::vector<std::string>& vc);
const std::vector<std::string>& scanFilesUseRecursive(const std::string& rootPath, std::vector<std::string>& vc);
void parseBuf(const char* buf, boost::container::flat_map<std::string, uint32_t>& wordmap);

int main(int argc, char* argv[]) {

	std::string filePath;

	bpo::options_description opts("options");
	opts.add_options()
		("help", "help info")
		("dir", bpo::value<std::string>(),"the directory of need to parse");

	bpo::variables_map vm;
	try {
		bpo::store(parse_command_line(argc, argv, opts), vm);
	}
	catch (bpo::error_with_no_option_name &ex) {
		std::cout << ex.what() << std::endl;
	}

	bpo::notify(vm);

	

	if (vm.count("help"))
	{
		std::cout << opts << std::endl;
	}

	if (vm.count("dir")) 
	{
		filePath = vm["dir"].as<std::string>();
	}

	std::cout << filePath << std::endl;
	
	
	
	std::vector<std::string> fileList;

	scanFilesUseRecursive(filePath, fileList);

	boost::thread th(CountWords, fileList);

	th.join();

	return 0;
}

void CountWords(std::vector<std::string>& vc)
{
	boost::container::flat_map<std::string, uint32_t> wordmap;
	uint32_t fsize;
	bfs::fstream fs;

	auto readfile = [&](std::string strPath, char* pBuf) {
		try {
			fs.open(strPath);
			fs.read(pBuf, fsize);
		}
		catch (bfs::filesystem_error& ec)
		{
			std::cout << "exception:"<< ec.what() << std::endl;
			fs.close();
			return ;
		}
	
		fs.close();

		//std::cout << std::string(&buf[0]) << std::endl;
	};



	for (auto item : vc) {
		
		fsize = bfs::file_size(item);

		if (fsize <= 0)
			continue;

		char* pBuf = new char[fsize];
		memset(pBuf, 0, fsize);

		readfile(item, pBuf);
		parseBuf(pBuf, wordmap);

		delete[] pBuf;
	
	}
	std::cout << "444" << std::endl;
	for (auto item : wordmap) {
	
		std::cout << item.first << ": " << item.second << std::endl;
	}

	
	std::cout << "finished!" << std::endl;
	return ;
}


void parseBuf(const char* buf, boost::container::flat_map<std::string, uint32_t>& wordmap) {


	std::vector<char> wc;

	auto isVisable = [&](const char ch) {
		if (('0' <= ch && ch <= '9') ||
			('a' <= ch && ch <= 'z') ||
			('A' <= ch && ch <= 'Z') 
			) {
			return true;
		}

		return false;
	};

	std::string strkey;


	while(*buf++ != '\0') {
	
		char ch = *buf;
		bool bVisable = isVisable(ch);
		if ( !bVisable && wc.size() != 0)
		{
			strkey = std::string(&wc[0], wc.size());
			boost::container::flat_map<std::string, uint32_t>::iterator it = wordmap.find(strkey);

			if(it == wordmap.end())
			{
				wordmap[strkey] = 1;
			}
			else
			{
				wordmap[strkey] = wordmap[strkey] + 1;
			}
			
			wc.clear();
		}
		else if(bVisable)
		{
			wc.push_back(ch);
		}

	}

}

const std::vector<std::string>& scanFilesUseRecursive(const std::string& rootPath , std::vector<std::string>& vc) {
	
	bfs::path fullpath(rootPath, bfs::native);

	if (!bfs::exists(fullpath)) { return vc; }
	bfs::recursive_directory_iterator end_iter;
	for (bfs::recursive_directory_iterator iter(fullpath); iter != end_iter; iter++) {
		try {
			if (bfs::is_directory(*iter)) {
				//std::cout << *iter << "is dir" << std::endl;
				vc.push_back(iter->path().string());
				scanFilesUseRecursive(iter->path().string(),vc);
			}
			else {
				vc.push_back(iter->path().string());
				//std::cout << *iter << " is a file" << std::endl;
			}
		}
		catch (const std::exception & ex) {
			std::cerr << ex.what() << std::endl;
			continue;
		}
	}
	return vc;
}
