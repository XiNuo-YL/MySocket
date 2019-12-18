
#include "MySocketApi.h"
#include "../socket/data_socket.h"

void* CreateSocketClient(const char* ip, int port) {
  FileSocket* socket = FileSocket::Connect(ip, port);
  return socket;
}

int DeleteSocketClient(void* client) {
  FileSocket* socket = reinterpret_cast<FileSocket*>(client);
  if (nullptr != socket) {
    delete socket;
    return 0;
  }

  return -1;
}
