
#ifndef _STRINGCONVERSION_H_INCLUDED_
#define _STRINGCONVERSION_H_INCLUDED_

#include <string>

#pragma once

using namespace std;

extern CSSTOOLS_API string  U2A(const wstring &s);

extern CSSTOOLS_API string  U2A(const wchar_t * const s);

extern CSSTOOLS_API wstring A2U(const string &s);

extern CSSTOOLS_API wstring A2U(const char * const s);

#endif
