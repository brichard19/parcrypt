import sqlite3
import time
import util
import uuid

class WorkUnit:

  def __init__(self, project_name, id, start_key, end_key, target):
    self.work_id = id
    self.start_key = start_key
    self.end_key = end_key
    self.target = target
    self.name = project_name

class BruteForceDB:

  def __init__(self, db_path):
    self._db_path = db_path
    self._init_db()

  def _connect_to_database(self):
    conn = sqlite3.connect(self._db_path)
    conn.row_factory = sqlite3.Row

    return conn

  def _init_db(self):
    conn = self._connect_to_database()
    cursor = conn.cursor()

    q = '''create table if not exists ProjectInfo(
        name varchar(256) not null,
        target varchar(256) not null,
        start_key varchar(256) not null,
        end_key varchar(256) not null,
        next_block integer not null,
        block_size_bits integer not null,
        status varchar(64) not null,
        private_key varchar(4096),
        num_blocks varchar(256) NOT NULL);
        '''
    cursor.execute(q)

    q = '''create table if not exists WorkUnits(
          work_unit_id varchar(64) not null,
          block integer not null,
          status varchar(64) not null);
        '''
    cursor.execute(q)

    q = '''create table if not exists OutstandingWork(
        work_unit_id varchar(64) not null,
        expiry int not null,
        status varchar(64) not null,
        user_id integer not null);
        '''
    cursor.execute(q)

    conn.close()

  def _cleanup_expired(self):
    conn = self._connect_to_database()
    cursor = conn.cursor()

    sql = '''
          update OutstandingWork set status = 'expired' where status != 'done' and expiry < {}
          '''.format(int(time.time()))
    
    cursor.execute(sql)
    conn.commit()
    conn.close()

  def create_project(self, project_name, target, start_key_hex, end_key_hex, block_size_bits):
    conn = self._connect_to_database()
    cursor = conn.cursor()

    start_key = util.from_hex(start_key_hex)
    end_key = util.from_hex(end_key_hex)

    num_blocks = (end_key - start_key + (1 << block_size_bits) - 1) // (1 << block_size_bits)

    # Always at least 1 block.
    if num_blocks == 0:
      num_blocks = 1

    num_blocks_hex = util.int_to_hex(num_blocks)
    sql = '''insert into ProjectInfo(
            name, target, start_key, end_key, block_size_bits, next_block, status, private_key, num_blocks
            )
            values('{}', '{}', '{}', '{}', {}, {}, '{}', '{}', '{}')'''
    
    sql = sql.format(project_name, target, start_key_hex, end_key_hex, block_size_bits, 0, 'running', '', num_blocks_hex)
    cursor.execute(sql)

    conn.commit()
    conn.close()
  
  def request_work(self, project_name, user_id = 0):

    self._cleanup_expired()

    # TODO: Configure expiry time
    expiry_time_seconds = 600
    conn = self._connect_to_database()
    cursor = conn.cursor()

    # Get project info
    sql = "select * from ProjectInfo where name = '{}'".format(project_name)

    cursor.execute(sql)
    r = cursor.fetchone()

    if len(r) == 0:
      return None

    block_num = r['next_block']
    block_size_bits = r['block_size_bits']
    project_start_key = util.from_hex(r['start_key'])
    project_end_key = util.from_hex(r['end_key'])
    target = r['target']
    status = r['status']
  
    num_blocks = util.from_hex(r['num_blocks'])
  
    if status == 'done':
      return None

    # Check for any expired work items
    sql = "select * from OutstandingWork where status = 'expired' limit 1"
    cursor.execute(sql)
    r = cursor.fetchone()

    # If there are no expired workunits available, create a new one
    if r == None:

      # There are no expired workunits, and there are no more blocks that can be created
      if block_num >= num_blocks:
        return None

      work_unit_id = uuid.uuid4()
      sql = "insert into WorkUnits (block, status, work_unit_id) values ({}, '{}', '{}')".format(block_num, 'running', work_unit_id)
      cursor.execute(sql)
      conn.commit()



      next_block = block_num + 1
    
      sql = "update ProjectInfo set next_block = {}".format(next_block)
      cursor.execute(sql)
      conn.commit()

      # Add to outstanding work table
      expiry = int(time.time()) + expiry_time_seconds
      sql = "insert into OutstandingWork (work_unit_id, expiry, status, user_id) values ('{}', {}, '{}', {})".format(work_unit_id, expiry, 'running', user_id)
      cursor.execute(sql)
      conn.commit()
    else:
      work_unit_id = r['work_unit_id']
      new_expiry = int(time.time()) + expiry_time_seconds
      sql = "update OutstandingWork set expiry = {}, status = '{}', user_id = {} where work_unit_id = '{}'".format(new_expiry, 'running', user_id, work_unit_id)
      cursor.execute(sql)

    conn.commit()
    conn.close()

    start_key = project_start_key + block_num * (1 << block_size_bits)
    end_key = project_start_key + ((block_num + 1) * (1 << block_size_bits)) - 1

    if end_key > project_end_key:
      end_key = project_end_key

    work_unit = WorkUnit(project_name, work_unit_id, start_key, end_key, target)

    return work_unit

  def report_work(self, work_id, private_key, user_id):

    self._cleanup_expired()

    conn = self._connect_to_database()
    cursor = conn.cursor()

    sql = "select * from OutstandingWork where work_unit_id = '{}'".format(work_id)
    cursor.execute(sql)

    r = cursor.fetchone()

    sql = "update OutstandingWork set status = '{}' where work_unit_id = '{}'".format('done', work_id)
    cursor.execute(sql)
    conn.commit()
    
    sql = "update WorkUnits set status = '{}' where work_unit_id = '{}'".format('done', work_id)
    cursor.execute(sql)
    conn.commit()

    conn.close()

  def set_private_key(self, private_key):
    conn = self._connect_to_database()
    cursor = conn.cursor()

    sql = "update ProjectInfo set status = 'done', private_key ='{}'".format(private_key)
    cursor.execute(sql)
    conn.commit()

    conn.close()

  def get_state(self):
    conn = self._connect_to_database()
    cursor = conn.cursor()

    sql = "select status from ProjectInfo"
    cursor.execute(sql)
    r = cursor.fetchone()

    state = r['status']

    conn.close()

    return state

  # Returns a json object with the status of this project
  # {
  # total_blocks:<int>
  # completed_blocks:<int>
  # total_keys:<int>
  # keys_tested:<int>
  # percent_complete:<float>
  # status:<string>
  #}

  def get_status(self):
    conn = self._connect_to_database()
    cursor = conn.cursor()

    # Get the total number of blocks and the block size
    sql = "select * from ProjectInfo"
    cursor.execute(sql)
    r = cursor.fetchone()

    total_blocks = util.from_hex(r['num_blocks'])
    block_size = 1 << r['block_size_bits']
    state = r['status']

    # Get the number of completed blocks
    sql = "select count(*) as completed_blocks from WorkUnits where status = 'done'"
    cursor.execute(sql)
    r = cursor.fetchone()
    completed_blocks = r['completed_blocks']
    conn.close()

    status = {}
    status['status'] = state
    status['total_blocks'] = total_blocks
    status['completed_blocks'] = completed_blocks
    status['total_keys'] = total_blocks * block_size
    status['keys_tested'] = completed_blocks * block_size
    status['percent_completed'] = (float(completed_blocks) / float(total_blocks)) * 100.0
    
    return status
