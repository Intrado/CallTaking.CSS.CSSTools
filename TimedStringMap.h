#pragma warning (disable: 4786)
#pragma warning (disable: 4251)

#ifndef TIMEDSTRINGMAP_H
#define TIMEDSTRINGMAP_H

#include <map>
#include <string>
#include "cTimerManager.h"
#include "cTimerHandler.h"
#include "cCritSec.h"

class CSSTOOLS_API cTimedStringMap : public cTimerHandler
{
public:
  cTimedStringMap();
  ~cTimedStringMap();
  
  bool Insert(std::string key, std::string val, int ttl);
  bool Remove(std::string key);
  bool Find(std::string key, std::string &val);
  int Size();
  
protected:
  virtual void OnTimerEvent(unsigned int timerId, cTEvent* eventInfo);

  struct tDataEntry
  {
    tDataEntry()
    {
      mTimerId = INVALID_TIMER;
    }

    std::string mValue;
    unsigned int mTimerId;
  };

  bool StopTimer(std::string, tDataEntry key);

  static const char *kModuleName;
  cCritSec mDataLock;
  std::map<std::string, struct tDataEntry> mData;

  cTimerManager mTimerManager;
};

#endif