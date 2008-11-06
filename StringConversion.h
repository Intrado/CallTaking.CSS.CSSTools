
#ifndef _STRINGCONVERSION_H_INCLUDED_
#define _STRINGCONVERSION_H_INCLUDED_

#include <string>
#include "CSSTools.h"

#pragma once

using namespace std;

CSSTOOLS_API string  U2A(const wstring &s);

CSSTOOLS_API string  U2A(const wchar_t * const s);

CSSTOOLS_API wstring A2U(const string &s);

CSSTOOLS_API wstring A2U(const char * const s);

#endif
