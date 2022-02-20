#ifndef _MP_SOCKET_H
#define _MP_SOCKET_H

#include "socketlib.h"

class MessagePacket {

private:
  uint8_t* _data = nullptr;
  uint32_t _size = 0;

public:

  MessagePacket(uint32_t size) : _size(size) {}

  MessagePacket(void* data, uint32_t size) : _size(size)
  {
    _data = new uint8_t[_size];
    memcpy(_data, data, _size);
  }

  MessagePacket(const MessagePacket& other) : MessagePacket(other._data, other._size) {}

  MessagePacket(MessagePacket& other) : MessagePacket(other._data, other._size) {}

  ~MessagePacket()
  {
    delete[] _data;
  }

  void* data()
  {
    return (void*)_data;
  }

  uint32_t size()
  {
    return _size;
  }

  MessagePacket& operator=(const MessagePacket& other)
  {
    _size = other._size;
    _data = new uint8_t[_size];
    memcpy(_data, other._data, _size);

    return *this;
  }
};

class MPSocket {

private:
  RawSocket _raw_socket;

public:
  MPSocket(RawSocket raw_socket)
  {
    _raw_socket = raw_socket;
  }

  void send_msg(const void* msg, size_t len)
  {
    uint32_t len32 = static_cast<uint32_t>(len);

    // Send message length
    _raw_socket.send_data(&len, sizeof(len32));

    // Send message
    if(len != 0) {
      _raw_socket.send_data(msg, len);
    }
  }

  void recv_msg(uint8_t** msg, uint32_t* msg_len)
  {
    uint32_t len = 0;
    uint8_t* tmp = nullptr;

    _raw_socket.recv_all(&len, sizeof(len));

    if(len != 0) {
      tmp = new uint8_t[len];
      _raw_socket.recv_all(tmp, len);
    }

    *msg = tmp;
    *msg_len = len;
  }

  MessagePacket recv_msg()
  {
    uint8_t* msg;
    uint32_t len = 0;

    recv_msg(&msg, &len);

    MessagePacket mp(msg, len);

    delete[] msg;

    return mp;
  }
};

#endif