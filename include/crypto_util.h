#ifndef _CRYPTO_UTIL_H

namespace crypto {

	void ripemd160(unsigned int *msg, unsigned int *digest);

	void sha256Init(unsigned int *digest);
	void sha256(unsigned int *msg, unsigned int *digest);

	unsigned int checksum(const unsigned int *hash);
};

#endif