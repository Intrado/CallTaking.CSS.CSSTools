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
mpClient(NULL), mpServer(NULL), mpRxMsg(NULL), mpTxMsg(NULL), mpRxMsgHandlerCb(),
  mHeartbeatTimerId(INVALID_TIMER), mRxTimerId(INVALID_TIMER), mTXstate(LinkDown), mRXstate(LinkDown), mRxThreadHandle(NULL), mTxThreadHandle(NULL),
mTxThreadRunning(false), mpfOnEventCB(NULL), mpfReportLinkStatusCB(NULL), mOnEventOwnerCB(NULL), mOnReportLinkStatusOwnerCB(NULL)
{
  DiagTrace(moduleName, "cTcp::cTcp", string("Started Tcp")+(mInitParams.mode == modeClient ? " in Client mode" : " in Server mode"));
  // id's to assign clients for our table
  client_id = 1;

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

  mInitParams.prependCount = pInitParamsIf->prependCount;

  if (pInitParamsIf->pTimerManager)
  {
    mpTimerManager = pInitParamsIf->pTimerManager;
  }
  else
  {
    mpTimerManager = &mTimerManager;
  }

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
    mpServer = new cTcpServer(this, pInitParamsIf->port, clientAddresses, mpTimerManager, pInitParamsIf->singleClientServer, pInitParamsIf->prependCount);
  }

  // client mode
  else if (mInitParams.mode == modeClient)
  {
    mpClient = new cTcpClient(pInitParamsIf->port, serverAddress, pInitParamsIf->prependCount);
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

  if (mpClient) delete mpClient;
  DiagTrace(moduleName, "cTcp::~cTcp", "pClient deleted.");
  if (mpServer) delete mpServer;
  DiagTrace(moduleName, "cTcp::~cTcp", "pServer deleted.");
  if (mpRxMsg) delete mpRxMsg;
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

  if (mpServer)
  {
    initialConnectedState = mpServer->IsConnected();
    cAutoLock lock(mLockMutex);
    // get new clients
    if(mpServer->AcceptNewClient(client_id))
    {
      DiagWarning(moduleName, "cTcp::Update", "Client "+ToStr(client_id)+" has been connected to the server");
      client_id++;
    }

    receiveFromClients();

    currentConnectedState = mpServer->IsConnected();
  }
  /*
   * CLIENT MODE
   */
  else if (mpClient)
  {
    initialConnectedState = mpClient->IsConnected();
    bool wasconnected = mpClient->IsConnected();
    receiveFromServer();
    if (!wasconnected && mpClient->IsConnected() && (mInitParams.hb != ""))
    {
      // Send heartbeat message
      DiagTrace(moduleName, "cTcp::Update", "Sending hearbeat message after it was re-connected");
      Send((char *)mInitParams.hb.c_str());
    }
    else if (wasconnected && !mpClient->IsConnected())
    {
      // stop heartbeat timer
      DiagTrace(moduleName, "cTcp::Update", "Stopping hearbeat message since disconnected");
      StopHBTimer();
    }
    currentConnectedState = mpClient->IsConnected();
  }

  if (initialConnectedState != currentConnectedState)
  {
    if (currentConnectedState)
    {
      string connectedClients;
      if (mpServer)
      {
        connectedClients = mpServer->GetClientsList();
      }
      if (mpRxMsgHandlerCb)
      {
        mpRxMsgHandlerCb->CBReportLinkStatus("SOCKET", LinkUp, (char*)connectedClients.c_str());
      }
      else if (mpfReportLinkStatusCB)
      {
        mpfReportLinkStatusCB(mOnReportLinkStatusOwnerCB, "SOCKET", LinkUp, (char*)connectedClients.c_str());
      }
      DiagWarning(moduleName, "cTcp::Update", string("Socket state changed to ")+(Status[LinkUp]));
    }
    else
    {
      if (mpRxMsgHandlerCb)
      {
        mpRxMsgHandlerCb->CBReportLinkStatus("SOCKET", LinkDown, "");
      }
      else if (mpfReportLinkStatusCB)
      {
        mpfReportLinkStatusCB(mOnReportLinkStatusOwnerCB, "SOCKET", LinkDown, "");
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
  if (mpClient)
  {
    memset(mRxBuf, 0, sizeof(mRxBuf));
    int data_length = mpClient->ReceivePackets(mRxBuf);

    if (data_length <= 0) 
    {
      //no data received
      return;
    }

    // handle received data to build receive message
    if (mpRxMsg == NULL)
    {
      mpRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr, mInitParams.prependCount);
    }

    int i = 0;
    int lenAdded = 0;
    while (mpRxMsg && (i < data_length))
    {
      lenAdded = mpRxMsg->Add(&mRxBuf[i], data_length-i);
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
      if (mpRxMsg->isCompleted() || ((mInitParams.RxCarStartP == -1) && (mInitParams.RxEndStrP == "") && !mpRxMsg->MsgWithPrependCount()))
      {
        KickRxTimer(); // since a message has been received, reset Rx timer
        if (IsAckNak(mpRxMsg) == cTcpMsg::noackReceived) // use callback if not an ACK or NAK handled within this class
        {
          // callback function needs to be called
          DiagTrace(moduleName, "cTcp::receiveFromServer", "Callback function is called since a complete message was received.");
          // pass received message to callback if valid
          if (mpRxMsgHandlerCb)
          {
            mpRxMsgHandlerCb->CBOnEvent(mpRxMsg);
            // de-allocation needs to take place in the handler since allocating a new one for next message
            mpRxMsg->Dismiss(); mpRxMsg = NULL; // !+ea - need to do that here since consumed in callback but not freed
            mpRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr, mInitParams.prependCount);
          }
          else if (mpfOnEventCB)
          {
            mpfOnEventCB(mOnEventOwnerCB, mpRxMsg->GetData());
            // de-allocation needs to take place in the handler since allocating a new one for next message
            mpRxMsg->Dismiss(); mpRxMsg = NULL; // !+ea - need to do that here since consumed in callback but not freed
            mpRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr, mInitParams.prependCount);
          }
          else
          {
            // clear message for now -- or TBC: delete handle by called back entity and new message to instantiate if handle
            mpRxMsg->Clear();
          }
        }
        else
        {
          // de-allocation needs to take place for this ACK/NAK message
          mpRxMsg->Dismiss(); mpRxMsg = NULL;
          mpRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr, mInitParams.prependCount);
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
  if (mpServer)
  {
    // go through all clients
    std::map<unsigned int, cClientSocket*>::iterator iter;

    for(iter = mpServer->sessions.begin(); iter != mpServer->sessions.end();)
    {
      bool removeIterator;
      memset(mRxBuf, 0, sizeof(mRxBuf));
      int data_length = mpServer->ReceiveData(iter->first, mRxBuf, removeIterator);

      if (data_length <= 0) 
      {
        if (removeIterator)
        {
          iter = mpServer->sessions.erase(iter);
        }
        else
        {
          iter++;
        }
        //no data received
        continue;
      }

      // handle received data to build receive message
      if (iter->second->mpRxMsg == NULL)
      {
        iter->second->mpRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr, mInitParams.prependCount, iter->first);
      }

      int i = 0;
      int lenAdded = 0;
      while (iter->second->mpRxMsg && (i < data_length))
      {
        lenAdded = iter->second->mpRxMsg->Add(&mRxBuf[i], data_length - i);
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
        if (iter->second->mpRxMsg->isCompleted() || ((mInitParams.RxCarStartP == -1) && (mInitParams.RxEndStrP == "") && !iter->second->mpRxMsg->MsgWithPrependCount()))
        {
          KickRxTimer(); // since a message has been received, reset Rx timer
          if (IsAckNak(iter->second->mpRxMsg) == cTcpMsg::noackReceived) // use callback if not an ACK or NAK handled within this class
          {
            // callback function needs to be called
            DiagTrace(moduleName, "cTcp::receiveFromClients", "Callback function is called since a complete message was received.");
            // use callback if valid
            if (mpRxMsgHandlerCb)
            {
              mpRxMsgHandlerCb->CBOnEvent(iter->second->mpRxMsg);
              // de-allocation needs to take place in the handler since allocating a new one for next message
              iter->second->mpRxMsg->Dismiss(); 
              iter->second->mpRxMsg = NULL;
              iter->second->mpRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr, mInitParams.prependCount, iter->first);
            }
            else if (mpfOnEventCB)
            {
              mpfOnEventCB(mOnEventOwnerCB, iter->second->mpRxMsg->GetData());
              // de-allocation needs to take place in the handler since allocating a new one for next message
              iter->second->mpRxMsg->Dismiss();
              iter->second->mpRxMsg = NULL; // !+ea - need to do that here since consumed in callback but not freed
              iter->second->mpRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr, mInitParams.prependCount, iter->first);
            }
            else
            {
              // clear message for now -- or TBC: delete handle by called back entity and new message to instantiate if handle
              iter->second->mpRxMsg->Clear();
            }
          }
          else
          {
              // de-allocation needs to take place for this ACK/NAK message
            iter->second->mpRxMsg->Dismiss(); 
            iter->second->mpRxMsg = NULL;
            iter->second->mpRxMsg = new cTcpMsg(mInitParams.RxCarStartP, mInitParams.RxCarEndP, mInitParams.RxEndStrP, mInitParams.RemoveEndStr, mInitParams.prependCount, iter->first);
          }
        }
      }

      if (removeIterator)
      {
        iter = mpServer->sessions.erase(iter);
      }
      else
      {
        iter++;
      }
    }
  }
}

