#include "stdafx.h"
#include "cTcpClient.h"
#include "Diag.h"
#include "cTcp.h"
#include "ToStr.h"

////////////////////////////////////////////////////////////////
cTcpClient::cTcpClient(const unsigned int port, const std::string& ip, bool prependCount) : mPort(port), mServerAddress(ip), mPrependCount(prependCount)
{
  // start as not connected
  mConnected = false;
  ConnectSocket = INVALID_SOCKET;

  // try connecting to server
  TryToConnect();
}


////////////////////////////////////////////////////////////////
cTcpClient::~cTcpClient(void)
{
  // close connection
  DiagTrace(cTcp::moduleName, "cTcpClient::~cTcpClient", "Connection closed");
  if (ConnectSocket != INVALID_SOCKET)
    closesocket(ConnectSocket);
  WSACleanup();
  mConnected = false;
}

////////////////////////////////////////////////////////////////
bool cTcpClient::TryToConnect()
{
  if (!mConnected)
  {
    try
    {
      // create WSADATA object
      WSADATA wsaData;

      // socket
      ConnectSocket = INVALID_SOCKET;

      // holds address info for socket to connect to
#if _MSC_VER >= 1300 // not available in VC6
      struct addrinfo *result = NULL,
        *ptr = NULL,
        hints;
#else
      SOCKADDR_IN target;
#endif

      // Initialize Winsock
      iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

      if (iResult != 0) {
        DiagError(cTcp::moduleName, "cTcpClient::TryToConnect", string("WSAStartup failed with error: ")+ToStr(iResult));
        throw iResult;
      }

      // set address info
#if _MSC_VER > 1300 // not available in VC6
      ZeroMemory( &hints, sizeof(hints) );
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!

      //resolve server address and port 
      string portStr = ToStr(mPort);
      iResult = getaddrinfo(mServerAddress.c_str(), portStr.c_str(), &hints, &result);

      if( iResult != 0 ) 
      {
        DiagError(cTcp::moduleName, "cTcpClient::TryToConnect", string("getaddrinfo failed with error: ")+ToStr(iResult));
        WSACleanup();
        throw iResult;
      }
#else
      ZeroMemory(&target, sizeof(target));
      target.sin_family = AF_INET;
      target.sin_port = htons(mPort);
      target.sin_addr.s_addr = inet_addr(mServerAddress.c_str())/*INADDR_ANY*//*INADDR_LOOPBACK - inet_addr("127.0.0.1"); // local host address only accepted*/;
#endif
      // Attempt to connect to an address until one succeeds
#if _MSC_VER >= 1300 // not available in VC6
      for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
#else
        ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif

        if (ConnectSocket == INVALID_SOCKET) {
          DiagError(cTcp::moduleName, "cTcpClient::TryToConnect", string("socket failed with error: ")+ToStr(WSAGetLastError()));
          WSACleanup();
          throw iResult;
        }

        // Connect to server.
#if _MSC_VER >= 1300 // not available in VC6
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
#else
        iResult = connect( ConnectSocket, (SOCKADDR*) &target, sizeof(target));
#endif

        if (iResult == SOCKET_ERROR)
        {
          closesocket(ConnectSocket);
          ConnectSocket = INVALID_SOCKET;
          DiagWarning(cTcp::moduleName, "cTcpClient::TryToConnect", "The server (" + mServerAddress + ":" + ToStr(mPort)+ ") is down... failed to connect");
        }
#if _MSC_VER >= 1300 // not available in VC6
      }
#endif

#if _MSC_VER >= 1300 // not available in VC6
      // no longer need address info for server
      freeaddrinfo(result);
#endif

      // if connection failed
      if (ConnectSocket == INVALID_SOCKET) 
      {
        DiagWarning(cTcp::moduleName, "cTcpClient::TryToConnect", "--> Unable to connect to server "+mServerAddress+":"+ToStr(mPort));
        WSACleanup();
        throw INVALID_SOCKET;
      }

      // Set the mode of the socket to be nonblocking
      u_long iMode = 1;

      iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
      if (iResult == SOCKET_ERROR)
      {
        DiagError(cTcp::moduleName, "cTcpClient::TryToConnect", string("ioctlsocket failed with error: ")+ToStr(WSAGetLastError()));
        closesocket(ConnectSocket);
        WSACleanup();
        throw iResult;        
      }

      //disable nagle
      char value = 1;
      if (setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value)) != 0)
      {
        DiagError(cTcp::moduleName, "cTcpClient::TryToConnect", string("setsockopt failed with error: ") + ToStr(WSAGetLastError()));
      }
      mConnected = true;
      DiagOut(cTcp::moduleName, "cTcpClient::TryToConnect", "==> Client is now connected to server @"+mServerAddress+":"+ToStr(mPort));
    }
    catch(...)
    {
      // failed to connect
      mConnected = false;
    }
  }
  return mConnected;
}

////////////////////////////////////////////////////////////////
int cTcpClient::ReceivePackets(char * recvbuf) 
{
  iResult = 0;
  if (TryToConnect())
  {
    iResult = recv(ConnectSocket, recvbuf, MAX_BUFFER_SIZE, 0);

    if ( (iResult == 0) || ((iResult == SOCKET_ERROR) &&
      ((WSAGetLastError() == WSAENOTCONN) ||
      (WSAGetLastError() == WSAESHUTDOWN) ||
      (WSAGetLastError() == WSAECONNABORTED) ||
      (WSAGetLastError() == WSAECONNRESET)))
      )
    {
      DiagOut(cTcp::moduleName, "cTcpClient::ReceivePackets", "==> Client has been disconnected from server "+ mServerAddress + ":" + ToStr(mPort));
      closesocket(ConnectSocket);
      WSACleanup();
      mConnected = false; // connection was closed !
    }
  }
  return iResult;
}

////////////////////////////////////////////////////////////////
bool cTcpClient::Send(cTcpMsg* pMsg)
{
  bool sent = false;
  if (mConnected)
  {
    int sres = 0;
    unsigned int packet_size = pMsg->GetSize();
    char *packet_data = NULL;

    if (mPrependCount)
    {
      packet_data = new char[packet_size + 4];
      packet_size += 3;
      packet_data[0] = (char)((packet_size & 0xFF0000) >> 16);
      packet_data[1] = (char)((packet_size & 0xFF00) >> 8);
      packet_data[2] = (char)(packet_size & 0xFF);
      memcpy(packet_data + 3, (const char *)pMsg->GetData(), packet_size-3);
    }
    else
    {
      packet_data = (char *)pMsg->GetData();
    }

    sres = send(ConnectSocket, packet_data, packet_size, 0);

    if (mPrependCount)
    {
      delete[] packet_data;
    }

    if ( (iResult == 0) || ((iResult == SOCKET_ERROR) &&
      ((WSAGetLastError() == WSAENOTCONN) ||
      (WSAGetLastError() == WSAESHUTDOWN) ||
      (WSAGetLastError() == WSAECONNABORTED) ||
      (WSAGetLastError() == WSAECONNRESET)))
      )
    {
      // NOTE: This case is handler in Tx thread to reconnect if needed ---
      //DiagOut(cTcp::moduleName, "cTcpClient::Send", "==> Client has been disconnected from server @"+mServerAddress+":"+ToStr(mPort));
      //closesocket(ConnectSocket);
      //WSACleanup();
      //mConnected = false; // connection was closed !
    }
    else
    {
      sent = true;
    }
  }
  // remove message regardless
  pMsg->Dismiss();
  return(sent);
}
