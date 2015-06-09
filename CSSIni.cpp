#pragma warning(disable: 4786)
#include <string>
#include <ctype.h>
#include <algorithm>
#include "CSSIni.h"
#include "IniFile.h"
#include "ToStr.h"
#include "cAutoLock.h"
#include "CSS.h"

namespace CSS
{  
  const char *cCSSIni::moduleName = "CSSIni";
  const char *cCSSIni::INI_EXT = ".INI";
  const int cCSSIni::N_SIZE_KEY = 1024;
  const int cCSSIni::N_SIZE_BUFFER = 512;
  const int cCSSIni::N_TEXT = 10;

  cCSSIni::tIniInstanceMap cCSSIni::gIniInstanceMap;

	/*  Methodes Surcharge */
  namespace
  {
    string FilterSpace(string TheText);
  }

  cCSSIni* cCSSIni::GetIni(string name)
  {
    cAutoLock a(gIniInstanceMap);
    tIniInstanceMap::iterator it = gIniInstanceMap.find(name);
    return (it != gIniInstanceMap.end() ? it->second : 0);
  }

  cCSSIni::cCSSIni(): mInitialized(false)
	{
	}

	cCSSIni::~cCSSIni()
	{
    if (!mName.empty())
    {
      cAutoLock a(gIniInstanceMap);
      gIniInstanceMap.erase(mName);
    }
	}

  cCSSIni * cCSSIni::init(string File, string name)
	{
		int WinResult;
		void *lpFilePart;
		string lpPath;
		string lpFileName;
		string lpExtension;
		int nPathFileNameFoundLength;
		char *lpValidPathFileNameFound = NULL;
		int KeySize;
		IniFile = File;
//		lpPath; // T2S
//		lpPath = NULL;
		lpExtension = INI_EXT;
		lpFilePart = NULL;
		lpFileName = IniFile;

		if (lpFileName.empty())
		{
			/*  Validation de la presence du fichier de configuration lors de l'initialisation */
			throw E(FileIniNotDefined);
		};
		nPathFileNameFoundLength = N_SIZE_BUFFER;
		lpValidPathFileNameFound = new char[N_SIZE_BUFFER + 1];
		WinResult = IniFile::PathSearch(lpPath, lpFileName, lpExtension, nPathFileNameFoundLength, lpValidPathFileNameFound, lpFilePart);

    /*  Le fichier est-il present ?, On effectue une recherche */
		if (Diag::TraceOn)
		{
			if (Path != "")
			{
				KeySize = (Path).length();
				DiagTrace(moduleName, "Init", Path + "Taille: ", KeySize);
			}
			else
			{
				DiagTrace(moduleName, "Init", "No Path specified");;
			};
		};
    if (WinResult > 0 && lpValidPathFileNameFound != NULL)
		{
			Path = string(lpValidPathFileNameFound);
		}
		else
		{
			Path = "";
			if (lpValidPathFileNameFound != NULL)
			{
        delete[] lpValidPathFileNameFound;     // Coverity #14296: Add [] because it is an array
				lpValidPathFileNameFound = NULL;
			};
			throw E(FileIniNotFound);
		};
		if (lpValidPathFileNameFound != NULL)
		{
			delete [] lpValidPathFileNameFound;     // Coverity #14296: Add [] because it is an array
			lpValidPathFileNameFound = NULL;
		};

    // The file was found, so the object is valid.
    {
      cAutoLock a(gIniInstanceMap);
      mName = name;
      gIniInstanceMap[mName] = this;
    }

    mInitialized = true;
		return (cCSSIni* )this;
	}
	
	int cCSSIni::getAllSectionKey(string section, string * &KeyArray, int NbKeyMax)
	{
		char C;
		/*  Obtient toutes les cle d'une section donne */
		int WinResult;
		int nSize;
		char SectionKey[N_SIZE_BUFFER * N_TEXT + 1];
		int NbSectionKey;
		int KeySize;
		int J;
		int K;
		char TmpString[255 + 1];
		string SubArray;
		string lpFileName;
		string NewKeyFilter;
		WinResult = 0;
		nSize = N_SIZE_BUFFER * N_TEXT;
		NbSectionKey = 0;
		lpFileName = IniFile;
		if (!mInitialized)
		{
			/*  Validation de la presence du fichier de configuration lors de l'initialisation */
			throw E(FileIniNotDefined);
		};
		
		WinResult = IniFile::GetPrivateProfileSect(section, SectionKey, nSize, Path);
		if (WinResult > 0)
		{
			/*  Decomposition des chaines */
			KeySize = WinResult;
  		DiagTrace(moduleName, "GetAllSectionKey", "Taille: ", KeySize);

      J = 0;
			K = 0;
			for (int I = 0; I <= KeySize - 1; I++)
			{
				C = SectionKey[I];
				if (C == 0)
				{
					if (J == NbKeyMax)
					{
						throw E(ArraySizeTooShort);
					};
					SubArray = string(TmpString);
					KeyArray[J] = (SubArray).substr(0, K );

          /*  Filtrage des commentaires */
					DiagTrace(moduleName, "GetAllSectionKey", "KeyRead:", KeyArray[J]);
					DiagTrace(moduleName, "GetAllSectionKey", "KeySize:", KeyArray[J].length());

          NewKeyFilter = FilterTextLine(KeyArray[J]);
					KeyArray[J] = NewKeyFilter;

          DiagTrace(moduleName, "GetAllSectionKey", "KeyRead (After filter): ", KeyArray[J]);
					DiagTrace(moduleName, "GetAllSectionKey", "KeySize (After filter): ", KeyArray[J].length());

          NbSectionKey++;
					J = J + 1;
					K = 0;
				}
				else
				{
					TmpString[K] = C;
					K = K + 1;
				};
			};
		}
		else
		{
			throw E(KeyNotFound);
		};
		return NbSectionKey;
	}
	
