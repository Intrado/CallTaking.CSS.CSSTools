/***************************************************************************
 * Copyright (C) Positron Public Safety Systems Inc., 2000
 * ALL RIGHTS RESERVED.
 * 
 *  created by:    sroy              date created:    2000/03/13
 *
 * name:          CSSReporterObject.h
 * version:       1
 *
 *
 * Description:
 *
 *     Ancestor object for all objects.with report capabilities
 *
	Last change:  SR    8 May 2000    4:10 pm
 ***************************************************************************/

// We don't want to create an interface to CSSObject only for this warning
#pragma warning(disable:4251)

#ifndef __CSSReporterObject__
#define __CSSReporterObject__

#include "CSSTools.h"
#include "CSSObject.h"
#include "CSSReporter.h"

class CSSTOOLS_API cCSSReporterObject : public cCSSObject
{
public:
  cCSSReporterObject() { reportersList = new CSS::cCSSEvent::ReporterList; }
  virtual ~cCSSReporterObject() { delete reportersList; }

  virtual void registerReporter (CSS::cCSSReporter * reporter);
  virtual void unregisterReporter (CSS::cCSSReporter * reporter);

  CSS::cCSSEvent::ReporterList *reportersList;
  static const char *moduleName;
};

#endif

#pragma warning( default : 4705 )

