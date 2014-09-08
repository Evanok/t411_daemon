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
 * \enum type_torrent
 * \brief Specifies possibility of type for torrent
 */
enum type_torrent
{
  UNDEFINED	= 0,	/**< Undefined type */
  ANIMATION,		/**< Animation type */
  TV_SHOW,		/**< Tv show type */
};



/**
 * \struct str_torrent
 * \brief Structure that contain info to define a torrent that can be an anime or tv show
 */
typedef struct Torrent
{
  char name[64];		/*!< Name of the torrent */
  enum type_torrent type;	/*!< Type of the torrent */
  int season;			/*!< Number of season */
  int episode;			/*!< Number of episode */
} str_torrent;


/**
 * \struct str_t411_config
 * \brief Structure that contain config info on t411 daemon
 */
typedef struct T411Config
{
  char username[64];		/*!< Username to login on t411 */
  char password[64];		/*!< Password to login on t411 */
  char mail[64];		/*!< Email to warn user when new data are available */
  char uuid[32];		/*!< Uuid info from t411 */
  str_torrent* torrents;	/*!< List of torrent to poll */
  FILE* fd_config;		/*!< fd on config file of the t411 daemon */
  char token[64];		/*!< Token used to be allowed to request t411 */
} str_t411_config;

#define WORKING_DIR	"/"
#define FILE_LOCK	"/var/lock/t411_daemon.lock"

#endif /* !T411_DAEMON_H_ */
