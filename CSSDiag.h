#ifndef __CSSDiag_INCLUDED__
#define __CSSDiag_INCLUDED__

#include "Diag.h"

#define CSSTrace(module, position, message) \
    if (Diag::TraceOn) DiagTrace(module, position, message)

#define CSSDiagExec(command)\
  if (Diag::TraceOn)\
  {\
    command\
  }

#endif
