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


enum enFileType{
	ENU_FILE = 1,
	ENU_DIR,
};

const uint32_t max_char_line = 65535;
uint8_t g_lineBuf[max_char_line];

typedef std::set<bfs::path> SetPath;

const std::string oldfield = "eos";
const std::string newfield = "uos";

const std::string oldfieldupper = "EOS";
const std::string newfieldupper = "UOS";


bool scanFilesUseRecursive(const std::string& rootPath, SetPath& fileSet);
bfs::path modifyName( bfs::path path);
bool modifyContent(bfs::path path);
char* modify(char* pBuf, uint64_t fsize);
void printPath(const SetPath& fileSet);

int main(int argc, char* argv[]) {

	std::string filePath;

	bpo::options_description opts("options");
	opts.add_options()
		("help", "help info")
		("dir", bpo::value<std::string>(), "the directory of need to parse");
		
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

	std::string root("\\eos");

	bfs::path curPath = bfs::current_path();
	std::cout << curPath << std::endl;

	curPath += root;

	std::cout << curPath << std::endl;

	SetPath fileSet;

	scanFilesUseRecursive(curPath.string(), fileSet);
	//printPath(fileSet);
	std::cout << fileSet.size() << std::endl;

	try
	{
		for (SetPath::reverse_iterator iter = fileSet.rbegin(); iter != fileSet.rend(); ++iter)
		{
			modifyContent(*iter);
		}
	}
	catch (const std::exception& e)
	{
		std::cout << std::string("modifyContent exception: ") + e.what() << std::endl;
	}

	try
	{
		for (SetPath::reverse_iterator iter = fileSet.rbegin(); iter != fileSet.rend(); ++iter)
		{
			modifyName(*iter);
		}
	}
	catch (const std::exception& e)
	{
		std::cout << std::string("modifyName exception: ") + e.what() << std::endl;
	}
	
	system("pause");

	return 0;
}


bool scanFilesUseRecursive(const std::string& rootPath, SetPath& fileSet) {

	bfs::path fullpath(rootPath, bfs::native);

	if (!bfs::exists(fullpath))
	{
		std::cout << std::string("scanFilesUseRecursive : file not exist! ") << rootPath.c_str() << std::endl;
		return false;
	}

	bfs::recursive_directory_iterator end_iter;

	try {
		for (bfs::recursive_directory_iterator iter(fullpath); iter != end_iter; iter++) {
			fileSet.insert(iter->path());
		}
	}
	catch(bfs::filesystem_error& e)
	{
		std::cout << e.what() << std::endl;
	}

	return true;
}

bfs::path modifyName(const bfs::path path)
{
	std::string name = path.filename().string();

	size_t pos = name.find(oldfield);
	if (pos != std::string::npos)
	{
		name[pos] = 'u';
	}

	pos = name.find(oldfieldupper);
	if (pos != std::string::npos)
	{
		name[pos] = 'U';
	}
	
	bfs::path newpath = path.parent_path() / name;

	bfs::rename(path, newpath);

	return newpath;
}

void printPath(const SetPath& fileSet)
{
	for (SetPath::iterator iter = fileSet.begin(); iter != fileSet.end(); ++iter)
	{
		std::cout << iter->string().c_str() << std::endl;
	}
}

bool modifyContent(bfs::path path) {

	if (bfs::is_directory(path)) {
		return true;
	}

	std::fstream file;
	uint64_t fsize;
	char* pBuf = NULL;
	try
	{
		fsize = bfs::file_size(path);
		if (fsize == 0)
		{
			return true;
		}

		file.open(path.string().c_str(), std::ios::in | std::ios::out | std::ios::binary);
		if (!file.is_open())
		{
			std::cout << "modifyContent() open file failed! path: " << path.string().data() << std::endl;
			return false;
		}

		char* pBuf = new char[fsize];
		memset(pBuf, 0, fsize);
		file.read(pBuf, fsize);
		char* pHead = modify(pBuf, fsize);

		file.seekg(std::ios::beg);
		file.write(pHead, fsize);

		file.close();
		delete pBuf;
		pBuf = NULL;
	}
	catch (const std::exception&)
	{
		delete pBuf;
		file.close();
		std::cout << "modifyContent() exception! path: " << path.string().data() << std::endl;
	}

	return true;
}

char* modify(char* pBuf, uint64_t fsize)
{
	char* ret = pBuf;
	uint64_t nCount = 0;

	while (nCount < fsize)
	{
		if (memcmp(pBuf, oldfield.c_str(), oldfield.length()) == 0)
		{
			*pBuf = 'u';
		}

		if (memcmp(pBuf, oldfieldupper.c_str(), oldfieldupper.length()) == 0)
		{
			*pBuf = 'U';
		}

		++nCount;
		++pBuf;
	}

	return ret;
}
