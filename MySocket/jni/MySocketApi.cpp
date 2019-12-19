
#include "MySocketApi.h"
#include "../socket/data_socket.h"
#include "mylog.h"

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

#define FILE_NAME   "123.456"
int PullFromServer(void* client, const char* path) {
  FileSocket* socket = reinterpret_cast<FileSocket*>(client);
  if (nullptr != socket) {
    char fileName[260] = {0};
    sprintf(fileName, "%s/%s", path, FILE_NAME);
    FILE* pFile = fopen(fileName, "wb");
    if (nullptr == pFile)
      return -10;

    SOCKET_DATA data = {0};
    data.ver = DATA_VER;
    data.type = DATA_FILE;
    data.msg = FILE_PULL;
    data.size = 0;

    int len = HEAD_SIZE;
    bool ret = socket->Send((const char*)&data, len);
    LogOut(DEBUG, "Send:%d", ret);
    if (!ret)
      return -11;

    bool close = false;
    int totalSize = 0;
    memset(&data, 0, sizeof(data));
    do
    {
      ret = socket->RecvFileData(&close, data);

      LogOut(DEBUG, "OnDataAvailable:%d  ver:%x, type:%x, msg:%d, size:%d", ret, data.ver, data.type, data.msg, data.size);

      if (ret) {
        totalSize += data.size;
        fwrite(data.data, data.size, 1, pFile);
      }

    } while (ret && !close);

    fclose(pFile);

    LogOut(DEBUG, "OnDataAvailable:%d  close:%d  totalSize:%d", ret, close, totalSize);

    return 0;
  }

  return -1;
}

int SyncFromServer(void* client, const char* path) {
  FileSocket* socket = reinterpret_cast<FileSocket*>(client);
  if (nullptr != socket) {
    char fileName[260] = {0};
    sprintf(fileName, "%s/%s", path, FILE_NAME);

    SOCKET_DATA data = {0};
    data.ver = DATA_VER;
    data.type = DATA_FILE;
    data.msg = FILE_SYNC;
    data.size = 0;

    {
      FILE* pFile = fopen(fileName, "rb");
      if (nullptr == pFile)
        return -10;

      fseek(pFile,0L,SEEK_END); // 定位到文件末尾
      long flen = ftell(pFile); // 得到文件大小
      fseek(pFile,0L,SEEK_SET); // 定位到文件开始
      fclose(pFile);
      char* buf = (char*)data.data;
      sprintf(buf, "%ld", flen);
      data.size = strlen(buf)+4;
      LogOut(DEBUG, "buf:%s   size:%d", buf, data.size);
    }

    int len = HEAD_SIZE;
    bool ret = socket->Send((const char*)&data, len + data.size);
    LogOut(DEBUG, "Send:%d", ret);
    if (!ret)
      return -11;

    ret = socket->SendFile(fileName);

    LogOut(DEBUG, "SendFile:%d ", ret);

    memset(&data, 0, sizeof(data));
    bool close = false;
    int nRet = -12;
    // TODO 等待1分钟
    for (int i = 0; i < 60; i++) {
      ret = socket->RecvSocketData(&close, data);
      LogOut(DEBUG, "RecvSocketData:%d close:%d  ver:%x, type:%x, msg:%d, size:%d",
        ret, close, data.ver, data.type, data.msg, data.size);
      if (ret && !close) {
        nRet = 0;
        break;
      }
    }

    return nRet;
  }

  return -1;
}
