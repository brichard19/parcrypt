
constant uint _K[64] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

constant uint _IV[8] = {
  0x6a09e667,
  0xbb67ae85,
  0x3c6ef372,
  0xa54ff53a,
  0x510e527f,
  0x9b05688c,
  0x1f83d9ab,
  0x5be0cd19
};

#define rotr(x, n) ((x) >> (n)) ^ ((x) << (32 - (n)))

#define MAJ(a, b, c) (((a) & (b)) ^ ((a) & (c)) ^ ((b) & (c)))

#define CH(e, f, g) (((e) & (f)) ^ (~(e) & (g)))

#define s0(x) (rotr((x), 7) ^ rotr((x), 18) ^ ((x) >> 3))

#define s1(x) (rotr((x), 17) ^ rotr((x), 19) ^ ((x) >> 10))

#define s256rnd(a, b, c, d, e, f, g, h, m, k)\
    t = CH((e), (f), (g)) + (rotr((e), 6) ^ rotr((e), 11) ^ rotr((e), 25)) + (k) + (m);\
    (d) += (t) + (h);\
    (h) += (t) + MAJ((a), (b), (c)) + (rotr((a), 2) ^ rotr((a), 13) ^ rotr((a), 22))


void sha256_key_uncompressed(const uint x[8], const uint y[8], uint digest[8])
{
  uint a, b, c, d, e, f, g, h;
  uint w[16];
  uint t;

  // 0x04 || x || y
  w[0] = (x[7] >> 8) | 0x04000000;
  w[1] = (x[6] >> 8) | (x[7] << 24);
  w[2] = (x[5] >> 8) | (x[6] << 24);
  w[3] = (x[4] >> 8) | (x[5] << 24);
  w[4] = (x[3] >> 8) | (x[4] << 24);
  w[5] = (x[2] >> 8) | (x[3] << 24);
  w[6] = (x[1] >> 8) | (x[2] << 24);
  w[7] = (x[0] >> 8) | (x[1] << 24);
  w[8] = (y[7] >> 8) | (x[0] << 24);
  w[9] = (y[6] >> 8) | (y[7] << 24);
  w[10] = (y[5] >> 8) | (y[6] << 24);
  w[11] = (y[4] >> 8) | (y[5] << 24);
  w[12] = (y[3] >> 8) | (y[4] << 24);
  w[13] = (y[2] >> 8) | (y[3] << 24);
  w[14] = (y[1] >> 8) | (y[2] << 24);
  w[15] = (y[0] >> 8) | (y[1] << 24);

  a = _IV[0];
  b = _IV[1];
  c = _IV[2];
  d = _IV[3];
  e = _IV[4];
  f = _IV[5];
  g = _IV[6];
  h = _IV[7];

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[0]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[1]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[2]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[3]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[4]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[5]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[6]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[7]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[8]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[9]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[10]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[11]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[12]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[13]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[14]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[15]);

  w[0] = w[0] + s0(w[1]) + w[9] + s1(w[14]);
  w[1] = w[1] + s0(w[2]) + w[10] + s1(w[15]);
  w[2] = w[2] + s0(w[3]) + w[11] + s1(w[0]);
  w[3] = w[3] + s0(w[4]) + w[12] + s1(w[1]);
  w[4] = w[4] + s0(w[5]) + w[13] + s1(w[2]);
  w[5] = w[5] + s0(w[6]) + w[14] + s1(w[3]);
  w[6] = w[6] + s0(w[7]) + w[15] + s1(w[4]);
  w[7] = w[7] + s0(w[8]) + w[0] + s1(w[5]);
  w[8] = w[8] + s0(w[9]) + w[1] + s1(w[6]);
  w[9] = w[9] + s0(w[10]) + w[2] + s1(w[7]);
  w[10] = w[10] + s0(w[11]) + w[3] + s1(w[8]);
  w[11] = w[11] + s0(w[12]) + w[4] + s1(w[9]);
  w[12] = w[12] + s0(w[13]) + w[5] + s1(w[10]);
  w[13] = w[13] + s0(w[14]) + w[6] + s1(w[11]);
  w[14] = w[14] + s0(w[15]) + w[7] + s1(w[12]);
  w[15] = w[15] + s0(w[0]) + w[8] + s1(w[13]);

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[16]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[17]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[18]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[19]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[20]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[21]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[22]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[23]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[24]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[25]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[26]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[27]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[28]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[29]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[30]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[31]);

  w[0] = w[0] + s0(w[1]) + w[9] + s1(w[14]);
  w[1] = w[1] + s0(w[2]) + w[10] + s1(w[15]);
  w[2] = w[2] + s0(w[3]) + w[11] + s1(w[0]);
  w[3] = w[3] + s0(w[4]) + w[12] + s1(w[1]);
  w[4] = w[4] + s0(w[5]) + w[13] + s1(w[2]);
  w[5] = w[5] + s0(w[6]) + w[14] + s1(w[3]);
  w[6] = w[6] + s0(w[7]) + w[15] + s1(w[4]);
  w[7] = w[7] + s0(w[8]) + w[0] + s1(w[5]);
  w[8] = w[8] + s0(w[9]) + w[1] + s1(w[6]);
  w[9] = w[9] + s0(w[10]) + w[2] + s1(w[7]);
  w[10] = w[10] + s0(w[11]) + w[3] + s1(w[8]);
  w[11] = w[11] + s0(w[12]) + w[4] + s1(w[9]);
  w[12] = w[12] + s0(w[13]) + w[5] + s1(w[10]);
  w[13] = w[13] + s0(w[14]) + w[6] + s1(w[11]);
  w[14] = w[14] + s0(w[15]) + w[7] + s1(w[12]);
  w[15] = w[15] + s0(w[0]) + w[8] + s1(w[13]);

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[32]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[33]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[34]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[35]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[36]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[37]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[38]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[39]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[40]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[41]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[42]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[43]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[44]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[45]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[46]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[47]);

  w[0] = w[0] + s0(w[1]) + w[9] + s1(w[14]);
  w[1] = w[1] + s0(w[2]) + w[10] + s1(w[15]);
  w[2] = w[2] + s0(w[3]) + w[11] + s1(w[0]);
  w[3] = w[3] + s0(w[4]) + w[12] + s1(w[1]);
  w[4] = w[4] + s0(w[5]) + w[13] + s1(w[2]);
  w[5] = w[5] + s0(w[6]) + w[14] + s1(w[3]);
  w[6] = w[6] + s0(w[7]) + w[15] + s1(w[4]);
  w[7] = w[7] + s0(w[8]) + w[0] + s1(w[5]);
  w[8] = w[8] + s0(w[9]) + w[1] + s1(w[6]);
  w[9] = w[9] + s0(w[10]) + w[2] + s1(w[7]);
  w[10] = w[10] + s0(w[11]) + w[3] + s1(w[8]);
  w[11] = w[11] + s0(w[12]) + w[4] + s1(w[9]);
  w[12] = w[12] + s0(w[13]) + w[5] + s1(w[10]);
  w[13] = w[13] + s0(w[14]) + w[6] + s1(w[11]);
  w[14] = w[14] + s0(w[15]) + w[7] + s1(w[12]);
  w[15] = w[15] + s0(w[0]) + w[8] + s1(w[13]);

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[48]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[49]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[50]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[51]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[52]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[53]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[54]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[55]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[56]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[57]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[58]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[59]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[60]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[61]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[62]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[63]);

  a += _IV[0];
  b += _IV[1];
  c += _IV[2];
  d += _IV[3];
  e += _IV[4];
  f += _IV[5];
  g += _IV[6];
  h += _IV[7];

  // store the intermediate hash value
  uint tmp[8];
  tmp[0] = a;
  tmp[1] = b;
  tmp[2] = c;
  tmp[3] = d;
  tmp[4] = e;
  tmp[5] = f;
  tmp[6] = g;
  tmp[7] = h;

  w[0] = (y[0] << 24) | 0x00800000;
  w[15] = 65 * 8;

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[0]);
  s256rnd(h, a, b, c, d, e, f, g, 0, _K[1]);
  s256rnd(g, h, a, b, c, d, e, f, 0, _K[2]);
  s256rnd(f, g, h, a, b, c, d, e, 0, _K[3]);
  s256rnd(e, f, g, h, a, b, c, d, 0, _K[4]);
  s256rnd(d, e, f, g, h, a, b, c, 0, _K[5]);
  s256rnd(c, d, e, f, g, h, a, b, 0, _K[6]);
  s256rnd(b, c, d, e, f, g, h, a, 0, _K[7]);
  s256rnd(a, b, c, d, e, f, g, h, 0, _K[8]);
  s256rnd(h, a, b, c, d, e, f, g, 0, _K[9]);
  s256rnd(g, h, a, b, c, d, e, f, 0, _K[10]);
  s256rnd(f, g, h, a, b, c, d, e, 0, _K[11]);
  s256rnd(e, f, g, h, a, b, c, d, 0, _K[12]);
  s256rnd(d, e, f, g, h, a, b, c, 0, _K[13]);
  s256rnd(c, d, e, f, g, h, a, b, 0, _K[14]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[15]);

  w[0] = w[0] + s0(0) + 0 + s1(0);
  w[1] = 0 + s0(0) + 0 + s1(w[15]);
  w[2] = 0 + s0(0) + 0 + s1(w[0]);
  w[3] = 0 + s0(0) + 0 + s1(w[1]);
  w[4] = 0 + s0(0) + 0 + s1(w[2]);
  w[5] = 0 + s0(0) + 0 + s1(w[3]);
  w[6] = 0 + s0(0) + w[15] + s1(w[4]);
  w[7] = 0 + s0(0) + w[0] + s1(w[5]);
  w[8] = 0 + s0(0) + w[1] + s1(w[6]);
  w[9] = 0 + s0(0) + w[2] + s1(w[7]);
  w[10] = 0 + s0(0) + w[3] + s1(w[8]);
  w[11] = 0 + s0(0) + w[4] + s1(w[9]);
  w[12] = 0 + s0(0) + w[5] + s1(w[10]);
  w[13] = 0 + s0(0) + w[6] + s1(w[11]);
  w[14] = 0 + s0(w[15]) + w[7] + s1(w[12]);
  w[15] = w[15] + s0(w[0]) + w[8] + s1(w[13]);

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[16]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[17]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[18]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[19]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[20]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[21]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[22]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[23]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[24]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[25]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[26]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[27]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[28]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[29]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[30]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[31]);

  w[0] = w[0] + s0(w[1]) + w[9] + s1(w[14]);
  w[1] = w[1] + s0(w[2]) + w[10] + s1(w[15]);
  w[2] = w[2] + s0(w[3]) + w[11] + s1(w[0]);
  w[3] = w[3] + s0(w[4]) + w[12] + s1(w[1]);
  w[4] = w[4] + s0(w[5]) + w[13] + s1(w[2]);
  w[5] = w[5] + s0(w[6]) + w[14] + s1(w[3]);
  w[6] = w[6] + s0(w[7]) + w[15] + s1(w[4]);
  w[7] = w[7] + s0(w[8]) + w[0] + s1(w[5]);
  w[8] = w[8] + s0(w[9]) + w[1] + s1(w[6]);
  w[9] = w[9] + s0(w[10]) + w[2] + s1(w[7]);
  w[10] = w[10] + s0(w[11]) + w[3] + s1(w[8]);
  w[11] = w[11] + s0(w[12]) + w[4] + s1(w[9]);
  w[12] = w[12] + s0(w[13]) + w[5] + s1(w[10]);
  w[13] = w[13] + s0(w[14]) + w[6] + s1(w[11]);
  w[14] = w[14] + s0(w[15]) + w[7] + s1(w[12]);
  w[15] = w[15] + s0(w[0]) + w[8] + s1(w[13]);

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[32]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[33]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[34]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[35]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[36]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[37]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[38]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[39]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[40]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[41]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[42]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[43]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[44]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[45]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[46]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[47]);

  w[0] = w[0] + s0(w[1]) + w[9] + s1(w[14]);
  w[1] = w[1] + s0(w[2]) + w[10] + s1(w[15]);
  w[2] = w[2] + s0(w[3]) + w[11] + s1(w[0]);
  w[3] = w[3] + s0(w[4]) + w[12] + s1(w[1]);
  w[4] = w[4] + s0(w[5]) + w[13] + s1(w[2]);
  w[5] = w[5] + s0(w[6]) + w[14] + s1(w[3]);
  w[6] = w[6] + s0(w[7]) + w[15] + s1(w[4]);
  w[7] = w[7] + s0(w[8]) + w[0] + s1(w[5]);
  w[8] = w[8] + s0(w[9]) + w[1] + s1(w[6]);
  w[9] = w[9] + s0(w[10]) + w[2] + s1(w[7]);
  w[10] = w[10] + s0(w[11]) + w[3] + s1(w[8]);
  w[11] = w[11] + s0(w[12]) + w[4] + s1(w[9]);
  w[12] = w[12] + s0(w[13]) + w[5] + s1(w[10]);
  w[13] = w[13] + s0(w[14]) + w[6] + s1(w[11]);
  w[14] = w[14] + s0(w[15]) + w[7] + s1(w[12]);
  w[15] = w[15] + s0(w[0]) + w[8] + s1(w[13]);

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[48]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[49]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[50]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[51]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[52]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[53]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[54]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[55]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[56]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[57]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[58]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[59]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[60]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[61]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[62]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[63]);

  digest[0] = tmp[0] + a;
  digest[1] = tmp[1] + b;
  digest[2] = tmp[2] + c;
  digest[3] = tmp[3] + d;
  digest[4] = tmp[4] + e;
  digest[5] = tmp[5] + f;
  digest[6] = tmp[6] + g;
  digest[7] = tmp[7] + h;
}

