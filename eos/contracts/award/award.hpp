#ifndef __AWARD_H__
#define __AWARD_H__

#include <eosiolib/eosio.hpp>

namespace eosio {

#define DEBUG_INFO 0


class awardeos : public eosio:: contract {
   
private:
	/// @abi table nodeinfo i64
           struct st_nodeinfo {
				//uint64_t  key;
                account_name name; 
				std::string pubkey;
				//public_key  pubkey;	
				
				account_name  get_name() const { return name; }
				uint64_t get_pubkey() const { return string_to_name(pubkey.data()); }
				//uint64_t get_pubkey()  { return string_to_name(std::string(pubkey)); }
				uint64_t  primary_key() const { return name; }
		
				EOSLIB_SERIALIZE(st_nodeinfo, (name)(pubkey))
          };

         // @abi table 
          typedef eosio::multi_index<N(nodeinfo), st_nodeinfo, indexed_by<N(name), const_mem_fun<st_nodeinfo, account_name, &st_nodeinfo::get_name>>,
							indexed_by<N(pubkey), const_mem_fun<st_nodeinfo, uint64_t, &st_nodeinfo::get_pubkey>> > node;
	
	   // @abi table deposit i64
           struct st_deposit {
		std::string nodepubkey;
		uint64_t	score;
		uint64_t	cpu;
		uint64_t	disk;
		uint64_t	net;
		
		st_deposit() {}
		st_deposit(const std::string& key, const uint64_t& s, const uint64_t& c, const uint64_t& d, const uint64_t& n):nodepubkey(key),score(s),cpu(c),disk(d),net(n) {}
		
		uint64_t  primary_key() const { return string_to_name(nodepubkey.data()); }	
		uint64_t get_nodepubkey() const { return string_to_name(nodepubkey.data() ); }		
		uint64_t get_score() const { return score; }
		uint64_t get_cpu() const { return cpu; }
		uint64_t get_disk() const { return disk; }	
		uint64_t get_net() const { return net; }

		EOSLIB_SERIALIZE(st_deposit, (nodepubkey)(score)(cpu)(disk)(net))
 	};
	
	// @abi table
	typedef eosio::multi_index<N(deposit), st_deposit,
				indexed_by<N(nodepubkey), const_mem_fun<st_deposit, uint64_t, &st_deposit::get_nodepubkey>>, 
				indexed_by<N(score), const_mem_fun<st_deposit, uint64_t, &st_deposit::get_score>>,
				indexed_by<N(cpu), const_mem_fun<st_deposit, uint64_t, &st_deposit::get_cpu>>,
				indexed_by<N(disk), const_mem_fun<st_deposit, uint64_t, &st_deposit::get_disk>>,
				indexed_by<N(net), const_mem_fun<st_deposit, uint64_t, &st_deposit::get_net>> 
			>  deposit;



   private:
		void create_deposit_data_by_account(const std::string& pubkey);
		void delete_deposit_data_by_account(const std::string& pubkey);

   public:
          awardeos(account_name self):contract(self) {}

          void hello(account_name user);
	// @abi action
          void createacnt( const account_name& name, const std::string& pubkey);
	
	// @abi action
	  void deleteacnt(const account_name& name, const std::string& pubkey); 
          
	// @abi action
	  void moddeposit(const std::string& pubkey, const uint64_t& score, const uint64_t& cpu, const uint64_t& disk, const uint64_t& net );
          
	// @abi action
	void issue();

#if DEBUG_INFO
	// @abi action 
	void deldeposit(const std::string& pubkey);
#endif

};





}

#if DEBUG_INFO
 EOSIO_ABI(eosio::awardeos, (hello)(createacnt)(deleteacnt)(moddeposit)(issue)(deldeposit))
#else
  EOSIO_ABI(eosio::awardeos, (hello)(createacnt)(deleteacnt)(moddeposit)(issue))
#endif















#endif
