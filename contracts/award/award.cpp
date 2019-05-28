#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include "./award.hpp"

using namespace eosio;



void awardeos::hello(account_name user) {
	print("Welcome to EOS! ", name{user});
}
  
void awardeos::createacnt( const account_name& name, const std::string& pubkey) {
	print("awardeos::modquali() ++++\n");

	eosio_assert(name > 0, "name empty \n");	
	eosio_assert(pubkey.size() > 0, "pubkey empty \n");
	
	//check auth
	require_auth(_self);
	node  actns(_self, _self);
	auto exist = actns.find(name);
	eosio_assert( exist == actns.end(), "name already exists\n");

	//modify data
	actns.emplace( _self, [&]( auto& n) {
	//n.key = actns.available_primary_key();
	n.name = name;
	n.pubkey = pubkey;
	}); 

	create_deposit_data_by_account(pubkey);	
}

void awardeos::deleteacnt(const account_name& name, const std::string& pubkey) {
	
 	eosio_assert(name > 0, "name empty \n");
    eosio_assert(pubkey.size() > 0, "pubkey empty \n");

	require_auth( _self );
	node actns( _self, _self);	
	
	auto idx= actns.get_index<N(name)>();
	
	for(auto it = idx.begin(); it != idx.end(); ++it ) {
		print("nodename:", it->name, " pubkey:", it->pubkey.data(), "\n");
	}

	auto existing = idx.find(name);  
	eosio_assert(existing != idx.end(), "name not found \n");
	eosio_assert(existing->pubkey == pubkey, "pubkey wrong \n");

	idx.erase(existing);
	delete_deposit_data_by_account(pubkey);
}

void awardeos:: create_deposit_data_by_account(const std::string& pubkey) {

	deposit sit(_self, _self);
	 sit.emplace(_self, [&](auto& d) {
             d.nodepubkey = pubkey;
             d.score = 0;
             d.cpu = 0;
             d.disk = 0;
             d.net = 0;
         });

}
	
void awardeos::moddeposit(const std::string& pubkey, const uint64_t& score, const uint64_t& cpu, const uint64_t& disk, const uint64_t& net ) {
	print("awardeos::moddeposit()  ", "pubkey=", pubkey, "score=", score, ",cpu=", cpu, ",disk=", disk, ",net=", net, "\n");	
	
	eosio_assert(pubkey.size() > 0, "pubkey empty \n");

	//require_auth(_self);
	
	node acnts(_self, _self);
	auto idx_node_pubkey = acnts.get_index<N(pubkey)>();

#if DEBUG_INFO
	for(auto it = idx_node_pubkey.begin(); it != idx_node_pubkey.end(); ++it ) {
                  print("\nnodename:", it->name, " pubkey:", it->pubkey.data(), "\n");
          }
	print("pubkey: ", string_to_name(pubkey.data()));
#endif

	auto exist_node_pubkey = idx_node_pubkey.find(string_to_name(pubkey.data()));
	//auto exist_node_pubkey = idx_node_pubkey.find(pubkey);

	eosio_assert(exist_node_pubkey != idx_node_pubkey.end(), "pubkey not foud on accounts");

	//check auth
	require_auth(exist_node_pubkey->name);	





 
	deposit sit(_self, _self);
	auto idx_deposit_pubkey = sit.get_index<N(nodepubkey)>();
	auto exist_deposit_pubkey = idx_deposit_pubkey.find(string_to_name(pubkey.data()));
		print("awardeos::moddeposit()  find pubkey in deposit: ", (exist_deposit_pubkey == idx_deposit_pubkey.end() ? "false" : "true"));	
	
	if(exist_deposit_pubkey != idx_deposit_pubkey.end()) {
	
		sit.modify(*exist_deposit_pubkey, _self , [&]( auto& d ) {
     	//d.nodepubkey = pubkey;
		d.score = score;
        d.cpu = cpu;
        d.disk = disk;
        d.net = net;	
		});	
	}
}
void awardeos::delete_deposit_data_by_account(const std::string& pubkey)
{
	print("awardeos::delete_deposit_data_by_account() ++++");
	deposit sit(_self, _self);
	auto idx_deposit_pubkey = sit.get_index<N(nodepubkey)>();
 	auto exist_deposit_pubkey = idx_deposit_pubkey.find(string_to_name(pubkey.data()));
	if(exist_deposit_pubkey != idx_deposit_pubkey.end()) {
		idx_deposit_pubkey.erase(exist_deposit_pubkey);
		print("delete deposit data: pubkey=", pubkey.data());	
	}
}

void awardeos::issue() {
	print("awardeos::issue() ++++");
			
}

#if DEBUG_INFO

void awardeos::deldeposit(const std::string& pubkey) {

	delete_deposit_data_by_account(pubkey);
}

#endif
