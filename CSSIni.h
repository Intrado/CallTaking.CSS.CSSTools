#ifndef __CSSIni_INCLUDED__
#define __CSSIni_INCLUDED__
#pragma warning(disable:4786)

#include <string>
#include "LockableStlContainers.h"
#include "CSSObject.h"
#include "cCritSec.h"
#include "CSSTools.h"

namespace CSS
{
  class CSSTOOLS_API cCSSIni:public virtual cCSSObject
  {
  public:
    static const char *moduleName;
    static const char *INI_EXT;
  
    enum Failure
    {
      FileIniNotDefined, FileIniNotFound, SectionNotFound, KeyNotFound, ArraySizeTooShort
    };
  
    class E
    {
      Failure exception;
    public:
      E (Failure e) {exception = e;}
      operator Failure () {return exception;}
    };

    static cCSSIni* GetIni(std::string name);
  
    cCSSIni ();
    virtual ~cCSSIni ();
    static std::string FilterTextLine (std::string TheText);
    static void DiagWithExcept (bool DiagTrace, std::string FromModuleName, std::string FromFunction, Failure v);
    static void DiagWithoutExcept (bool DiagTrace, std::string FromModuleName, std::string FromFunction, Failure v);	
    
    static const int N_SIZE_KEY;
    static const int N_SIZE_BUFFER;
    static const int N_TEXT;      

    //  Constructor
    virtual cCSSIni *init (std::string file, std::string name);

    // Obtain all keys of the specified section
    virtual int getAllSectionKey (std::string section, std::string * &KeyArray, int NbKeyMax);
    
    // Return the text value of the given key
    virtual std::string getConfigKey (std::string section, std::string key);
    
    // Return the text value of the given key, with a default value in the case it is not found
    virtual std::string getConfigKeyDef (std::string section, std::string key, std::string defaultVal);
    
    // Return the int value of the given key
    virtual int ngetConfigKey (std::string section, std::string key);
    
    // Return the int value of the given key, with a default value in the case it is not found
    virtual int ngetConfigKeyDef (std::string section, std::string key, int defaultVal);
    
    // Returns the name of the ini file associated with the Ini object
    virtual std::string getIniFileName ();

  private:
    struct cNameCmp
    {
      bool operator()(const std::string s1, const std::string s2) const
      {
        return (stricmp(s1.c_str(), s2.c_str()) < 0); // Case-insensitive comparison.
      }
    };
    typedef cLockableMap<std::string, cCSSIni*, cNameCmp> tIniInstanceMap;

    static tIniInstanceMap gIniInstanceMap;

    bool mInitialized;
    std::string Path;
    std::string IniFile;
    std::string mName;
  };
};

#endif
