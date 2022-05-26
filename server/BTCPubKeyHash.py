import BruteForceProject
import ServerConfig
import pathlib
import os

class BTCPubKeyHash(BruteForceProject.BruteForceProject):

  def __init__(self, project_name):
    self._project_name = project_name

    # Create project directory
    self._project_dir = os.path.join(ServerConfig.project_directory(), project_name)
    pathlib.Path(self._project_dir).mkdir(parents=True, exist_ok=True)
    
    db_name = '{}.db'.format(project_name)
    db_path = os.path.join(self._project_dir, db_name)
    super().__init__(db_path)

  def create_project(self, payload):

    p = payload

    if p['compression'] == 'compressed':
      p['target'] = 'compressed::' + payload['address']
    else:
      p['target'] = 'uncompressed::' + payload['address']

    self.write_info()

    return super().create_project(p)
  
  def report_work(self, payload):
    err, done = super().report_work(payload)

    private_key = payload['private_key']

    if private_key != None and len(private_key) > 0:
      done = True
      super().set_private_key(private_key)

      # Save to file
      file_name = '{}_key.txt'.format(self._project_name)
      file_path = os.path.join(self._project_dir, file_name)
      with open(file_path, 'w') as f:
        f.write(private_key)

    else:
      done = False
 
    return err, done

  def get_type(self):
    return 'BTC_PUB_KEY_HASH'

  def write_info(self):
    with open(os.path.join(self._project_dir, 'INFO.txt'), 'wt') as f:
      f.write('BTC_PUB_KEY_HASH')