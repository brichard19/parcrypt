
typedef ulong uint64_t;

typedef struct {
  uint v[8];
}uint256_t;

#define make_uint256(x) {{(x), 0, 0, 0, 0, 0, 0, 0}}

/**
 Prime modulus 2^256 - 2^32 - 977
 */
/*
constant uint _P[8] = {
  0xFFFFFC2F, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};
*/


/**
 Base point X
 */
/*
constant uint _GX[8] = {
  0x16F81798, 0x59F2815B, 0x2DCE28D9, 0x029BFCDB, 0xCE870B07, 0x55A06295, 0xF9DCBBAC, 0x79BE667E
};
*/

/**
 Base point Y
 */
/*
constant uint _GY[8] = {
  0xFB10D4B8, 0x9C47D08F, 0xA6855419, 0xFD17B448, 0x0E1108A8, 0x5DA4FBFC, 0x26A3C465, 0x483ADA77
};
*/

/**
 * Group order
 */
/*
constant uint _N[8] = {
  0xD0364141, 0xBFD25E8C, 0xAF48A03B, 0xBAAEDCE6, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};
*/

void print_big_int(const uint x[8])
{
  printf("%.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x\n",
    x[7], x[6], x[5], x[4],
    x[3], x[2], x[1], x[0]);
}

void print_uint256(uint256_t x)
{
  print_big_int(x.v);
}

// Add with carry
inline uint addc(uint a, uint b, uint* carry)
{
  ulong sum = (ulong)a + b + *carry;

  *carry = (uint)(sum >> 32);

  return (uint)sum;
}

// Subtract with borrow
inline uint subc(uint a, uint b, uint* borrow)
{
  ulong diff = (ulong)a - b - *borrow;

  *borrow = (uint)((diff >> 32) & 1);

  return (uint)diff;
}


// 32 x 32 multiply-add
inline void madd977(uint* high, uint* low, uint a, uint c)
{
  ulong prod = (ulong)a * 977 + c;

  *high = (uint)(prod >> 32);
  *low = (uint)prod;
}


// 32 x 32 multiply-add
inline void madd(uint* high, uint* low, uint a, uint b, uint c)
{
  ulong prod = (ulong)a * b + c;

  *high = (uint)(prod >> 32);
  *low = (uint)prod;
}

inline void mull(uint* high, uint* low, uint a, uint b)
{
  *low = a * b;
  *high = mul_hi(a, b);
}

inline void addP(const uint a[8], uint c[8])
{
  uint carry = 0;
  c[0] = addc(a[0], 0xFFFFFC2F, &carry);
  c[1] = addc(a[1], 0xFFFFFFFE, &carry);
  c[2] = addc(a[2], 0xFFFFFFFF, &carry);
  c[3] = addc(a[3], 0xFFFFFFFF, &carry);
  c[4] = addc(a[4], 0xFFFFFFFF, &carry);
  c[5] = addc(a[5], 0xFFFFFFFF, &carry);
  c[6] = addc(a[6], 0xFFFFFFFF, &carry);
  c[7] = a[7] + 0xFFFFFFFF + carry;
}

inline void subP(const uint a[8], uint c[8])
{
  // Add two's compliment of P
  const uint t0 = ~0xFFFFFC2E + 1;
  const uint t1 = ~0xFFFFFFFE;

  uint carry = 0;
  c[0] = addc(a[0], t0, &carry);
  c[1] = addc(a[1], t1, &carry);
  c[2] = addc(a[2], t1, &carry);
  c[3] = addc(a[3], t1, &carry);
  c[4] = addc(a[4], t1, &carry);
  c[5] = addc(a[5], t1, &carry);
  c[6] = addc(a[6], t1, &carry);
  c[7] = a[7] + carry;
}

uint256_t sub256(uint256_t a, uint256_t b, uint* borrow_ptr)
{
  uint borrow = 0;
  uint256_t c;

  for(int i = 0; i < 8; i++) {
    c.v[i] = subc(a.v[i], b.v[i], &borrow);
  }

  *borrow_ptr = borrow;

  return c;
}

bool gte_p(const uint a[8])
{
  for (int i = 7; i >= 2; i--) {
    if(a[i] != 0xffffffff) {
      return false;
    }
  }

  if(a[1] < 0xfffffffe) {
    return false;
  }

  if(a[0] < 0xFFFFFC2F) {
    return false;
  }

  return true;

}

