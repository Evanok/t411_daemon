/**
**

** \file t411_daemon.h
** \brief define main header of t411_daemon
** \author Arthur LAMBERT
** \date 30/08/2014
**
**/

#ifndef T411_DAEMON_H_
# define T411_DAEMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#define WORKING_DIR "/tmp"

#define DEBUG_PRINT(...) \
            do { if (DEBUG) syslog(LOG_DEBUG, __VA_ARGS__); } while (0)


#endif /* !T411_DAEMON_H_ */
