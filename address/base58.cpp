#include <map>
#include <string>
#include "crypto_util.h"

#include "address_util.h"

namespace {
const std::string BASE58_STRING = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

struct Base58Map {
  static std::map<char, int> createBase58Map()
  {
    std::map<char, int> m;
    for(int i = 0; i < 58; i++) {
      m[BASE58_STRING[i]] = i;
    }

    return m;
  }

  static std::map<char, int> myMap;
};

std::map<char, int> Base58Map::myMap = Base58Map::createBase58Map();
}


/**
 * Converts a base58 string to uint256
 */
secp256k1::uint256 Base58::to_big_int(const std::string& s)
{
  secp256k1::uint256 value;

  for(unsigned int i = 0; i < s.length(); i++) {
    value = value.mul(58);

    int c = Base58Map::myMap[s[i]];
    value = value.add(c);
  }

  return value;
}

void Base58::to_hash160(const std::string& s, unsigned int hash[5])
{
  secp256k1::uint256 value = to_big_int(s);
  unsigned int words[6];

  value.exportWords(words, 6, secp256k1::uint256::BigEndian);

  // Extract words, ignore checksum
  for(int i = 0; i < 5; i++) {
    hash[i] = words[i];
  }
}

bool Base58::is_base58(std::string s)
{
  for(unsigned int i = 0; i < s.length(); i++) {
    if(BASE58_STRING.find(s[i]) < 0) {
      return false;
    }
  }

  return true;
}

std::string Base58::to_base58(const secp256k1::uint256& x)
{
  std::string s;

  secp256k1::uint256 value = x;

  while(!value.isZero()) {
    secp256k1::uint256 digit = value.mod(58);
    int digit_int = digit.to_uint32();

    s = BASE58_STRING[digit_int] + s;

    value = value.div(58);
  }

  return s;
}