#ifndef _ADDRESS_UTIL_H
#define _ADDRESS_UTIL_H

#include "secp256k1.h"

namespace Address {
	std::string from_public_key(const secp256k1::ecpoint &p, bool compressed = false);
	bool verify_address(std::string address);
};

namespace Base58 {
	std::string to_base58(const secp256k1::uint256 &x);
	secp256k1::uint256 to_big_int(const std::string &s);

	void to_hash160(const std::string &s, uint32_t hash[5]);

	bool is_base58(std::string s);
};



namespace Hash {


	void hash_public_key(const secp256k1::ecpoint &p, uint32_t *digest);
	void hash_public_key_compressed(const secp256k1::ecpoint &p, uint32_t *digest);

	void hash_public_key(const uint32_t *x, const uint32_t *y, uint32_t *digest);
	void hash_public_key_compressed(const uint32_t *x, const uint32_t *y, uint32_t *digest);

};


#endif