void multiply128(const uint x[4], const uint y[4], uint z[8])
{
  uint high = 0;

  // First round, overwrite z
  #pragma unroll
  for(int j = 0; j < 4; j++) {
    uint64_t product = (uint64_t)x[0] * y[j];

    product = product + high;

    z[j] = (uint)product;
    high = (uint)(product >> 32);
  }
  z[4] = high;

  #pragma unroll
  for(int i = 1; i < 4; i++) {

    high = 0;

    for(int j = 0; j < 4; j++) {

      uint64_t product = (uint64_t)x[i] * y[j];

      product = product + z[i + j] + high;

      z[i + j] = (uint)product;

      high = product >> 32;
    }

    z[4 + i] = high;
  }
}

void multiply256(const uint x[8], const uint y[8], uint out_high[8], uint out_low[8])
{
  uint z[16];

  uint high = 0;

  // First round, overwrite z
  #pragma unroll
  for(int j = 0; j < 8; j++) {

    uint64_t product = (uint64_t)x[0] * y[j];

    product = product + high;

    z[j] = (uint)product;
    high = (uint)(product >> 32);
  }
  z[8] = high;

  #pragma unroll
  for(int i = 1; i < 8; i++) {

    high = 0;

    for(int j = 0; j < 8; j++) {

      uint64_t product = (uint64_t)x[i] * y[j];

      product = product + z[i + j] + high;

      z[i + j] = (uint)product;

      high = product >> 32;
    }

    z[8 + i] = high;
  }

  for(int i = 0; i < 8; i++) {
    out_high[i] = z[8 + i];
    out_low[i] = z[i];
  }
}

void square256(const uint x[8], uint out_high[8], uint out_low[8])
{
  multiply128(&x[0], &x[0], out_low);
  multiply128(&x[4], &x[4], out_high);

  uint ab[8];
  multiply128(&x[0], &x[4], ab);
  uint s = ab[7] >> 31;
  ab[7] = (ab[7] << 1) | (ab[6] >> 31);
  ab[6] = (ab[6] << 1) | (ab[5] >> 31);
  ab[5] = (ab[5] << 1) | (ab[4] >> 31);
  ab[4] = (ab[4] << 1) | (ab[3] >> 31);
  ab[3] = (ab[3] << 1) | (ab[2] >> 31);
  ab[2] = (ab[2] << 1) | (ab[1] >> 31);
  ab[1] = (ab[1] << 1) | (ab[0] >> 31);
  ab[0] <<= 1;

  uint carry = 0;
  out_low[4] = addc(out_low[4], ab[0], &carry);
  out_low[5] = addc(out_low[5], ab[1], &carry);
  out_low[6] = addc(out_low[6], ab[2], &carry);
  out_low[7] = addc(out_low[7], ab[3], &carry);

  out_high[0] = addc(out_high[0], ab[4], &carry);
  out_high[1] = addc(out_high[1], ab[5], &carry);
  out_high[2] = addc(out_high[2], ab[6], &carry);
  out_high[3] = addc(out_high[3], ab[7], &carry);

  out_high[4] = addc(out_high[4], s, &carry);
  out_high[5] = addc(out_high[5], 0, &carry);
  out_high[6] = addc(out_high[6], 0, &carry);
  out_high[7] = addc(out_high[7], 0, &carry);

}

uint add256(const uint a[8], const uint b[8], uint c[8])
{
  uint carry = 0;

  for(int i = 0; i < 8; i++) {
    c[i] = addc(a[i], b[i], &carry);
  }

  return carry;
}

uint256_t add256k(uint256_t a, uint256_t b, uint* carry_ptr)
{
  uint256_t c;
  uint carry = 0;

  for(int i = 0; i < 8; i++) {
    c.v[i] = addc(a.v[i], b.v[i], &carry);
  }

  *carry_ptr = carry;

  return c;
}


bool is_infinity(const uint256_t x)
{
  bool isf = true;

  for(int i = 0; i < 8; i++) {
    if(x.v[i] != 0xffffffff) {
      isf = false;
    }
  }

  return isf;
}


bool equal256k(uint256_t a, uint256_t b)
{
  for(int i = 0; i < 8; i++) {
    if(a.v[i] != b.v[i]) {
      return false;
    }
  }

  return true;
}


/*
 * Read least-significant word
 */
uint readLSW256k(global const uint256_t* ara, int idx)
{
  return ara[idx].v[0];
}

