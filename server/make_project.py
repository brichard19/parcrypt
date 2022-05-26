import requests
import sys
import argparse
import util

_challenge_addresses = [
  '1BgGZ9tcN4rm9KBzDn7KprQz87SZ26SAMH',
  '1CUNEBjYrCn2y1SdiUMohaKUi4wpP326Lb',
  '19ZewH8Kk1PDbSNdJ97FP4EiCjTRaZMZQA',
  '1EhqbyUMvvs7BfL8goY6qcPbD6YKfPqb7e',
  '1E6NuFjCi27W5zoXg8TRdcSRq84zJeBW3k',
  '1PitScNLyp2HCygzadCh7FveTnfmpPbfp8',
  '1McVt1vMtCC7yn5b9wgX1833yCcLXzueeC',
  '1M92tSqNmQLYw33fuBvjmeadirh1ysMBxK',
  '1CQFwcjw1dwhtkVWBttNLDtqL7ivBonGPV',
  '1LeBZP5QCwwgXRtmVUvTVrraqPUokyLHqe',
  '1PgQVLmst3Z314JrQn5TNiys8Hc38TcXJu',
  '1DBaumZxUkM4qMQRt2LVWyFJq5kDtSZQot',
  '1Pie8JkxBT6MGPz9Nvi3fsPkr2D8q3GBc1',
  '1ErZWg5cFCe4Vw5BzgfzB74VNLaXEiEkhk',
  '1QCbW9HWnwQWiQqVo5exhAnmfqKRrCRsvW',
  '1BDyrQ6WoF8VN3g9SAS1iKZcPzFfnDVieY',
  '1HduPEXZRdG26SUT5Yk83mLkPyjnZuJ7Bm',
  '1GnNTmTVLZiqQfLbAdp9DVdicEnB5GoERE',
  '1NWmZRpHH4XSPwsW6dsS3nrNWfL1yrJj4w',
  '1HsMJxNiV7TLxmoF6uJNkydxPFDog4NQum',
  '14oFNXucftsHiUMY8uctg6N487riuyXs4h',
  '1CfZWK1QTQE3eS9qn61dQjV89KDjZzfNcv',
  '1L2GM8eE7mJWLdo3HZS6su1832NX2txaac',
  '1rSnXMr63jdCuegJFuidJqWxUPV7AtUf7',
  '15JhYXn6Mx3oF4Y7PcTAv2wVVAuCFFQNiP',
  '1JVnST957hGztonaWK6FougdtjxzHzRMMg',
  '128z5d7nN7PkCuX5qoA4Ys6pmxUYnEy86k',
  '12jbtzBb54r97TCwW3G1gCFoumpckRAPdY',
  '19EEC52krRUK1RkUAEZmQdjTyHT7Gp1TYT',
  '1LHtnpd8nU5VHEMkG2TMYYNUjjLc992bps',
  '1LhE6sCTuGae42Axu1L1ZB7L96yi9irEBE',
  '1FRoHA9xewq7DjrZ1psWJVeTer8gHRqEvR',
  '187swFMjz1G54ycVU56B7jZFHFTNVQFDiu',
  '1PWABE7oUahG2AFFQhhvViQovnCr4rEv7Q',
  '1PWCx5fovoEaoBowAvF5k91m2Xat9bMgwb',
  '1Be2UF9NLfyLFbtm3TCbmuocc9N1Kduci1',
  '14iXhn8bGajVWegZHJ18vJLHhntcpL4dex',
  '1HBtApAFA9B2YZw3G2YKSMCtb3dVnjuNe2',
  '122AJhKLEfkFBaGAd84pLp1kfE7xK3GdT8',
  '1EeAxcprB2PpCnr34VfZdFrkUWuxyiNEFv',
  '1L5sU9qvJeuwQUdt4y1eiLmquFxKjtHr3E',
  '1E32GPWgDyeyQac4aJxm9HVoLrrEYPnM4N',
  '1PiFuqGpG8yGM5v6rNHWS3TjsG6awgEGA1',
  '1CkR2uS7LmFwc3T2jV8C1BhWb5mQaoxedF',
  '1NtiLNGegHWE3Mp9g2JPkgx6wUg4TW7bbk',
  '1F3JRMWudBaj48EhwcHDdpeuy2jwACNxjP',
  '1Pd8VvT49sHKsmqrQiP61RsVwmXCZ6ay7Z',
  '1DFYhaB2J9q1LLZJWKTnscPWos9VBqDHzv',
  '12CiUhYVTTH33w3SPUBqcpMoqnApAV4WCF',
  '1MEzite4ReNuWaL5Ds17ePKt2dCxWEofwk',
  '1NpnQyZ7x24ud82b7WiRNvPm6N8bqGQnaS',
  '15z9c9sVpu6fwNiK7dMAFgMYSK4GqsGZim',
  '15K1YKJMiJ4fpesTVUcByoz334rHmknxmT',
  '1KYUv7nSvXx4642TKeuC2SNdTk326uUpFy',
  '1LzhS3k3e9Ub8i2W1V8xQFdB8n2MYCHPCa',
  '17aPYR1m6pVAacXg1PTDDU7XafvK1dxvhi',
  '15c9mPGLku1HuW9LRtBf4jcHVpBUt8txKz',
  '1Dn8NF8qDyyfHMktmuoQLGyjWmZXgvosXf',
  '1HAX2n9Uruu9YDt4cqRgYcvtGvZj1rbUyt',
  '1Kn5h2qpgw9mWE5jKpk8PP4qvvJ1QVy8su',
  '1AVJKwzs9AskraJLGHAZPiaZcrpDr1U6AB',
  '1Me6EfpwZK5kQziBwBfvLiHjaPGxCKLoJi',
  '1NpYjtLira16LfGbGwZJ5JbDPh3ai9bjf4',
  '16jY7qLJnxb7CHZyqBP8qca9d51gAjyXQN',
  '18ZMbwUFLMHoZBbfpCjUJQTCMCbktshgpe',
  '13zb1hQbWVsc2S7ZTZnP2G4undNNpdh5so',
  '1BY8GQbnueYofwSuFAT3USAhGjPrkxDdW9',
  '1MVDYgVaSN6iKKEsbzRUAYFrYJadLYZvvZ',
  '19vkiEajfhuZ8bs8Zu2jgmC6oqZbWqhxhG',
  '19YZECXj3SxEZMoUeJ1yiPsw8xANe7M7QR',
  '1PWo3JeB9jrGwfHDNpdGK54CRas7fsVzXU',
  '1JTK7s9YVYywfm5XUH7RNhHJH1LshCaRFR',
  '12VVRNPi4SJqUTsp6FmqDqY5sGosDtysn4',
  '1FWGcVDK3JGzCC3WtkYetULPszMaK2Jksv',
  '1J36UjUByGroXcCvmj13U6uwaVv9caEeAt',
  '1DJh2eHFYQfACPmrvpyWc8MSTYKh7w9eRF',
  '1Bxk4CQdqL9p22JEtDfdXMsng1XacifUtE',
  '15qF6X51huDjqTmF9BJgxXdt1xcj46Jmhb',
  '1ARk8HWJMn8js8tQmGUJeQHjSE7KRkn2t8',
  '1BCf6rHUW6m3iH2ptsvnjgLruAiPQQepLe'
]