void sha256_key_compressed(const uint x[8], uint yParity, uint digest[8])
{
  uint a, b, c, d, e, f, g, h;
  uint w[16];
  uint t;

  // 0x03 || x  or  0x02 || x
  w[0] = 0x02000000 | ((yParity & 1) << 24) | (x[7] >> 8);

  w[1] = (x[6] >> 8) | (x[7] << 24);
  w[2] = (x[5] >> 8) | (x[6] << 24);
  w[3] = (x[4] >> 8) | (x[5] << 24);
  w[4] = (x[3] >> 8) | (x[4] << 24);
  w[5] = (x[2] >> 8) | (x[3] << 24);
  w[6] = (x[1] >> 8) | (x[2] << 24);
  w[7] = (x[0] >> 8) | (x[1] << 24);
  w[8] = (x[0] << 24) | 0x00800000;
  w[15] = 33 * 8;

  a = _IV[0];
  b = _IV[1];
  c = _IV[2];
  d = _IV[3];
  e = _IV[4];
  f = _IV[5];
  g = _IV[6];
  h = _IV[7];

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[0]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[1]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[2]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[3]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[4]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[5]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[6]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[7]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[8]);
  s256rnd(h, a, b, c, d, e, f, g, 0, _K[9]);
  s256rnd(g, h, a, b, c, d, e, f, 0, _K[10]);
  s256rnd(f, g, h, a, b, c, d, e, 0, _K[11]);
  s256rnd(e, f, g, h, a, b, c, d, 0, _K[12]);
  s256rnd(d, e, f, g, h, a, b, c, 0, _K[13]);
  s256rnd(c, d, e, f, g, h, a, b, 0, _K[14]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[15]);

  w[0] = w[0] + s0(w[1]) + 0 + s1(0);
  w[1] = w[1] + s0(w[2]) + 0 + s1(w[15]);
  w[2] = w[2] + s0(w[3]) + 0 + s1(w[0]);
  w[3] = w[3] + s0(w[4]) + 0 + s1(w[1]);
  w[4] = w[4] + s0(w[5]) + 0 + s1(w[2]);
  w[5] = w[5] + s0(w[6]) + 0 + s1(w[3]);
  w[6] = w[6] + s0(w[7]) + w[15] + s1(w[4]);
  w[7] = w[7] + s0(w[8]) + w[0] + s1(w[5]);
  w[8] = w[8] + s0(0) + w[1] + s1(w[6]);
  w[9] = 0 + s0(0) + w[2] + s1(w[7]);
  w[10] = 0 + s0(0) + w[3] + s1(w[8]);
  w[11] = 0 + s0(0) + w[4] + s1(w[9]);
  w[12] = 0 + s0(0) + w[5] + s1(w[10]);
  w[13] = 0 + s0(0) + w[6] + s1(w[11]);
  w[14] = 0 + s0(w[15]) + w[7] + s1(w[12]);
  w[15] = w[15] + s0(w[0]) + w[8] + s1(w[13]);

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[16]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[17]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[18]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[19]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[20]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[21]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[22]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[23]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[24]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[25]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[26]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[27]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[28]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[29]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[30]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[31]);

  w[0] = w[0] + s0(w[1]) + w[9] + s1(w[14]);
  w[1] = w[1] + s0(w[2]) + w[10] + s1(w[15]);
  w[2] = w[2] + s0(w[3]) + w[11] + s1(w[0]);
  w[3] = w[3] + s0(w[4]) + w[12] + s1(w[1]);
  w[4] = w[4] + s0(w[5]) + w[13] + s1(w[2]);
  w[5] = w[5] + s0(w[6]) + w[14] + s1(w[3]);
  w[6] = w[6] + s0(w[7]) + w[15] + s1(w[4]);
  w[7] = w[7] + s0(w[8]) + w[0] + s1(w[5]);
  w[8] = w[8] + s0(w[9]) + w[1] + s1(w[6]);
  w[9] = w[9] + s0(w[10]) + w[2] + s1(w[7]);
  w[10] = w[10] + s0(w[11]) + w[3] + s1(w[8]);
  w[11] = w[11] + s0(w[12]) + w[4] + s1(w[9]);
  w[12] = w[12] + s0(w[13]) + w[5] + s1(w[10]);
  w[13] = w[13] + s0(w[14]) + w[6] + s1(w[11]);
  w[14] = w[14] + s0(w[15]) + w[7] + s1(w[12]);
  w[15] = w[15] + s0(w[0]) + w[8] + s1(w[13]);

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[32]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[33]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[34]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[35]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[36]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[37]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[38]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[39]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[40]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[41]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[42]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[43]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[44]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[45]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[46]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[47]);


  w[0] = w[0] + s0(w[1]) + w[9] + s1(w[14]);
  w[1] = w[1] + s0(w[2]) + w[10] + s1(w[15]);
  w[2] = w[2] + s0(w[3]) + w[11] + s1(w[0]);
  w[3] = w[3] + s0(w[4]) + w[12] + s1(w[1]);
  w[4] = w[4] + s0(w[5]) + w[13] + s1(w[2]);
  w[5] = w[5] + s0(w[6]) + w[14] + s1(w[3]);
  w[6] = w[6] + s0(w[7]) + w[15] + s1(w[4]);
  w[7] = w[7] + s0(w[8]) + w[0] + s1(w[5]);
  w[8] = w[8] + s0(w[9]) + w[1] + s1(w[6]);
  w[9] = w[9] + s0(w[10]) + w[2] + s1(w[7]);
  w[10] = w[10] + s0(w[11]) + w[3] + s1(w[8]);
  w[11] = w[11] + s0(w[12]) + w[4] + s1(w[9]);
  w[12] = w[12] + s0(w[13]) + w[5] + s1(w[10]);
  w[13] = w[13] + s0(w[14]) + w[6] + s1(w[11]);
  w[14] = w[14] + s0(w[15]) + w[7] + s1(w[12]);
  w[15] = w[15] + s0(w[0]) + w[8] + s1(w[13]);

  s256rnd(a, b, c, d, e, f, g, h, w[0], _K[48]);
  s256rnd(h, a, b, c, d, e, f, g, w[1], _K[49]);
  s256rnd(g, h, a, b, c, d, e, f, w[2], _K[50]);
  s256rnd(f, g, h, a, b, c, d, e, w[3], _K[51]);
  s256rnd(e, f, g, h, a, b, c, d, w[4], _K[52]);
  s256rnd(d, e, f, g, h, a, b, c, w[5], _K[53]);
  s256rnd(c, d, e, f, g, h, a, b, w[6], _K[54]);
  s256rnd(b, c, d, e, f, g, h, a, w[7], _K[55]);
  s256rnd(a, b, c, d, e, f, g, h, w[8], _K[56]);
  s256rnd(h, a, b, c, d, e, f, g, w[9], _K[57]);
  s256rnd(g, h, a, b, c, d, e, f, w[10], _K[58]);
  s256rnd(f, g, h, a, b, c, d, e, w[11], _K[59]);
  s256rnd(e, f, g, h, a, b, c, d, w[12], _K[60]);
  s256rnd(d, e, f, g, h, a, b, c, w[13], _K[61]);
  s256rnd(c, d, e, f, g, h, a, b, w[14], _K[62]);
  s256rnd(b, c, d, e, f, g, h, a, w[15], _K[63]);

  a += _IV[0];
  b += _IV[1];
  c += _IV[2];
  d += _IV[3];
  e += _IV[4];
  f += _IV[5];
  g += _IV[6];
  h += _IV[7];

  digest[0] = a;
  digest[1] = b;
  digest[2] = c;
  digest[3] = d;
  digest[4] = e;
  digest[5] = f;
  digest[6] = g;
  digest[7] = h;
}







