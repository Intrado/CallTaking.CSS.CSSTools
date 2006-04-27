#ifndef __CSSDiagEx_INCLUDED__
#define __CSSDiagEx_INCLUDED__

#include "CSSTools.h"
#include "Diag.h"
#include <sstream>

// DiagDll* functions.
CSSTOOLS_API void DiagDllTraceI(const char *module, const char *position, const char *message, const int iVal);
CSSTOOLS_API void DiagDllTrace2(const char *module, const char *position, const char *message, const char *message2);
CSSTOOLS_API void DiagDllTrace3(const char *module, const char *position, const char *message, const char *message2 ,const char *message3);

CSSTOOLS_API void DiagDllWarningI(const char *module, const char *position, const char *message, const int iVal);
CSSTOOLS_API void DiagDllWarning2(const char *module, const char *position, const char *message, const char *message2);
CSSTOOLS_API void DiagDllWarning3(const char *module, const char *position, const char *message, const char *message2 ,const char *message3);

CSSTOOLS_API void DiagDllErrorI(const char *module, const char *position, const char *message, const int iVal, bool trace = true);

CSSTOOLS_API void DiagDllOutI(const char *module, const char *position, const char *message, const int iVal);
CSSTOOLS_API void DiagDllOut2(const char *module, const char *position, const char *message, const char *message2);
CSSTOOLS_API void DiagDllOut3(const char *module, const char *position, const char *message, const char *message2 ,const char *message3);

// DiagTrace functions.
inline void DiagTrace(const char *module, const char *position, const char *message = "Starting") {  DiagDllTrace(module, position, message); }
inline void DiagTrace(const char *module, const char *position, const std::string &message) {  DiagDllTrace(module, position, message.c_str()); }

inline void DiagTrace(const char *module, const char *position, const std::string &message, const int iVal) { DiagDllTraceI(module, position, message.c_str(), iVal); }

inline void DiagTrace(const char *module, const char *position, const std::string &message, const char *message2) { DiagDllTrace2(module, position, message.c_str(), message2); }
inline void DiagTrace(const char *module, const char *position, const char *message, const std::string &message2) { DiagDllTrace2(module, position, message, message2.c_str()); }
inline void DiagTrace(const char *module, const char *position, const std::string &message, const std::string &message2) { DiagDllTrace2(module, position, message.c_str(), message2.c_str()); }
inline void DiagTrace(const char *module, const char *position, const char *message, const char *message2, const std::string &message3) { DiagDllTrace3(module, position, message, message2 ,message3.c_str()); }
inline void DiagTrace(const char *module, const char *position, const char *message, const std::string &message2, const char *message3) { DiagDllTrace3(module, position, message, message2.c_str(), message3); }
inline void DiagTrace(const char *module, const char *position, const char *message, const std::string &message2, const std::string &message3) { DiagDllTrace3(module, position, message, message2.c_str(), message3.c_str()); }
inline void DiagTrace(const char *module, const char *position, const std::string &message, const char *message2, const char *message3) { DiagDllTrace3(module, position, message.c_str(), message2, message3); }
inline void DiagTrace(const char *module, const char *position, const std::string &message, const char *message2, const std::string &message3) { DiagDllTrace3(module, position, message.c_str(), message2, message3.c_str()); }
inline void DiagTrace(const char *module, const char *position, const std::string &message, const std::string &message2, const char *message3) { DiagDllTrace3(module, position, message.c_str(), message2.c_str(), message3); }
inline void DiagTrace(const char *module, const char *position, const std::string &message, const std::string &message2, const std::string &message3) { DiagDllTrace3(module, position, message.c_str(), message2.c_str(), message3.c_str()); }

inline void DiagTrace(const char *module, const char *position, const std::ostringstream &message) { DiagDllTrace(module, position, message.str().c_str()); }

// DiagWarning functions.
inline void DiagWarning(const char *module, const char *position, const char *message) {  DiagDllWarning(module, position, message); }
inline void DiagWarning(const char *module, const char *position, const std::string &message) {  DiagDllWarning(module, position, message.c_str()); }

inline void DiagWarning(const char *module, const char *position, const char *message, const int iVal) { DiagDllWarningI(module, position, message, iVal); }
inline void DiagWarning(const char *module, const char *position, const std::string &message, const int iVal) { DiagDllWarningI(module, position, message.c_str(), iVal); }

inline void DiagWarning(const char *module, const char *position, const std::string &message, const char *message2) { DiagDllWarning2(module, position, message.c_str(), message2); }
inline void DiagWarning(const char *module, const char *position, const char *message, const std::string &message2) { DiagDllWarning2(module, position, message, message2.c_str()); }
inline void DiagWarning(const char *module, const char *position, const std::string &message, const std::string &message2) { DiagDllWarning2(module, position, message.c_str(), message2.c_str()); }
inline void DiagWarning(const char *module, const char *position, const char *message, const char *message2, const std::string &message3) { DiagDllWarning3(module, position, message, message2 ,message3.c_str()); }
inline void DiagWarning(const char *module, const char *position, const char *message, const std::string &message2, const char *message3) { DiagDllWarning3(module, position, message, message2.c_str(), message3); }
inline void DiagWarning(const char *module, const char *position, const char *message, const std::string &message2, const std::string &message3) { DiagDllWarning3(module, position, message, message2.c_str(), message3.c_str()); }
inline void DiagWarning(const char *module, const char *position, const std::string &message, const char *message2, const char *message3) { DiagDllWarning3(module, position, message.c_str(), message2, message3); }
inline void DiagWarning(const char *module, const char *position, const std::string &message, const char *message2, const std::string &message3) { DiagDllWarning3(module, position, message.c_str(), message2, message3.c_str()); }
inline void DiagWarning(const char *module, const char *position, const std::string &message, const std::string &message2, const char *message3) { DiagDllWarning3(module, position, message.c_str(), message2.c_str(), message3); }
inline void DiagWarning(const char *module, const char *position, const std::string &message, const std::string &message2, const std::string &message3) { DiagDllWarning3(module, position, message.c_str(), message2.c_str(), message3.c_str()); }

