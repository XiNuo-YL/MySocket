/*
 *  Copyright 2011 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef EXAMPLES_PEERCONNECTION_SERVER_DATA_SOCKET_H_
#define EXAMPLES_PEERCONNECTION_SERVER_DATA_SOCKET_H_

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

#include <string>

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

private:
  FILE* file_;
};

// The server socket.  Accepts connections and generates DataSocket instances
// for each new connection.
class ListeningSocket : public SocketBase {
 public:
  ListeningSocket() {}

  bool Listen(unsigned short port);
  FileSocket* Accept() const;
};

#endif  // EXAMPLES_PEERCONNECTION_SERVER_DATA_SOCKET_H_
