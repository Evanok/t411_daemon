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
#include <fcntl.h>
#include <unistd.h>

#include "log.h"

/**
 * \struct str_tv_shows
 * \brief Structure that contain info to define a tv show
 */
typedef struct TvShows
{
  char name[64];	/*!< Name of the tv show*/
  int season;		/*!< Number of season */
  int episode;		/*!< Number of episode*/
} str_tv_shows;

/**
 * \struct str_t411_config
 * \brief Structure that contain config info on t411 daemon
 */
typedef struct T411Config
{
  char username[64];		/*!< Username to login on t411 */
  char password[64];		/*!< Password to login on t411 */
  str_tv_shows shows[256];	/*!< List of tv shows */
  FILE* fd_config;		/*!< fd on config file of the t411 daemon */
} str_t411_config;

#define WORKING_DIR	"/"
#define FILE_LOCK	"/var/lock/t411_daemon.lock"

#endif /* !T411_DAEMON_H_ */