uint readWord256k(global const uint256_t* ara, int idx, int word)
{
  return ara[idx].v[word];
}



/**
 * Subtraction mod p
 */
uint256_t subModP256(uint256_t a, uint256_t b)
{
  uint borrow = 0;
  uint256_t c = sub256(a, b, &borrow);
  if(borrow) {
    addP(c.v, c.v);
  }

  return c;
}


uint256_t addModP256(uint256_t a, uint256_t b)
{
  uint carry = 0;

  uint256_t c = add256k(a, b, &carry);

  if(carry) {
    subP(c.v, c.v);
  } else if(c.v[0] == 0xffffffff) {
    if(gte_p(c.v)) {
      subP(c.v, c.v);
    } 

  }

  return c;
}

void squareModP(const uint a[8], uint product_low[8])
{
  uint high[8];

  uint hWord = 0;
  uint carry = 0;

  // 256 x 256 multiply
  square256(a, high, product_low);

  // Perform reduction by multiplying the high 256 bits by 2^32 + 977 and
  // adding it to the low 256 bits 
  ulong product = (ulong)high[0] * 977 + product_low[0];
  hWord = (uint)(product >> 32);
  product_low[0] = (uint)product;

  product = (ulong)high[1] * 977 + high[0] + product_low[1] + hWord;
  hWord = (uint)(product >> 32);
  product_low[1] = (uint)product;

  product = (ulong)high[2] * 977 + high[1] + product_low[2] + hWord;
  hWord = (uint)(product >> 32);
  product_low[2] = (uint)product;
  
  product = (ulong)high[3] * 977 + high[2] + product_low[3] + hWord;
  hWord = (uint)(product >> 32);
  product_low[3] = (uint)product;
  
  product = (ulong)high[4] * 977 + high[3] + product_low[4] + hWord;
  hWord = (uint)(product >> 32);
  product_low[4] = (uint)product;
  
  product = (ulong)high[5] * 977 + high[4] + product_low[5] + hWord;
  hWord = (uint)(product >> 32);
  product_low[5] = (uint)product;
  
  product = (ulong)high[6] * 977 + high[5] + product_low[6] + hWord;
  hWord = (uint)(product >> 32);
  product_low[6] = (uint)product;
  
  product = (ulong)high[7] * 977 + high[6] + product_low[7] + hWord;
  hWord = (uint)(product >> 32);
  product_low[7] = (uint)product;

  // Second reduction
  ulong sum = (ulong)high[7] + hWord;
  high[0] = (uint)sum;
  high[1] = (uint)(sum >> 32); 

  product = (ulong)high[0] * 977 + product_low[0];
  hWord = (uint)(product >> 32);
  product_low[0] = (uint)product;

  product = (ulong)high[1] * 977 + high[0] + product_low[1] + hWord;
  hWord = (uint)(product >> 32);
  product_low[1] = (uint)product;

  sum = (ulong)high[1] + product_low[2] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[2] = (uint)sum;

  sum = (ulong)product_low[3] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[3] = (uint)sum;
  
  sum = (ulong)product_low[4] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[4] = (uint)sum;
  
  sum = (ulong)product_low[5] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[5] = (uint)sum;

  sum = (ulong)product_low[6] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[6] = (uint)sum;
  
  sum = (ulong)product_low[7] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[7] = (uint)sum;

  // Reduce if >= P
  if(hWord) {
    subP(product_low, product_low);
  } else if(product_low[7] == 0xffffffff) {
    if(gte_p(product_low)) {
      subP(product_low, product_low);
    }
  }
}

