#ifndef _SERVER_COMMON_H
#define _SERVER_COMMON_H

enum class ServerResponse {
    Ok,
    NoWorkAvailable,
    PuzzleExists,
    PuzzleDoesNotExist,
    InvalidCredentials,
    ServerError
};

ServerResponse res_from_string(const std::string& response);
std::string to_string(ServerResponse response);
std::string to_friendly_string(ServerResponse response);

#endif