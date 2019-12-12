// MySocketClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../socket/data_socket.h"

void SendFile(FileSocket* socket, const char* fileName) {
  if (nullptr == socket || nullptr == fileName) {
    printf("Error param...\n");
    return;
  }

  FILE* pFile = fopen(fileName, "rb");
  if (nullptr == pFile) {
    printf("Open %s failed, err:%d", fileName, GetLastError());
    return ;
  }
  char buf[1024] = {0};
  int read = 0;
  long total = 0;
  fseek(pFile,0L,SEEK_END); // ��λ���ļ�ĩβ
  long flen = ftell(pFile); // �õ��ļ���С
  fseek(pFile,0L,SEEK_SET); // ��λ���ļ���ʼ
  do 
  {
    read = fread(buf, sizeof(char), sizeof(buf), pFile);
    if (read > 0) {
      total += read;
      socket->Send(buf, read);
      printf("\rSend %s  %.1f%%", fileName, total*100.0/flen);
    }
  } while (read > 0);
  printf("\n");

  if (nullptr != pFile)
    fclose(pFile);
}

#define FILE_SOCKET_SERVER      "192.168.0.113"
int _tmain(int argc, _TCHAR* argv[])
{
  FileSocket* socket = FileSocket::Connect(FILE_SOCKET_SERVER, FILE_SOCKET_PORT);
  if (nullptr == socket)
    printf("Connect %s:%d failed, err:%d\n", FILE_SOCKET_SERVER, FILE_SOCKET_PORT, WSAGetLastError());
  else {
    // Test Send File
    printf("Connect %s:%d, start sending file\n", FILE_SOCKET_SERVER, FILE_SOCKET_PORT);

    SendFile(socket, "e:\\ffmpeg.dec.yuv");

    Sleep(1*1000);

    socket->Close();
    delete socket;
  }

  system("pause");
	return 0;
}

