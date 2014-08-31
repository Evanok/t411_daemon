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


#define CONF_FILE "/run/t411/t411_daemon.conf"
#define SIZE 256

int read_config (str_t411_config* config);

#endif /* !CONFIG_H_ */
