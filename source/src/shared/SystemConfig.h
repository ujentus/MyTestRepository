#ifndef MANGOS_SYSTEMCONFIG_H
#define MANGOS_SYSTEMCONFIG_H

#include "Platform/CompilerDefs.h"

#ifndef _VERSION
#if PLATFORM == PLATFORM_WIN32
# define _VERSION "0.7 SM9 custom"
#else
# define _VERSION "@VERSION@"
#endif
#endif

// Format is YYYYMMDDRR where RR is the change in the conf file
// for that day.
#ifndef _MANGOSDCONFVERSION
# define _MANGOSDCONFVERSION 2006060401
#endif
#ifndef _REALMDCONFVERSION
# define _REALMDCONFVERSION 2006060401
#endif

#if PLATFORM == PLATFORM_WIN32
# define _FULLVERSION "/" _VERSION " (Win32)"
# define _MANGOSD_CONFIG  "mangosd.conf"
# define _REALMD_CONFIG   "realmd.conf"
#else
# define _FULLVERSION "/" _VERSION " (Unix)"
# define _MANGOSD_CONFIG  "@MANGOSD_CONFIG@"
# define _REALMD_CONFIG  "@REALMD_CONFIG@"
#endif

#define DEFAULT_PLAYER_LIMIT 100
#define DEFAULT_WORLDSERVER_PORT 8085                       //8129
#define DEFAULT_REALMSERVER_PORT 3724
#define DEFAULT_SOCKET_SELECT_TIME 10000
#endif
