#include <string>
#include <vector>
#include <stdexcept>

#include "server_common.h"


ServerResponse res_from_string(const std::string& res)
{
  if(res == "Ok") {
    return ServerResponse::Ok;
  } else if(res == "PuzzleExists") {
    return ServerResponse::PuzzleExists;
  } else if(res == "PuzzleDoesNotExist") {
    return ServerResponse::PuzzleDoesNotExist;
  } else if(res == "NoWorkAvailable") {
    return ServerResponse::NoWorkAvailable;
  } else if(res == "InvalidCredentials") {
    return ServerResponse::InvalidCredentials;
  } else if(res == "ServerError") {
    return ServerResponse::ServerError;
  }

  throw std::invalid_argument("Invalid response string");
}

std::string to_string(ServerResponse response)
{
  switch(response) {
  case ServerResponse::Ok:
    return "Ok";
  case ServerResponse::NoWorkAvailable:
    return "NoWorkAvailable";
  case ServerResponse::PuzzleDoesNotExist:
    return "PuzzleDoesNotExist";
  case ServerResponse::PuzzleExists:
    return "PuzzleExists";
  }

  throw std::invalid_argument("Invalid server response");
}


std::string to_friendly_string(ServerResponse response)
{
  switch(response) {
  case ServerResponse::Ok:
    return "Ok";
    break;
  case ServerResponse::NoWorkAvailable:
    return "No work available";
    break;
  case ServerResponse::PuzzleDoesNotExist:
    return "No such puzzle exists";
    break;
  case ServerResponse::PuzzleExists:
    return "Puzzle already exists";
    break;
  case ServerResponse::InvalidCredentials:
    return "Incorrect user name or password";
    break;
  case ServerResponse::ServerError:
    return "Internal server error";
  }

  return "Unknown error.";
}