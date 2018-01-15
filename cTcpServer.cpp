#pragma warning(disable:4786) // remove an annoying warning
#include "stdafx.h"
#include "cTcpServer.h"
#include "Diag.h"
#include "cTcp.h"
#include "ToStr.h"
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////
cTcpServer::cTcpServer(unsigned int port, string clientAddresses, cTimerManager* pTimerManager, bool singleClientConnection) : mPort(port), mpTimerManager(pTimerManager), mSingleClientConnection(singleClientConnection), mAllowedClients(clientAddresses)
{
  mSendTimerId = INVALID_TIMER;

  // create WSADATA object
  WSADATA wsaData;

  mConnected = false;

  // our sockets for the server
  ListenSocket = INVALID_SOCKET;

  DiagTrace(cTcp::moduleName, "cTcpServer::cTcpServer", string("Create with port: ")+ToStr(port) + string(", clientAddresses: ") + clientAddresses);

#if _MSC_VER >= 1300 // not available in VC6
  // address info for the server to listen to
  struct addrinfo *result = NULL;
  struct addrinfo hints;
#else
  SOCKADDR_IN target;
#endif

  // Create timer
  InitializeCriticalSection(&mSendCs);

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) 
  {
    DiagError(cTcp::moduleName, "cTcpServer::cTcpServer", string("WSAStartup failed with error: ")+ToStr(iResult));
    throw iResult;
  }

  // set address information
#if _MSC_VER >= 1300 // not available in VC6
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  string portStr = ToStr(mPort);
  // Resolve the server address and port
  iResult = getaddrinfo(NULL, portStr.c_str(), &hints, &result);

  if ( iResult != 0 ) {
    DiagError(cTcp::moduleName, "cTcpServer::cTcpServer", string("getaddrinfo failed with error: ")+ToStr(iResult));
    WSACleanup();
    throw iResult;
  }
#else
  ZeroMemory(&target, sizeof(target));
  target.sin_family = AF_INET;
  target.sin_port = htons(mPort);
  target.sin_addr.s_addr = INADDR_ANY/*INADDR_LOOPBACK - inet_addr("127.0.0.1"); // local host address only accepted*/;
#endif


  // Create a SOCKET for connecting to server
#if _MSC_VER >= 1300 // not available in VC6
  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
#else
  ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif

  if (ListenSocket == INVALID_SOCKET) {
    DiagError(cTcp::moduleName, "cTcpServer::cTcpServer", string("socket failed with error: ")+ToStr(WSAGetLastError()));
#if _MSC_VER >= 1300 // not available in VC6
    freeaddrinfo(result);
#endif
    WSACleanup();
    throw iResult;
  }

  // Set the mode of the socket to be nonblocking
  u_long iMode = 1;
  iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);

  if (iResult == SOCKET_ERROR) {
    DiagError(cTcp::moduleName, "cTcpServer::cTcpServer", string("ioctlsocket failed with error: ")+ToStr(WSAGetLastError()));
    closesocket(ListenSocket);
    WSACleanup();
    throw iResult;
  }

  // Setup the TCP listening socket
#if _MSC_VER >= 1300 // not available in VC6
  iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
#else
  iResult = bind( ListenSocket, (SOCKADDR*) &target, sizeof(target));
#endif

  if (iResult == SOCKET_ERROR) {
    DiagError(cTcp::moduleName, "cTcpServer::cTcpServer", string("bind failed with error: ")+ToStr(WSAGetLastError()));
#if _MSC_VER >= 1300 // not available in VC6
    freeaddrinfo(result);
#endif
    closesocket(ListenSocket);
    WSACleanup();
    throw iResult;
  }

#if _MSC_VER >= 1300 // not available in VC6
  // no longer need address information
  freeaddrinfo(result);
#endif

  // start listening for new clients attempting to connect
  iResult = listen(ListenSocket, SOMAXCONN);

  if (iResult == SOCKET_ERROR) {
    DiagError(cTcp::moduleName, "cTcpServer::cTcpServer", string("listen failed with error: ")+ToStr(WSAGetLastError()));
    closesocket(ListenSocket);
    WSACleanup();
    throw iResult;
  }
  else
  {
    DiagError(cTcp::moduleName, "cTcpServer::cTcpServer", string("listening to port: ") + ToStr(port));
  }
}


