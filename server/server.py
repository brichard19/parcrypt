import flask
import sys
import logging
import re
import ServerConfig
import json
import os

# Modules
import BTCPubKeyHash

# Log everthing to stdout for now
logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

# Load sever config
try:
  ServerConfig.load_config('server_config.json')
except Exception as e:
  print('Error loading config file: {}'.format(e))
  sys.exit(1)

app = flask.Flask(__name__)

_projects = {}

# Retrieves an existing project or returns a new
# project object of the specified type
def get_project(name, type):

  if name not in _projects:
    if type == 'BTC_PUB_KEY_HASH':
      return BTCPubKeyHash.BTCPubKeyHash(name)
    else:
      raise ValueError('Unknown project type {}'.format(type))
  else:
    return _projects['name']

# Loads projects from disk
def load_projects():
  project_dir = ServerConfig.project_directory()
  if not os.path.exists(project_dir):
    return

  projects = os.listdir(project_dir)

  for p in projects:
    try:
      with open(os.path.join(project_dir, p, 'INFO.txt'), 'rt') as f:
        project_type = f.read()
    except Exception as e:
      logging.error('Unable to get info for {}: {}'.format(p, e))
      continue

    _projects[p] = get_project(p, project_type)

# TODO: Break this up into multiple routes
@app.route('/', methods=['POST'])
def post_root():

  try:
    json = flask.request.get_json()
    
    cmd = json['cmd']

    if cmd == 'request_work':
      return request_work(json)
    elif cmd == 'create_project':
      logging.info(json)
      return create_project(json)
    elif cmd == 'report_work':
      logging.info(json)
      return report_work(json)
    else:
      response = {}
      response['status'] = 'ServerError'
      response['msg'] = 'Invalid command'
      return flask.jsonify(response)
  except:
    return flask.Response(500)


@app.route('/status', methods=['GET'])
def get_status():

  status = ''
  for project_name, obj in _projects.items():
    status = status + '{}\n{}\n'.format(project_name, json.dumps(obj.get_status(), indent=2))

  status = status.replace('\n', '<br/>')
  return status


@app.route('/', methods=['GET'])
def get_root():
  return 'Parcrypt server is running'

# Expected:
# {
#   "name":"project_name"
#   "payload"{ // project-specific stuff}
# }
#
# If name is a wildcard (*) then work from any
# available project will be returned
def request_work(json):

  name = json['name']

  projects = []

  # Pick the first available project for now
  if name == '*':
    projects = []
    for p in _projects:
      if _projects[p].get_state() == 'running':
        projects.append(p)

    if len(projects) == 0:
      response = {}
      response['status'] = 'NoWorkAvailable'
      return flask.jsonify(response)
  else:
    if name not in _projects:
      response = {}
      response['status'] = 'NoWorkAvailable'
      return flask.jsonify(response)
    else:
      projects.append(name)

  # Find first available project
  for project_name in projects:

    # Payload might not exist
    if 'payload' in json:
      payload = json['payload']
    else:
      payload = {}

    payload['name'] = project_name

    err, payload_out = _projects[project_name].request_work(payload)

    if payload_out != None:
      response = {}
      response['name'] = project_name
      response['type'] = _projects[project_name].get_type()
      response['status'] = err
      response['payload'] = payload_out
      return flask.jsonify(response)

  response = {}
  response['status'] = 'NoWorkAvailable'

  return flask.jsonify(response)

def validate_project_name(name):

  if name == None or len(name) == 0:
    return False

  regexp = '^[a-zA-Z0-9_-]*$'

  r = re.compile(regexp)

  if r.search(name) == None:
    return False
  
  return True

# Expected:
# {
#  "type":"project_type",
#  "payload":{
#     "name":<project name>,
#     // project-specific work info
#   }
# }
# Returns
# {
#  "status":"<status>"
# }
def create_project(json):
  type = json['type']
  payload = json['payload']
  name = payload['name']

  if not validate_project_name(name):
    response = {}
    response['status'] = 'InvalidProjectName'
    return flask.jsonify(response)

  if name not in _projects:
    project = get_project(name, type)

    err = project.create_project(payload)

    _projects[name] = project
  else:
    err = 'PuzzleExists'

  response = {}

  response['status'] = err

  return flask.jsonify(response)


# Expected:
# {
# "name":"project_name"
# "id":<work unit id>
# "payload":{ // Project-specific info}
# }
def report_work(json):
  name = json['name']
  payload = json['payload']
  payload['id'] = json['id']

  if name in _projects:
    err, done = _projects[name].report_work(payload)
  else:
    # Ignore for now
    err = 'Ok'

  response = {}
  response['status'] = err

  return flask.jsonify(response)

def main():
  load_projects()
  app.run(host='0.0.0.0', port = 8080)

if __name__ == "__main__":
  logging.info('Starting server')

  main()