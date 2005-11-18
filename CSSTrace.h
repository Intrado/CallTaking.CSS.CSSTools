/* The multiple-inclusion guard has been voluntarily removed from this file, so that it can
   be included inside multiple namespaces in the same compilation unit.  This file should
   not be included inside the global namespace. */

// #ifndef __CSSTrace_INCLUDED__
// #define __CSSTrace_INCLUDED__

// CSSTrace functions
inline void CSSTrace(const char *module, const char *position, const char *message, const int iVal) { if (Diag::TraceOn) DiagDllTraceI(module, position, message, iVal); }
inline void CSSTrace(const char *module, const char *position, const std::string &message, const int iVal) { if (Diag::TraceOn) DiagDllTraceI(module, position, message.c_str(), iVal); }

inline void CSSTrace(const char *module, const char *position, const char *message = "Starting") { if (Diag::TraceOn) DiagDllTrace(module, position, message); }
inline void CSSTrace(const char *module, const char *position, const std::string &message = "Starting") { if (Diag::TraceOn) DiagDllTrace(module, position, message.c_str()); }

inline void CSSTrace(const char *module, const char *position, const char *message, const char *message2) { if (Diag::TraceOn) DiagDllTrace2(module, position, message, message2); }
inline void CSSTrace(const char *module, const char *position, const std::string &message, const char *message2) { if (Diag::TraceOn) DiagDllTrace2(module, position, message.c_str(), message2); }
inline void CSSTrace(const char *module, const char *position, const char *message, const std::string &message2) { if (Diag::TraceOn) DiagDllTrace2(module, position, message, message2.c_str()); }
inline void CSSTrace(const char *module, const char *position, const std::string &message, const std::string &message2) { if (Diag::TraceOn) DiagDllTrace2(module, position, message.c_str(), message2.c_str()); }

inline void CSSTrace(const char *module, const char *position, const char *message, const char *message2, const char *message3) { if (Diag::TraceOn) DiagDllTrace3(module, position, message, message2 ,message3); }
inline void CSSTrace(const char *module, const char *position, const char *message, const char *message2, const std::string &message3) { if (Diag::TraceOn) DiagDllTrace3(module, position, message, message2 ,message3.c_str()); }
inline void CSSTrace(const char *module, const char *position, const char *message, const std::string &message2, const char *message3) { if (Diag::TraceOn) DiagDllTrace3(module, position, message, message2.c_str(), message3); }
inline void CSSTrace(const char *module, const char *position, const char *message, const std::string &message2, const std::string &message3) { if (Diag::TraceOn) DiagDllTrace3(module, position, message, message2.c_str(), message3.c_str()); }
inline void CSSTrace(const char *module, const char *position, const std::string &message, const char *message2, const char *message3) { if (Diag::TraceOn) DiagDllTrace3(module, position, message.c_str(), message2, message3); }
inline void CSSTrace(const char *module, const char *position, const std::string &message, const char *message2, const std::string &message3) { if (Diag::TraceOn) DiagDllTrace3(module, position, message.c_str(), message2, message3.c_str()); }
inline void CSSTrace(const char *module, const char *position, const std::string &message, const std::string &message2, const char *message3) { if (Diag::TraceOn) DiagDllTrace3(module, position, message.c_str(), message2.c_str(), message3); }
inline void CSSTrace(const char *module, const char *position, const std::string &message, const std::string &message2, const std::string &message3) { if (Diag::TraceOn) DiagDllTrace3(module, position, message.c_str(), message2.c_str(), message3.c_str()); }

inline void CSSTrace(const char *module, const char *position, const std::ostringstream &message) { if (Diag::TraceOn) DiagDllTrace(module, position, message.str().c_str()); }

#define CSSDiagExec(command)\
  if (Diag::TraceOn)\
  {\
    command\
  }

// #endif
