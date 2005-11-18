#ifndef CSSTOOLS_H
#define CSSTOOLS_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CSSTOOLS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CSSTOOLS_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CSSTOOLS_EXPORTS
#define CSSTOOLS_API __declspec(dllexport)
#else
#define CSSTOOLS_API __declspec(dllimport)
#endif

// Diag flag for misc CSSTools stuff outside the CSS namespace
extern bool CSSToolsTraceOn;

#endif