////////////////////////////////////////////////////////////////
// create and put message into queue
int cTcp::Send(char* pMsg, unsigned int client_id)
{
  int res = 0;
  DiagTrace(moduleName, "cTcp::Send", "Create message");
  cTcpMsg* pToSendMsg = new cTcpMsg(pMsg, mInitParams.TxCarStartP, mInitParams.TxCarEndP, mInitParams.TxEndStrP, mInitParams.RemoveEndStr, client_id);
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
      if ((mpClient && mpClient->IsConnected()) || !mpClient)
      {
        UpdateTxState(LinkUp);
      }
    }
  }
  if ((mpClient && mpClient->IsConnected()) || !mpClient)
  {
    KickHBTimer();
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
  if (mpServer)
  {
    return(mpServer->SendMsgToClient(pMsg));
  }
  if (mpClient)
  {
    return(mpClient->Send(pMsg));
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
  if ((mInitParams.hb != "") && (!mpClient || (mpClient && mpClient->IsConnected())))
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
  if (mpServer)
  {
    cAutoLock lock(mLockMutex);
    nb = mpServer->GetNbClients();
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
    if (mpRxMsgHandlerCb)
    {
      mpRxMsgHandlerCb->CBReportLinkStatus("TX", state, "");
    }
    else if (mpfReportLinkStatusCB)
    {
      mpfReportLinkStatusCB(mOnReportLinkStatusOwnerCB, "TX", state, "");
    }
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
    if (mpRxMsgHandlerCb)
    {
      mpRxMsgHandlerCb->CBReportLinkStatus("RX", state, "");
    }
    else if (mpfReportLinkStatusCB)
    {
      mpfReportLinkStatusCB(mOnReportLinkStatusOwnerCB, "RX", state, "");
    }

    DiagWarning(moduleName, "cTcp::UpdateRxState", string("RX state changed to ")+(Status[state]));
  }
}

////////////////////////////////////////////////////////////////
int cTcp::RegisterOnEventCB(cTcp::tOnEventCB onEventCB, void* owner)
{
  mpfOnEventCB = onEventCB;
  mOnEventOwnerCB = owner;
  return 0;
}

////////////////////////////////////////////////////////////////
int cTcp::RegisterReportLinkStatusCB(cTcp::tReportLinkStatusCB reportLinkStatuseCB, void* owner)
{
  mpfReportLinkStatusCB = reportLinkStatuseCB;
  mOnReportLinkStatusOwnerCB = owner;
  return 0;
}

////////////////////////////////////////////////////////////////
int cTcp::UnregisterOnEventCB(void* owner)
{
  if (owner == mOnEventOwnerCB)
  {
    mpfOnEventCB = NULL;
    mOnEventOwnerCB = NULL;
  }
  return 0;
}

////////////////////////////////////////////////////////////////
int cTcp::UnregisterReportLinkStatusCB(void* owner)
{
  if (owner == mOnReportLinkStatusOwnerCB)
  {
    mpfReportLinkStatusCB = NULL;
    mOnReportLinkStatusOwnerCB = NULL;
  }
  return 0;
}

////////////////////////////////////////////////////////////////
void cTcp::ReportClientStatus(eClientStatus state, unsigned int clientId, char* address)
{
  if (mpRxMsgHandlerCb)
  {
    mpRxMsgHandlerCb->CBReportClientStatus((eStatus)state, clientId, address);
  }
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API long Init(int mode, unsigned int port, const char* pServerAddress, const char* pClientAddresses, char* hb,
                       int hbtoP, int acktoP, int maxretriesP, int silenttoP, int maxperiodsP, int TxCarStartP, int TxCarEndP, char* TxEndStrP,
                       int RxCarStartP, int RxCarEndP, char* RxEndStrP, bool RemoveEndStr, bool waitForAckP, bool sendAckP, bool singleClientServer,
                       bool prependCount)
{
  cTcp::tInitParamsIF initParams;
  initParams.mode = (cTcp::eModeType)mode;
  initParams.port = port;
  initParams.pServerAddress = pServerAddress;
  initParams.pClientAddresses = pClientAddresses;
  initParams.pHb = hb;
  initParams.hbtoP = hbtoP;
  initParams.acktoP = acktoP;
  initParams.maxretriesP = maxretriesP;
  initParams.silenttoP = silenttoP;
  initParams.TxCarStartP = TxCarStartP;
  initParams.TxCarEndP = TxCarEndP;
  initParams.pTxEndStrP = TxEndStrP;
  initParams.RxCarStartP = RxCarStartP;
  initParams.RxCarEndP = RxCarEndP;
  initParams.pRxEndStrP = RxEndStrP;
  initParams.RemoveEndStr = RemoveEndStr;
  initParams.waitForAckP = waitForAckP;
  initParams.sendAckP = sendAckP;
  initParams.singleClientServer = singleClientServer;
  initParams.prependCount = prependCount;
  initParams.pTimerManager = NULL;

  cTcp* ptr = new cTcp(&initParams);
  long handle = NULL;
  if (ptr)
  {
    ptr->Start();
    handle = (long)ptr;
  }
  return handle;
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API int Exit(long handle)
{
  cTcp* ptr = (cTcp*)handle;
  if (ptr) delete ptr;
  return 0;
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API int Send(long handle, char *pMsg)
{
  cTcp* ptr = (cTcp*)handle;
  if (ptr)
  {
    ptr->Send(pMsg);
  }
  return 0;
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API int SendClient(long handle, char *pMsg, unsigned int client_id)
{
  cTcp* ptr = (cTcp*)handle;
  if (ptr)
  {
    ptr->Send(pMsg, client_id);
  }
  return 0;
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API int Update(long handle)
{
  cTcp* ptr = (cTcp*)handle;
  if (ptr)
  {
    ptr->Update();
  }
  return 0;
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API int GetNbClients(long handle)
{
  cTcp* ptr = (cTcp*)handle;
  int nb = 0;
  if (ptr)
  {
    nb = ptr->GetNbClients();
  }
  return nb;
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API int RegisterOnEventCB(long handle, cTcp::tOnEventCB onEventCB, void* owner)
{
  cTcp* ptr = (cTcp*)handle;
  if (ptr)
  {
    ptr->RegisterOnEventCB(onEventCB, owner);
    return 0;
  }
  return -1;
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API int RegisterReportLinkStatusCB(long handle, cTcp::tReportLinkStatusCB reportLinkStatuseCB, void* owner)
{
  cTcp* ptr = (cTcp*)handle;
  if (ptr)
  {
    ptr->RegisterReportLinkStatusCB(reportLinkStatuseCB, owner);
    return 0;
  }
  return -1;
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API int UnregisterOnEventCB(long handle, void* owner)
{
  cTcp* ptr = (cTcp*)handle;
  if (ptr)
  {
    ptr->UnregisterOnEventCB(owner);
    return 0;
  }
  return -1;
}

////////////////////////////////////////////////////////////////
CSSTOOLS_API int UnregisterReportLinkStatusCB(long handle, void* owner)
{
  cTcp* ptr = (cTcp*)handle;
  if (ptr)
  {
    ptr->UnregisterReportLinkStatusCB(owner);
    return 0;
  }
  return -1;
}
