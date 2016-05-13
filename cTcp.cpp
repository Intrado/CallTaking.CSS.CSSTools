#include "stdafx.h"
#include "cTcp.h"
#include <process.h>
#include "Diag.h"
#include "cAutoLock.h"
#include "ToStr.h"

unsigned int cTcp::client_id; 

const char *cTcp::moduleName = "Tcp";

const char* const cTcp::Status[] =
{
  "LinkUp",
  "LinkDown",
  "LinkUnknown"
};

////////////////////////////////////////////////////////////////
cTcp::cTcp(const tInitParamsIF* pInitParamsIf) : 
  pClient(NULL), pServer(NULL), pRxMsg(NULL), pTxMsg(NULL), pRxMsgHandlerCb(),
  mHeartbeatTimerId(INVALID_TIMER), mRxTimerId(INVALID_TIMER), mTXstate(LinkDown), mRXstate(LinkDown), mRxThreadHandle(NULL), mTxThreadHandle(NULL),
  mTxThreadRunning(false)
{
  DiagTrace(moduleName, "cTcp::cTcp", string("Started Tcp")+(mInitParams.mode == modeClient ? " in Client mode" : " in Server mode"));
  // id's to assign clients for our table
  client_id = 0;

  mInitParams.mode = pInitParamsIf->mode;
  if (pInitParamsIf->pHb)
  {
    mInitParams.hb = pInitParamsIf->pHb;
  }
  mInitParams.hbtoP = pInitParamsIf->hbtoP;
  mInitParams.acktoP = pInitParamsIf->acktoP;
  mInitParams.maxretriesP = pInitParamsIf->maxretriesP;
  mInitParams.silenttoP = pInitParamsIf->silenttoP;
  mInitParams.maxperiodsP = pInitParamsIf->maxperiodsP;
  mInitParams.TxCarStartP = pInitParamsIf->TxCarStartP;
  mInitParams.TxCarEndP = pInitParamsIf->TxCarEndP;
  if (pInitParamsIf->pTxEndStrP)
  {
    mInitParams.TxEndStrP = pInitParamsIf->pTxEndStrP;
  }
  mInitParams.RxCarStartP = pInitParamsIf->RxCarStartP;
  mInitParams.RxCarEndP = pInitParamsIf->RxCarEndP;
  if (pInitParamsIf->pRxEndStrP)
  {
    mInitParams.RxEndStrP = pInitParamsIf->pRxEndStrP;
  }
  mInitParams.waitForAckP = pInitParamsIf->waitForAckP;
  mInitParams.sendAckP = pInitParamsIf->sendAckP; 

  mInitParams.RemoveEndStr = pInitParamsIf->RemoveEndStr;

  mpTimerManager = pInitParamsIf->pTimerManager;

  std::string clientAddresses, serverAddress;
  if (pInitParamsIf->pClientAddresses)
  {
    clientAddresses = pInitParamsIf->pClientAddresses;
  }

  if (pInitParamsIf->pServerAddress)
  {
    serverAddress = pInitParamsIf->pServerAddress;
  }

  // server mode
  if (mInitParams.mode == modeServer)
  {
    // set up the server to listen 
    pServer = new cTcpServer(pInitParamsIf->port, clientAddresses, mpTimerManager, pInitParamsIf->singleClientServer);
  }

  // client mode
  else if (mInitParams.mode == modeClient)
  {
    pClient = new cTcpClient(pInitParamsIf->port, serverAddress);
  }

}

