#ifndef __CHJDKVER

#define __CHJDKVER

#define VERSION_STRING "2.0"

#define CURRENT_JDK_PATH "/System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK"
#define TARGET_DIR       "/Library/Java/JavaVirtualMachines"
#define VERSION_PATTERN  "0123456789-_."

#define LIST       0x001
#define CHANGE     0x002
#define INIT_ENV   0x004
#define INIT_JDKS  0x008
#define HELP       0x010
#define VERSION    0x020

extern int init_jdks();
extern int init_env();
extern int change_version(char *);
extern int list_versions();

#endif
