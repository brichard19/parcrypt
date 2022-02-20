
#include "http_client.h"

#include <exception>
#include "fmt/format.h"
#include "util.h"

extern "C" {
#define CURL_STATICLIB
#include "curl/curl.h"
}

namespace {

static size_t write_callback(char* contents, size_t size, size_t nmemb, void* userp)
{
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

} // namespace

std::string HTTPClient::post_json(json11::Json& json)
{
  std::string json_str = json.dump();

  CURL* curl;
  CURLcode res = CURLE_OK;

  curl = curl_easy_init();
  if(!curl) {
    throw std::runtime_error("Error initializing curl");
  }

  struct curl_slist* slist1 = nullptr;
  slist1 = curl_slist_append(slist1, "Content-Type: application/json");

  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
  res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  if(res != CURLE_OK) {
    std::string err_msg = fmt::format("Connection to {} on port {} failed: {}", _hostname, _port, curl_easy_strerror(res));
    throw NetworkException(err_msg);
  }

  return response;
}

HTTPClient::HTTPClient(const std::string hostname, int port, const ClientAuth& auth)
  : _hostname(hostname),
  _port(port)
{
  if(auth.token.empty()) {
    _use_token = false;
    _username = auth.username;
    _password = auth.password;
  } else {
    _use_token = true;
    _token = auth.token;
  }

  _url = "http://" + _hostname + ":" + std::to_string(_port);
}


std::string HTTPClient::get_auth_string()
{
  if(_use_token) {
    return fmt::format("token={}", _token);
  } else {
    return fmt::format("user={}&password={}", _username, _password);
  }
}

json11::Json::object HTTPClient::get_auth_json()
{
  if(_use_token) {
    json11::Json::object auth = { {"token", _token} };
    return auth;
  } else {
    json11::Json::object auth { {"user", _username}, {"password", _password} };
    return auth;
  }
}

ServerResponse HTTPClient::get_work(const std::string& project_name, parcrypt::WorkData& wu)
{
  json11::Json req = json11::Json::object {
    {"auth", get_auth_json()},
    {"cmd", "request_work"},
    {"name", project_name}
  };

  std::string response = post_json(req);

  std::string err;
  const auto json = json11::Json::parse(response, err);

  std::string status = json["status"].string_value();

  if(status != "Ok") {
    return res_from_string(status);
  }

  // TODO: Validate json
  wu.type = json["type"].string_value();
  wu.name = json["name"].string_value();
  wu.work_id = json["payload"]["id"].string_value();
  wu.payload = json["payload"];

  return ServerResponse::Ok;
}


ServerResponse HTTPClient::report_results(const parcrypt::CompletedWorkData& info)
{
  json11::Json req = json11::Json::object{
    {"auth", get_auth_json()},
    {"cmd", "report_work"},
    {"name", info.project},
    {"payload", info.payload},
    {"id", info.id},
  };

  std::string response = post_json(req);

  std::string err;
  const auto json = json11::Json::parse(response, err);

  std::string status = json["status"].string_value();

  if(status != "Ok") {
    return res_from_string(status);
  }

  return ServerResponse::Ok;
}
