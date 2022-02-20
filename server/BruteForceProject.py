import BruteForceDB
import ParcryptProject
import util
import logging
import os

class BruteForceProject(ParcryptProject.ParcryptProject):

  def __init__(self, db_path):
    self.db_path = db_path
    self.db_exists = os.path.exists(db_path)

    # If it doesn't exist, we don't need to create it
    # until create_project is called
    if self.db_exists: 
      self.db = BruteForceDB.BruteForceDB(self.db_path)


  def request_work(self, payload_in):
    """Retrives work for this project if available

    payload_in is dict object containing the following fields
    'name': The name of the project

    Returns a dictionary containing
    'target' The target ciphertext
    'start' The start of the keyspace to search
    'end' The end of the keyspace to search
    'id' The id of the work unit
    """
    try:
      user_id = 0
      name = payload_in['name']

      work = self.db.request_work(name, user_id)

      if work != None:

        payload_out = {}


        payload_out['target'] = work.target
        payload_out['start'] = util.int_to_hex(work.start_key)
        payload_out['end'] = util.int_to_hex(work.end_key)
        payload_out['id'] = work.work_id
        
        return 'Ok', payload_out
      else:
        return 'NoWorkAvailable', None

    except Exception as e:
      status = str(e)
      return status, None

  def create_project(self, payload):
    """Creates a new project
    
    The payload contains the following fields
    'name': The unique name of the project
    'target': The target ciphertext
    'start': The start of the keyspace to search
    'end': The end of the keyspace to search
    'sharesize': The number of keys, in bits, that a work unit will search
    """
    try:

      # Create the database if it does not exist yet
      if not self.db_exists:
        self.db = BruteForceDB.BruteForceDB(self.db_path)
        self.db_exists = True

      name = payload['name']
      target = payload['target']
      start_key_hex = payload['start']
      end_key_hex = payload['end']
      block_size_bits = payload['sharesize']

      self.db.create_project(name, target, start_key_hex, end_key_hex, block_size_bits)

    except Exception as e:
      logging.error('BTCPubKeyHash: ' + str(e))
      return 'ServerError'

    return 'Ok'

  def report_work(self, payload):
    """Report the results of a work unit
    The payload object contains the following fields:
    'id': The id of the work unit
    'private_key': The private key (if found), otherwise
    an empty string
    """
    try: 
      done = False
      user_id = 0

      share_id = payload['id']
      private_key = payload['private_key']

      self.db.report_work(share_id, private_key, user_id)

      # For now, just remove the project from the
      # active projects list if we received a
      # priate key
      if private_key != None and len(private_key) > 0:
        done = True

    except:
      return 'ServerError', False

    return 'Ok', done
  
  def set_private_key(self, private_key):
    self.db.set_private_key(private_key)
  
  def get_status(self):
    return self.db.get_status()

  def get_state(self):
    return self.db.get_state()