#define _RIPEMD160_IV0 0x67452301
#define _RIPEMD160_IV1 0xefcdab89
#define _RIPEMD160_IV2 0x98badcfe
#define _RIPEMD160_IV3 0x10325476
#define _RIPEMD160_IV4 0xc3d2e1f0

#define _K0 0x5a827999
#define _K1 0x6ed9eba1
#define _K2 0x8f1bbcdc
#define _K3 0xa953fd4e

#define _K4 0x7a6d76e9
#define _K5 0x6d703ef3
#define _K6 0x5c4dd124
#define _K7 0x50a28be6

#define rotl(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define F(x, y, z) ((x) ^ (y) ^ (z))

#define G(x, y, z) (((x) & (y)) | (~(x) & (z)))

#define H(x, y, z) (((x) | ~(y)) ^ (z))

#define I(x, y, z) (((x) & (z)) | ((y) & ~(z)))

#define J(x, y, z) ((x) ^ ((y) | ~(z)))

#define FF(a, b, c, d, e, m, s)\
    a += (F((b), (c), (d)) + (m));\
    a = (rotl((a), (s)) + (e));\
    c = rotl((c), 10)

#define GG(a, b, c, d, e, x, s)\
    a += G((b), (c), (d)) + (x) + _K0;\
    a = rotl((a), (s)) + (e);\
    c = rotl((c), 10)

