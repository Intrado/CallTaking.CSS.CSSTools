#include "CSSDiagEx.h"

void DiagDllWarningI(const char *module, const char *position, const char *message, const int iVal)
{
  char txt[16];
  DiagDllWarning(module, position, (string(message) + _itoa(iVal, txt, 10)).c_str());
}


void DiagDllErrorI(const char *module, const char *position, const char *message, const int iVal, bool trace)
{
  char txt[16];
  DiagDllError(module, position, (string(message) + _itoa(iVal, txt, 10)).c_str());
}
