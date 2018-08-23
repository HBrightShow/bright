#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include "./keephand.hpp"

using namespace eosio;
  
void keephand::createacnt( const account_name& name, const std::string& pubkey) {

	eosio_assert(name > 0, "name empty \n");	
	eosio_assert(pubkey.size() > 0, "pubkey empty \n");
	
	//check auth
	require_auth(_self);
	user  actns(_self, _self);
	auto exist = actns.find(name);
	eosio_assert( exist == actns.end(), "name already exists\n");

	//modify data
	actns.emplace( _self, [&]( auto& n) {
	n.name = name;
	n.pubkey = pubkey;
	}); 

	create_asset_data_by_account(name);	
}

void keephand::deleteacnt(const account_name& name) {
	
 	eosio_assert(name > 0, "name empty \n");

	require_auth( _self );
	user acnts( _self, _self);	

	auto existing = acnts.find(name);  
	eosio_assert(existing != acnts.end(), "name not found \n");

	acnts.erase(existing);
	delete_asset_data_by_account(name);
}

void keephand:: create_asset_data_by_account(const account_name name) {

	asset ass(_self, _self);
	 ass.emplace(_self, [&](auto& d) {
             d.name = name;
             d.funds = 0;
         });

}
	
void keephand::modasset(const account_name& name, const double& funds, bool bsaving ) {
	
	eosio_assert( name  > 0, "name empty \n");
	eosio_assert( funds > 0, "funds input error");

	user acnts(_self, _self);

	auto existing_user = acnts.find(name);

	eosio_assert(existing_user != acnts.end(), "name not foud on userinfo");

	//check auth
	require_auth(name);	

	asset ass(_self, _self);
	auto existing_asset = ass.find(name);
	//print("keephand::modasset()  find name in asset: ", (existing_asset == ass.end() ? "false" : "true"));	
	eosio_assert(existing_asset != ass.end(), "name not fund on asset" );

			
	
	if(existing_asset != ass.end()) {
	
	double op_funds = 0.00000000;
	if(bsaving) {
		op_funds = existing_asset->funds + funds;
	} else {
		//is enough funds
		op_funds = existing_asset->funds - funds - funds * fee_rate;
		eosio_assert(op_funds >= 0, "not enough asset.\n");
		
		auto existing_owner = ass.find(_self);
		eosio_assert(existing_owner != ass.end(), " owner user not fund.\n");
	
		ass.modify(*existing_owner, _self , [&]( auto& d ) {
                  d.name = _self;
                  d.funds = funds* fee_rate;
                  });
          }

	
	
	ass.modify(*existing_asset, _self , [&]( auto& d ) {
     		d.name = name;
		d.funds = op_funds;
		});	
	}
}
void keephand::delete_asset_data_by_account(const account_name name)
{	
	asset ass(_self, _self);
 	auto existing = ass.find(name);
	
	if(existing != ass.end()) {
		ass.erase(existing);
		//print("delete asset name=", name);	
	}
}

void keephand::transfer(const account_name& from, const account_name& to, const double& funds, const std::string& strremark ) {
	eosio_assert( from  > 0, "from empty \n");
	eosio_assert( to > 0, "to empty \n");
	eosio_assert(funds > 0, "funds error \n");
	
	require_auth(from);

	eosio_assert(is_exist_account(to), "fund receiver user failed on userinfo table. \n");
	
	asset ass(_self, _self);

	auto existing_from = ass.find(from);
	eosio_assert(existing_from != ass.end(), "fund sender failed.\n");

	double fee = funds * fee_rate;
	if(fee < fee_min) {
		fee = fee_min;
	}
	double asset_sender = existing_from->funds - fee - funds;
	eosio_assert(asset_sender >= 0.00000000, " sender not enough asset.");

	funds_option(from, fee + funds, false);
	
	auto existing_to = ass.find(to);
	 eosio_assert(existing_to != ass.end(), "fund receiver failed on asset table\n");
	funds_option(to, funds, true );	

	auto existing_self = ass.find(_self);
         eosio_assert(existing_self != ass.end(), "fund self failed on asset table\n");
         funds_option(_self, fee, true );	
}

bool keephand::is_exist_account(const account_name name) {
	user acnts(_self, _self);
	auto idx = acnts.find(name);
	
	return idx != acnts.end();
}


void keephand::funds_option(const account_name& name, const double& funds, bool opt) {
	asset ass(_self, _self);
	auto idx = ass.find(name);
	eosio_assert(idx != ass.end(), "fund name failed. \n");

	double user_asset = 0.00000000;
	if(opt) {
		user_asset = idx->funds + funds;
	} else {
		user_asset = idx->funds - funds;
	}
	
	eosio_assert(user_asset >= 0, "asset not enought. \n");	

	modify_asset(name, user_asset);	
}

void keephand::modify_asset(const account_name& name, const double& funds) const {
	asset ass(_self, _self);
	auto idx = ass.find(name);
	eosio_assert(idx != ass.end(), "fund name failed on asset table. \n");

	ass.modify(*idx, _self , [&]( auto& a ) {
                    a.name = name;
                    a.funds = funds;
                  });
}




