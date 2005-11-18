/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000.
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    olarouche    date created:    2000/03/15
 * %derived_by:   sroy %       %date_modified:  2000/08/14 14:52:29 %
 *
 * %name:         cTimerHandler.h %
 * %version:      3 %
 *
 * References:
 *
 *     documents
 *
 * Description: Virtual interface for the cTimerHandler class.
 *    Used by cTimerManager.
 *
 *
 ***************************************************************************/

#ifndef CTIMERHANDLER_H
#define CTIMERHANDLER_H

#include "cTEvent.h"

class cTimerHandler
{
public:
	virtual void OnTimerEvent(unsigned int timerId, cTEvent* eventInfo) = 0;
};

#endif
