/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    sroy              date created:    2000/04/04
 *
 * name:          CSSObject.h
 * version:       1
 *
 *
 * Description:
 *
 *     Ancestor object for all telephony objects.
 *
	Last change:  SR   4 April 2000
 ***************************************************************************/
#include "CSSReporterObject.h"
#include <assert.h>
#include "CSS.h"

const char *cCSSReporterObject::moduleName = "CSSReporterObject";


void cCSSReporterObject::registerReporter(CSS::cCSSReporter * reporter)
{
  if (reporter == 0)
  {
	  return;
  }
  
  if (CSSToolsTraceOn)
  {
  DiagTrace(moduleName, "RegisterReporter", "New reporter to register");
  }

  reportersList->add(reporter);
}

void cCSSReporterObject::unregisterReporter(CSS::cCSSReporter * reporter)
{
  if (reporter == 0)
  {
    return;
  }

  if (CSSToolsTraceOn)
  {
  DiagTrace(moduleName, "UnregisterReporter", "Reporter to unregister");
  }
  
  reportersList->remove(reporter);
}
