/*
 *  Copyright 2011 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "data_socket.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <assert.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h> // POSIX
#endif

static const char kHeaderTerminator[] = "\r\n\r\n";
static const int kHeaderTerminatorLength = sizeof(kHeaderTerminator) - 1;

#if defined(WIN32)
class WinsockInitializer {
  static WinsockInitializer singleton;

  WinsockInitializer() {
    WSADATA data;
    WSAStartup(MAKEWORD(1, 0), &data);
  }

 public:
  ~WinsockInitializer() { WSACleanup(); }
};
WinsockInitializer WinsockInitializer::singleton;
#endif

//
// SocketBase
//

bool SocketBase::Create() {
  assert(!valid());
  socket_ = ::socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM:TCP   SOCK_DGRAM:UDP
  return valid();
}

void SocketBase::Close() {
  if (socket_ != INVALID_SOCKET) {
    closesocket(socket_);
    socket_ = INVALID_SOCKET;
  }
}

//
// ListeningSocket
//

bool ListeningSocket::Listen(unsigned short port) {
  assert(valid());
  int enabled = 1;
  setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR,
             reinterpret_cast<const char*>(&enabled), sizeof(enabled));
  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  if (bind(socket_, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) ==
      SOCKET_ERROR) {
    printf("bind failed\n");
    return false;
  }
  return listen(socket_, 5) != SOCKET_ERROR;
}

FileSocket* ListeningSocket::Accept() const {
  assert(valid());
  struct sockaddr_in addr = {0};
  socklen_t size = sizeof(addr);
  NativeSocket client =
      accept(socket_, reinterpret_cast<sockaddr*>(&addr), &size);
  if (client == INVALID_SOCKET)
    return NULL;

  return new FileSocket(client);
}

FileSocket* FileSocket::Connect(const char* ip, unsigned short port) {

  struct sockaddr_in server_addr = {0};
  server_addr.sin_addr.S_un.S_addr=inet_addr(ip);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  int m_iRecvTimeOut = 1000;

  int nSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (INVALID_SOCKET == nSocket) {
      return nullptr;
  }

  int ret = setsockopt(nSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&m_iRecvTimeOut, sizeof(m_iRecvTimeOut));

  if (connect(nSocket, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1) 
  {
      //pDlg->SetDlgItemText(IDOK, "connect  return -1");
      closesocket(nSocket);
      return nullptr;
  }
  return new FileSocket(nSocket);
}

FileSocket::~FileSocket() {
  if (nullptr != file_)
    fclose(file_);
}

bool FileSocket::OnDataAvailable(bool* close_socket) {
  assert(valid());
  char buffer[0xfff] = {0};
  int bytes = recv(socket_, buffer, sizeof(buffer), 0);

  printf("OnDataAvailable   bytes:%d\n", bytes);

  if (bytes == SOCKET_ERROR || bytes == 0) {
    *close_socket = true;
    return false;
  }

  *close_socket = false;
  if (nullptr == file_) {
    file_ = fopen("E:\\server.recv", "wb");
  }
  if (nullptr != file_) {
    fwrite(buffer, 1, bytes, file_);
  }

  bool ret = true;
  
  return ret;
}

bool FileSocket::Send(const char* buf, int len) const {
  return send(socket_, buf, len, 0) != SOCKET_ERROR;
}