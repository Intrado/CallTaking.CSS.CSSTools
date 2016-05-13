#pragma once
// Networking libraries
#include <winsock2.h>
//#include <Windows.h>
#include <ws2tcpip.h>
#include <stdio.h> 
#include <string>
#include "cTcpMsg.h"

// size of our buffer
#define DEFAULT_BUFLEN 512
// port to connect sockets through 
#define DEFAULT_PORT "6543"
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class cTcpClient
{
public:
  // ctor/dtor
  cTcpClient(const unsigned int port, const std::string& ip);
  ~cTcpClient(void);

  int ReceivePackets(char *);
  // send data
  bool Send(cTcpMsg* pMsg);

  bool IsConnected() {return mConnected;};

  // for error checking function calls in Winsock library
  int iResult;

private:
  // socket for client to connect to server
  SOCKET ConnectSocket;
  bool mConnected;
  bool TryToConnect();
  unsigned int mPort;
  std::string mServerAddress; 
};