#define HH(a, b, c, d, e, x, s)\
    a += H((b), (c), (d)) + (x) + _K1;\
    a = rotl((a), (s)) + (e);\
    c = rotl((c), 10)

#define II(a, b, c, d, e, x, s)\
    a += I((b), (c), (d)) + (x) + _K2;\
    a = rotl((a), (s)) + e;\
    c = rotl((c), 10)

#define JJ(a, b, c, d, e, x, s)\
    a += J((b), (c), (d)) + (x) + _K3;\
    a = rotl((a), (s)) + (e);\
    c = rotl((c), 10)

#define FFF(a, b, c, d, e, x, s)\
    a += F((b), (c), (d)) + (x);\
    a = rotl((a), (s)) + (e);\
    c = rotl((c), 10)

#define GGG(a, b, c, d, e, x, s)\
    a += G((b), (c), (d)) + x + _K4;\
    a = rotl((a), (s)) + (e);\
    c = rotl((c), 10)

#define HHH(a, b, c, d, e, x, s)\
    a += H((b), (c), (d)) + (x) + _K5;\
    a = rotl((a), (s)) + (e);\
    c = rotl((c), 10)

#define III(a, b, c, d, e, x, s)\
    a += I((b), (c), (d)) + (x) + _K6;\
    a = rotl((a), (s)) + (e);\
    c = rotl((c), 10)

#define JJJ(a, b, c, d, e, x, s)\
    a += J((b), (c), (d)) + (x) + _K7;\
    a = rotl((a), (s)) + (e);\
    c = rotl((c), 10)


