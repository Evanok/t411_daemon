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

/* We will pool t411 every day */
#define LOOP_POOLING 86400

/* integer to sum to get real episode/season number */
#define INDEX_EPISODE 936
#define INDEX_SEASON 967

#define TERM_LANGAGE 17
#define TERM_EPISODE 46
#define TERM_SEASON 45

/**
 * \enum type_torrent
 * \brief Specifies possibility of type for torrent
 */
enum type_torrent
{
  UNDEFINED	= 0,	/**< Undefined type */
  ANIMATION 	= 455,	/**< Animation type */
  TV_SHOW 	= 433 	/**< Tv show type */
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
 * \struct str_torrent_config
 * \brief Structure that contain result from t411 search
 */
typedef struct TorrentResult
{
  char name[128];		/*!< Label name of the torrent on t411 */
  int id;			/*!< Id which identifies torrent in t411 database */
  int seeders;			/*!< Number of seeders */
  int leechers;			/*!< Number of leechers */
  int size;			/*!< Size of the torrent in Mb */
  int completed;		/*!< Number of times where the torrent was completed */
  int is_verified;		/*:< Indicate if the torrent is verified by the community */
} str_torrent_result;

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
  int nb_torrent;		/*!< Current number of torrent in list */
  char token[64];		/*!< Token used to be allowed to request t411 */
} str_t411_config;

#define WORKING_DIR	"/"
#define FILE_LOCK	"/var/lock/t411_daemon.lock"

#endif /* !T411_DAEMON_H_ */
