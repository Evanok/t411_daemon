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

#include "log.h"

typedef struct TvShows
{
  char name[64];
  int season;
  int episode;
} str_tv_shows;

typedef struct T411Config
{
  char username[64];
  char password[64];
  str_tv_shows shows[256];
  FILE* fd_config;
} str_t411_config;

#define WORKING_DIR "/"

#endif /* !T411_DAEMON_H_ */
