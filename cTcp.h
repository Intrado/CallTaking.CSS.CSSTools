#pragma once
#ifndef CTCP_H
#define CTCP_H
#pragma warning(disable:4786) // remove an annoying warning
#pragma warning(disable:4251)
#include <winsock2.h>
#include <Ws2tcpip.h>
#include "cTcpMsg.h"
#include "cTcpClient.h"
#include "cTcpServer.h"
#include "cTimerHandler.h"
#include "cTimerManager.h"
#include "cEvent.h"
#include "cCritSec.h"

class CSSTOOLS_API cTcp : public cTimerHandler
{
public:
  static const char *moduleName;

  // also defined in CAsync.h - need to match
  enum eStatus{ LinkUp, LinkDown, LinkUnknown };

  enum eModeType { modeClient = 0, modeServer = 1 };

  // function callback prototype
  typedef void (__stdcall *tRxMsgHandlerCb)(cTcpMsg* pMsg);

  // function callback for Delphi
  typedef void(*tOnEventCB)(void* Handle, const char* msg);
  typedef void(*tReportLinkStatusCB)(void* Handle, char* pTyp, cTcp::eStatus state, char *connectedList);

  // structure used for initial parameter passed during creation
  struct CSSTOOLS_API tInitParamsIF
  {
    // TCP specific
    eModeType mode;            // TCP communication mode type, i.e. client or server
    unsigned int port;         // TCP port used (listening port if server and destination port if client mode)
    const char* pServerAddress; // Server address to connect to if client mode
    const char* pClientAddresses; // comma delimited ip addresses list of clients allowed to connect to server
    // heartbeat and link handling behavior
    const char* pHb;                 // Heart beat message to use.
    int hbtoP;                 // Timeout period to send an heart beat if no message was sent during this period (in msec).
    int acktoP;                // Timeout period to receive acknowledge after sending a message (in msec).
    int maxretriesP;           // Number of retries for sending a message.
    int silenttoP;             // The silent timeout period.
    int maxperiodsP;           // The number of silent periods before the link is turned down. 

    // Tx related parameters
    int TxCarStartP;           // Transmission start of block character. -1 for none and TxEndStrP will be used.
    int TxCarEndP;             // Transmission end of block character. -1 for none.
    const char* pTxEndStrP;     // Transmission end of block string.

    // Rx related parameters
    int RxCarStartP;           // Reception start of block character. -1 for none and RxEndStrP will be used.
    int RxCarEndP;             // Reception end of block character. -1 for none.
    const char* pRxEndStrP;    // Reception end of block string.
    bool RemoveEndStr;         // Remove the end of block string from the reception buffer. 

    bool waitForAckP;          // Have to wait for acknowledge after sending a message?
    bool sendAckP;             // Generate an ACK after each message received?

    bool singleClientServer;   // Server that support only one connected client.

    bool prependCount;         // Count of message is prepending the message on first 3 Bytes

    cTimerManager* pTimerManager; //Pointer to the timer manager to use
  };

  // ctor/dtor
  cTcp(const tInitParamsIF* pInitParamsIf);
  virtual ~cTcp();

  // class for callback registration
  class CSSTOOLS_API cRxCb
  {
  public:
    virtual void CALLBACK CBOnEvent(cTcpMsg* pMsg) = 0;
    virtual void CALLBACK CBReportLinkStatus(char* pTyp, eStatus state, char *connectedList) = 0;
  };

  // to send a message over TCP link (assumed to end with null character)
  virtual int Send(char *pMsg);

  // for both server and client
  virtual void Update();

  // Callback registration for received message handling
  virtual void __stdcall RegisterRxMsgHandlerCB(cRxCb* cb) {mpRxMsgHandlerCb = cb;};
  virtual void __stdcall UnregisterRxMsgHandlerCB() {mpRxMsgHandlerCb = 0;};


  int RegisterOnEventCB(cTcp::tOnEventCB onEventCB, void* owner);
  int RegisterReportLinkStatusCB(cTcp::tReportLinkStatusCB reportLinkStatuseCB, void* owner);
  int UnregisterOnEventCB(void* owner);
  int UnregisterReportLinkStatusCB(void* owner);

  // Start
  virtual int Start();

  // to know how many clients are connected when operating in server mode 
  virtual int GetNbClients();

  virtual char* const GetHBMsg() {return((char* const)mInitParams.hb.c_str()); };

  virtual bool IsClientConnected() { return mpClient->IsConnected();}

protected:
  // from cTimerHandler for timer handler callback
  virtual void OnTimerEvent(unsigned int timerId, cTEvent* eventInfo);

private:

    // structure used for initial parameter passed during creation
  typedef struct tInitParams
  {
    // TCP specific
    eModeType mode;            // TCP communication mode type, i.e. client or server
    // heartbeat and link handling behavior
    std::string hb;                 // Heart beat message to use.
    int hbtoP;                 // Timeout period to send an heart beat if no message was sent during this period (in msec).
    int acktoP;                // Timeout period to receive acknowledge after sending a message (in msec).
    int maxretriesP;           // Number of retries for sending a message.
    int silenttoP;             // The silent timeout period.
    int maxperiodsP;           // The number of silent periods before the link is turned down. 

    // Tx related parameters
    int TxCarStartP;           // Transmission start of block character. -1 for none and TxEndStrP will be used.
    int TxCarEndP;             // Transmission end of block character. -1 for none.
    std::string TxEndStrP;     // Transmission end of block string.

    // Rx related parameters
    int RxCarStartP;           // Reception start of block character. -1 for none and RxEndStrP will be used.
    int RxCarEndP;             // Reception end of block character. -1 for none.
    std::string RxEndStrP;     // Reception end of block string.
    bool RemoveEndStr;         // Remove the end of block string from the reception buffer. 

    bool waitForAckP;          // Have to wait for acknowledge after sending a message?
    bool sendAckP;             // Generate an ACK after each message received?
    bool prependCount;         // Count of message is prepending the message on first 3 Bytes
  } tInitParams;