////////////////////////////////////////////////////////////////
cTcpServer::~cTcpServer(void)
{
  // close listen socket
  closesocket(ListenSocket);
  DiagWarning(cTcp::moduleName, "cTcpServer::~cTcpServer", "Listening socket closed");

  // close all sessions
  SOCKET currentSocket;
  std::map<unsigned int, cClientSocket*>::iterator iter;

  for (iter = sessions.begin(); iter != sessions.end(); iter++)
  {
    currentSocket = iter->second->mSock;
    int id = iter->first;
    DiagWarning(cTcp::moduleName, "cTcpServer::~cTcpServer", string("Server closing client ") + ToStr(id) + " session socket on port "+ToStr(mPort));
    closesocket(currentSocket);
    for (std::vector<cTcpMsg*>::iterator it = iter->second->msgsToSend.begin(); it < iter->second->msgsToSend.end();)
    {
      // Clear message
      (*it)->Dismiss();
      // erase and go to next element
      it = iter->second->msgsToSend.erase(it);
    }
  }

  // cleanup winsock
  WSACleanup();

  DeleteCriticalSection(&mSendCs);
  if (mSendTimerId != INVALID_TIMER)
  {
    mpTimerManager->KillTimer(mSendTimerId);
    mSendTimerId = INVALID_TIMER;
  }
}

////////////////////////////////////////////////////////////////
// accept new connections
bool cTcpServer::AcceptNewClient(unsigned int & id)
{
  // Socket to give to the clients
  SOCKET ClientSocket = INVALID_SOCKET;

  // if client waiting, accept the connection and save the socket
  ClientSocket = accept(ListenSocket,NULL,NULL);

  if (ClientSocket != INVALID_SOCKET) 
  {
    DiagTrace(cTcp::moduleName, "cTcpServer::AcceptNewClient", "Accept New Client");
    // If a list of allowed client is configured check if it is part of it
    if (mAllowedClients.size() > 0)
    {
      struct sockaddr_in adr_inet;
      int len_inet = sizeof(adr_inet);

      if (getpeername(ClientSocket, (struct sockaddr *)&adr_inet, &len_inet) == SOCKET_ERROR)
      {
        DiagError(cTcp::moduleName, "cTcpServer::AcceptNewClient", string("getpeername failed with error: ") + ToStr(WSAGetLastError()));
        //Unable to retrieve peer information 
        closesocket(ClientSocket);
        return false;
      }
      else
      {

#if _MSC_VER > 1700 // not available in VC6, VC7
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET, &(adr_inet.sin_addr), (PSTR)ipstr, sizeof(ipstr));
        string ClientAddress(ipstr);
#else
        string ClientAddress(inet_ntoa(adr_inet.sin_addr));
#endif
        if (mAllowedClients.find(ClientAddress) == string::npos)
        {
          bool foundByHostName = false;

#if _MSC_VER >= 1700 // not available in VC6, VC7
          // Check by host name
          char hostname[NI_MAXHOST], servInfo[NI_MAXSERV];
          DWORD dwRetval = getnameinfo((struct sockaddr *) &adr_inet, sizeof(struct sockaddr), hostname, NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);
          if (dwRetval == 0) {
            if (mAllowedClients.find(hostname) != string::npos)
            {
              foundByHostName = true;
            }
          }
#else
          struct hostent *remoteHost = gethostbyaddr((const char *)&adr_inet.sin_addr, 4, AF_INET);

          if (remoteHost != NULL) 
          {
            if (mAllowedClients.find(remoteHost->h_name) != string::npos)
            {
              foundByHostName = true;
            }
          }
#endif

          if (!foundByHostName)
          {
            DiagWarning(cTcp::moduleName, "cTcpServer::AcceptNewClient", string("Client <") + ClientAddress + string("> not allowed to connect"));
            //Peer is not allowed to connect to server
            closesocket(ClientSocket);
            return false;
          }
        }
      }
    }

    //disable nagle on the client's socket
    char value = 1;
    if (setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value)) != 0)
    {
      DiagError(cTcp::moduleName, "cTcpServer::AcceptNewClient", string("setsockopt failed with error: ") + ToStr(WSAGetLastError()));
    }

    if (mSingleClientConnection)
    {
      // DO NOT ALLOW more than one session at a time - so close anything left before opening a new one
      // close all sessions
      SOCKET currentSocket;
      std::map<unsigned int, cClientSocket*>::iterator iter;

      for (iter = sessions.begin(); iter != sessions.end();)
      {
        currentSocket = iter->second->mSock;
        int myid = iter->first;
        DiagWarning(cTcp::moduleName, "cTcpServer::AcceptNewClient", string("Server closing client ") + ToStr(myid) + " session socket on port "+ToStr(mPort));
        closesocket(currentSocket);
        for (std::vector<cTcpMsg*>::iterator it = iter->second->msgsToSend.begin(); it < iter->second->msgsToSend.end();)
        {
          // Clear message
          (*it)->Dismiss();
          // erase and go to next element
          it = iter->second->msgsToSend.erase(it);
        }
        iter = sessions.erase(iter);
      }
    }

    // insert new client into session id table
    DiagWarning(cTcp::moduleName, "cTcpServer::AcceptNewClient", string("Server has accepted new client id ") + ToStr(id) + " on port "+ ToStr(mPort));
    cClientSocket* pClientSocket = new cClientSocket(ClientSocket);
    sessions.insert( pair<unsigned int, cClientSocket*> (id, pClientSocket) );
    mConnected = true;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////