_presets = []

def build_presets():
  for i in range(len(_challenge_addresses)):
    start = 2**i
    end = 2**(i+1) - 1

    preset = {}
    preset['address'] = _challenge_addresses[i]
    preset['start'] = util.int_to_hex(start)
    preset['end'] = util.int_to_hex(end)

    _presets.append(preset)

def list_challenges():
  print('Puzzle    Address                             Difficulty')
  for i in range(len(_challenge_addresses)):
    print('{:2d}        {:<35} {:2d}-bit'.format(i + 1, _challenge_addresses[i], i))

def main():

  build_presets()

  hostname = '127.0.0.1'
  port = 8080
  share_size_bits = 36
  start = util.int_to_hex(1)
  end = util.int_to_hex(2**256 - 2**32 - 977 - 1)
  compressed = True
  use_preset = False

  req = {}
  req['cmd'] = 'create_project'
  req['type'] = 'BTC_PUB_KEY_HASH'
  req['payload'] ={}

  parser = argparse.ArgumentParser(description='Create new project.')
  parser.add_argument('--name', type=str, required=False, help='name of the project')
  parser.add_argument('--address', type=str, required=False, help='target address')
  parser.add_argument('--start', type=str, required=False, help='start of private key range in hex')
  parser.add_argument('--end', type=str, required=False, help='end of private key range in hex')
  parser.add_argument('--host', type=str, required=False, help='host server name or IP address')
  parser.add_argument('--port', type=int, required=False, help='port to connect on')
  parser.add_argument('--share-size-bits', type=int, required=False, help='size of shares in bits')
  parser.add_argument('--challenge', type=int, required=False, help='use address from Bitcoin challenge')
  parser.add_argument('--list-challenge', action='store_true', required=False, help='list challenge addresses and exit')
  parser.add_argument('--compressed', action='store_true', required=False, help='Compressed public key(default)')
  parser.add_argument('--uncompressed', action='store_true', required=False, help='Uncompressed public key')

  args = vars(parser.parse_args())

  if args['list_challenge'] == True:
    list_challenges()
    return 0

  # --name is a required argument if creating a new project.
  if args['name'] == None:
    print('--name is required')
    parser.parse_args(['--help'])
    return 1

  # --address is required if we are not using a pre-set challenge
  if args['challenge'] == None and args['address'] == None:
    print('--address is required')
    parser.parse_args(['--help'])

  if args['compressed'] == True and args['uncompressed'] == True:
    print('Cannot use --compressed and --uncompressed at the same time')
    return 1
 
  project_name = args['name']

  if args['address'] != None:
    # TODO: Validate the address
    address = args['address']
  if args['start'] != None:
    start = args['start']
    if not util.is_hex(start):
      print('--start must be hex string')
      return 1
  if args['end'] != None:
    end = args['end']
    if not util.is_hex(end):
      print('--end must be hex string')
      return 1
  if args['port'] != None:
    port = args['port']
  if args['challenge'] != None:
    use_preset = True
    challenge_size = args['challenge']
  if args['share_size_bits'] != None:
    share_size_bits = args['share_size_bits']
  if args['host'] != None:
    hostname = args['host']
  if use_preset and args['address'] != None:
    print('--challenge cannot be used with --address')
    return 1
  if args['compressed']:
    compressed = True
  if args['uncompressed']:
    compressed = False

  if share_size_bits > 56:
    print('Invalid share size')
    return 1

  if use_preset:
    idx = challenge_size - 1
    address = _presets[idx]['address']
    start = _presets[idx]['start']
    end = _presets[idx]['end']

  json = {}
  json['cmd'] = 'create_project'
  json['type'] = 'BTC_PUB_KEY_HASH'

  payload = {} 
  payload['name'] = project_name
  payload['start'] = start
  payload['end'] = end
  payload['sharesize'] = share_size_bits
  payload['address'] = address
  payload['compression'] = 'compressed' if compressed else 'uncompressed'
  json['payload'] = payload

  url = 'http://{0}:{1}'.format(hostname, port)
  ret = requests.post(url, json = json)
  json = ret.json()

  status = json['status']
  if status != 'Ok':
    print('Error: ', status)

  return 0

if __name__ == "__main__":
  ret = main()
  sys.exit(ret)
