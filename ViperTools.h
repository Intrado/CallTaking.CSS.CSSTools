#ifndef VIPERTOOLS_H
#define VIPERTOOLS_H

#include "CSSTools.h"
#include <string>

class CSSTOOLS_API cViperTools
{
public:
  // Read the Viper version registry key and return the version value
  static void SetDefaultViperVersion(double defaultViperVersion, int defaultViperServicePack);

  // Read the Viper version registry key and return the version value
  static double GetViperVersion(std::string diagModuleName);

  // Read the Viper service pack registry key and return the value
  static int GetViperServicePack(std::string diagModuleName);

protected:
  static double gViperVersion;
  static int gViperServicePack;
  static double gDefaultViperVersion;
  static int gDefaultViperServicePack;
};

#endif