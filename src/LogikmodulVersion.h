#define ModuleVersion 21

// ATTENTION: This file is evaluated during build, by OpenKNXproducer and some build scripts.
// Do not add any other information than the module version of this OFM.
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)

#ifndef LOG_ModuleVersion
    #pragma message "\n\n<op:verify File=\"../lib/OFM-LogicModule/src/LogikmodulVersion.h\" ModuleVersion=\"" VALUE(ModuleVersion) "\" />\n\n"
    #pragma GCC error "\n\nPlease use OpenKNXproducer v2.3.4 or higher and rebuild your knxprod with correct op:verify entry\n\n"
#elif LOG_ModuleVersion != ModuleVersion
    #pragma message "\n\n\nYou need to >>> INCREASE YOUR <<< ETS ApplicationVersion and manually synchronize op:verify to ModuleVersion " VALUE(ModuleVersion) "\n\n\n"
    #pragma GCC error "\n\nETS Application Version problem (see next message)\n\n"
#endif
