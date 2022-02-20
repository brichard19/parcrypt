#ifndef _HOST_SECP256K1_H
#define _HOST_SECP256K1_H

#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<string>
#include<vector>
#include<iostream>
#include <cmath>


namespace secp256k1 {


class uint256 final {

public:
  static const int BigEndian = 1;
  static const int LittleEndian = 2;

  bool is_hex(char c)
  {
    return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9');
  }

  uint32_t v[8] = { 0 };

  uint256()
  {
    memset(this->v, 0, sizeof(this->v));
  }

  uint256(const std::string& s)
  {
    std::string t = s;

    // 0x prefix
    if(t.length() >= 2 && (t[0] == '0' && t[1] == 'x' || t[1] == 'X')) {
      t = t.substr(2);
    }

    // 'h' suffix
    if(t.length() >= 1 && t[t.length() - 1] == 'h') {
      t = t.substr(0, t.length() - 1);
    }

    if(t.length() == 0) {
      throw std::string("Incorrect hex formatting");
    }

    // Verify only valid hex characters
    for(int i = 0; i < (int)t.length(); i++) {
      if(!is_hex(t[i])) {
        throw std::string("Incorrect hex formatting");
      }
    }

    // Ensure the value is 64 hex digits. If it is longer, take the least-significant 64 hex digits.
    // If shorter, pad with 0's.
    if(t.length() > 64) {
      t = t.substr(t.length() - 64);
    } else if(t.length() < 64) {
      t = std::string(64 - t.length(), '0') + t;
    }

    int len = (int)t.length();

    memset(this->v, 0, sizeof(uint32_t) * 8);

    int j = 0;
    for(int i = len - 8; i >= 0; i -= 8) {
      std::string sub = t.substr(i, 8);
      uint32_t val;
      if(sscanf(sub.c_str(), "%x", &val) != 1) {
        throw std::string("Incorrect hex formatting");
      }
      this->v[j] = val;
      j++;
    }
  }

  uint256(unsigned int x)
  {
    memset(this->v, 0, sizeof(this->v));
    this->v[0] = x;
  }

  uint256(uint64_t x)
  {
    memset(this->v, 0, sizeof(this->v));
    this->v[0] = (unsigned int)x;
    this->v[1] = (unsigned int)(x >> 32);
  }

  uint256(int x)
  {
    memset(this->v, 0, sizeof(this->v));
    this->v[0] = (unsigned int)x;
  }

  uint256(long long x)
  {
    memset(this->v, 0, sizeof(this->v));
    this->v[0] = (unsigned int)x;
    this->v[1] = (unsigned int)(x >> 32);
  }

  uint256(const uint8_t x[32])
  {
    for(int i = 7; i >= 0; i--) {
      unsigned int word = 0;
      for(int j = 0; j < 4; j++) {
        word |= (unsigned int)x[(7 - i) * 4 + j] << (24 - j * 8);
      }
      this->v[i] = word;
    }
  }

  uint256(const uint32_t x[8], int endian = LittleEndian)
  {
    if(endian == LittleEndian) {
      for(int i = 0; i < 8; i++) {
        this->v[i] = x[i];
      }
    } else {
      for(int i = 0; i < 8; i++) {
        this->v[i] = x[7 - i];
      }
    }
  }

  bool operator==(const uint256& x) const
  {
    for(int i = 0; i < 8; i++) {
      if(this->v[i] != x.v[i]) {
        return false;
      }
    }

    return true;
  }

  bool operator!=(const uint256& x) const
  {
    return !(*this == x);
  }

  bool operator>(const uint256& x) const
  {
    for(int i = 7; i >= 0; i--) {
      if(this->v[i] > x.v[i]) {
        return true;
      } else if(this->v[i] < x.v[i]) {
        return false;
      }
    }

    return false;
  }

  bool operator<(const uint256& x) const
  {
    for(int i = 7; i >= 0; i--) {
      if(this->v[i] < x.v[i]) {
        return true;
      } else if(this->v[i] > x.v[i]) {
        return false;
      }
    }

    return false;
  }

  bool operator<=(const uint256& x) const
  {
    return !(*this > x);
  }

  bool operator>=(const uint256& x) const
  {
    return !(*this < x);
  }

  uint256 operator+(const uint256& x) const
  {
    return add(x);
  }

  uint256 operator+(uint32_t x) const
  {
    return add(x);
  }

  uint256 operator*(uint32_t x) const
  {
    return mul(x);
  }

  uint256 operator*(const uint256& x) const
  {
    return mul(x);
  }

  uint256 operator*(uint64_t x) const
  {
    return mul(x);
  }

  uint256 operator-(const uint256& x) const
  {
    return sub(x);
  }

  void exportWords(unsigned int* buf, int len = 8, int endian = LittleEndian) const
  {
    if(endian == LittleEndian) {
      for(int i = 0; i < len; i++) {
        buf[i] = v[i];
      }
    } else {
      for(int i = 0; i < len; i++) {
        buf[len - i - 1] = v[i];
      }
    }
  }