	int cCSSIni::
		ngetConfigKey(string section, string key)
	{
		int WinSize;
		string lpFileName;
		lpFileName = IniFile;
		if (!mInitialized)
		{
			/*  Validation de la presence du fichier de configuration lors de l'initialisation */
			throw E(FileIniNotDefined);
		};
		WinSize = IniFile::GetPrivateProfileI(section, key, -1, Path);
		/*  Obtient l'information desiree de la Section et de la Cle */
		if (WinSize >= 0)
		{
			DiagTrace(moduleName, "nGetConfigKey", "KeyRead:", WinSize);
		}
		else
		{
			throw E(KeyNotFound);
		};
		return WinSize;
	}
	
	int cCSSIni::
		ngetConfigKeyDef(string section, string key, int defaultVal)
	{
		int WinSize;
		string lpFileName;
		lpFileName = IniFile;
		if (!mInitialized)
		{
			/*  Validation de la presence du fichier de configuration lors de l'initialisation */
			throw E(FileIniNotDefined);
		};
		WinSize = IniFile::GetPrivateProfileI(section, key, -1, Path);
		/*  Obtient l'information desiree de la Section et de la Cle */
		if (WinSize >= 0)
		{
			DiagTrace(moduleName, "nGetConfigKeyDef", "KeyRead:", WinSize);
		}
		else
		{
			DiagTrace(moduleName, "nGetConfigKeyDef", "defaut value returned : ", defaultVal);
			return defaultVal;
		};
		return WinSize;
	}
	
	string cCSSIni::getConfigKey(string section, string key)
	{
		string KeyRead = " ";
		/*  Obtient la definition (TEXT) d'une cle d'une section donnee */
		string SubArray;
		int WinSize;
		string lpFileName;
		char KeyData[N_SIZE_KEY + 1];
		lpFileName = IniFile;
		if (!mInitialized)
		{
			/*  Validation de la presence du fichier de configuration lors de l'initialisation */
			throw E(FileIniNotDefined);
		};
		WinSize = IniFile::GetPrivateProfileStr(section, key, " "/*NULL*/, KeyData, N_SIZE_KEY, Path); //T2S
		/*  Obtient l'information desiree de la Section et de la Cle */
		if (WinSize > 0)
		{
			SubArray = string(KeyData);
			KeyRead = (SubArray).substr(0, WinSize );

      DiagTrace(moduleName, "GetConfigKey", "KeyRead:", KeyRead);
			DiagTrace(moduleName, "GetConfigKey", "KeySize:", WinSize);
		}
		else
		{
			throw E(KeyNotFound);
		};
		KeyRead = FilterTextLine(KeyRead);

    DiagTrace(moduleName, "GetConfigKey", "KeyRead (After filter):", KeyRead);
		DiagTrace(moduleName, "GetConfigKey", "KeySize (After filter):", KeyRead.length());
		return KeyRead;
	}
	
	string cCSSIni::getConfigKeyDef(string section, string key, string defaultVal)
	{
		string KeyRead = " ";
		/*  Obtient la definition (TEXT) d'une cle d'une section donnee */
		string SubArray;
		int WinSize;
		string lpFileName;
		char KeyData[N_SIZE_KEY + 1];
		lpFileName = IniFile;
		if (!mInitialized)
		{
			/*  Validation de la presence du fichier de configuration lors de l'initialisation */
			throw E(FileIniNotDefined);
		};
		WinSize = IniFile::GetPrivateProfileStr(section, key, " "/*NULL*/, KeyData, N_SIZE_KEY, Path);
		/*  Obtient l'information desiree de la Section et de la Cle */
		if (WinSize > 0)
		{
			SubArray = string(KeyData);
			KeyRead = (SubArray).substr(0, WinSize );

      DiagTrace(moduleName, "GetConfigKeyDef", "KeyRead:", KeyRead);
			DiagTrace(moduleName, "GetConfigKeyDef", "KeySize:", WinSize);
		}
		else
		{
      DiagTrace(moduleName, "GetConfigKeyDef", "defaut value returned : ", defaultVal);
			return defaultVal;
		};
		KeyRead = FilterTextLine(KeyRead);

    DiagTrace(moduleName, "GetConfigKeyDef", "KeyRead (After filter):", KeyRead);
		DiagTrace(moduleName, "GetConfigKeyDef", "KeySize (After filter):", KeyRead.length());
		return KeyRead;
	}