  // Rx thread
  static void RxThread(void *vptr);

  // Tx thread
  static void TxThread(void *vptr);

  // for client mode
  void receiveFromServer();

  // for server mode
  void receiveFromClients();

  // to handle message transmission
  void SendQueuedMsg();
  bool SendMsg(cTcpMsg* pMsg);
  // to get next message to send
  cTcpMsg* GetNextMsgToSend(int timeout = 100); // make the default timeout 100 msec

  // to check if message is an ACK on a NACK
  cTcpMsg::eAckNakStatus IsAckNak(cTcpMsg* pMsg);

  // IDs for the clients connecting for table in cTcpServer
  static unsigned int client_id;

  // The Tcp server object 
  cTcpServer* mpServer;

  // The TCP client object
  cTcpClient* mpClient;

  // data buffer
  char mRxBuf[MAX_BUFFER_SIZE];

  // message handling
  cTcpMsg* mpRxMsg;
  cTcpMsg* mpTxMsg;

  // registered callback function to call for each receive message
  cRxCb* mpRxMsgHandlerCb;

  cCritSec mLockMutex;

  cCritSec mTxQueueMutex;

  tInitParams mInitParams;

  // timer for handling message to send if queued
  cTimerManager* mpTimerManager; //Pointer to the timer manager
  void KickHBTimer();
  void StopHBTimer();
  unsigned int mHeartbeatTimerId;

  // for handling Rx link
  void KickRxTimer();
  void StopRxTimer();
  unsigned int mRxTimerId;

  // for managing ACK/NAK
  cEvent mAckNakEvent;
  cTcpMsg::eAckNakStatus mAckNakStatus;

  // to maintain TX and RX link status
  eStatus mTXstate;
  eStatus mRXstate;
  cCritSec mStateMutex;
  void UpdateTxState(eStatus state);
  void UpdateRxState(eStatus state);

  static const char* const Status[3];
  cTimerManager mTimerManager;

  // thread handles and flags for Tx and Rx
  HANDLE mTxThreadHandle;
  HANDLE mRxThreadHandle;
  bool mTxThreadRunning;
  bool mRxThreadRunning;

  cEvent mTxMsgEvent;
  vector<cTcpMsg*> msgsTxQueue;

  tOnEventCB mpfOnEventCB;
  void* mOnEventOwnerCB;
  tReportLinkStatusCB mpfReportLinkStatusCB;
  void* mOnReportLinkStatusOwnerCB;
};

// exported function prototypes for Delphi

extern "C"
{
  CSSTOOLS_API long Init(
    int mode,                     // TCP communication mode type, i.e. 0 for client or  1 for server
    unsigned int port,            // TCP port used (listening port if server and destination port if client mode)
    const char* pServerAddress,   // Server address to connect to if client mode
    const char* pClientAddresses, // comma delimited ip addresses list of clients allowed to connect to server
    char* hb,                     // Heart beat message to use.
    int hbtoP,                    // Timeout period to send an heart beat if no message was sent during this period (in msec).
    int acktoP,                   // Timeout period to receive acknowledge after sending a message (in msec).
    int maxretriesP,              // Number of retries for sending a message.
    int silenttoP,                // The silent timeout period.
    int maxperiodsP,              // The number of silent periods before the link is turned down. 
    int TxCarStartP,              // Transmission start of block character. -1 for none and TxEndStrP will be used.
    int TxCarEndP,                // Transmission end of block character. -1 for none.
    char* TxEndStrP,              // Transmission end of block string.
    int RxCarStartP,              // Reception start of block character. -1 for none and RxEndStrP will be used.
    int RxCarEndP,                // Reception end of block character. -1 for none.
    char* RxEndStrP,              // Reception end of block string.
    bool RemoveEndStr,            // Remove the end of block string from the reception buffer. 
    bool waitForAckP,             // Have to wait for acknowledge after sending a message?
    bool sendAckP,                // Generate an ACK after each message received?
    bool singleClientServer,      // Server that support only one connected client.
    bool prependCount             // Count of message is prepending the message on first 3 Bytes
    );
  CSSTOOLS_API int Exit(long handle);
  CSSTOOLS_API int Send(long handle, char *pMsg);
  CSSTOOLS_API int Update(long handle);
  CSSTOOLS_API int GetNbClients(long handle);
  CSSTOOLS_API int RegisterOnEventCB(long handle, cTcp::tOnEventCB onEventCB, void* owner);
  CSSTOOLS_API int RegisterReportLinkStatusCB(long handle, cTcp::tReportLinkStatusCB reportLinkStatuseCB, void* owner);
  CSSTOOLS_API int UnregisterOnEventCB(long handle, void* owner);
  CSSTOOLS_API int UnregisterReportLinkStatusCB(long handle, void* owner);
}

#endif