  //void exportWords(uint256_t& buf, int endian = LittleEndian) const
  //{
  //	exportWords(buf.v, 8, endian);
  //}

  void exportBytes(unsigned char* buf) const
  {
    int buf_idx = 0;
    for(int i = 7; i >= 0; i--) {
      buf[buf_idx++] = (unsigned char)(v[i] >> 24);
      buf[buf_idx++] = (unsigned char)(v[i] >> 16);
      buf[buf_idx++] = (unsigned char)(v[i] >> 8);
      buf[buf_idx++] = (unsigned char)v[i];
    }
  }

  void set_bit(int n, bool value)
  {
    if(value) {
      unsigned int mask = 1 << (n % 32);
      this->v[n / 32] |= mask;
    } else {
      unsigned int mask = ~(1 << (n % 32));
      this->v[n / 32] &= mask;
    }
  }

  uint256 mul(const uint256& val) const;

  uint256 mul(int val) const;

  uint256 mul(uint32_t val) const;

  uint256 mul(uint64_t val) const;

  uint256 add(int val) const;

  uint256 add(unsigned int val) const;

  uint256 add(uint64_t val) const;

  uint256 sub(int val) const;

  uint256 sub(const uint256& val) const;

  uint256 add(const uint256& val) const;

  uint256 div(uint32_t val) const;

  uint256 mod(uint32_t val) const;

  uint256 neg() const;

  uint256 pow(int n);
  uint256 powModP(const uint256 x) const;
  uint256 sqrtModP() const;

  bool isNeg() const
  {
    return (v[7] & 0x80000000) != 0;
  }

  uint32_t to_uint32() const
  {
    return this->v[0];
  }

  bool isZero() const
  {
    for(int i = 0; i < 8; i++) {
      if(this->v[i] != 0) {
        return false;
      }
    }

    return true;
  }

  int cmp(const uint256& val) const
  {
    for(int i = 7; i >= 0; i--) {

      if(this->v[i] < val.v[i]) {
        // less than
        return -1;
      } else if(this->v[i] > val.v[i]) {
        // greater than
        return 1;
      }
    }

    // equal
    return 0;
  }

  int cmp(unsigned int& val) const
  {
    // If any higher bits are set then it is greater
    for(int i = 7; i >= 1; i--) {
      if(this->v[i]) {
        return 1;
      }
    }

    if(this->v[0] > val) {
      return 1;
    } else if(this->v[0] < val) {
      return -1;
    }

    return 0;
  }

  //uint256 pow(int n)
  //{
  //	uint256 product(1);
  //	uint256 square = *this;

  //	while(n) {
  //		if(n & 1) {
  //			product = product.mul(square);
  //		}
  //		square = square.mul(square);

  //		n >>= 1;
  //	}

  //	return product;
  //}

//      uint256 powModP(const uint256 x) const
//      {
//          uint256 n = x;
//          uint256 product(1);
//          uint256 square = *this;

//          while(!n.isZero()) {
//              if(n.isEven()) {
//                  product = secp256k1::multiplyModP(product, square);
//              }
//              square = secp256k1::multiplyModP(square, square);

//              n = n.div(2);
//          }

//          return product;
//      }

      //uint256 sqrtModP()
      //{
      //    uint256 e = (secp256k1::P + 1).div(4);

      //    return this->powModP(e);
      //}
  /*
  int len_bits(uint32_t n)
  {
    uint32_t mask = 0x80000000;

    int count = 0;
    while(mask > 0 && n & mask == 0) {
      count++;
    }

    return 32 - count;
  }

  int len_bits()
  {
    // Find the highest non-zero word
    for(int i = 7; i >= 0; i--) {

    }
  }*/

  bool bit(int n)
  {
    n = n % 256;

    return (this->v[n / 32] & (0x1 << (n % 32))) != 0;
  }

  bool is_even() const
  {
    return (this->v[0] & 1) == 0;
  }

  std::string to_string(int base = 16) const;

  uint64_t to_uint64()
  {
    return ((uint64_t)this->v[1] << 32) | v[0];
  }

