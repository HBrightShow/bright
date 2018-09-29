#ifndef __AWARD_H__
#define __AWARD_H__

#include <eosiolib/eosio.hpp>

namespace eosio {



class keephand : public eosio:: contract {
   
private:
	/// @abi table userinfo i64
           struct st_userinfo {
                		account_name name; 
				std::string pubkey;
				
				uint64_t get_pubkey() const   { return string_to_name(pubkey.data()); }
				uint64_t  primary_key() const { return name; }
		
				EOSLIB_SERIALIZE(st_userinfo, (name)(pubkey))
          };

         // @abi table 
          typedef eosio::multi_index<N(userinfo), st_userinfo, indexed_by<N(pubkey), const_mem_fun<st_userinfo, uint64_t, &st_userinfo::get_pubkey>> > user;
	
	   // @abi table userasset i64
           struct st_asset {
				account_name    name;
				double 	funds;
		
				uint64_t primary_key() const { return name; }
				//uint64_t get_funds() const { return funds; }		

				EOSLIB_SERIALIZE(st_asset, (name)(funds))
 	};
	
	// @abi table
	typedef eosio::multi_index<N(userasset), st_asset/*, indexed_by<N(funds), const_mem_fun<st_asset, uint64_t, &st_asset::get_funds>>*/ >  asset;
   private:
	static constexpr  double fee_rate = 0.01000000;   // rate
	static constexpr  double fee_min = 0.01000000;	// the smallest funds of take money once time

	enum en_fee_type {
		EN_TAKE = 1,	// take money
		EN_TRANSFER,
	};

   private:
		void create_asset_data_by_account(const account_name name);
		void delete_asset_data_by_account(const account_name name);

		bool is_exist_account(const account_name name);
		void funds_option(const account_name& name, const double& funds, bool opt = false);  // option:  true -- add     false--sub
		void modify_asset(const account_name& name, const double& funds) const;

   public:
          keephand(account_name self):contract(self) {}

	// @abi action
          void createacnt( const account_name& name, const std::string& pubkey);
	
	// @abi action
	  void deleteacnt(const account_name& name); 
          
	// @abi action
	  void modasset(const account_name& name, const double& funds, bool bsaving = false );

	// @abi action	
	void transfer(const account_name& from, const account_name& to, const double& funds, const std::string& str );
	
};

}

  EOSIO_ABI(eosio::keephand, (createacnt)(deleteacnt)(modasset) (transfer) )

#endif
