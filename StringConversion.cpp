#include <string>
#include <atlbase.h>

#pragma warning(push)
#pragma warning(disable:4267)

using namespace std;

static const char *ModuleName = "StringConversion";

#define STACK_LIMIT 56 * 1024   // 56 K

static string convertU2A(const wchar_t *s)
{
  size_t len = wcslen(s);
  string ret;
  if(len*2 < STACK_LIMIT)
  {
    USES_CONVERSION;
    ret = W2A(s);
  }
  else
  {
    char *buffer = new char[len+1];
    buffer[len] = 0;
    int res = WideCharToMultiByte(CP_ACP, 0, s, len, buffer, len, 0, 0);
    if(res != 0)
    {
      ret = buffer;
    }

    delete [] buffer;
  }
  return ret;

}

static wstring convertA2U(const char *s)
{
  size_t len = strlen(s);
  wstring ret;
  if(len < STACK_LIMIT)
  {
    USES_CONVERSION;
    ret = A2W(s);
  }
  else
  {
    wchar_t *buffer = new wchar_t[len+1];
    buffer[len] = 0;
    int res = MultiByteToWideChar(CP_ACP, 0, s, len, buffer, len);
    if(res != 0)
    {
      ret = buffer;
    }

    delete [] buffer;
  }
  return ret;
}

string  unicodeToAnsi(const wstring &s)
{
  return convertU2A(s.c_str());
}

string  unicodeToAnsi(const wchar_t * const s)
{
  return convertU2A(s);
}

wstring ansiToUnicode(const string &s)
{
  return convertA2U(s.c_str());
}

wstring ansiToUnicode(const char * const s)
{
  return convertA2U(s);
}

#pragma warning(pop)