void mulModP(const uint a[8], const uint b[8], uint product_low[8])
{
  uint high[8];

  uint hWord = 0;
  uint carry = 0;

  // 256 x 256 multiply
  multiply256(a, b, high, product_low);

  // Perform reduction by multiplying the high 256 bits by 2^32 + 977 and
  // adding it to the low 256 bits 
  ulong product = (ulong)high[0] * 977 + product_low[0];
  hWord = (uint)(product >> 32);
  product_low[0] = (uint)product;

  product = (ulong)high[1] * 977 + high[0] + product_low[1] + hWord;
  hWord = (uint)(product >> 32);
  product_low[1] = (uint)product;

  product = (ulong)high[2] * 977 + high[1] + product_low[2] + hWord;
  hWord = (uint)(product >> 32);
  product_low[2] = (uint)product;
  
  product = (ulong)high[3] * 977 + high[2] + product_low[3] + hWord;
  hWord = (uint)(product >> 32);
  product_low[3] = (uint)product;
  
  product = (ulong)high[4] * 977 + high[3] + product_low[4] + hWord;
  hWord = (uint)(product >> 32);
  product_low[4] = (uint)product;
  
  product = (ulong)high[5] * 977 + high[4] + product_low[5] + hWord;
  hWord = (uint)(product >> 32);
  product_low[5] = (uint)product;
  
  product = (ulong)high[6] * 977 + high[5] + product_low[6] + hWord;
  hWord = (uint)(product >> 32);
  product_low[6] = (uint)product;
  
  product = (ulong)high[7] * 977 + high[6] + product_low[7] + hWord;
  hWord = (uint)(product >> 32);
  product_low[7] = (uint)product;

  // Second reduction
  ulong sum = (ulong)high[7] + hWord;
  high[0] = (uint)sum;
  high[1] = (uint)(sum >> 32); 

  product = (ulong)high[0] * 977 + product_low[0];
  hWord = (uint)(product >> 32);
  product_low[0] = (uint)product;

  product = (ulong)high[1] * 977 + high[0] + product_low[1] + hWord;
  hWord = (uint)(product >> 32);
  product_low[1] = (uint)product;

  sum = (ulong)high[1] + product_low[2] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[2] = (uint)sum;

  sum = (ulong)product_low[3] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[3] = (uint)sum;
  
  sum = (ulong)product_low[4] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[4] = (uint)sum;
  
  sum = (ulong)product_low[5] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[5] = (uint)sum;

  sum = (ulong)product_low[6] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[6] = (uint)sum;
  
  sum = (ulong)product_low[7] + hWord;
  hWord = (uint)(sum >> 32);
  product_low[7] = (uint)sum;

  // Reduce if >= P
  if(hWord) {
    subP(product_low, product_low);
  } else if(product_low[7] == 0xffffffff) {
    if(gte_p(product_low)) {
      subP(product_low, product_low);
    }
  }

}

uint256_t mulModP256(uint256_t a, uint256_t b)
{
  uint256_t c;

  mulModP(a.v, b.v, c.v);

  return c;
}


uint256_t squareModP256k(uint256_t a)
{
  uint256_t b;
  squareModP(a.v, b.v);

  return b;
}


/**
 * Multiplicative inverse mod P using Fermat's method of x^(p-2) mod p and addition chains
 */
uint256_t inverse_mod_p(uint256_t value)
{
  uint256_t x = value;

  // 0xd - 1101
  uint256_t y = x;
  x = squareModP256k(x);
  //y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);

  // 0x2 - 0010
  //y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  //y = mulModP256(x, y);
  x = squareModP256k(x);
  //y = mulModP256(x, y);
  x = squareModP256k(x);

  // 0xc = 0x1100
  //y = mulModP256(x, y);
  x = squareModP256k(x);
  //y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);


  // 0xfffff
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);


  // 0xe - 1110
  //y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  y = mulModP256(x, y);
  x = squareModP256k(x);
  // 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffff
  for(int i = 0; i < 219; i++) {
    y = mulModP256(x, y);
    x = squareModP256k(x);
  }
  y = mulModP256(x, y);

  return y;
}


void begin_batch_add(uint256_t px, uint256_t x, global uint256_t* chain, int batchIdx, uint256_t* inverse)
{
  const int gid = get_global_id(0);
  const int dim = get_global_size(0);

  // x = Gx - x
  uint256_t t = subModP256(px, x);


  // Keep a chain of multiples of the diff, i.e. c[0] = diff0, c[1] = diff0 * diff1,
  // c[2] = diff2 * diff1 * diff0, etc
  *inverse = mulModP256(*inverse, t);

  chain[batchIdx * dim + gid] = *inverse;
}


void begin_batch_add_with_double(uint256_t px, uint256_t py, global uint256_t* xPtr, global uint256_t* chain, int i, int batchIdx, uint256_t* inverse)
{
  const int gid = get_global_id(0);
  const int dim = get_global_size(0);

  uint256_t x = xPtr[i];
  uint256_t t;
  if(equal256k(px, x)) {
    // 2 * y
    t = addModP256(py, py);
  } else {
    // Gx - x
    t = subModP256(px, x);
  }

  // Keep a chain of multiples of the diff, i.e. c[0] = diff0, c[1] = diff0 * diff1,
  // c[2] = diff2 * diff1 * diff0, etc
  *inverse = mulModP256(t, *inverse);

  chain[batchIdx * dim + gid] = *inverse;
}


