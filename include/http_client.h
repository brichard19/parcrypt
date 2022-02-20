#ifndef _HTTP_CLIENT_H
#define _HTTP_CLIENT_H

#include <exception>

#include "secp256k1.h"
#include "server_common.h"
#include "work_unit.h"
#include "json11.hpp"

class NetworkException : public std::runtime_error {

private:
  std::string _msg;

public:

  NetworkException(const std::string& msg) : std::runtime_error(msg) {}
  NetworkException(const char* msg) : std::runtime_error(msg) {}

  std::string msg()
  {
    return _msg;
  }
};

class ClientAuth {

public:
  std::string username;
  std::string password;
  std::string token;

  ClientAuth() {}

  ClientAuth(const std::string& username, const std::string& password)
    : username(username),
    password(password)
  {
  }

  ClientAuth(const std::string token)
    : token(token)
  {
  }
};

class HTTPClient {

  int _port = 80;
  std::string _hostname = "127.0.0.1";
  std::string _url;
  std::string _user_token;

  std::string _username;
  std::string _password;
  std::string _token;

  bool _use_token = false;

  std::string get_auth_string();
  json11::Json::object get_auth_json();

  std::string post_json(json11::Json& json);

public:
  HTTPClient(const std::string hostname, int port, const ClientAuth& auth);

  ServerResponse get_work(const std::string& project_name, parcrypt::WorkData& wu);


  ServerResponse report_results(const parcrypt::CompletedWorkData& info);
};


#endif