	string cCSSIni::getIniFileName()
	{
		/*  Obtient le nom du fichier du Registreur avec son repertoire au complet */
		return Path;
	}
	
	string cCSSIni::FilterTextLine(string TheText)
	{
		string TheFilterTextLine;
		/* ---------------------------------METHODES PRIVEES--------------------------------------- */
		/* ---------------------------------METHODE DE CLASSE-------------------------------------- */
		/*  Ces procedures pourront etre utiliser par d'autre modules */
		/*  Cette procedure filtre le texte de caracteres non visible et indesirables */
		char C;
		int Lg = 0;
		int NB_GUILLEMETS = 0;
		if (TheText != "")
		{
			Lg = (TheText).length();
			for (int I = 0; I <= Lg - 1; I++)
			{
				C = TheText[I];
				if (((C >= ' ') && (C <= '~')))
				{
					/*  Caracteres affichables pour du text */
					if ((C == '"'))
					{
						/*  Caracteres de filtrage */
						/*  String entre guillemets , ne prend que le contenu */
						NB_GUILLEMETS++;
						if (NB_GUILLEMETS == 2)
						{
							return TheFilterTextLine;
						};
					}
					else if ((C == ';'))
					{
						TheFilterTextLine = FilterSpace(TheFilterTextLine);
						/*  Filtre tous les blancs precedent le ; */
						return TheFilterTextLine;
					}
					else
					{
						TheFilterTextLine = (TheFilterTextLine + C);
					};
				}
				else
				{
					return TheFilterTextLine;
				};
			};
		};
		return TheFilterTextLine;
	}
	
	
  namespace
  {
	  string FilterSpace(string TheText)
	  {
		  string TheFilterTextLine;
		  char C = ' ';
		  int Lg = 0;
		  string TheNewText;
		  int J = 0;
		  int K = 0;
		  Lg = (TheText).length();
		  if (Diag::TraceOn)
		  {
			  DiagTrace(cCSSIni::moduleName, "FilterSpace", "CaR:", C);;
		  };
		  for (int I = Lg - 1; I >= 0; I += -1)
		  {
			  C = TheText[I];
			  if (Diag::TraceOn)
			  {
				  DiagTrace(cCSSIni::moduleName, "FilterSpace", "CaR:", C);;
			  };
			  if ((C != ' '))
			  {
			  }
			  else
			  {
				  /*  Caracteres de filtrage */
				  J++;
			  };
		  };

      DiagTrace(cCSSIni::moduleName, "FilterSpace", "Size to cut:", J);

      K = Lg - J - 1;
		  for (int I1 = 0; I1 <= K; I1++)
		  {
			  TheNewText = TheNewText + TheText[I1];
		  };
		  return TheNewText;
	  }
  }
	
	void cCSSIni::DiagWithoutExcept(bool DiagTrace, string FromModuleName, string FromFunction, Failure v)
	{
		if (((v) == FileIniNotDefined))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: FileIniNotDefined");
		}
		else if (((v) == FileIniNotFound))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: FileIniNotFound");
		}
		else if (((v) == SectionNotFound))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: SectionNotFound");
		}
		else if (((v) == KeyNotFound))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: KeyNotFound");
		}
		else if (((v) == ArraySizeTooShort))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: ArraySizeTooShort");
		}
		else
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception non traiter CSSIni\n");
		};
	}
	
	void cCSSIni::DiagWithExcept(bool DiagTrace, string FromModuleName, string FromFunction, Failure v)
	{
		/*  Traitement des exceptions de CSSIni.T */
		if (((v) == FileIniNotDefined))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: FileIniNotDefined");
			throw E(FileIniNotDefined);
		}
		else if (((v) == FileIniNotFound))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: FileIniNotFound");
			throw E(FileIniNotFound);
		}
		else if (((v) == SectionNotFound))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: SectionNotFound");
			throw E(SectionNotFound);
		}
		else if (((v) == KeyNotFound))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: KeyNotFound");
			throw E(KeyNotFound);
		}
		else if (((v) == ArraySizeTooShort))
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception !!!: ArraySizeTooShort");
			throw E(ArraySizeTooShort);
		}
		else
		{
			::DiagTrace(FromModuleName, FromFunction, "Exception non traiter CSSIni\n");
		};
	}
}