////////////////////////////////////////////////////////////////
cTcp::~cTcp(void)
{
  // stop rx and tx threads
  if (mRxThreadRunning)
  {
    mRxThreadRunning = false;
    WaitForSingleObject(mRxThreadHandle, INFINITE);
  }
  DiagTrace(moduleName, "cTcp::~cTcp", "RX thread stopped.");
  if (mTxThreadRunning)
  {
    mTxThreadRunning = false;
    WaitForSingleObject(mTxThreadHandle, INFINITE);
  }
  DiagTrace(moduleName, "cTcp::~cTcp", "TX thread stopped.");

  if (mHeartbeatTimerId != INVALID_TIMER)
  {
    mpTimerManager->KillTimer(mHeartbeatTimerId);
    mHeartbeatTimerId = INVALID_TIMER;
  }
  DiagTrace(moduleName, "cTcp::~cTcp", "HB timer stopped.");
  if (mRxTimerId != INVALID_TIMER)
  {
    mpTimerManager->KillTimer(mRxTimerId);
    mRxTimerId = INVALID_TIMER;
  }
  DiagTrace(moduleName, "cTcp::~cTcp", "RX timer stopped.");

  if (pClient) delete pClient;
  DiagTrace(moduleName, "cTcp::~cTcp", "pClient deleted.");
  if (pServer) delete pServer;
  DiagTrace(moduleName, "cTcp::~cTcp", "pServer deleted.");
  if (pRxMsg) delete pRxMsg;
  DiagTrace(moduleName, "cTcp::~cTcp", "pRxMsg deleted.");
  // clean up tx message queue
  for (std::vector<cTcpMsg*>::iterator it = msgsTxQueue.begin(); it != msgsTxQueue.end();)
  {
    (*it)->Dismiss();
    it = msgsTxQueue.erase(it);
  }
  DiagTrace(moduleName, "cTcp::~cTcp", "TX message queue cleaned up.");
}

////////////////////////////////////////////////////////////////
void cTcp::Update()
{
  /*
   * SERVER MODE
   */
  bool initialConnectedState = false;
  bool currentConnectedState = false;

  if (pServer)
  {
    initialConnectedState = pServer->IsConnected();
    cAutoLock lock(mLockMutex);
    // get new clients
    if(pServer->AcceptNewClient(client_id))
    {
      DiagWarning(moduleName, "cTcp::Update", "Client "+ToStr(client_id)+" has been connected to the server");
      client_id++;
    }

    receiveFromClients();

    currentConnectedState = pServer->IsConnected();
  }
  /*
   * CLIENT MODE
   */
  else if (pClient)
  {
    initialConnectedState = pClient->IsConnected();
    bool wasconnected = pClient->IsConnected();
    receiveFromServer();
    if (!wasconnected && pClient->IsConnected() && (mInitParams.hb != ""))
    {
      // Send heartbeat message
      DiagTrace(moduleName, "cTcp::Update", "Sending hearbeat message after it was re-connected");
      Send((char *)mInitParams.hb.c_str());
    }
    else if (wasconnected && !pClient->IsConnected())
    {
      // stop heartbeat timer
      DiagTrace(moduleName, "cTcp::Update", "Stopping hearbeat message since disconnected");
      StopHBTimer();
    }
    currentConnectedState = pClient->IsConnected();
  }

  if (initialConnectedState != currentConnectedState)
  {
    if (currentConnectedState)
    {
      if (pRxMsgHandlerCb)
      {
        pRxMsgHandlerCb->CBReportLinkStatus("SOCKET", LinkUp);
      }
      DiagWarning(moduleName, "cTcp::Update", string("Socket state changed to ")+(Status[LinkUp]));
    }
    else
    {
      if (pRxMsgHandlerCb)
      {
        pRxMsgHandlerCb->CBReportLinkStatus("SOCKET", LinkDown);
      }
      DiagWarning(moduleName, "cTcp::Update", string("Socket state changed to ")+(Status[LinkDown]));
    }
  }
}

////////////////////////////////////////////////////////////////
// used to verify if receive message is a NAK or ACK
// will only check if setup to wait for ACK 
cTcpMsg::eAckNakStatus cTcp::IsAckNak(cTcpMsg* pMsg)
{
  cTcpMsg::eAckNakStatus status = cTcpMsg::noackReceived;

  if (mInitParams.waitForAckP)
  {
    char c = pMsg->GetData()[0];
    if (c == ACK)
    {
      status = cTcpMsg::ackReceived;
      mAckNakStatus = cTcpMsg::ackReceived;
      mAckNakEvent.Signal();
      DiagWarning(moduleName, "cTcp::IsAckNak", "ACK");
    }
    else if (c == NAK)
    {
      status = cTcpMsg::nakReceived;
      mAckNakStatus = cTcpMsg::nakReceived;
      mAckNakEvent.Signal();
      DiagWarning(moduleName, "cTcp::IsAckNak", "NAK");
    }
  }
  return status;
}

