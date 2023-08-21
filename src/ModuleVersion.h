#define ModuleVersion 21
#define ModuleRevision 1

// ATTENTION: This file is evaluated during build, by OpenKNXproducer and some build scripts.
// Do not add any other information than the module version of this OFM.
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

#ifndef LOG_ModuleVersion
    #pragma message "\n\n<op:verify File=\"../lib/OFM-LogicModule/src/ModuleVersion.h\" ModuleVersion=\"" VALUE(ModuleVersion) "\" />\n\n"
    #pragma GCC error "\n\nPlease use OpenKNXproducer v2.3.4 or higher and rebuild your knxprod with correct op:verify entry\n\n"
#elif LOG_ModuleVersion != ModuleVersion
    #pragma message "\n\n\nYou need to >>> INCREASE YOUR <<< ETS ApplicationVersion and manually synchronize op:verify of the LOG Module to ModuleVersion " VALUE(ModuleVersion) "\n\n( see https://github.com/OpenKNX/OpenKNX/wiki/Versionierung-von-Modulen-(OFM)#fehler-vom-compiler )\n\n\n"
    #pragma GCC error "\n\nETS Application Version problem (see next message)\n\n"
#endif
