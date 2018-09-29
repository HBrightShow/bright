/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include "eosio.system.hpp"

#include <eosiolib/eosio.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/print.hpp>
#include <eosiolib/datastream.hpp>
#include <eosiolib/serialize.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosiolib/privileged.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>
#include <eosio.token/eosio.token.hpp>

#include <algorithm>
#include <cmath>

namespace eosiosystem {
   using eosio::indexed_by;
   using eosio::const_mem_fun;
   using eosio::bytes;
   using eosio::print;
   using eosio::singleton;
   using eosio::transaction;

   #define DEBUG_VOTING_ON 1

   /**
    *  This method will create a producer_config and producer_info object for 'producer'
    *
    *  @pre producer is not already registered
    *  @pre producer to register is an account
    *  @pre authority of producer to register
    *
    */
   void system_contract::regproducer( const account_name producer, const eosio::public_key& producer_key, const std::string& url, uint16_t location ) {
      eosio_assert( url.size() < 512, "url too long" );
      eosio_assert( producer_key != eosio::public_key(), "public key should not be the default value" );
      require_auth( producer );

      auto prod = _producers.find( producer );

      if ( prod != _producers.end() ) {
         _producers.modify( prod, producer, [&]( producer_info& info ){
               info.producer_key = producer_key;
               info.is_active    = true;
               info.url          = url;
               info.location     = location;
            });
      } else {
         _producers.emplace( producer, [&]( producer_info& info ){
               info.owner         = producer;
               info.total_votes   = 0;
               info.producer_key  = producer_key;
               info.is_active     = true;
               info.url           = url;
               info.location      = location;
         });
      }
   }

   void system_contract::unregprod( const account_name producer ) {
      require_auth( producer );

      const auto& prod = _producers.get( producer, "producer not found" );

      _producers.modify( prod, 0, [&]( producer_info& info ){
            info.deactivate();
      });
   }

   void system_contract::update_elected_producers( block_timestamp block_time ) {
      _gstate.last_producer_schedule_update = block_time;

      auto idx = _producers.get_index<N(prototalvote)>();

      std::vector< std::pair<eosio::producer_key,uint16_t> > top_producers;
      top_producers.reserve(21);

      for ( auto it = idx.cbegin(); it != idx.cend() && top_producers.size() < 21 && 0 < it->total_votes && it->active(); ++it ) {
         top_producers.emplace_back( std::pair<eosio::producer_key,uint16_t>({{it->owner, it->producer_key}, it->location}) );
      }

      if ( top_producers.size() < _gstate.last_producer_schedule_size ) {
         return;
      }

      /// sort by producer name
      std::sort( top_producers.begin(), top_producers.end() );

      std::vector<eosio::producer_key> producers;

      producers.reserve(top_producers.size());
      for( const auto& item : top_producers )
         producers.push_back(item.first);

      bytes packed_schedule = pack(producers);

      if( set_proposed_producers( packed_schedule.data(),  packed_schedule.size() ) >= 0 ) {
         _gstate.last_producer_schedule_size = static_cast<decltype(_gstate.last_producer_schedule_size)>( top_producers.size() );
      }
   }

   double stake2vote( int64_t staked ) {
      /// TODO subtract 2080 brings the large numbers closer to this decade
      double weight = int64_t( (now() - (block_timestamp::block_timestamp_epoch / 1000)) / (seconds_per_day * 7) )  / double( 52 );
      return double(staked) * std::pow( 2, weight );
   }
   /**
    *  @pre producers must be sorted from lowest to highest and must be registered and active
    *  @pre if proxy is set then no producers can be voted for
    *  @pre if proxy is set then proxy account must exist and be registered as a proxy
    *  @pre every listed producer or proxy must have been previously registered
    *  @pre voter must authorize this action
    *  @pre voter must have previously staked some EOS for voting
    *  @pre voter->staked must be up to date
    *
    *  @post every producer previously voted for will have vote reduced by previous vote weight
    *  @post every producer newly voted for will have vote increased by new vote amount
    *  @post prior proxy will proxied_vote_weight decremented by previous vote weight
    *  @post new proxy will proxied_vote_weight incremented by new vote weight
    *
    *  If voting for a proxy, the producer votes will not change until the proxy updates their own vote.
    */
   void system_contract::voteproducer( const account_name voter_name, asset quantity, const account_name proxy, const std::vector<account_name>& producers ) {
      require_auth( voter_name );
      update_votes( voter_name, quantity, proxy, producers, true );
   }