////////////////////////////////////////////////////////////////
void cTcp::receiveFromServer()
{
  // for client only
  if (pClient)
  {
    memset(mRxBuf, 0, sizeof(mRxBuf));
    int data_length = pClient->ReceivePackets(mRxBuf);

    if (data_length <= 0) 
    {
      //no data received
      return;
    }

    // handle received data to build receive message
    if (pRxMsg == NULL)
    {
      pRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr);
    }

    int i = 0;
    int lenAdded = 0;
    while (pRxMsg && (i < data_length))
    {
      lenAdded = pRxMsg->Add(&mRxBuf[i], data_length-i);
      if (lenAdded < 0)
      {
        // an error occurred, skip data
        return;
      }
      else if (lenAdded == 0)
      {
        // this case should not occur since it should have been processed below once completed
      }
      i += lenAdded;

      // if message is complete, or no delimiter exists process it
      if (pRxMsg->isCompleted() || ((mInitParams.RxCarStartP == -1) && (mInitParams.RxEndStrP == "")))
      {
        KickRxTimer(); // since a message has been received, reset Rx timer
        if (IsAckNak(pRxMsg) == cTcpMsg::noackReceived) // use callback if not an ACK or NAK handled within this class
        {
          // callback function needs to be called
          DiagTrace(moduleName, "cTcp::receiveFromServer", "Callback function is called since a complete message was received.");
          // pass received message to callback if valid
          if (pRxMsgHandlerCb)
          {
            pRxMsgHandlerCb->CBOnEvent(pRxMsg);
            // de-allocation needs to take place in the handler since allocating a new one for next message
            pRxMsg->Dismiss(); pRxMsg = NULL; // !+ea - need to do that here since consumed in callback but not freed
            pRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr);
          }
          else
          {
            // clear message for now -- or TBC: delete handle by called back entity and new message to instantiate if handle
            pRxMsg->Clear();
          }
        }
        else
        {
          // de-allocation needs to take place for this ACK/NAK message
          pRxMsg->Dismiss(); pRxMsg = NULL;
          pRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////
// used only if mode is server
void cTcp::receiveFromClients()
{
  // for server only
  if (pServer)
  {
    // go through all clients
    std::map<unsigned int, cClientSocket*>::iterator iter;

    for(iter = pServer->sessions.begin(); iter != pServer->sessions.end(); iter++)
    {
      memset(mRxBuf, 0, sizeof(mRxBuf));
      int data_length = pServer->ReceiveData(iter->first, mRxBuf);

      if (data_length <= 0) 
      {
        //no data received
        return;
      }

      // handle received data to build receive message
      if (pRxMsg == NULL)
      {
        pRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr);
      }

      int i = 0;
      int lenAdded = 0;
      while (pRxMsg && (i < data_length))
      {
        lenAdded = pRxMsg->Add(&mRxBuf[i], data_length-i);
        if (lenAdded < 0)
        {
          // an error occurred, skip data
          break;
        }
        else if (lenAdded == 0)
        {
          // this case should not occur since it should have been processed below once completed
        }
        i += lenAdded;

        // if message is complete, process it
        if (pRxMsg->isCompleted() || ((mInitParams.RxCarStartP == -1) && (mInitParams.RxEndStrP == "")))
        {
          KickRxTimer(); // since a message has been received, reset Rx timer
          if (IsAckNak(pRxMsg) == cTcpMsg::noackReceived) // use callback if not an ACK or NAK handled within this class
          {
            // callback function needs to be called
            DiagTrace(moduleName, "cTcp::receiveFromClients", "Callback function is called since a complete message was received.");
            // use callback if valid
            if (pRxMsgHandlerCb)
            {
              pRxMsgHandlerCb->CBOnEvent(pRxMsg);
              // de-allocation needs to take place in the handler since allocating a new one for next message
              pRxMsg->Dismiss(); pRxMsg = NULL;
              pRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr);
            }
            else
            {
              // clear message for now -- or TBC: delete handle by called back entity and new message to instantiate if handle
              pRxMsg->Clear();
            }
          }
          else
          {
              // de-allocation needs to take place for this ACK/NAK message
              pRxMsg->Dismiss(); pRxMsg = NULL;
              pRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr);
          }
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////
// create and put message into queue
int cTcp::Send(char* pMsg)
{
  int res = 0;
  DiagTrace(moduleName, "cTcp::Send", "Create message");
  cTcpMsg* pToSendMsg = new cTcpMsg(pMsg, mInitParams.TxCarStartP, mInitParams.TxCarEndP, mInitParams.TxEndStrP, mInitParams.RemoveEndStr);
  DiagTrace(moduleName, "cTcp::Send", "Put message into queue");

  if (mInitParams.waitForAckP)
  {
    // signal thread that something is ready to be sent
    pToSendMsg->AddRef();

    DiagWarning(moduleName, "Send SIGNAL", pToSendMsg->GetData());

    {
      cAutoLock qlock(mTxQueueMutex);
      msgsTxQueue.push_back(pToSendMsg);
    }
    mTxMsgEvent.Signal();

    if (pToSendMsg->mAckEvent.Wait(mInitParams.acktoP + 250))
    {
      if (pToSendMsg->mAckNakStatus != cTcpMsg::ackReceived)
      {
        res = -1;
        cAutoLock qlock(mTxQueueMutex);
        msgsTxQueue.clear();
      }
    }
    else
    {
      res = -1;
      cAutoLock qlock(mTxQueueMutex);
      msgsTxQueue.clear();
    }
    pToSendMsg->Dismiss();
  }
  else
  {
    cAutoLock qlock(mTxQueueMutex);
    msgsTxQueue.push_back(pToSendMsg);

    // signal thread that something is ready to be sent
    mTxMsgEvent.Signal();
  }

  return res;
}

////////////////////////////////////////////////////////////////
// wait for data to be ready up to a timeout
// returns NULL if no message is ready to be sent
cTcpMsg* cTcp::GetNextMsgToSend(int timeout /*= 100*/)
{
  cTcpMsg* pMsg = NULL;
  mTxMsgEvent.Wait(timeout); // this timeout defines how quick the thread loops when there is no data

  cAutoLock qlock(mTxQueueMutex);
  if (!msgsTxQueue.empty())
  {
    // get front message to send and remove from queue
    pMsg = msgsTxQueue.front();
    msgsTxQueue.erase(msgsTxQueue.begin());
  }

  return pMsg;
}

////////////////////////////////////////////////////////////////
// -- called from Tx thread loop to send a queued message
// wait until there is a message to be sent
// if a message is ready to be sent within the timeout period, then
// the message is sent and, if an ACK is expected, waits for it
// for the timeout configured and resend the same message up to the
// number of tries configured until an ACK is received
//
void cTcp::SendQueuedMsg()
{
  cTcpMsg* pToSendMsg = GetNextMsgToSend();

  // if no message to be sent, return to loop
  if (pToSendMsg == NULL)
    return;

  DiagTrace(moduleName, "cTcp::SendQueuedMsg", "There is a message to be sent");
  mAckNakStatus = cTcpMsg::noackReceived; // reset ACK receive status
  pToSendMsg->AddRef(); // make sure message cannot be deleted until we complete since it may need to be resent
  if (SendMsg(pToSendMsg))
  {
    KickHBTimer();

    // check if it should wait for ACK before continuing
    if (mInitParams.waitForAckP)
    {
      DiagTrace(moduleName, "cTcp::SendQueuedMsg", "Waiting for ACK after sending message");
      int retriesLeft = mInitParams.maxretriesP + 1;
      while (retriesLeft)
      {
        retriesLeft--;
        // wait for ACK 
        if (mAckNakEvent.Wait(mInitParams.acktoP) && (mAckNakStatus != cTcpMsg::nakReceived))
        {
          // signal TX link up since ACK was received in time
          UpdateTxState(LinkUp);
        
          DiagTrace(moduleName, "cTcp::SendQueuedMsg", "ACK received as expected");
          // ACK was received on time as expected - nothing else to do here
          retriesLeft = 0;
          DiagWarning(moduleName, "cTcp::SendQueuedMsg SIGNAL", pToSendMsg->GetData());
          pToSendMsg->mAckNakStatus = mAckNakStatus;
          pToSendMsg->mAckEvent.Signal();
        }
        else // ACK wait timed out
        {
          if (retriesLeft)
          {
            DiagWarning(moduleName, "cTcp::SendQueuedMsg", "ACK TO, re-sending");
            mAckNakStatus = cTcpMsg::noackReceived; // reset ACK receive status
            // re-send message
            SendMsg(pToSendMsg);
            KickHBTimer();
          }
          else
          {
            DiagWarning(moduleName, "cTcp::SendQueuedMsg", "ACK TO, TX link goes down");
            // signal TX link down since ACK has timed out for the number of time
            UpdateTxState(LinkDown);
            pToSendMsg->mAckNakStatus = mAckNakStatus;
            pToSendMsg->mAckEvent.Signal();
          }
        }
      }
    }
    else
    {
      // a message was sent, consider TX link as up
      if ((pClient && pClient->IsConnected()) || !pClient)
        UpdateTxState(LinkUp);
    }
  }
  pToSendMsg->Dismiss();
}

////////////////////////////////////////////////////////////////
bool cTcp::SendMsg(cTcpMsg* pMsg)
{
  if (pMsg == NULL)
  {
    DiagError(moduleName, "cTcp::SendMsg", "message pointer is NULL");
    return false;
  }

  cAutoLock lock(mLockMutex);

  // send action packet
  if (pServer)
  {
    return(pServer->SendToAll(pMsg));
  }
  if (pClient)
  {
    return(pClient->Send(pMsg));
  }
  return false;
}

////////////////////////////////////////////////////////////////
// to start TX and RX threads and send first HB if needed
int cTcp::Start()
{
  // create thread with arguments for the receive side
  mRxThreadRunning = true;
  mRxThreadHandle = (HANDLE) _beginthread(cTcp::RxThread, 0, (void *)this);
  // start Tx thread
  mTxThreadRunning = true;
  mTxThreadHandle = (HANDLE) _beginthread(cTcp::TxThread, 0, (void *)this);
  // send initial heartbeat if set to get things going
  if ((mInitParams.hb != "") && (!pClient || (pClient && pClient->IsConnected())))
  {
    // Send heartbeat message
    DiagWarning(moduleName, "cTcp::Start", "Sending first hearbeat message.");
    Send((char *)mInitParams.hb.c_str());
  }

  return 0;
}

////////////////////////////////////////////////////////////////
int cTcp::GetNbClients()
{
  int nb = 0;
  if (pServer)
  {
    cAutoLock lock(mLockMutex);
    nb = pServer->GetNbClients();
  }
  return nb;
}


////////////////////////////////////////////////////////////////
void cTcp::RxThread(void *vptr)
{
  cTcp* ptr = (cTcp *)vptr;
  while (ptr->mRxThreadRunning)
  {
    ptr->Update();
    Sleep(10);
  }
  DiagWarning(moduleName, "cTcp::RxThread", "Exiting");
}

////////////////////////////////////////////////////////////////
void cTcp::TxThread(void *vptr)
{
  cTcp* ptr = (cTcp *)vptr;
  while (ptr->mTxThreadRunning)
  {
    // to do
    ptr->SendQueuedMsg();
  }
  DiagWarning(moduleName, "cTcp::TxThread", "Exiting");
}

////////////////////////////////////////////////////////////////
void cTcp::KickHBTimer()
{
  // if not setup to send Heartbeat, do nothing and simply return
  if (mInitParams.hb == "")
    return;

  if (mHeartbeatTimerId == INVALID_TIMER)
  {
    // create timer
    DiagTrace(cTcp::moduleName, "cTcp::KickHBTimer", "Kicking a non-created timer. Recreating HB timer TOUT = "+ToStr(mInitParams.hbtoP));
    mHeartbeatTimerId = mpTimerManager->CreateTimer(this, mInitParams.hbtoP);
  }
  else
  {
    // to reset timer, need to kill it and recreate it
    DiagTrace(cTcp::moduleName, "cTcp::KickHBTimer", "Kicking timer. Recreating HB timer TOUT = "+ToStr(mInitParams.hbtoP));
    mpTimerManager->KillTimer(mHeartbeatTimerId);
    mHeartbeatTimerId = mpTimerManager->CreateTimer(this, mInitParams.hbtoP);
  }
}

////////////////////////////////////////////////////////////////
void cTcp::StopHBTimer()
{
  if (mHeartbeatTimerId != INVALID_TIMER)
  {
    DiagTrace(cTcp::moduleName, "cTcp::StopHBTimer", "Stopping HB timer.");
    mpTimerManager->KillTimer(mHeartbeatTimerId);
    mHeartbeatTimerId = INVALID_TIMER;
  }
}

////////////////////////////////////////////////////////////////
void cTcp::KickRxTimer()
{
  // Report Rx link up
  UpdateRxState(LinkUp);

  // if not setup to send Heartbeat
  if (mInitParams.silenttoP == 0 ||  mInitParams.maxperiodsP == 0)
    return;

  if (mRxTimerId != INVALID_TIMER)
  {
    // stop timer first to reset it
    mpTimerManager->KillTimer(mRxTimerId);
  }
  // create timer
  mRxTimerId = mpTimerManager->CreateTimer(this, mInitParams.silenttoP * mInitParams.maxperiodsP);
}

////////////////////////////////////////////////////////////////
void cTcp::StopRxTimer()
{
  if (mRxTimerId != INVALID_TIMER)
  {
    mpTimerManager->KillTimer(mRxTimerId);
    mRxTimerId = INVALID_TIMER;
  }
}

////////////////////////////////////////////////////////////////
void cTcp::OnTimerEvent(unsigned int timerId, cTEvent* eventInfo)
{
  DiagTrace(cTcp::moduleName, "cTcp::OnTimerEvent", "Timeout occurred.");

  if (timerId == mHeartbeatTimerId)
  {
    // Stop timer since it timed out already
    StopHBTimer();

    // Send heartbeat message
    DiagWarning(cTcp::moduleName, "cTcp::OnTimerEvent", "Got HB timer event.  Sending a HB message.");
    Send((char *)mInitParams.hb.c_str());
  }
  else if (timerId == mRxTimerId) // Rx timer has timed out, means link is down
  {
    DiagWarning(cTcp::moduleName, "cTcp::OnTimerEvent", "Got RX timer event.");
    // Stop timer since it timed out already
    StopRxTimer();

    // Report Rx link down
    UpdateRxState(LinkDown);
  }
  else
  {
    DiagWarning(cTcp::moduleName, "cTcp::OnTimerEvent", "Got UNKNOWN timer event.");
  }
}

////////////////////////////////////////////////////////////////
void cTcp::UpdateTxState(eStatus state)
{
  cAutoLock _lock(mStateMutex);
  // check if state has changed before reporting
  if (state != mTXstate)
  {
    mTXstate = state;
    if (pRxMsgHandlerCb)
      pRxMsgHandlerCb->CBReportLinkStatus("TX", state);
    DiagWarning(moduleName, "cTcp::UpdateTxState", string("TX state changed to ")+(Status[state]));
  }
}

////////////////////////////////////////////////////////////////
void cTcp::UpdateRxState(eStatus state)
{
  cAutoLock _lock(mStateMutex);
  // check if state has changed before reporting
  if (state != mRXstate)
  {
    mRXstate = state;
    if (pRxMsgHandlerCb)
      pRxMsgHandlerCb->CBReportLinkStatus("RX", state);
    DiagWarning(moduleName, "cTcp::UpdateRxState", string("RX state changed to ")+(Status[state]));
  }
}

