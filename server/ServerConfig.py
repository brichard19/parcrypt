import json

_config = None



def load_config(config_file):
  global _config

  f = open(config_file, 'r')

  _config = json.load(f)


def project_directory():
  return _config['project_dir']