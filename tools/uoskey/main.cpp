#include<iostream>
#include <fc/crypto/private_key.hpp>
#include <fc/crypto/public_key.hpp>




using public_key_type = fc::crypto::public_key;
using private_key_type = fc::crypto::private_key;

void generatekey(const std::string seed);

int main(int argc, char* argv[]) {

    for(int i = 1; i < argc; ++i) {
        std::cout << "seed: " <<  argv[i] << std::endl;
        generatekey(std::string(argv[i]));
    }

    return 0;
}

void generatekey(const std::string seed) {
    auto default_priv_key = private_key_type::regenerate<fc::ecc::private_key_shim>(fc::sha256::hash(seed));
    std::cout << "UOS private key: " << std::string(default_priv_key) << std::endl;
    std::cout << "UOS public key: " << std::string(default_priv_key.get_public_key()) << std::endl;

    return ;
}