	//Begin: modify by UOS(hml) Date:2018-09-21
	//attention: Here is not deal with the case of  proxy, if add furture, the funtion need deal it.
   void system_contract::cancelvote( const account_name voter_name, asset quantity, const std::vector<account_name>& producers ) {
      require_auth( voter_name );

	 auto voter = _voters.find(voter_name);
     eosio_assert( voter != _voters.end(), "user have not voted before" );
	 eosio_assert(quantity.amount > 100, "min cancel vote is 0.01");
	 eosio_assert(producers.size() > 0 && producers.size() < max_voter_producers, "None of cancel vote producer.");

#if DEBUG_VOTING_ON
	print("before cancelvote, map_vote_producer","\n");
	
	 for(auto vp : voter->vote_producers) {
		 print("prudecer: ", vp.producer, " voted: ", vp.voted.amount, "\n");
	 }
#endif
 
	 for( size_t i = 1; i < producers.size(); ++i ) {
        eosio_assert( producers[i-1] < producers[i], "producer votes must be unique and sorted" );
     }

	 bool isFind;
	
	 for(auto p : producers) {
	 	eosio_assert( voter->vote_producers.size() > 0 , "not exist in voted producer list.");
		
		isFind = false;
		for(auto vp = voter->vote_producers.begin(); vp != voter->vote_producers.end();) {
			if(vp->producer == p) {
				eosio_assert(vp->voted.amount - quantity.amount >= 0, "amount of cancel vote is greater than voted");

				isFind = true;

#if DEBUG_VOTING_ON
				print("voted_producer->second=",vp->voted.amount," quantity.amount=",quantity.amount, "\n" );
#endif
				if(vp->voted.amount - quantity.amount >= 0) {
					 _voters.modify( voter, 0, [&]( auto& av ) {
					 		bool isfind = false;
					 		for(auto it = av.vote_producers.begin(); it != av.vote_producers.end(); ++it) {
								if(p == it->producer) {
									isfind = true;
									it->voted.amount -= quantity.amount;
								}
							} 	
							eosio_assert( isfind, "not exist in voted producer list.");
					 	});

					
					auto list_producer = _producers.find(p);
					eosio_assert( list_producer != _producers.end(), "not exist in voted producer list.");
					
					//auto new_vote_weight = stake2vote( quantity.amount );
					auto new_vote_weight =  quantity.amount;
#if DEBUG_VOTING_ON
					print("new_vote_weight=",new_vote_weight," list_producer->total_votes=",list_producer->total_votes,"\n" );
#endif

					eosio_assert(list_producer->total_votes - new_vote_weight >= 0, "amount of cancel voted is exceed.");

					_producers.modify( list_producer, 0, [&]( auto& producer ) {
			               producer.total_votes -= new_vote_weight;
			               
			               _gstate.total_producer_vote_weight -= new_vote_weight;
			               
	           		 });		
				}


				if(vp->voted.amount  == 0) {
					 _voters.modify( voter, 0, [&]( auto& av ) {
	         			
	         				vp = av.vote_producers.erase(vp);	
					 	});
				}
				
				break;
				
			}
			else {
					++vp;
			}
	 	}


		if(!isFind) {
			eosio_assert(false, "The cancel vote list include not existed on voted producer list.");
		}
	 }

	 
#if DEBUG_VOTING_ON
		print("End cancelvote, map_vote_producer","\n");
		for(auto vp : voter->vote_producers) {
			print("prudecer: ", vp.producer, " voted: ", vp.voted.amount, "\n");
		}
#endif	

   }
   //End: modify by UOS(hml) Date:2018-09-21


void system_contract::validate_votes(const account_name voter_name, asset quantity) {

		print(" voter_name= ",name{voter_name}, " quantity=", quantity.amount, "\n");
		
		auto voter = _voters.find(voter_name);
		eosio_assert( voter != _voters.end(), "user must stake before they can vote" );

		asset total_voted(0);
		
	  	for(auto vp = voter->vote_producers.begin(); vp != voter->vote_producers.end(); ++vp) 
		{
			total_voted += vp->voted;
		}

		print(" voter->staked ", voter->staked, " total_voted.amount =", total_voted.amount, "quantity.amount =", quantity.amount, "\n");
		
		eosio_assert( voter->staked - total_voted.amount /*+ quantity.amount*/ >= 0, "user must cancel vote producer before undelegate." );
   }

