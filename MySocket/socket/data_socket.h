/*
 *  Copyright 2011 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef _DATA_SOCKET_H_
#define _DATA_SOCKET_H_

#ifdef WIN32
#include <winsock2.h>
typedef int socklen_t;
typedef SOCKET NativeSocket;
#else
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#define closesocket close
typedef int NativeSocket;

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET static_cast<NativeSocket>(-1)
#endif
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif
#ifndef uint16_t
#define uint16_t unsigned short
#endif

#include <string>

enum DATA_TYPE{
  DATA_FILE = 0x19,
};
enum FILE_MESSAGE {
  FILE_PULL   = 1,
  FILE_SYNC,
  FILE_PUSH,
};
#define HEAD_SIZE 5 // 根据 SOCKET_DATA排除data所占用的大小
#define MTU 10240
#define MAX_DATA_SIZE (MTU-HEAD_SIZE)
#define DATA_VER 0x5A
typedef struct tagSOCKET_DATA{
  BYTE ver; // 版本号 0x5A 开始累加
  BYTE type; // 类型 DATA_TYPE
  BYTE msg; // DATA_TYPE对应的消息类型
  uint16_t size; // 数据长度
  BYTE data[MAX_DATA_SIZE]; // 实际负载数据
}SOCKET_DATA, *PSOCKET_DATA;

class SocketBase {
 public:
  SocketBase() : socket_(INVALID_SOCKET) {}
  explicit SocketBase(NativeSocket socket) : socket_(socket) {}
  ~SocketBase() { Close(); }

  NativeSocket socket() const { return socket_; }
  bool valid() const { return socket_ != INVALID_SOCKET; }

  bool Create();
  void Close();

 protected:
  NativeSocket socket_;
};



#define FILE_SOCKET_PORT      8990
class FileSocket : public SocketBase {
public:
  explicit FileSocket(NativeSocket socket)
      : SocketBase(socket), file_(nullptr) {}
  ~FileSocket();

  static FileSocket* Connect(const char* ip, unsigned short port);

  // Called when we have received some data from clients.
  // Returns false if an error occurred.
  bool OnDataAvailable(bool* close_socket);

  // Send a raw buffer of bytes.
  bool Send(const char* buf, int len) const;

  bool SendFile(const char* fileName);

  bool RecvSocketData(bool* close_socket, SOCKET_DATA& data);

  bool RecvFileData(bool* close_socket, SOCKET_DATA& data);

private:
  FILE* file_;
};

// The server socket.  Accepts connections and generates DataSocket instances
// for each new connection.
class ListeningSocket : public SocketBase {
 public:
  ListeningSocket() {}

  bool Listen(unsigned short port);
  FileSocket* AcceptFile() const;
};

#endif  // _DATA_SOCKET_H_
