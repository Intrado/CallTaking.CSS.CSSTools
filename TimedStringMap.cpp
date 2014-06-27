#include "TimedStringMap.h"
#include "cAutoLock.h"
#include "Diag.h"

const char *cTimedStringMap::kModuleName = "cTimedStringMap";
class cTimerArg : public cTEvent
{
public:
  std::string mKey;
};

cTimedStringMap::cTimedStringMap()
{
  DiagTrace(kModuleName, "cTimedStringMap", "Construction");
}

cTimedStringMap::~cTimedStringMap()
{
  cAutoLock _Lock (mDataLock);

  DiagTrace(kModuleName, "~cTimedStringMap", "Stopping all timer and emptying map");

  for (std::map<std::string, struct tDataEntry>::iterator It = mData.begin() ; It != mData.end() ; It++)
  {
    StopTimer(It->first, It->second);
  }
  mData.clear();
  
}

bool cTimedStringMap::Insert(std::string key, std::string val, int ttl)
{
  cAutoLock _Lock (mDataLock);

  DiagTrace(kModuleName, "Insert", "Removing before inserting new element. Key: " + key);
  Remove(key);
  
  tDataEntry newEntry;
  newEntry.mValue = val;
  cTimerArg *pEvent = new cTimerArg;
  pEvent->mKey = key;
  newEntry.mTimerId = mTimerManager.CreateTimer(this, ttl, pEvent);
  mData[key] = newEntry;
  DiagTrace(kModuleName, "Insert", "New element inserted. Key: " + key);
  return true;
}

bool cTimedStringMap::Remove(std::string key)
{
  cAutoLock _Lock (mDataLock);
  std::map<std::string, struct tDataEntry>::iterator It = mData.find(key);

  if (It != mData.end())
  {
    DiagTrace(kModuleName, "Remove", "Item found. Stopping timer and erasing item with key: " + key);
    StopTimer(It->first, It->second);
    mData.erase(It);
    return true;
  }
  else
  {
    DiagTrace(kModuleName, "Remove", "Item to remove not found. Key: " + key);
    return false;
  }
}

bool cTimedStringMap::Find(std::string key, std::string &val)
{
  cAutoLock _Lock (mDataLock);
  std::map<std::string, struct tDataEntry>::iterator It = mData.find(key);

  if (It != mData.end())
  {
    DiagTrace(kModuleName, "Find", "Item found. Key: " + key);
    val = It->second.mValue;
    return true;
  }
  else
  {
    DiagTrace(kModuleName, "Find", "Item not found. Key: " + key);
    return false;
  }
}

void cTimedStringMap::OnTimerEvent(unsigned int timerId, cTEvent* eventInfo)
{
  cAutoLock _Lock (mDataLock);
    
  // Find the entry and delete it
  if (eventInfo)
  {
    cTimerArg* pTimerArg = (cTimerArg*) eventInfo;
    if (pTimerArg && pTimerArg->mKey.size() > 0)
    {
      DiagTrace(kModuleName, "OnTimerEvent", "Timer expired with Key: " + pTimerArg->mKey);
      Remove(pTimerArg->mKey);
    }
    else
    {
      DiagWarning(kModuleName, "OnTimerEvent", "Timer expired with no valid Key");
    }
  }
  else
  {
    DiagWarning(kModuleName, "OnTimerEvent", "Timer expired with no attached eventInfo");
  }
}

bool cTimedStringMap::StopTimer(std::string key, cTimedStringMap::tDataEntry data)
{
  if (data.mTimerId != INVALID_TIMER)
  {
    mTimerManager.KillTimer(data.mTimerId);
    data.mTimerId = INVALID_TIMER;
    DiagTrace(kModuleName, "StopTimer", "Timer killed. Key: " + key);
    return true;
  }
  else
  {
    DiagTrace(kModuleName, "StopTimer", "No timer to kill. Key: " + key);
    return false;
  }
}

int cTimedStringMap::Size()
{
  return mData.size();
}