

class ParcryptProject:

  # Can be:
  # 'running'
  # 'done'
  # 'paused'
  def get_state(self):
    raise Exception('get_state() must be implemented by subclass')

  def get_type(self):
    raise Exception('get_type() must be implemented by subclass')
  
  def write_info(self):
    raise Exception('write_info() must be implemented in subclass')