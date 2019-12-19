// MySocketServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../socket/data_socket.h"

#include <conio.h> // getch
#include <process.h> // _beginthreadex
#include <assert.h>

#include <vector>
typedef std::vector<FileSocket*> SocketArray;
static const size_t kMaxConnections = (FD_SETSIZE - 2);

bool running = true;

void ListenInternal(ListeningSocket& listener) {
  SocketArray sockets;

  SOCKET_DATA data = {0};
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

    for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i) {
      FileSocket* s = *i;
      bool socket_done = true;
      if (FD_ISSET(s->socket(), &socket_set)) {
        memset(&data, 0, sizeof(data));
        if (s->RecvSocketData(&socket_done, data)) {
          printf("OnDataAvailable:ver:%x, type:%x, msg:%d, size:%d\n", data.ver, data.type, data.msg, data.size);
          if (DATA_FILE == data.type) {
            long fileSize = 0;
            if (FILE_SYNC == data.msg) {
              char* buf = (char*)data.data;
              fileSize = atol((const char*)data.data);
            }
            if (FILE_PULL == data.msg) {
              printf("DATA_FILE->FILE_PULL, now sending file to client...\n");
              s->SendFile("E:\\1.2");
            } else if (FILE_SYNC == data.msg) {
              SOCKET_DATA data = {0};
              FILE* pFile = fopen("E:\\3.4", "wb");
              int totalSize = 0;
              bool ret = false;
              bool close = false;
              do
              {
                ret = s->RecvFileData(&close, data);

                printf("\rRecvFileData:%d  close:%d  %.1f%%", ret, close, totalSize*100.0/fileSize);

                if (ret) {
                  totalSize += data.size;
                  fwrite(data.data, data.size, 1, pFile);
                }
                if (totalSize >= fileSize)
                  break;
              } while (ret && !close);

              printf("\nRecvFileData:%d  close:%d  totalSize:%d\n", ret, close, totalSize);
              fclose(pFile);

              Sleep(5791);

              memset(&data, 0, sizeof(data));
              data.ver = DATA_VER;
              data.type = DATA_FILE;
              data.msg = FILE_PULL;
              data.size = 0;
              s->Send((const char*)&data, HEAD_SIZE + data.size);
            }
          }
        }
      } else {
        socket_done = false;
      }

      if (socket_done) {
        printf("Disconnecting socket\n");
        assert(s->valid());  // Close must not have been called yet.
        FD_CLR(s->socket(), &socket_set);
        delete (*i);
        i = sockets.erase(i);
        if (i == sockets.end())
          break;
      }
    }

    if (FD_ISSET(listener.socket(), &socket_set)) {
      FileSocket* s = listener.AcceptFile();
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

  int port = FILE_SOCKET_PORT;
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