void complete_batch_add_with_double(
  uint256_t px,
  uint256_t py,
  global const uint256_t* xPtr,
  global const uint256_t* yPtr,
  int i,
  int batchIdx,
  global uint256_t* chain,
  uint256_t* inverse,
  uint256_t* newX,
  uint256_t* newY)
{
  const int gid = get_global_id(0);
  const int dim = get_global_size(0);
  uint256_t s;
  uint256_t x;
  uint256_t y;

  x = xPtr[i];
  y = yPtr[i];

  if(batchIdx >= 1) {

    uint256_t c;

    c = chain[(batchIdx - 1) * dim + gid];
    s = mulModP256(*inverse, c);

    uint256_t diff;
    if(equal256k(px, x)) {
      diff = addModP256(py, py);
    } else {
      diff = subModP256(px, x);
    }

    *inverse = mulModP256(diff, *inverse);
  } else {
    s = *inverse;
  }


  if(equal256k(px, x)) {
    // currently s = 1 / 2y

    uint256_t x2;
    uint256_t tx2;
    uint256_t x3;

    // 3x^2
    x2 = mulModP256(x, x);
    tx2 = addModP256(x2, x2);
    tx2 = addModP256(x2, tx2);

    // s = 3x^2 * 1/2y
    s = mulModP256(tx2, s);

    // s^2
    uint256_t s2;
    s2 = squareModP256k(s);

    // Rx = s^2 - 2px
    *newX = subModP256(s2, x);
    *newX = subModP256(*newX, x);

    // Ry = s(px - rx) - py
    uint256_t k;
    k = subModP256(px, *newX);
    *newY = mulModP256(s, k);
    *newY = subModP256(*newY, py);
  } else {

    uint256_t rise;
    rise = subModP256(py, y);

    s = mulModP256(rise, s);

    // Rx = s^2 - Gx - Qx
    uint256_t s2;
    s2 = mulModP256(s, s);

    *newX = subModP256(s2, px);
    *newX = subModP256(*newX, x);

    // Ry = s(px - rx) - py
    uint256_t k;
    k = subModP256(px, *newX);
    *newY = mulModP256(s, k);
    *newY = subModP256(*newY, py);
  }
}


void complete_batch_add(
  uint256_t px,
  uint256_t py,
  uint256_t x,
  uint256_t y,
  int batchIdx,
  global uint256_t* chain,
  uint256_t* inverse,
  uint256_t* newX,
  uint256_t* newY)
{
  const int gid = get_global_id(0);
  const int dim = get_global_size(0);

  uint256_t s;

  if(batchIdx >= 1) {
    uint256_t c;

    c = chain[(batchIdx - 1) * dim + gid];
    s = mulModP256(*inverse, c);

    uint256_t diff;
    diff = subModP256(px, x);
    *inverse = mulModP256(diff, *inverse);
  } else {
    s = *inverse;
  }

  uint256_t rise;
  rise = subModP256(py, y);

  s = mulModP256(rise, s);

  // Rx = s^2 - Gx - Qx
  uint256_t s2;
  s2 = squareModP256k(s);

  *newX = subModP256(s2, px);
  *newX = subModP256(*newX, x);

  // Ry = s(px - rx) - py
  uint256_t k;
  k = subModP256(px, *newX);
  *newY = mulModP256(s, k);
  *newY = subModP256(*newY, py);
}

uint256_t batch_inverse(uint256_t x)
{
  return inverse_mod_p(x);
}

bool point_exists(uint256_t x, uint256_t y)
{
  uint256_t y2 = squareModP256k(y);

  uint256_t x2 = squareModP256k(x);
  uint256_t x3 = mulModP256(x, x2);

  uint256_t seven = make_uint256(7);
  x3 = addModP256(x3, seven);

  return equal256k(y2, x3);
}

