#include "CSSDiagEx.h"

void DiagDllTraceI(const char *module, const char *position, const char *message, const int iVal)
{
  char txt[16];
  DiagDllTrace(module, position, (string(message) + _itoa(iVal, txt, 10)).c_str());
}


void DiagDllTrace2(const char *module, const char *position, const char *message, const char *message2)
{
  DiagDllTrace(module, position, (string(message) + message2).c_str());
}


void DiagDllTrace3(const char *module, const char *position, const char *message, const char *message2 ,const char *message3)
{
  DiagDllTrace(module, position, (string(message) + message2 + message3).c_str());
}


void DiagDllWarningI(const char *module, const char *position, const char *message, const int iVal)
{
  char txt[16];
  DiagDllWarning(module, position, (string(message) + _itoa(iVal, txt, 10)).c_str());
}


void DiagDllWarning2(const char *module, const char *position, const char *message, const char *message2)
{
  DiagDllWarning(module, position, (string(message) + message2).c_str());
}


void DiagDllWarning3(const char *module, const char *position, const char *message, const char *message2 ,const char *message3)
{
  DiagDllWarning(module, position, (string(message) + message2 + message3).c_str());
}


void DiagDllErrorI(const char *module, const char *position, const char *message, const int iVal, bool trace)
{
  char txt[16];
  DiagDllError(module, position, (string(message) + _itoa(iVal, txt, 10)).c_str());
}


void DiagDllError2(const char *module, const char *position, const char *message, const char *message2, bool trace)
{
  DiagDllError(module, position, (string(message) + message2).c_str());
}


void DiagDllError3(const char *module, const char *position, const char *message, const char *message2 ,const char *message3, bool trace)
{
  DiagDllError(module, position, (string(message) + message2 + message3).c_str());
}


void DiagDllOutI(const char *module, const char *position, const char *message, const int iVal)
{
  char txt[16];
  DiagDllOut(module, position, (string(message) + _itoa(iVal, txt, 10)).c_str());
}


void DiagDllOut2(const char *module, const char *position, const char *message, const char *message2)
{
  DiagDllOut(module, position, (string(message) + message2).c_str());
}


void DiagDllOut3(const char *module, const char *position, const char *message, const char *message2 ,const char *message3)
{
  DiagDllOut(module, position, (string(message) + message2 + message3).c_str());
}
