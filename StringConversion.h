
#ifndef _STRINGCONVERSION_H_INCLUDED_
#define _STRINGCONVERSION_H_INCLUDED_

#include <string>

#pragma once

using namespace std;

extern  string  U2A(const wstring &s);

extern  string  U2A(const wchar_t * const s);

extern  wstring A2U(const string &s);

extern  wstring A2U(const char * const s);

#endif