kernel void init_public_keys(
  int totalPoints,
  int step,
  const global uint256_t* start_key,
  global uint256_t* chain,
  global uint256_t* gxPtr,
  global uint256_t* gyPtr,
  global uint256_t* xPtr,
  global uint256_t* yPtr)
{
  uint256_t gx;
  uint256_t gy;
  const int gid = get_global_id(0);
  const int dim = get_global_size(0);

  uint256_t base_key = *start_key;

  gx = gxPtr[step];
  gy = gyPtr[step];

  // Multiply together all (_Gx - x) and then invert
  uint256_t inverse = { {1, 0, 0, 0, 0, 0, 0, 0} };

  int batchIdx = 0;
  int i = gid;
  for(; i < totalPoints; i += dim) {

    uint256_t k = make_uint256(i);
    uint256_t private_key;

    add256(k.v, base_key.v, private_key.v);

    uint p = private_key.v[step / 32];

    uint bit = p & (1 << (step % 32));
    uint256_t x;
    x = xPtr[i];

    if(bit != 0) {
      if(!is_infinity(x)) {
        begin_batch_add_with_double(gx, gy, xPtr, chain, i, batchIdx, &inverse);
        batchIdx++;
      }
    }
  }

  inverse = batch_inverse(inverse);

  i -= dim;
  for(; i >= 0; i -= dim) {

    uint256_t newX;
    uint256_t newY;

    uint256_t k = make_uint256(i);
    uint256_t private_key;

    add256(k.v, base_key.v, private_key.v);

    uint p = private_key.v[step / 32];

    uint bit = p & (1 << (step % 32));

    uint256_t x = xPtr[i];

    bool infinity = is_infinity(x);

    if(bit != 0) {
      if(!infinity) {
        batchIdx--;
        complete_batch_add_with_double(gx, gy, xPtr, yPtr, i, batchIdx, chain, &inverse, &newX, &newY);
      } else {
        newX = gx;
        newY = gy;
      }
      xPtr[i] = newX;
      yPtr[i] = newY;
    }
  }
}



kernel void increment_keys(
  uint totalPoints,
  int compression,
  global uint256_t* chain,
  global uint256_t* xPtr,
  global uint256_t* yPtr,
  global uint256_t* incXPtr,
  global uint256_t* incYPtr)
{
  int gid = get_global_id(0);
  int dim = get_global_size(0);

  uint256_t incX = *incXPtr;
  uint256_t incY = *incYPtr;

  // Multiply together all (_Gx - x) and then invert
  uint256_t inverse = make_uint256(1);
  int i = gid;
  int batchIdx = 0;

  for(; i < totalPoints; i += dim) {
    uint256_t x = xPtr[i];

    begin_batch_add(incX, x, chain, batchIdx, &inverse);
    batchIdx++;
  }

  inverse = batch_inverse(inverse);

  i -= dim;

  for(; i >= dim; i -= dim) {

    uint256_t newX;
    uint256_t newY;
    uint256_t x = xPtr[i];
    uint256_t y = yPtr[i];
    batchIdx--;
    complete_batch_add(incX, incY, x, y, batchIdx, chain, &inverse, &newX, &newY);

    xPtr[i] = newX;
    yPtr[i] = newY;
  }
}


kernel void increment_keys_with_double(
  uint totalPoints,
  int compression,
  global uint256_t* chain,
  global uint256_t* xPtr,
  global uint256_t* yPtr,
  global uint256_t* incXPtr,
  global uint256_t* incYPtr)
{
  int gid = get_global_id(0);
  int dim = get_global_size(0);

  uint256_t incX = *incXPtr;
  uint256_t incY = *incYPtr;

  // Multiply together all (_Gx - x) and then invert
  uint256_t inverse = make_uint256(1);

  int i = gid;
  int batchIdx = 0;
  for(; i < totalPoints; i += dim) {

    uint256_t x = xPtr[i];

    begin_batch_add_with_double(incX, incY, xPtr, chain, i, batchIdx, &inverse);
    batchIdx++;
  }

  inverse = batch_inverse(inverse);

  i -= dim;

  for(; i >= dim; i -= dim) {
    uint256_t newX;
    uint256_t newY;
    batchIdx--;
    complete_batch_add_with_double(incX, incY, xPtr, yPtr, i, batchIdx, chain, &inverse, &newX, &newY);
    xPtr[i] = newX;
    yPtr[i] = newY;
  }
}

kernel void init_private_keys(const global uint256_t* start, global uint256_t* private_keys, uint total_keys)
{
  uint gid = get_global_id(0);
  uint dim = get_global_size(0);

  uint256_t base_key = *start;

  for(uint i = gid; i < total_keys; i += dim) {
    uint256_t k = make_uint256(i);
    uint256_t new_key;

    add256(k.v, base_key.v, new_key.v);

    private_keys[i] = new_key;
  }
}
