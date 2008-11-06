
#ifndef _STRINGCONVERSION_H_INCLUDED_
#define _STRINGCONVERSION_H_INCLUDED_

#include <string>

#pragma once

using namespace std;

extern  string  unicodeToAnsi(const wstring &s);

extern  string  unicodeToAnsi(const wchar_t * const s);

extern  wstring ansiToUnicode(const string &s);

extern  wstring ansiToUnicode(const char * const s);

#endif
