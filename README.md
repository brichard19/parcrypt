# parcrypt - Parallel Cryptanalysis

parcrypt is designed for parallel computational problems which have low communication requirements. It is a server-client model where clients request work from the server, perform processing and submit the results.

### Supported algorithms
|Algorithm|Type|OpenCL|CUDA|CPU|Description
|-----------|----------|------|--------|-------|-------|
|BTCPubKeyHash|Brute force|Yes|No|No|Brute force search to find the private key for a Bitcoin address (compressed legacy addresses only right now).


## Server

The server is written in Python and uses Flask to handle HTTP requests. It is responsible for distributing work to the clients.
**Note:** The server has no built-in security right now. It should not be public-facing.

## Client

The client is a console program that retrieves work from the server, does a computation and reports the result back to the server.

## Getting started

### Server Config

The server reads configuration information from the `server_config.json` file. Currently the only configuration option is which directory the projects are stored

```
{
"project_dir":"projects"
}
```

### Start the server
```
$ python server/server.py
```
Database files will be created automatically in the current directory.

### Create a project

Create a project on the server using the `server/make_project.py` script.

For example, to search for the private key to `1DeCk95yY3QkTH8RREeenxJZPvwCeQAqjw` in the key range `1` and `8000000000` use:
```
python server/make_project.py --name test_project --address 1DeCk95yY3QkTH8RREeenxJZPvwCeQAqjw --start 1 --end 8000000000
```

**Note:** Currently the software only supports compressed legacy addresses

For convenience, the script includes the Bitcoin puzzle transaction addresses:
```
$ python server/toos/create_project.py --list-challenge
Puzzle    Address                             Difficulty
 1        1BgGZ9tcN4rm9KBzDn7KprQz87SZ26SAMH   0-bit
 2        1CUNEBjYrCn2y1SdiUMohaKUi4wpP326Lb   1-bit
 3        19ZewH8Kk1PDbSNdJ97FP4EiCjTRaZMZQA   2-bit
 4        1EhqbyUMvvs7BfL8goY6qcPbD6YKfPqb7e   3-bit
 5        1E6NuFjCi27W5zoXg8TRdcSRq84zJeBW3k   4-bit
 6        1PitScNLyp2HCygzadCh7FveTnfmpPbfp8   5-bit
 7        1McVt1vMtCC7yn5b9wgX1833yCcLXzueeC   6-bit
 8        1M92tSqNmQLYw33fuBvjmeadirh1ysMBxK   7-bit
 9        1CQFwcjw1dwhtkVWBttNLDtqL7ivBonGPV   8-bit
10        1LeBZP5QCwwgXRtmVUvTVrraqPUokyLHqe   9-bit
11        1PgQVLmst3Z314JrQn5TNiys8Hc38TcXJu  10-bit
12        1DBaumZxUkM4qMQRt2LVWyFJq5kDtSZQot  11-bit
13        1Pie8JkxBT6MGPz9Nvi3fsPkr2D8q3GBc1  12-bit

...

77        1Bxk4CQdqL9p22JEtDfdXMsng1XacifUtE  76-bit
78        15qF6X51huDjqTmF9BJgxXdt1xcj46Jmhb  77-bit
79        1ARk8HWJMn8js8tQmGUJeQHjSE7KRkn2t8  78-bit
80        1BCf6rHUW6m3iH2ptsvnjgLruAiPQQepLe  79-bit
```
For example to create a project that attacks the 44th puzzle:
```
$ python server/make_project.py --name test_project --challange 44
```

Currently there is no way to see the progress except by looking at the database directly. A useful tool for this is [DB Browser for SQLite](https://sqlitebrowser.org/).

### Configure the client

The client configuration is in client.json. It contains the hostname of the server, the port to connect on, and the list of GPUs to use and the max amount of GPU memory to use:

```
{
  "hostname": "127.0.0.1",
  "port": 8080,
  "gpu_devices":[
    {"opencl":0, "mem_usage":"85%"},
    {"opencl":1, "mem_usage":"2GB"}
  ]
}
```

Start the client by running `parcrypt` (Linux) or `parcrypt.exe` (Windows) from the command-line.

The client will detect the GPUs and wait for work from the server.

When it receives work it will display the progress:
![alt text](https://github.com/brichard19/parcrypt/blob/main/screenshots/parcrypt-client.png?raw=true)

### Results
If a key is found the server will log it to the output and also save it to a text file located in the same directory as the project database.



## Building
### Client

#### Windows

##### Requirements
- Visual Studio 2019 or newer
- python3

To build the client, build the `parcrypt` project in the Visual Studio solution.

#### Linux
##### Requirements
- make
- g++ 8.0 or later
- libcurl
- python3

To build the client, run `make`

### Server

- python3
- flask
	- `pip3 install flask`



# Architecture
Projects are submitted to the server. The submission includes all the details about the problem as well as parameters about how the work should be divided. For example, a key search project might specify the size of the keyspace distributed to clients.

The work is divided into Work Units. A Work Unit has a unique ID and the server tracks which work units were sent out.

The client requests a work unit from the server. The client divides the work unit into one or more Work Items. The work items are distributed to one or more GPUs on the client machine. If a single work unit does not provide enough work items, the client can request another work unit from the server.

# To-do list

## Short term:
- Update Flask to use production server
- Simple web interface for adding new jobs and looking at job status
- Improve client console interface. Decide what should be displayed
- More robust server code
- CUDA implementation for BTCPubKeyHash
- CPU implementation for BTCPubKeyHash

## Medium-long term:
- Server HTTPS support
- Server user names / passwords
- More algorithms
