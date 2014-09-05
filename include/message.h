/**
**

** \file message.h
** \brief define header of t411_daemon messages
** \author Arthur LAMBERT
** \date 30/08/2014
**
**/

#ifndef MESSAGE_H_
# define MESSAGE_H_

#define T411_URL "http://www.t411.me/public/index.php?_url=/%s"
#define TIMEOUT_SECONDS 5

#include <curl/curl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "t411_daemon.h"

struct MemoryStruct
{
  char *memory;
  size_t size;
};

char* process_message (CURL *curl, char* url, char* message);
int get_authentification (CURL *curl, str_t411_config* config);

#endif /* !MESSAGE_H_ */