void ripemd160_sha256(const uint x[8], uint digest[5])
{
  uint a1 = _RIPEMD160_IV0;
  uint b1 = _RIPEMD160_IV1;
  uint c1 = _RIPEMD160_IV2;
  uint d1 = _RIPEMD160_IV3;
  uint e1 = _RIPEMD160_IV4;

  const uint x8 = 0x00000080;
  const uint x14 = 256;

  /* s256rnd 1 */
  FF(a1, b1, c1, d1, e1, x[0], 11);
  FF(e1, a1, b1, c1, d1, x[1], 14);
  FF(d1, e1, a1, b1, c1, x[2], 15);
  FF(c1, d1, e1, a1, b1, x[3], 12);
  FF(b1, c1, d1, e1, a1, x[4], 5);
  FF(a1, b1, c1, d1, e1, x[5], 8);
  FF(e1, a1, b1, c1, d1, x[6], 7);
  FF(d1, e1, a1, b1, c1, x[7], 9);
  FF(c1, d1, e1, a1, b1, x8, 11);
  FF(b1, c1, d1, e1, a1, 0, 13);
  FF(a1, b1, c1, d1, e1, 0, 14);
  FF(e1, a1, b1, c1, d1, 0, 15);
  FF(d1, e1, a1, b1, c1, 0, 6);
  FF(c1, d1, e1, a1, b1, 0, 7);
  FF(b1, c1, d1, e1, a1, x14, 9);
  FF(a1, b1, c1, d1, e1, 0, 8);

  /* s256rnd 2 */
  GG(e1, a1, b1, c1, d1, x[7], 7);
  GG(d1, e1, a1, b1, c1, x[4], 6);
  GG(c1, d1, e1, a1, b1, 0, 8);
  GG(b1, c1, d1, e1, a1, x[1], 13);
  GG(a1, b1, c1, d1, e1, 0, 11);
  GG(e1, a1, b1, c1, d1, x[6], 9);
  GG(d1, e1, a1, b1, c1, 0, 7);
  GG(c1, d1, e1, a1, b1, x[3], 15);
  GG(b1, c1, d1, e1, a1, 0, 7);
  GG(a1, b1, c1, d1, e1, x[0], 12);
  GG(e1, a1, b1, c1, d1, 0, 15);
  GG(d1, e1, a1, b1, c1, x[5], 9);
  GG(c1, d1, e1, a1, b1, x[2], 11);
  GG(b1, c1, d1, e1, a1, x14, 7);
  GG(a1, b1, c1, d1, e1, 0, 13);
  GG(e1, a1, b1, c1, d1, x8, 12);

  /* s256rnd 3 */
  HH(d1, e1, a1, b1, c1, x[3], 11);
  HH(c1, d1, e1, a1, b1, 0, 13);
  HH(b1, c1, d1, e1, a1, x14, 6);
  HH(a1, b1, c1, d1, e1, x[4], 7);
  HH(e1, a1, b1, c1, d1, 0, 14);
  HH(d1, e1, a1, b1, c1, 0, 9);
  HH(c1, d1, e1, a1, b1, x8, 13);
  HH(b1, c1, d1, e1, a1, x[1], 15);
  HH(a1, b1, c1, d1, e1, x[2], 14);
  HH(e1, a1, b1, c1, d1, x[7], 8);
  HH(d1, e1, a1, b1, c1, x[0], 13);
  HH(c1, d1, e1, a1, b1, x[6], 6);
  HH(b1, c1, d1, e1, a1, 0, 5);
  HH(a1, b1, c1, d1, e1, 0, 12);
  HH(e1, a1, b1, c1, d1, x[5], 7);
  HH(d1, e1, a1, b1, c1, 0, 5);

  /* s256rnd 4 */
  II(c1, d1, e1, a1, b1, x[1], 11);
  II(b1, c1, d1, e1, a1, 0, 12);
  II(a1, b1, c1, d1, e1, 0, 14);
  II(e1, a1, b1, c1, d1, 0, 15);
  II(d1, e1, a1, b1, c1, x[0], 14);
  II(c1, d1, e1, a1, b1, x8, 15);
  II(b1, c1, d1, e1, a1, 0, 9);
  II(a1, b1, c1, d1, e1, x[4], 8);
  II(e1, a1, b1, c1, d1, 0, 9);
  II(d1, e1, a1, b1, c1, x[3], 14);
  II(c1, d1, e1, a1, b1, x[7], 5);
  II(b1, c1, d1, e1, a1, 0, 6);
  II(a1, b1, c1, d1, e1, x14, 8);
  II(e1, a1, b1, c1, d1, x[5], 6);
  II(d1, e1, a1, b1, c1, x[6], 5);
  II(c1, d1, e1, a1, b1, x[2], 12);

  /* s256rnd 5 */
  JJ(b1, c1, d1, e1, a1, x[4], 9);
  JJ(a1, b1, c1, d1, e1, x[0], 15);
  JJ(e1, a1, b1, c1, d1, x[5], 5);
  JJ(d1, e1, a1, b1, c1, 0, 11);
  JJ(c1, d1, e1, a1, b1, x[7], 6);
  JJ(b1, c1, d1, e1, a1, 0, 8);
  JJ(a1, b1, c1, d1, e1, x[2], 13);
  JJ(e1, a1, b1, c1, d1, 0, 12);
  JJ(d1, e1, a1, b1, c1, x14, 5);
  JJ(c1, d1, e1, a1, b1, x[1], 12);
  JJ(b1, c1, d1, e1, a1, x[3], 13);
  JJ(a1, b1, c1, d1, e1, x8, 14);
  JJ(e1, a1, b1, c1, d1, 0, 11);
  JJ(d1, e1, a1, b1, c1, x[6], 8);
  JJ(c1, d1, e1, a1, b1, 0, 5);
  JJ(b1, c1, d1, e1, a1, 0, 6);

  uint a2 = _RIPEMD160_IV0;
  uint b2 = _RIPEMD160_IV1;
  uint c2 = _RIPEMD160_IV2;
  uint d2 = _RIPEMD160_IV3;
  uint e2 = _RIPEMD160_IV4;

  /* parallel s256rnd 1 */
  JJJ(a2, b2, c2, d2, e2, x[5], 8);
  JJJ(e2, a2, b2, c2, d2, x14, 9);
  JJJ(d2, e2, a2, b2, c2, x[7], 9);
  JJJ(c2, d2, e2, a2, b2, x[0], 11);
  JJJ(b2, c2, d2, e2, a2, 0, 13);
  JJJ(a2, b2, c2, d2, e2, x[2], 15);
  JJJ(e2, a2, b2, c2, d2, 0, 15);
  JJJ(d2, e2, a2, b2, c2, x[4], 5);
  JJJ(c2, d2, e2, a2, b2, 0, 7);
  JJJ(b2, c2, d2, e2, a2, x[6], 7);
  JJJ(a2, b2, c2, d2, e2, 0, 8);
  JJJ(e2, a2, b2, c2, d2, x8, 11);
  JJJ(d2, e2, a2, b2, c2, x[1], 14);
  JJJ(c2, d2, e2, a2, b2, 0, 14);
  JJJ(b2, c2, d2, e2, a2, x[3], 12);
  JJJ(a2, b2, c2, d2, e2, 0, 6);

  /* parallel s256rnd 2 */
  III(e2, a2, b2, c2, d2, x[6], 9);
  III(d2, e2, a2, b2, c2, 0, 13);
  III(c2, d2, e2, a2, b2, x[3], 15);
  III(b2, c2, d2, e2, a2, x[7], 7);
  III(a2, b2, c2, d2, e2, x[0], 12);
  III(e2, a2, b2, c2, d2, 0, 8);
  III(d2, e2, a2, b2, c2, x[5], 9);
  III(c2, d2, e2, a2, b2, 0, 11);
  III(b2, c2, d2, e2, a2, x14, 7);
  III(a2, b2, c2, d2, e2, 0, 7);
  III(e2, a2, b2, c2, d2, x8, 12);
  III(d2, e2, a2, b2, c2, 0, 7);
  III(c2, d2, e2, a2, b2, x[4], 6);
  III(b2, c2, d2, e2, a2, 0, 15);
  III(a2, b2, c2, d2, e2, x[1], 13);
  III(e2, a2, b2, c2, d2, x[2], 11);

  /* parallel s256rnd 3 */
  HHH(d2, e2, a2, b2, c2, 0, 9);
  HHH(c2, d2, e2, a2, b2, x[5], 7);
  HHH(b2, c2, d2, e2, a2, x[1], 15);
  HHH(a2, b2, c2, d2, e2, x[3], 11);
  HHH(e2, a2, b2, c2, d2, x[7], 8);
  HHH(d2, e2, a2, b2, c2, x14, 6);
  HHH(c2, d2, e2, a2, b2, x[6], 6);
  HHH(b2, c2, d2, e2, a2, 0, 14);
  HHH(a2, b2, c2, d2, e2, 0, 12);
  HHH(e2, a2, b2, c2, d2, x8, 13);
  HHH(d2, e2, a2, b2, c2, 0, 5);
  HHH(c2, d2, e2, a2, b2, x[2], 14);
  HHH(b2, c2, d2, e2, a2, 0, 13);
  HHH(a2, b2, c2, d2, e2, x[0], 13);
  HHH(e2, a2, b2, c2, d2, x[4], 7);
  HHH(d2, e2, a2, b2, c2, 0, 5);

  /* parallel s256rnd 4 */
  GGG(c2, d2, e2, a2, b2, x8, 15);
  GGG(b2, c2, d2, e2, a2, x[6], 5);
  GGG(a2, b2, c2, d2, e2, x[4], 8);
  GGG(e2, a2, b2, c2, d2, x[1], 11);
  GGG(d2, e2, a2, b2, c2, x[3], 14);
  GGG(c2, d2, e2, a2, b2, 0, 14);
  GGG(b2, c2, d2, e2, a2, 0, 6);
  GGG(a2, b2, c2, d2, e2, x[0], 14);
  GGG(e2, a2, b2, c2, d2, x[5], 6);
  GGG(d2, e2, a2, b2, c2, 0, 9);
  GGG(c2, d2, e2, a2, b2, x[2], 12);
  GGG(b2, c2, d2, e2, a2, 0, 9);
  GGG(a2, b2, c2, d2, e2, 0, 12);
  GGG(e2, a2, b2, c2, d2, x[7], 5);
  GGG(d2, e2, a2, b2, c2, 0, 15);
  GGG(c2, d2, e2, a2, b2, x14, 8);

  /* parallel s256rnd 5 */
  FFF(b2, c2, d2, e2, a2, 0, 8);
  FFF(a2, b2, c2, d2, e2, 0, 5);
  FFF(e2, a2, b2, c2, d2, 0, 12);
  FFF(d2, e2, a2, b2, c2, x[4], 9);
  FFF(c2, d2, e2, a2, b2, x[1], 12);
  FFF(b2, c2, d2, e2, a2, x[5], 5);
  FFF(a2, b2, c2, d2, e2, x8, 14);
  FFF(e2, a2, b2, c2, d2, x[7], 6);
  FFF(d2, e2, a2, b2, c2, x[6], 8);
  FFF(c2, d2, e2, a2, b2, x[2], 13);
  FFF(b2, c2, d2, e2, a2, 0, 6);
  FFF(a2, b2, c2, d2, e2, x14, 5);
  FFF(e2, a2, b2, c2, d2, x[0], 15);
  FFF(d2, e2, a2, b2, c2, x[3], 13);
  FFF(c2, d2, e2, a2, b2, 0, 11);
  FFF(b2, c2, d2, e2, a2, 0, 11);

  digest[0] = _RIPEMD160_IV1 + c1 + d2;
  digest[1] = _RIPEMD160_IV2 + d1 + e2;
  digest[2] = _RIPEMD160_IV3 + e1 + a2;
  digest[3] = _RIPEMD160_IV4 + a1 + b2;
  digest[4] = _RIPEMD160_IV0 + b1 + c2;
}


