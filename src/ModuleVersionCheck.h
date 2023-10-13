#include "versions.h"

// Change the following 3 lines and adopt to your module names
#define ModuleVersion_ModuleName "LogicModule"
#define ModuleVersion_knxprod_h LOG_ModuleVersion
#define ModuleVersion_version_h MODULE_LogicModule_ETS

// ATTENTION: This file is evaluated during build, by OpenKNXproducer and some build scripts.
// No changes needed behind this point
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

#if ModuleVersion_knxprod_h != ModuleVersion_version_h
    #pragma message "\n\n\nYou need to >>> INCREASE YOUR <<< ETS ApplicationVersion and manually synchronize op:verify of " ModuleVersion_ModuleName " to " VALUE(ModuleVersion_version_h) "\n\n(see https://github.com/OpenKNX/OpenKNX/wiki/Versionierung-von-Modulen-(OFM)#fehler-vom-compiler )\n\n\n"
    #pragma GCC error "\n\nETS Application Version problem (see next message)\n\n"
#endif
