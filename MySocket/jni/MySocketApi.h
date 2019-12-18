#ifndef _MY_SOCKET_API_H_
#define _MY_SOCKET_API_H_

void* CreateSocketClient(const char* ip, int port);

int DeleteSocketClient(void* client);

#endif // _MY_SOCKET_API_H_