// get number of clients currently connected
int cTcpServer::GetNbClients()
{
  return(sessions.size());
}


////////////////////////////////////////////////////////////////
// receive incoming data
int cTcpServer::ReceiveData(unsigned int client_id, char * recvbuf)
{
  std::map<unsigned int, cClientSocket*>::iterator iter;
  iter = sessions.find(client_id);
  if( iter != sessions.end() )
  {
    SOCKET currentSocket = sessions[client_id]->mSock;
    iResult = recv(currentSocket, recvbuf, MAX_BUFFER_SIZE, 0);

    if ( (iResult == 0) || ((iResult == SOCKET_ERROR) &&
      ((WSAGetLastError() == WSAENOTCONN) ||
      (WSAGetLastError() == WSAESHUTDOWN) ||
      (WSAGetLastError() == WSAECONNABORTED) ||
      (WSAGetLastError() == WSAECONNRESET)))
      )
    {
      DiagWarning(cTcp::moduleName, "cTcpServer::ReceiveData", "Connection closed with client "+ToStr(client_id)+" on port "+ ToStr(mPort));
      closesocket(currentSocket);
      // clear message queue if any
      for (std::vector<cTcpMsg*>::iterator it = iter->second->msgsToSend.begin(); it < iter->second->msgsToSend.end();)
      {
        // Clear message
        (*it)->Dismiss();
        // erase and go to next element
        it = iter->second->msgsToSend.erase(it);
      }
      // remove session from sessions map
      sessions.erase(iter);
      // signify server state change

      mConnected = false;
    }

    return iResult;
  }

  return 0;
}

