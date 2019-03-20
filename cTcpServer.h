#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map>
#include <vector>
#include <set>
#include "cTcpMsg.h"
#include "cTimerHandler.h"
#include "cTimerManager.h"
using namespace std;
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "6543" 


class cClientSocket
{
public:
  cClientSocket(SOCKET sock, string clientAddress = ""){ mSock = sock; mClientAddress = clientAddress;};
  ~cClientSocket(){};
  SOCKET mSock;
  string mClientAddress;
  vector<cTcpMsg*> msgsToSend;
};

class cTcpServer : public cTimerHandler
{
public:
  cTcpServer(unsigned int port, string clientAddresses, cTimerManager* pTimerManager, bool singleClientConnection, bool prependCount);
  ~cTcpServer(void);

  // send data to all clients
  //void SendToAll(char * packets, int totalSize);
  bool SendToAll(cTcpMsg* pMsg);

  // receive incoming data
  int ReceiveData(unsigned int client_id, char * recvbuf);

  // accept new connections
  bool AcceptNewClient(unsigned int & id);

  // get number of clients currently connected
  int GetNbClients();

  // get list of clients currently connected
  string GetClientsList();

  bool IsConnected() {return mConnected;};

  // table to keep track of each client's socket
  std::map<unsigned int, cClientSocket*> sessions;

protected:
  // from cTimerHandler for timer handler callback
  virtual void OnTimerEvent(unsigned int timerId, cTEvent* eventInfo);

private:
  // Socket to listen for new connections
  SOCKET ListenSocket;

  // for error checking return values
  int iResult;

  // timer for handling message to send if queued
  cTimerManager* mpTimerManager;
  unsigned int mSendTimerId;
  void KickSendTimer();
  void StopSendTimer();

  // to protect SendToAll() in multi-thread 
  CRITICAL_SECTION mSendCs;

  // TCP port to listen to
  unsigned int mPort;

  bool mConnected;
  
  // List of clients allowed to connect
  string mAllowedClients;

  bool mPrependCount;  // Count of message is prepending the message on first 3 Bytes

  bool mSingleClientConnection;

};