void ripemd160_sha256_no_final(const uint* x, uint* digest)
{
  uint a1 = _RIPEMD160_IV0;
  uint b1 = _RIPEMD160_IV1;
  uint c1 = _RIPEMD160_IV2;
  uint d1 = _RIPEMD160_IV3;
  uint e1 = _RIPEMD160_IV4;

  const uint x8 = 0x00000080;
  const uint x14 = 256;

  /* s256rnd 1 */
  FF(a1, b1, c1, d1, e1, x[0], 11);
  FF(e1, a1, b1, c1, d1, x[1], 14);
  FF(d1, e1, a1, b1, c1, x[2], 15);
  FF(c1, d1, e1, a1, b1, x[3], 12);
  FF(b1, c1, d1, e1, a1, x[4], 5);
  FF(a1, b1, c1, d1, e1, x[5], 8);
  FF(e1, a1, b1, c1, d1, x[6], 7);
  FF(d1, e1, a1, b1, c1, x[7], 9);
  FF(c1, d1, e1, a1, b1, x8, 11);
  FF(b1, c1, d1, e1, a1, 0, 13);
  FF(a1, b1, c1, d1, e1, 0, 14);
  FF(e1, a1, b1, c1, d1, 0, 15);
  FF(d1, e1, a1, b1, c1, 0, 6);
  FF(c1, d1, e1, a1, b1, 0, 7);
  FF(b1, c1, d1, e1, a1, x14, 9);
  FF(a1, b1, c1, d1, e1, 0, 8);

  /* s256rnd 2 */
  GG(e1, a1, b1, c1, d1, x[7], 7);
  GG(d1, e1, a1, b1, c1, x[4], 6);
  GG(c1, d1, e1, a1, b1, 0, 8);
  GG(b1, c1, d1, e1, a1, x[1], 13);
  GG(a1, b1, c1, d1, e1, 0, 11);
  GG(e1, a1, b1, c1, d1, x[6], 9);
  GG(d1, e1, a1, b1, c1, 0, 7);
  GG(c1, d1, e1, a1, b1, x[3], 15);
  GG(b1, c1, d1, e1, a1, 0, 7);
  GG(a1, b1, c1, d1, e1, x[0], 12);
  GG(e1, a1, b1, c1, d1, 0, 15);
  GG(d1, e1, a1, b1, c1, x[5], 9);
  GG(c1, d1, e1, a1, b1, x[2], 11);
  GG(b1, c1, d1, e1, a1, x14, 7);
  GG(a1, b1, c1, d1, e1, 0, 13);
  GG(e1, a1, b1, c1, d1, x8, 12);

  /* s256rnd 3 */
  HH(d1, e1, a1, b1, c1, x[3], 11);
  HH(c1, d1, e1, a1, b1, 0, 13);
  HH(b1, c1, d1, e1, a1, x14, 6);
  HH(a1, b1, c1, d1, e1, x[4], 7);
  HH(e1, a1, b1, c1, d1, 0, 14);
  HH(d1, e1, a1, b1, c1, 0, 9);
  HH(c1, d1, e1, a1, b1, x8, 13);
  HH(b1, c1, d1, e1, a1, x[1], 15);
  HH(a1, b1, c1, d1, e1, x[2], 14);
  HH(e1, a1, b1, c1, d1, x[7], 8);
  HH(d1, e1, a1, b1, c1, x[0], 13);
  HH(c1, d1, e1, a1, b1, x[6], 6);
  HH(b1, c1, d1, e1, a1, 0, 5);
  HH(a1, b1, c1, d1, e1, 0, 12);
  HH(e1, a1, b1, c1, d1, x[5], 7);
  HH(d1, e1, a1, b1, c1, 0, 5);

  /* s256rnd 4 */
  II(c1, d1, e1, a1, b1, x[1], 11);
  II(b1, c1, d1, e1, a1, 0, 12);
  II(a1, b1, c1, d1, e1, 0, 14);
  II(e1, a1, b1, c1, d1, 0, 15);
  II(d1, e1, a1, b1, c1, x[0], 14);
  II(c1, d1, e1, a1, b1, x8, 15);
  II(b1, c1, d1, e1, a1, 0, 9);
  II(a1, b1, c1, d1, e1, x[4], 8);
  II(e1, a1, b1, c1, d1, 0, 9);
  II(d1, e1, a1, b1, c1, x[3], 14);
  II(c1, d1, e1, a1, b1, x[7], 5);
  II(b1, c1, d1, e1, a1, 0, 6);
  II(a1, b1, c1, d1, e1, x14, 8);
  II(e1, a1, b1, c1, d1, x[5], 6);
  II(d1, e1, a1, b1, c1, x[6], 5);
  II(c1, d1, e1, a1, b1, x[2], 12);

  /* s256rnd 5 */
  JJ(b1, c1, d1, e1, a1, x[4], 9);
  JJ(a1, b1, c1, d1, e1, x[0], 15);
  JJ(e1, a1, b1, c1, d1, x[5], 5);
  JJ(d1, e1, a1, b1, c1, 0, 11);
  JJ(c1, d1, e1, a1, b1, x[7], 6);
  JJ(b1, c1, d1, e1, a1, 0, 8);
  JJ(a1, b1, c1, d1, e1, x[2], 13);
  JJ(e1, a1, b1, c1, d1, 0, 12);
  JJ(d1, e1, a1, b1, c1, x14, 5);
  JJ(c1, d1, e1, a1, b1, x[1], 12);
  JJ(b1, c1, d1, e1, a1, x[3], 13);
  JJ(a1, b1, c1, d1, e1, x8, 14);
  JJ(e1, a1, b1, c1, d1, 0, 11);
  JJ(d1, e1, a1, b1, c1, x[6], 8);
  JJ(c1, d1, e1, a1, b1, 0, 5);
  JJ(b1, c1, d1, e1, a1, 0, 6);

  uint a2 = _RIPEMD160_IV0;
  uint b2 = _RIPEMD160_IV1;
  uint c2 = _RIPEMD160_IV2;
  uint d2 = _RIPEMD160_IV3;
  uint e2 = _RIPEMD160_IV4;

  /* parallel s256rnd 1 */
  JJJ(a2, b2, c2, d2, e2, x[5], 8);
  JJJ(e2, a2, b2, c2, d2, x14, 9);
  JJJ(d2, e2, a2, b2, c2, x[7], 9);
  JJJ(c2, d2, e2, a2, b2, x[0], 11);
  JJJ(b2, c2, d2, e2, a2, 0, 13);
  JJJ(a2, b2, c2, d2, e2, x[2], 15);
  JJJ(e2, a2, b2, c2, d2, 0, 15);
  JJJ(d2, e2, a2, b2, c2, x[4], 5);
  JJJ(c2, d2, e2, a2, b2, 0, 7);
  JJJ(b2, c2, d2, e2, a2, x[6], 7);
  JJJ(a2, b2, c2, d2, e2, 0, 8);
  JJJ(e2, a2, b2, c2, d2, x8, 11);
  JJJ(d2, e2, a2, b2, c2, x[1], 14);
  JJJ(c2, d2, e2, a2, b2, 0, 14);
  JJJ(b2, c2, d2, e2, a2, x[3], 12);
  JJJ(a2, b2, c2, d2, e2, 0, 6);

  /* parallel s256rnd 2 */
  III(e2, a2, b2, c2, d2, x[6], 9);
  III(d2, e2, a2, b2, c2, 0, 13);
  III(c2, d2, e2, a2, b2, x[3], 15);
  III(b2, c2, d2, e2, a2, x[7], 7);
  III(a2, b2, c2, d2, e2, x[0], 12);
  III(e2, a2, b2, c2, d2, 0, 8);
  III(d2, e2, a2, b2, c2, x[5], 9);
  III(c2, d2, e2, a2, b2, 0, 11);
  III(b2, c2, d2, e2, a2, x14, 7);
  III(a2, b2, c2, d2, e2, 0, 7);
  III(e2, a2, b2, c2, d2, x8, 12);
  III(d2, e2, a2, b2, c2, 0, 7);
  III(c2, d2, e2, a2, b2, x[4], 6);
  III(b2, c2, d2, e2, a2, 0, 15);
  III(a2, b2, c2, d2, e2, x[1], 13);
  III(e2, a2, b2, c2, d2, x[2], 11);

  /* parallel s256rnd 3 */
  HHH(d2, e2, a2, b2, c2, 0, 9);
  HHH(c2, d2, e2, a2, b2, x[5], 7);
  HHH(b2, c2, d2, e2, a2, x[1], 15);
  HHH(a2, b2, c2, d2, e2, x[3], 11);
  HHH(e2, a2, b2, c2, d2, x[7], 8);
  HHH(d2, e2, a2, b2, c2, x14, 6);
  HHH(c2, d2, e2, a2, b2, x[6], 6);
  HHH(b2, c2, d2, e2, a2, 0, 14);
  HHH(a2, b2, c2, d2, e2, 0, 12);
  HHH(e2, a2, b2, c2, d2, x8, 13);
  HHH(d2, e2, a2, b2, c2, 0, 5);
  HHH(c2, d2, e2, a2, b2, x[2], 14);
  HHH(b2, c2, d2, e2, a2, 0, 13);
  HHH(a2, b2, c2, d2, e2, x[0], 13);
  HHH(e2, a2, b2, c2, d2, x[4], 7);
  HHH(d2, e2, a2, b2, c2, 0, 5);

  /* parallel s256rnd 4 */
  GGG(c2, d2, e2, a2, b2, x8, 15);
  GGG(b2, c2, d2, e2, a2, x[6], 5);
  GGG(a2, b2, c2, d2, e2, x[4], 8);
  GGG(e2, a2, b2, c2, d2, x[1], 11);
  GGG(d2, e2, a2, b2, c2, x[3], 14);
  GGG(c2, d2, e2, a2, b2, 0, 14);
  GGG(b2, c2, d2, e2, a2, 0, 6);
  GGG(a2, b2, c2, d2, e2, x[0], 14);
  GGG(e2, a2, b2, c2, d2, x[5], 6);
  GGG(d2, e2, a2, b2, c2, 0, 9);
  GGG(c2, d2, e2, a2, b2, x[2], 12);
  GGG(b2, c2, d2, e2, a2, 0, 9);
  GGG(a2, b2, c2, d2, e2, 0, 12);
  GGG(e2, a2, b2, c2, d2, x[7], 5);
  GGG(d2, e2, a2, b2, c2, 0, 15);
  GGG(c2, d2, e2, a2, b2, x14, 8);

  /* parallel s256rnd 5 */
  FFF(b2, c2, d2, e2, a2, 0, 8);
  FFF(a2, b2, c2, d2, e2, 0, 5);
  FFF(e2, a2, b2, c2, d2, 0, 12);
  FFF(d2, e2, a2, b2, c2, x[4], 9);
  FFF(c2, d2, e2, a2, b2, x[1], 12);
  FFF(b2, c2, d2, e2, a2, x[5], 5);
  FFF(a2, b2, c2, d2, e2, x8, 14);
  FFF(e2, a2, b2, c2, d2, x[7], 6);
  FFF(d2, e2, a2, b2, c2, x[6], 8);
  FFF(c2, d2, e2, a2, b2, x[2], 13);
  FFF(b2, c2, d2, e2, a2, 0, 6);
  FFF(a2, b2, c2, d2, e2, x14, 5);
  FFF(e2, a2, b2, c2, d2, x[0], 15);
  FFF(d2, e2, a2, b2, c2, x[3], 13);
  FFF(c2, d2, e2, a2, b2, 0, 11);
  FFF(b2, c2, d2, e2, a2, 0, 11);

  digest[0] = c1 + d2;
  digest[1] = d1 + e2;
  digest[2] = e1 + a2;
  digest[3] = a1 + b2;
  digest[4] = b1 + c2;
}