////////////////////////////////////////////////////////////////
// send data to all clients
bool cTcpServer::SendToAll(cTcpMsg* pMsg)
{
  EnterCriticalSection(&mSendCs);
  SOCKET currentSocket;
  std::map<unsigned int, cClientSocket*>::iterator iter;
  int iSendResult;
  int totalSize = 0;
  char* packets = NULL;
  bool sent = false;

  for (iter = sessions.begin(); iter != sessions.end(); iter++)
  {
    bool pleaseCloseSocket = false;
    bool newMsgWasQueued = false;
    currentSocket = iter->second->mSock;
    // send any outstanding messages if queued
    if (iter->second->msgsToSend.size() > 0)
    {
      int sres = 0;
      for (vector<cTcpMsg*>::iterator it = iter->second->msgsToSend.begin(); it != iter->second->msgsToSend.end();)
      {
        totalSize = (*it)->GetSize();
        packets = (char *)(*it)->GetData();
        sres = send(currentSocket, packets, totalSize, 0);
        if (sres != SOCKET_ERROR)
        {
          // clear message since it was sent properly
          sent = true;
          (*it)->Dismiss();
          it = iter->second->msgsToSend.erase(it);
        }
        else if ((sres == SOCKET_ERROR) && (WSAGetLastError() == WSAEWOULDBLOCK))
        {
          // add current message to queue
          if (pMsg)
          {
            //DiagWarning(cTcp::moduleName, "cTcpServer::SendToAll", string("Failed to send to queued messages to client " + ToStr(iter->first) + " - queuing new message + kick-in timer."));
            pMsg->AddRef();
            iter->second->msgsToSend.push_back(pMsg);
            newMsgWasQueued = true;
          }
          //else
          //  DiagWarning(cTcp::moduleName, "cTcpServer::SendToAll", string("Failed to send to queued messages to client " + ToStr(iter->first) + " - kick-in timer."));

          // keep message and get out of loop since an error occurred
          // kick timer for resend if not already set
          KickSendTimer();
          // break out of for loop since unable to send
          break;
        }
        else
        {
          DiagWarning(cTcp::moduleName, "cTcpServer::SendToAll", string("An error occurred when trying to send queued message (forcing socket to close) - error ") + ToStr(WSAGetLastError()));
          // there was an error
          pleaseCloseSocket = true;
          // do not continue further with these messages since socket is not good
          break;
        }
      }
    }

    // send current message or queue it if unable to send right away
    if (pMsg && !newMsgWasQueued && !pleaseCloseSocket)
    {
      totalSize = pMsg->GetSize();
      packets = (char *)pMsg->GetData();
      iSendResult = send(currentSocket, packets, totalSize, 0);

      if ((iSendResult == SOCKET_ERROR) && (WSAGetLastError() == WSAEWOULDBLOCK))
      {
        //DiagWarning(cTcp::moduleName, "cTcpServer::SendToAll", string("Failed to send to client " + ToStr(iter->first) + " - queuing message."));
        // Add message to list of non sent message at end - keep order of messages
        pMsg->AddRef();
        iter->second->msgsToSend.push_back(pMsg);
        KickSendTimer();
      }
      else if (iSendResult == SOCKET_ERROR)
      {
        DiagWarning(cTcp::moduleName, "cTcpServer::SendToAll", string("An error occurred when trying to send new message (forcing socket to close) - error ") + ToStr(WSAGetLastError()));
        pleaseCloseSocket = true;
      }
    }

    if (pleaseCloseSocket)
    {
      DiagWarning(cTcp::moduleName, "cTcpServer::SendToAll", string("Failed to send with error: ") + ToStr(WSAGetLastError())
        + ", closing connection with client " + ToStr(iter->first));
      closesocket(currentSocket);
      // clear message queue if any
      for (std::vector<cTcpMsg*>::iterator it = iter->second->msgsToSend.begin(); it < iter->second->msgsToSend.end();)
      {
        // Clear message
        (*it)->Dismiss();
        // erase and go to next element
        it = iter->second->msgsToSend.erase(it);
      }
      // remove session from sessions map
      iter = sessions.erase(iter);
      // signify server state change
      mConnected = false;

    }
    else
    {
      // consider message sent if not closing the socket for at least one connection
      sent = true;
    }
  }
  LeaveCriticalSection(&mSendCs);
  return(sent);
}

////////////////////////////////////////////////////////////////
void cTcpServer::KickSendTimer()
{
  if (mSendTimerId == INVALID_TIMER)
  {
    // create timer
    DiagTrace(cTcp::moduleName, "cTcpServer::KickSendTimer", "Kicking a non-created timer. Recreating timer");
    mSendTimerId = mpTimerManager->CreateTimer(this, 2 * 1000);
  }
  else
  {
    // to reset timer, need to kill it and recreate it
    mpTimerManager->KillTimer(mSendTimerId);
    mSendTimerId = mpTimerManager->CreateTimer(this, 2*1000);
  }
}

////////////////////////////////////////////////////////////////
void cTcpServer::StopSendTimer()
{
  if (mSendTimerId != INVALID_TIMER)
  {
    mpTimerManager->KillTimer(mSendTimerId);
    mSendTimerId = INVALID_TIMER;
  }
}

////////////////////////////////////////////////////////////////
void cTcpServer::OnTimerEvent(unsigned int timerId, cTEvent* eventInfo)
{
  DiagTrace(cTcp::moduleName, "cTcpServer::OnTimerEvent", "Timeout occurred.");

  if (timerId == mSendTimerId)
  {
    // Stop timer since it timed out already
    StopSendTimer();

    // Try to send any remaining message
    DiagTrace(cTcp::moduleName, "cTcpServer::OnTimerEvent", "Try sending any remaining message.");
    SendToAll(NULL);
  }
}