#include<iostream>
#include<boost/filesystem.hpp>
#include<fstream>
#include<boost/tokenizer.hpp>
#include<map>
#include<set>
#include<vector>
#include<boost/lexical_cast.hpp>

namespace bfs = boost::filesystem;

const std::string snapfilepath = "./snapshot.csv";
//const std::string snapfilepath = "./test.csv";

const std::string errfilepath  = "./error.csv";
const uint32_t max_line_size = 256;


uint64_t g_failload = 0;
 std::vector<uint64_t> g_failitem; 



struct stuser{
    std::string account;
    std::string pubkey;
    std::string quantity; 
};

typedef std::map<std::string, stuser> mapuser;
typedef std::set<std::string> setusererror;


 mapuser g_mapuser;
 setusererror seterr;

bool parse(const std::string& strline);
bool checkinfo(const std::string& ulordaddr, const struct stuser& usr);
void run();
bool savefile();
void  printfailitem();


int main() {
    bfs::path path(snapfilepath);

    
    std::fstream snapfile;

    snapfile.open(snapfilepath.c_str());
    if (!snapfile.is_open()) {
        std::cout << "open error!" << std::endl;
         return -1;
    }

    std::string strline;

    while (getline(snapfile, strline)) {

        //std::cout << strline << std::endl;
        if( parse(strline))
        {
            continue;
        }

        

    }
    
    snapfile.close();
   
    run();
    savefile();
     printfailitem();    



	return 0;
}

void  printfailitem()
{ 
    std::cout << "load failed  item: " << std::endl;
    for(std::vector<uint64_t>::iterator iter = g_failitem.begin(); iter != g_failitem.end(); ++iter)
    {
        std::cout << *iter << std::endl;
    }

}

bool savefile(){

   std::fstream errlog(errfilepath.c_str(), std::ios::app);
    
    
    std::cout << "error item: " << seterr.size() << std::endl;

   for(setusererror::iterator it =  seterr.begin(); it != seterr.end(); ++it) {
        errlog << it->c_str();        

    } 

    errlog.close();

    return true;
}

bool checkinfo(const std::string& ulordaddr, const struct stuser& usr)
{
    ++g_failload;
#if 1
    if(usr.account.size() < 12 )
    {
        g_failitem.push_back(g_failload);
        return false;
    }
#endif

    return true;
}


bool parse(const std::string& strline)
{
    if(strline.empty()){
        return false;
    }   
    
    struct stuser usr;
    std::string ulordaddr;    
    int8_t calfiled = 1;
    const uint8_t maxfiled = 4;

    boost::tokenizer<boost::escaped_list_separator<char> > tokens(strline);
    for (boost::tokenizer<boost::escaped_list_separator<char> >::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
       
        switch(calfiled)
        {
            case 1:{
                ulordaddr =  *tok_iter;
                break;
            }
            
           case 2:{ 
                usr.account =  *tok_iter;
                break;
            }  

            case 3:{ 
                usr.pubkey =  *tok_iter;
                break;
            }  


            case 4:{ 
                usr.quantity =  *tok_iter;
                size_t pos = usr.quantity.find('\r');
               if(pos != std::string::npos)
                {
                    usr.quantity[pos] = ' ';
                }
                break;
            }  
            
            default:
                break; 
        }

       ++ calfiled; 
 #if   0   
       if( ++calfiled > maxfiled){
                return false;
         }
#endif
         //std::cout   <<  *tok_iter ;
         //std::cout << std::endl;
    }
       if( checkinfo(ulordaddr, usr)) {
            g_mapuser[ulordaddr] = usr;          
        }else {
            seterr.insert(strline);

        }


    //std::cout << std::endl;

  
}

void run()
{
    std::string log = "  >> info.log ";
    std::string head = "cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.141:8888 ";
    std::string create =  " system newaccount --transfer eosio ";
    std::string strtail = " --stake-net \"200.0000 UOS\" --stake-cpu \"200.0000 UOS\" --buy-ram \"200.0000 UOS\""; 
     
    std::string transfer = " transfer eosio ";
    std::string cmd;
    
    double assetall = 0;

    for(mapuser::iterator iter = g_mapuser.begin(); iter != g_mapuser.end(); ++iter) {
        cmd.clear();
        cmd = head + create + iter->second.account + " " + iter->second.pubkey + strtail + log;
        
        std::cout << cmd.c_str() << std::endl;
   
        try
        {
            double asset = boost::lexical_cast<double>(iter->second.quantity);
            assetall += asset;
        }
        catch(boost::bad_lexical_cast& e)
        {
            std::cout << "create error!" << std::endl;
           // continue;
        }     
    
        system(cmd.c_str());
        
        usleep(20);

        cmd.clear();
        cmd = head + transfer + iter->second.account + " \"" + iter->second.quantity + " UOS \"";
        std::cout << cmd.c_str() << std::endl;
        system(cmd.c_str());

        
        
    }
    std::cout << "assetall: " << assetall << std::endl;
    std::cout << "account item: " << g_mapuser.size() << std::endl;


}