uint endian(uint x)
{
  return (x << 24) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | (x >> 24);
}

void hash_key_uncompressed(uint256_t x, uint256_t y, uint* digestOut)
{
  uint hash[8];

  sha256_key_uncompressed(x.v, y.v, hash);

  // Swap to little-endian
  for(int i = 0; i < 8; i++) {
    hash[i] = endian(hash[i]);
  }

  ripemd160_sha256_no_final(hash, digestOut);
}

void hash_key_compressed(uint256_t x, uint yParity, uint* digest_out)
{
  uint hash[8];

  sha256_key_compressed(x.v, yParity, hash);

  // Swap to little-endian
  for(int i = 0; i < 8; i++) {
    hash[i] = endian(hash[i]);
  }

  ripemd160_sha256_no_final(hash, digest_out);
}


kernel void hash_public_keys_compressed(const global uint256_t* x_in,
  const global uint256_t* y_in,
  const global uint* target,
  uint total_keys,
  global int* result_flag,
  global uint* result_idx)
{
  uint gid = get_global_id(0);
  uint dim = get_global_size(0);

  for(uint i = gid; i < total_keys; i += dim) {
    uint digest[5];

    uint256_t x = x_in[i];

    hash_key_compressed(x, readLSW256k(y_in, i), digest);

    if(digest[0] == target[0]
      && digest[1] == target[1]
      && digest[2] == target[2]
      && digest[3] == target[3]
      && digest[4] == target[4]) {

      *result_flag = 1;
      *result_idx = i;
    }
  }
}

kernel void hash_public_keys_uncompressed(const global uint256_t* x_in,
  const global uint256_t* y_in,
  const global uint* target,
  uint total_keys,
  global int* result_flag,
  global uint* result_idx)
{
  uint gid = get_global_id(0);
  uint dim = get_global_size(0);

  for(uint i = gid; i < total_keys; i += dim) {
    uint digest[5];

    uint256_t x = x_in[i];
    uint256_t y = y_in[i];
    hash_key_uncompressed(x, y, digest);

    if(digest[0] == target[0]
      && digest[1] == target[1]
      && digest[2] == target[2]
      && digest[3] == target[3]
      && digest[4] == target[4]) {

      *result_flag = 1;
      *result_idx = i;
    }
  }
}