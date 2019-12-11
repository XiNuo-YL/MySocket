// MySocket.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "socket/data_socket.h"

#include <conio.h> // getch
#include <process.h> // _beginthreadex

#include <vector>
typedef std::vector<DataSocket*> SocketArray;
static const size_t kMaxConnections = (FD_SETSIZE - 2);

bool running = true;

void ListenInternal(ListeningSocket& listener) {
  SocketArray sockets;

  while (running) {
    fd_set socket_set;
    FD_ZERO(&socket_set);
    if (listener.valid())
      FD_SET(listener.socket(), &socket_set);
    for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i)
      FD_SET((*i)->socket(), &socket_set);
    struct timeval timeout = {0, 1000*30};
    if (select(FD_SETSIZE, &socket_set, NULL, NULL, &timeout) == SOCKET_ERROR) {
      printf("select failed\n");
      break;
    }

    if (FD_ISSET(listener.socket(), &socket_set)) {
      DataSocket* s = listener.Accept();
      if (sockets.size() >= kMaxConnections) {
        delete s;  // sorry, that's all we can take.
        printf("Connection limit reached   kMaxConnections:%d\n", kMaxConnections);
      } else {
        sockets.push_back(s);
        printf("New connection...\n");
      }
    }
  }

  for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i)
    delete (*i);
  sockets.clear();
}

unsigned __stdcall ListenThread(void * pParam) {

  int port = 8990;
  ListeningSocket listener;
  if (!listener.Create()) {
    printf("Failed to create server socket\n");
    return -1;
  } else if (!listener.Listen(port)) {
    printf("Failed to listen on server socket\n");
    return -1;
  }

  printf("Server listening on port %i\n", port);

  ListenInternal(listener);

  printf("Server stop listening\n");
  listener.Close();

  return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
  HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, &ListenThread, nullptr, 0, nullptr);

  do 
  {
    char ch = _getch();
    if ('q' == ch) {
      running = false;
      printf("Server stoping\n");
    } else {
      printf("Please input \'q\' to quit\n");
    }
  } while (running);

  WaitForSingleObject(handle, 1000*3);

  system("pause");
	return 0;
}

