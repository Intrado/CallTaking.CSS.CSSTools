#include "stdafx.h"
#include "cTcp.h"
#include "cTcpMsg.h"
#include "Diag.h"


////////////////////////////////////////////////////////////////
cTcpMsg::cTcpMsg(void) :
mIsCompleted(false),
mSize(0),
mUseCnt(1),
mRxStx(0x02),
mRxEtx(0x03),
mRxEndStr(),
mRemoveEndStr(true),
mPrependCount(false)
{
  mDataV.clear();
}

////////////////////////////////////////////////////////////////
// use this constructor for receive message
cTcpMsg::cTcpMsg(int rxStx, int rxEtx, string& rxEndStr, bool removeEndStr, bool prependCount) :
  mIsCompleted(false),
  mSize(0),
  mUseCnt(1),
  mRxStx(rxStx),
  mRxEtx(rxEtx),
  mRxEndStr(rxEndStr),
  mRemoveEndStr(removeEndStr),
  mPrependCount(prependCount)
{
  mDataV.clear();
}

////////////////////////////////////////////////////////////////
// directly create a message ready to be sent, format is added as specified
cTcpMsg::cTcpMsg(char* msg, int stx, int etx, const string& endStr, bool removeEndStr) :
  mIsCompleted(false),
  mSize(0),
  mUseCnt(1),
  mRemoveEndStr(removeEndStr),
  mPrependCount(false)
{
  mDataV.clear();
  int len = strlen(msg);
  string toSendStr = "";
  if (stx != -1)
    toSendStr = string(1, (char)stx);
  toSendStr += string(msg);
  if (etx != -1)
    toSendStr  += string(1, (char)etx);
  toSendStr += endStr;
  mDataV.insert(mDataV.end(), &toSendStr[0], &toSendStr[toSendStr.length()]);
  mDataV.push_back('\0');
  mSize = mDataV.size();
  mIsCompleted = true;
}

////////////////////////////////////////////////////////////////
cTcpMsg::~cTcpMsg(void)
{
  mDataV.clear();
}

////////////////////////////////////////////////////////////////
char* memsearch(const char *hay, int haysize, const char *needle, int needlesize) 
{
  int haypos, needlepos;
  haysize -= needlesize;
  for (haypos = 0; haypos <= haysize; haypos++) 
  {
    for (needlepos = 0; needlepos < needlesize; needlepos++) 
    {
      if (hay[haypos + needlepos] != needle[needlepos]) 
      {
        // Next character in haystack.
        break;
      }
    }
    if (needlepos == needlesize) 
    {
      return (char *)hay+haypos;
    }
  }
  return NULL;
}

////////////////////////////////////////////////////////////////
int cTcpMsg::Add(const char* rcvBuf, int rcvBufSize)
{
  char* pStart = (char *)rcvBuf;
  char* pStop = (char*) &rcvBuf[rcvBufSize-1];
  char* pTmp = NULL;
  int sizeToCopy = 0;
  int sizeRemaining = rcvBufSize;

  // cannot add to an already completed message
  if (mIsCompleted)
  {
    DiagTrace(cTcp::moduleName, "cMsg::Add", "Message already completed, cannot add buffer");
    return 0; // this message should be handled and a new message should be created by caller to receive new buffer
  }

  // first, check if ACK or NAK character to return this
  // NOTE that ACK/NAK is not encapsulated with <STX><ETX> or endstr
  if ((rcvBuf[0] == ACK) || (rcvBuf[0] == NAK))
  {
    if (mSize != 0)
    {
      // something is wrong, ACK/NAK received at wrong time, need to clear what was there before
      DiagWarning(cTcp::moduleName, "cMsg::Add", "ACK or NAK received at unexpected time.");
      mDataV.clear();
      mSize = 0;
    }
    mDataV.push_back(rcvBuf[0]); // put ACK or ACK in message data
    mDataV.push_back('\0');      // just make sure to NULL string (although it should not be necessary)
    mSize = 1;                   // only one character received and used for this message
    mIsCompleted = true;         // signal that we have a complete message
    return mSize; // 1 character in message
  }

  if ((mSize == 0) && (mRxStx != -1)) // means has not been started yet
  {
    // check if we have received a mRxStx (i.e. STX = 0x02)
    pStart = (char *) memchr(rcvBuf, (char)mRxStx, sizeRemaining);
    if (pStart == NULL)
    {
      DiagTrace(cTcp::moduleName, "cMsg::Add", "Did not find an STX in stream, skip this buffer");
      return -1; // error since buffer cannot be taken
    }
    sizeRemaining -= (pStart-rcvBuf);
  }
  // check for received mRxEtx (i.e. ETX = 3) for end of message
  if (((mRxEtx == -1) || (pTmp = (char *) memchr(pStart, 0x03, sizeRemaining)) != NULL))
  {
    if (mRxEndStr != "")
    {
      int len = mRxEndStr.size();
      if ((pTmp == NULL) && (sizeRemaining >= len))
      {
         pTmp =  memsearch(pStart, sizeRemaining, mRxEndStr.c_str(), len);
      }
      // in theory, this case should not occur
      else if ((pTmp != NULL) && ((sizeRemaining - (pTmp-pStart)) >= len))
      {
         pTmp =  memsearch(pTmp, len, mRxEndStr.c_str(), len);
      }
    }
    if (pTmp)
    {
      if (mRemoveEndStr || mRxEndStr.empty())
      { 
        pStop = pTmp;
      }
    }
  }
  // determine size to be copied
  sizeToCopy = (pStop-pStart)+1;
  // copy into array
  mDataV.insert(mDataV.begin()+mSize, pStart, pStart+sizeToCopy);
  mSize += sizeToCopy; 
  // check if end is there, if so declare completed
  if (pTmp)
  {
    // remove extra characters not required
    // -- at start
    if (mRxStx != -1)
    {
      // erase first character
      mDataV.erase(mDataV.begin());
      mSize--;
    }
    // -- at end
    int nCharsToRemoveAtEnd = 0;
    if (mRxEtx != -1) nCharsToRemoveAtEnd++;
    if (mRemoveEndStr && (mRxEndStr != "")) nCharsToRemoveAtEnd+=mRxEndStr.size();
    if (nCharsToRemoveAtEnd > 0)
    {
      // erase last characters
      mDataV.erase(mDataV.end()-nCharsToRemoveAtEnd, mDataV.end());
      mSize-=nCharsToRemoveAtEnd;
    }
    // add null character at the end to make sure string is ended
    mDataV.push_back('\0');
    mIsCompleted = true;
  }
  else if (mPrependCount && (mDataV.size() > 3))
  {
    unsigned int totalCount = ((unsigned char)mDataV.at(0) << 16);
    totalCount += ((unsigned char)mDataV.at(1) << 8);
    totalCount += (unsigned char)mDataV.at(2);
    if (totalCount <= (mDataV.size()))
    {
      sizeToCopy = rcvBufSize - (mDataV.size() - totalCount);
      while (mDataV.size() > totalCount)
      {
        mDataV.pop_back();
        mSize--;
      }

      mDataV.erase(mDataV.begin());
      mSize--;
      mDataV.erase(mDataV.begin());
      mSize--;
      mDataV.erase(mDataV.begin());
      mDataV.push_back('\0');  //Add NULL char
      mIsCompleted = true;
    }
  }
  else if ((mRxStx == -1) && (mRxEndStr == ""))
  {
    mDataV.push_back('\0');
    mIsCompleted = true;
  }
  return sizeToCopy;
}
