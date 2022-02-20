import sys

def main():

  input_files = sys.argv

  if len(input_files) < 2:
    sys.stderr.write('At least 1 file required')
    return 1

  input_files = sys.argv[1:]

  output = ''

  try:
    for file in input_files:
      with open(file, 'rt') as f:
        output = output + f.read()

    print(output, end='')
    return 0
  except Exception as e:
    sys.stderr.write(e)
    return 1

if __name__ == "__main__":
  ret = main()
  sys.exit(ret)