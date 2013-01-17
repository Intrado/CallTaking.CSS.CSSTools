#ifndef VIPERTOOLS_H
#define VIPERTOOLS_H

#include "CSSTools.h"
#include <string>
#include <map>


class CSSTOOLS_API cViperTools
{
public:
  typedef std::map<std::string, std::string> tStringMap;

  // Read the Viper version registry key and return the version value
  static void SetDefaultViperVersion(double defaultViperVersion, int defaultViperServicePack);

  // Read the Viper version registry key and return the version value
  static double GetViperVersion(std::string diagModuleName);

  // Read the Viper service pack registry key and return the value
  static int GetViperServicePack(std::string diagModuleName);

  // Check if the Viper KB provided exists in the values of the registry key
  static bool IsViperKBPresent(std::string diagModuleName, std::string kbToCheck);

protected:
  static double gViperVersion;
  static int gViperServicePack;
  static double gDefaultViperVersion;
  static int gDefaultViperServicePack;

private:
  static tStringMap gViperKBs;
  static bool gViperKBChecked;

};

#endif