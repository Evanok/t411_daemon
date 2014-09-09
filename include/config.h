/**
**

** \file config.h
** \brief define config header of t411_daemon
** \author Arthur LAMBERT
** \date 30/08/2014
**
**/

#ifndef CONFIG_H_
# define CONFIG_H_

#include "log.h"

#include "t411_daemon.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define CONF_FILE "/etc/t411-daemon.conf"
#define SIZE 256
/* alloc torrents array per size of 10 tu not realloc array at each new torrent */
#define POOL_TORRENT 10

int read_config (str_t411_config* config);

#endif /* !CONFIG_H_ */
