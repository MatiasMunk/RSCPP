#ifndef DATABASE_IMPL
#define DATABASE_IMPL

#include "socket_impl.h"

// Define this for broken MariaDB C connector release 2.1.0
#ifdef MARIADB_CC_2_1_0_WORKAROUND
#if defined(__EMX__) || !defined(HAVE_UINT)
typedef unsigned int uint;
#endif
#endif // MARIADB_CC_2_1_0_WORKAROUND

#include <mysql/mysql.h>
#include <mysql/errmsg.h>

#endif // DATABASE_IMPL