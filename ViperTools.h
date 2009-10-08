#ifndef VIPERTOOLS_H
#define VIPERTOOLS_H

#include "CSSTools.h"
#include <string>

class CSSTOOLS_API cViperTools
{
public:
  // Read the Viper version registry key and return the version value
  static double GetViperVersion(std::string diagModuleName);

protected:
  static double mViperVersion;
};

#endif