inline void DiagWarning(const char *module, const char *position, const std::ostringstream &message) { DiagDllWarning(module, position, message.str().c_str()); }

// DiagError functions.
inline void DiagError(const char *module, const char *position, const std::string &message, bool trace = true) {  DiagDllError(module, position, message.c_str(), trace); }

inline void DiagError(const char *module, const char *position, const char *message, const int iVal, bool trace = true) { DiagDllErrorI(module, position, message, iVal, trace); }
inline void DiagError(const char *module, const char *position, const std::string &message, const int iVal, bool trace = true) { DiagDllErrorI(module, position, message.c_str(), iVal, trace); }

inline void DiagError(const char *module, const char *position, const std::string &message, const char *message2, bool trace = true) { DiagDllError2(module, position, message.c_str(), message2, trace); }
inline void DiagError(const char *module, const char *position, const char *message, const std::string &message2, bool trace = true) { DiagDllError2(module, position, message, message2.c_str(), trace); }
inline void DiagError(const char *module, const char *position, const std::string &message, const std::string &message2, bool trace = true) { DiagDllError2(module, position, message.c_str(), message2.c_str(), trace); }
inline void DiagError(const char *module, const char *position, const char *message, const char *message2, const std::string &message3, bool trace = true) { DiagDllError3(module, position, message, message2 ,message3.c_str(), trace); }
inline void DiagError(const char *module, const char *position, const char *message, const std::string &message2, const char *message3, bool trace = true) { DiagDllError3(module, position, message, message2.c_str(), message3, trace); }
inline void DiagError(const char *module, const char *position, const char *message, const std::string &message2, const std::string &message3, bool trace = true) { DiagDllError3(module, position, message, message2.c_str(), message3.c_str(), trace); }
inline void DiagError(const char *module, const char *position, const std::string &message, const char *message2, const char *message3, bool trace = true) { DiagDllError3(module, position, message.c_str(), message2, message3, trace); }
inline void DiagError(const char *module, const char *position, const std::string &message, const char *message2, const std::string &message3, bool trace = true) { DiagDllError3(module, position, message.c_str(), message2, message3.c_str(), trace); }
inline void DiagError(const char *module, const char *position, const std::string &message, const std::string &message2, const char *message3, bool trace = true) { DiagDllError3(module, position, message.c_str(), message2.c_str(), message3, trace); }
inline void DiagError(const char *module, const char *position, const std::string &message, const std::string &message2, const std::string &message3, bool trace = true) { DiagDllError3(module, position, message.c_str(), message2.c_str(), message3.c_str(), trace); }

inline void DiagError(const char *module, const char *position, const std::ostringstream &message, bool trace = true) { DiagDllError(module, position, message.str().c_str(), trace); }

// DiagOut functions.
inline void DiagOut(const char *module, const char *position, const char *message) {  DiagDllOut(module, position, message); }
inline void DiagOut(const char *module, const char *position, const std::string &message) {  DiagDllOut(module, position, message.c_str()); }

inline void DiagOut(const char *module, const char *position, const std::string &message, const int iVal) { DiagDllOutI(module, position, message.c_str(), iVal); }

inline void DiagOut(const char *module, const char *position, const std::string &message, const char *message2) { DiagDllOut2(module, position, message.c_str(), message2); }
inline void DiagOut(const char *module, const char *position, const char *message, const std::string &message2) { DiagDllOut2(module, position, message, message2.c_str()); }
inline void DiagOut(const char *module, const char *position, const std::string &message, const std::string &message2) { DiagDllOut2(module, position, message.c_str(), message2.c_str()); }
inline void DiagOut(const char *module, const char *position, const char *message, const char *message2, const std::string &message3) { DiagDllOut3(module, position, message, message2 ,message3.c_str()); }
inline void DiagOut(const char *module, const char *position, const char *message, const std::string &message2, const char *message3) { DiagDllOut3(module, position, message, message2.c_str(), message3); }
inline void DiagOut(const char *module, const char *position, const char *message, const std::string &message2, const std::string &message3) { DiagDllOut3(module, position, message, message2.c_str(), message3.c_str()); }
inline void DiagOut(const char *module, const char *position, const std::string &message, const char *message2, const char *message3) { DiagDllOut3(module, position, message.c_str(), message2, message3); }
inline void DiagOut(const char *module, const char *position, const std::string &message, const char *message2, const std::string &message3) { DiagDllOut3(module, position, message.c_str(), message2, message3.c_str()); }
inline void DiagOut(const char *module, const char *position, const std::string &message, const std::string &message2, const char *message3) { DiagDllOut3(module, position, message.c_str(), message2.c_str(), message3); }
inline void DiagOut(const char *module, const char *position, const std::string &message, const std::string &message2, const std::string &message3) { DiagDllOut3(module, position, message.c_str(), message2.c_str(), message3.c_str()); }

inline void DiagOut(const char *module, const char *position, const std::ostringstream &message) { DiagDllOut(module, position, message.str().c_str()); }

#endif
