import argparse
import sys

def encode_c_array(symbol, data):
  s = 'char {0}[] = {{'.format(symbol)

  for byte in data:
    s += hex(byte) + ','

  # Null terminator
  s += '0x00'
  s += '};'
  s += '\n'

  return s

def read_file(file_name):
  in_file = open(file_name, 'rb')

  input_data = bytearray()

  byte = in_file.read(1)
  while byte:
    input_data += byte
    byte = in_file.read(1)

  in_file.close()
  
  return input_data

def write_file(file_name, data):
  out_file = open(file_name, 'w')

  out_file.write(data)
  out_file.close()

def main():

  parser = argparse.ArgumentParser(description='Embed data into C files')
  parser.add_argument('--symbol', type=str, required=True)
  parser.add_argument('--input', type=str, required=True)
  parser.add_argument('--output', type=str, required=False)

  args = vars(parser.parse_args())

  input_file = args['input']
  symbol = args['symbol']
  output_file = args['output']

  try:
    input_data = read_file(input_file)
    encoded = encode_c_array(symbol, input_data)

    if output_file == None:
      print(encoded)
    else:
      write_file(output_file, encoded)
   
  except Exception as e:
    print(e)
    return 1

  return 0

if __name__ == "__main__":
  ret = main()
  sys.exit(ret)