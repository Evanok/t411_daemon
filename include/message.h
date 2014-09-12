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
#define T411_API_URL "http://api.t411.me?_url="
#define T411_API_AUTH_URL "/auth"
#define T411_API_GETDETAIL_URL "/torrents/details/"
#define TIMEOUT_SECONDS 10

#define T411_TORRENT_TOKEN "<a href=\"//www.t411.me/torrents/"

#define T411_ID_TOKEN "<a href=\"/torrents/nfo/?id="

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

int t411_get_authentification (str_t411_config* config);
int t411_search_torrent_from_config (str_t411_config* config);
int t411_extract_torrent_info (char* data, str_t411_config* config);
int t411_download_torrent (char* data, str_t411_config* config);

int sendmail(const char *to, const char *from, const char *subject, const char *message);

#endif /* !MESSAGE_H_ */
