import string

def from_hex(s):
  return int(s, 16)

def int_to_hex(x):
  h = hex(x)
  h = h[2:]
  if len(h) == 0:
    h = '0'
 
  return h

def is_hex(x):
  return all(c in string.hexdigits for c in x)