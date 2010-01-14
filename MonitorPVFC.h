#ifndef MONITORPVFC_H
#define MONITORPVFC_H

#include "CSSTools.h"

/* This function starts a thread that will monitor for pure virtual function call 
windows (Runtime Error) and exit current process whenever they are issued */
extern CSSTOOLS_API bool StartMonitorForPVFC(int frequencyInSeconds);

// This function stops the pure virtual function call monitoring thread
extern CSSTOOLS_API bool StopMonitorForPVFC();

#endif