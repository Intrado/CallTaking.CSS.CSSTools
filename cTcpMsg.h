#pragma once

#pragma warning (disable: 4251)

#include <stdlib.h>
#include <string>
#include <list>
#include <vector>
#include "CSSTools.h"
#include "cEvent.h"

using namespace std;

// ACK/NAK definitions
#define ACK 6
#define NAK 21

// A message is formatted over TCP for transmission as follows:
// [TxCarStartP] ... string to send... [TxCarEndP][TxEndStrP]
//
// and for reception as follows:
// [RxCarStartP]... string received... [RxCarEndP][RxEndStrP]
// the exception is the ACK or NAK character which is all by itself


#define MAX_BUFFER_SIZE 512 // limit maximum buffer size to 512 bytes for reception

class CSSTOOLS_API cTcpMsg
{
public:

  enum eAckNakStatus {noackReceived=0,ackReceived=1,nakReceived=2};

  cTcpMsg(void);
  // constructor to be used for received message
  cTcpMsg(int rxStx, int rxEtx, string& rxEndStr, bool removeEndStr, bool prependCount);
  // constructor to use for message to send
  cTcpMsg(char* msg, int stx, int etx, const string& endStr, bool removeEndStr);
  // destructor
  ~cTcpMsg(void);

  // to add received buffer
  int Add(const char* rcvBuf, int rcvBufSize); // returns how much data was added (will complete message at [RxCarEndP][RxEndStrP] or ACK/NAK)

  // To check if message is complete
  bool isCompleted() {return mIsCompleted;};
  const char* GetData() {return &mDataV[0];};
  int GetSize() {return mSize;};
  bool MsgWithPrependCount() { return mPrependCount; }

  // clear message content
  void Clear() {mDataV.clear(); mSize=0; mIsCompleted=false;};

  void AddRef() { mUseCnt++; };

  void Dismiss() 
  {
    if (mUseCnt > 0) mUseCnt--;
    if (mUseCnt == 0)
    {
      delete this;
    }
  };

  cEvent mAckEvent;
  eAckNakStatus mAckNakStatus;

private:
  std::vector<char> mDataV;
  int mSize;
  bool mIsCompleted;
  int mUseCnt;
  int mRxStx;
  int mRxEtx;
  std::string mRxEndStr;
  bool mRemoveEndStr;
  bool mPrependCount;

};