  double to_double()
  {
    // Find the highest non-zero word
    double value = 0;
    double exponent = 0;
    int i = 0;
    for(i = 7; i >= 0; i--) {
      if(this->v[i] != 0) {
        value = (double)this->v[i];
        break;
      }
    }

    exponent = i * 32;

    return value * std::pow(2, exponent);
  }
};

const unsigned int _POINT_AT_INFINITY_WORDS[8] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
const unsigned int _P_WORDS[8] = { 0xFFFFFC2F, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
const unsigned int _N_WORDS[8] = { 0xD0364141, 0xBFD25E8C, 0xAF48A03B, 0xBAAEDCE6, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
const unsigned int _GX_WORDS[8] = { 0x16F81798, 0x59F2815B, 0x2DCE28D9, 0x029BFCDB, 0xCE870B07, 0x55A06295, 0xF9DCBBAC, 0x79BE667E };
const unsigned int _GY_WORDS[8] = { 0xFB10D4B8, 0x9C47D08F, 0xA6855419, 0xFD17B448, 0x0E1108A8, 0x5DA4FBFC, 0x26A3C465, 0x483ADA77 };
const unsigned int _BETA_WORDS[8] = { 0x719501EE, 0xC1396C28, 0x12F58995, 0x9CF04975, 0xAC3434E9, 0x6E64479E, 0x657C0710, 0x7AE96A2B };
const unsigned int _LAMBDA_WORDS[8] = { 0x1B23BD72, 0xDF02967C, 0x20816678, 0x122E22EA, 0x8812645A, 0xA5261C02, 0xC05C30E0, 0x5363AD4C };

const uint256 P(_P_WORDS);
const uint256 N(_N_WORDS);

const uint256 BETA(_BETA_WORDS);
const uint256 LAMBDA(_LAMBDA_WORDS);

uint256 negModP(const uint256& x);
uint256 negModN(const uint256& x);

uint256 addModP(const uint256& a, const uint256& b);
uint256 subModP(const uint256& a, const uint256& b);
uint256 multiplyModP(const uint256& a, const uint256& b);
uint256 multiplyModN(const uint256& a, const uint256& b);
uint256 invModP(const uint256& x);
uint256 addModN(const uint256& a, const uint256& b);
uint256 subModN(const uint256& a, const uint256& b);
uint256 fillBits(int bits);
uint256 generatePrivateKey();
uint256 generatePrivateKey(unsigned int bits);

uint256 parsebase10(std::string s);

class ecpoint {

  uint256 getY(uint256 xCoord, unsigned char sign)
  {
    uint256 x3 = xCoord.powModP(3);
    x3 = secp256k1::addModP(x3, 7);

    y = x3.sqrtModP();
    if(sign == 0x02) {
      if(y.is_even()) {
        return y;
      } else {
        return subModP(secp256k1::P, y);
      }
    } else {
      if(y.is_even()) {
        return subModP(secp256k1::P, y);
      } else {
        return y;
      }
    }
  }

public:
  uint256 x;
  uint256 y;

  ecpoint()
  {
    this->x = uint256(_POINT_AT_INFINITY_WORDS);
    this->y = uint256(_POINT_AT_INFINITY_WORDS);
  }

  ecpoint(const uint256& x, const uint256& y)
  {
    this->x = x;
    this->y = y;
  }

  ecpoint(const uint256& x, unsigned char sign)
  {
    if(sign != 0x02 && sign != 0x03) {
      throw std::string("sign byte must be 0x02 or 0x03");
    }

    this->x = x;
    this->y = getY(this->x, sign);
  }

  ecpoint(const ecpoint& p)
  {
    this->x = p.x;
    this->y = p.y;
  }

  ecpoint operator=(const ecpoint& p)
  {
    this->x = p.x;
    this->y = p.y;

    return *this;
  }

  bool operator==(const ecpoint& p) const
  {
    return this->x == p.x && this->y == p.y;
  }

  bool operator!=(const ecpoint& p) const
  {
    return !(*this == p);
  }

  std::string toString(bool compressed = false)
  {
    if(!compressed) {
      return "04" + this->x.to_string() + this->y.to_string();
    } else {
      if(this->y.is_even()) {
        return "02" + this->x.to_string();
      } else {
        return "03" + this->x.to_string();
      }
    }
  }

  void exportCompressed(unsigned char* buf)
  {
    unsigned char sign_byte = (this->y.v[0] & 1) ? 0x03 : 0x02;

    this->x.exportBytes(&buf[1]);
    buf[0] = sign_byte;
  }

  unsigned char getCompressionByte() const
  {
    if(this->y.is_even()) {
      return 0x02;
    } else {
      return 0x03;
    }
  }

  bool exists() const;
};

ecpoint pointAtInfinity();
ecpoint G();
ecpoint addPoints(const ecpoint& p, const ecpoint& q);
ecpoint doublePoint(const ecpoint& p);
ecpoint negPoint(const ecpoint& p);
bool isPointAtInfinity(const ecpoint& p);
ecpoint multiplyPoint(const uint256& k, const ecpoint& p);
bool pointExists(const ecpoint& p);
void generateKeyPairsBulk(unsigned int count, const ecpoint& basePoint, std::vector<uint256>& privKeysOut, std::vector<ecpoint>& pubKeysOut, int keylen = 256);
void generateKeyPairsBulk(const ecpoint& basePoint, std::vector<uint256>& privKeys, std::vector<ecpoint>& pubKeysOut);
ecpoint parsePublicKey(const std::string& pubKeyString);


struct uint256_t {
  uint32_t v[8];

  uint256_t() = default;

  uint256_t(const uint256& x)
  {
    x.exportWords(v);
  }

  uint256_t& operator=(const uint256& x)
  {
    x.exportWords(v);

    return *this;
  }
};
}

#endif