   void system_contract::update_votes( const account_name voter_name,  asset quantity, const account_name proxy, const std::vector<account_name>& producers, bool voting ) {

		 print(" voter_name= ",name{voter_name}, " quantity=", quantity.amount, " proxy = ", name{proxy}, " voting =", voting, "\n");
		 for(auto p : producers) {
			print("update_votes() producers:", p, "\n");
		 }
		 

	//Begin: modify by UOS(hml) Date:2018-09-20

		//cancel the interface of proxy by RPC .
		eosio_assert(proxy == 0, "not support proxy vote.");
		

	  //validate input
      auto voter = _voters.find(voter_name);
      eosio_assert( voter != _voters.end(), "user must stake before they can vote" ); /// staking creates voter object
      eosio_assert( producers.size() + voter->vote_producers.size() <= max_voter_producers, "attempt to vote for too many producers" );

      /**
       * The first time someone votes we calculate and set last_vote_weight, since they cannot unstake until
       * after total_activated_stake hits threshold, we can use last_vote_weight to determine that this is
       * their first vote and should consider their stake activated.
       */

	  auto new_vote_weight = quantity.amount  * producers.size();
	  _gstate.total_activated_stake +=	new_vote_weight;
	  
	  if( _gstate.total_activated_stake >= min_activated_stake && _gstate.thresh_activated_stake_time == 0 ) {
            _gstate.thresh_activated_stake_time = current_time();
         }
	  
	  
	  if(voting ) {
		int64_t total_voted = 0;
	  	for(auto vp = voter->vote_producers.begin(); vp != voter->vote_producers.end(); ++vp) 
		{
			total_voted += vp->voted.amount;
		}
		
	  	 print(" staked =", voter->staked, "total_voted=", total_voted, " amount = ", quantity.amount, "size=",voter->vote_producers.size(),"\n");
		eosio_assert(voter->staked - (total_voted + new_vote_weight) >= 0, "not enough stake asset.");
	  }

      
	  //auto new_vote_weight = stake2vote( quantity.amount );

      boost::container::flat_map<account_name, pair<double, bool /*new*/> > producer_deltas;

	  if ( voter->last_vote_weight > 0 ){
		for( const auto& p : voter->vote_producers ) {
           auto& d = producer_deltas[p.producer];
           d.first -= voter->last_vote_weight;
           d.second = false;
        }

	  }
     
     if( new_vote_weight >= 0 ) {
        for( const auto& p : producers ) {
           auto& d = producer_deltas[p];
           d.first += new_vote_weight;
           d.second = true;
        }
     }

      for( const auto& pd : producer_deltas ) {
         auto pitr = _producers.find( pd.first );
         if( pitr != _producers.end() ) {
            eosio_assert( !voting || pitr->active() || !pd.second.second /* not from new set */, "producer is not currently registered" );
            _producers.modify( pitr, 0, [&]( auto& p ) {
               p.total_votes += pd.second.first;
               if ( p.total_votes < 0 ) { // floating point arithmetics can give small negative numbers
                  p.total_votes = 0;
               }
               _gstate.total_producer_vote_weight += pd.second.first;
               //eosio_assert( p.total_votes >= 0, "something bad happened" );
            });
         } else {
            eosio_assert( !pd.second.second /* not from new set */, "producer is not registered" ); //data corruption
         }
      }


	vote_producer newvp;
	bool isFind = false;
	
	for(auto p : producers) {
		if(voter->vote_producers.size() > 0) {
			isFind = false;
		
			_voters.modify( voter, 0, [&]( auto& av ) {
				for(auto vp = av.vote_producers.begin(); vp != av.vote_producers.end(); ++vp) {
#if DEBUG_VOTING_ON
					 print("p: ", p, " vp.producer: ", vp->producer, "\n");
#endif
					if(vp->producer == p) {
						vp->voted.amount += quantity.amount;
						isFind = true;
						break;
						
					}
				}

				if(!isFind) {
					newvp.producer = p;
					newvp.voted = quantity;
					av.vote_producers.push_back(newvp);
				}
				
			});	
		}
		else {
			newvp.producer = p;
			newvp.voted = quantity;
			_voters.modify( voter, 0, [&]( auto& av ) {
				av.vote_producers.push_back(newvp);
				
				});
		}
	}




#if DEBUG_VOTING_ON	  
	  print("End update_votes, vote_producers:","\n");
	  for(auto vp : voter->vote_producers) {
		 print("prudecer: ", vp.producer, " voted: ", vp.voted.amount, "\n");
	 }
#endif

	  //End: Add by UOS(hml) Date:2018-09-20

   }

