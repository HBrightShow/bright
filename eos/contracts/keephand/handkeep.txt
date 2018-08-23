1.创建用户 eoskeephand2  与 eoseosbright
//owner
5JYQB6xwraFX53yaMnhht5bnceZRdS8fFQR2jxdeAFcj5DVjHK9
EOS7er3MePm841zP3VKzCvUhcbqwEy9BRXthZCBZu7ihPfvP4Rtg4
//active
5Jw9zKATACsqtTHJam4ciVYWoh9EBny93L1nswyazQjnEoUa9by
EOS723pRnWd43Lu53LWUq7ciVrmUXvADUoVsjo3ZhDHBi8jDWduYD

创建钱包
cleos --wallet-url http://127.0.0.1:8901 wallet create  -n  walletkeephand2
PW5JqBoV8GQhVP9GPrJwN7N4oGQ9yrm53LdjST6gYD862eeKM5aWb

//导入钱包
cleos --wallet-url http://127.0.0.1:8901 wallet import -n   walletkeephand2 --private-key 5JYQB6xwraFX53yaMnhht5bnceZRdS8fFQR2jxdeAFcj5DVjHK9
cleos --wallet-url http://127.0.0.1:8901 wallet import -n   walletkeephand2 --private-key 5Jw9zKATACsqtTHJam4ciVYWoh9EBny93L1nswyazQjnEoUa9by

//创建eoskeephand2用户
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.141:8888 system newaccount --transfer hml eoskeephand2 EOS7er3MePm841zP3VKzCvUhcbqwEy9BRXthZCBZu7ihPfvP4Rtg4 EOS723pRnWd43Lu53LWUq7ciVrmUXvADUoVsjo3ZhDHBi8jDWduYD --stake-net "200.0000 SYS" --stake-cpu "200.0000 SYS" --buy-ram "200.0000 SYS"   
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.141:8888 transfer hml eoskeephand2 "200.0000 SYS"
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.141:8888 system newaccount --transfer hml eoseosbright EOS7er3MePm841zP3VKzCvUhcbqwEy9BRXthZCBZu7ihPfvP4Rtg4 EOS723pRnWd43Lu53LWUq7ciVrmUXvADUoVsjo3ZhDHBi8jDWduYD --stake-net "200.0000 SYS" --stake-cpu "200.0000 SYS" --buy-ram "200.0000 SYS"   cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.141:8888 transfer hml eoseosbright "200.0000 SYS"

重复上面步骤自己创建一个用户 eoseosbright


2.编译与加载合约
eosiocpp -o keephand.wast keephand.cpp
eosiocpp -g keephand.abi keephand.cpp

//加载合约
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 set contract eoskeephand2  ~/eos/contracts/keephand/ -p eoskeephand2

//查询数据库
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 get table eoskeephand2 eoskeephand2  userinfo
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 get table eoskeephand2 eoskeephand2  userasset

3.增加合约创建者用户
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 createacnt '{"name":"eoskeephand2","pubkey":"EOS723pRnWd43Lu53LWUq7ciVrmUXvADUoVsjo3ZhDHBi8jDWduYD"}' -p eoskeephand2

以后的手率费都会放在这个用户下面

4.创建其它的一些用户usera,userb,userc
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 createacnt '{"name":"eoseosbright","pubkey":"EOS8KjRh1QFLqNECdqg7QXiBnv3F2DhVSQDdSkeFJX2ZLkR13p8Cs"}' -p eoskeephand2
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 createacnt '{"name":"eosxiaomingg","pubkey":"EOS8YHwHXqEdBcNLdNud4Uuste5kbsPyHwSzMuUWCygVHYNr7Gk7r"}' -p eoskeephand2
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 createacnt '{"name":"usera","pubkey":"EOS8Hn8Bbp5oska1LULgHFr2JPP2pGZmkktdYF5e1c1HBPVBakrBY"}' -p eoskeephand2
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 createacnt '{"name":"userb","pubkey":"EOS7Ef4kuyTbXbtSPP5Bgethvo6pbitpuEz2RMWhXb8LXxEgcR7MC"}' -p eoskeephand2
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 createacnt '{"name":"userc","pubkey":"EOS6XpxPXQz9zxpRoZwcX7qxZBGM5AW9UXmXx5gPj8TQsce2djSkn"}' -p eoskeephand2


5.清除用户数据
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 deleteacnt '{"name":"eoseosbright","pubkey":"EOS8KjRh1QFLqNECdqg7QXiBnv3F2DhVSQDdSkeFJX2ZLkR13p8Cs"}' -p eoskeephand2
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 deleteacnt '{"name":"eosxiaomingg","pubkey":"EOS8YHwHXqEdBcNLdNud4Uuste5kbsPyHwSzMuUWCygVHYNr7Gk7r"}' -p eoskeephand2
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 deleteacnt '{"name":"usera","pubkey":"EOS8Hn8Bbp5oska1LULgHFr2JPP2pGZmkktdYF5e1c1HBPVBakrBY"}' -p eoskeephand2
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 deleteacnt '{"name":"userb","pubkey":"EOS7Ef4kuyTbXbtSPP5Bgethvo6pbitpuEz2RMWhXb8LXxEgcR7MC"}' -p eoskeephand2
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 deleteacnt '{"name":"userc","pubkey":"EOS6XpxPXQz9zxpRoZwcX7qxZBGM5AW9UXmXx5gPj8TQsce2djSkn"}' -p eoskeephand2

cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 deleteacnt '{"name":"eoskeephand2","pubkey":"EOS723pRnWd43Lu53LWUq7ciVrmUXvADUoVsjo3ZhDHBi8jDWduYD"}' -p eoskeephand2只有合约所有者才有权限清除

6.存款取款
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 modasset '{"name":"eoseosbright","funds":"10000", "bsaving":"1"}' -p eoseosbright
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 modasset '{"name":"eoseosbright","funds":"1000", "bsaving":"0"}' -p eoseosbright
7.转帐
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 transfer '{"from":"eoseosbright","to":"eosxiaomingg","funds":"1000.0000","str":"thanks"}' -p eoseosbright

8.单用户查询余额
cleos --wallet-url http://127.0.0.1:8901 --url http://10.186.11.223:8888 push action  eoskeephand2 queryasset '{"name":"eosxiaomingg"}' -p eosxiaomingg











