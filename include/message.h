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

/* 721 is fetch to VOSTF in t411 database */
#define VOSTFR 721
#define INDEX_EPISODE 936
#define INDEX_SEASON 967
#define T411_HTTP_URL "http://www.t411.me/torrents/search/?name=%s&description=&file=&user=&cat=210&subcat=%d&term%%5B46%%5D%%5B%%5D=%d&term%%5B45%%5D%%5B%%5D=%d&term%%5B17%%5D%%5B%%5D=%d&search=%%40name+%s+&submit=Recherche"
#define T411_URL "http://www.t411.me/public/index.php?_url=/%s"
#define TIMEOUT_SECONDS 10

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

char* process_message (char* url, char* message, char* token);
int get_authentification (str_t411_config* config);
int looking_for_torrent (str_t411_config* config);

#endif /* !MESSAGE_H_ */