   /**
    *  An account marked as a proxy can vote with the weight of other accounts which
    *  have selected it as a proxy. Other accounts must refresh their voteproducer to
    *  update the proxy's weight.
    *
    *  @param isproxy - true if proxy wishes to vote on behalf of others, false otherwise
    *  @pre proxy must have something staked (existing row in voters table)
    *  @pre new state must be different than current state
    */
   void system_contract::regproxy( const account_name proxy, bool isproxy ) {
      require_auth( proxy );

      auto pitr = _voters.find(proxy);
      if ( pitr != _voters.end() ) {
         eosio_assert( isproxy != pitr->is_proxy, "action has no effect" );
         eosio_assert( !isproxy || !pitr->proxy, "account that uses a proxy is not allowed to become a proxy" );
         _voters.modify( pitr, 0, [&]( auto& p ) {
               p.is_proxy = isproxy;
            });
         propagate_weight_change( *pitr );
      } else {
         _voters.emplace( proxy, [&]( auto& p ) {
               p.owner  = proxy;
               p.is_proxy = isproxy;
            });
      }
   }

   void system_contract::propagate_weight_change( const voter_info& voter ) {
      eosio_assert( voter.proxy == 0 || !voter.is_proxy, "account registered as a proxy is not allowed to use a proxy" );

	//Begin: removed by UOS(hml) Date:2018-09-20
#if 0
	  double new_weight = stake2vote( voter.staked );
      if ( voter.is_proxy ) {
         new_weight += voter.proxied_vote_weight;
      }

      /// don't propagate small changes (1 ~= epsilon)
      if ( fabs( new_weight - voter.last_vote_weight ) > 1 )  {
         if ( voter.proxy ) {
            auto& proxy = _voters.get( voter.proxy, "proxy not found" ); //data corruption
            _voters.modify( proxy, 0, [&]( auto& p ) {
                  p.proxied_vote_weight += new_weight - voter.last_vote_weight;
               }
            );
            propagate_weight_change( proxy );
         } else {
            auto delta = new_weight - voter.last_vote_weight;
            for ( auto acnt : voter.producers ) {
               auto& pitr = _producers.get( acnt, "producer not found" ); //data corruption
               _producers.modify( pitr, 0, [&]( auto& p ) {
                     p.total_votes += delta;
                     _gstate.total_producer_vote_weight += delta;
               });
            }
         }
      }

	  _voters.modify( voter, 0, [&]( auto& v ) {
            v.last_vote_weight = new_weight;
         }
      );

	  #endif
	//End: removed by UOS(hml) Date:2018-09-20
      
   }


} /// namespace eosiosystem
