/**
**

** \file log.h
** \brief define header for logs feature
** \author Arthur LAMBERT
** \date 30/08/2014
**
**/

#ifndef LOG_H_
# define LOG_H_

# include <unistd.h>
# include <stdlib.h>
# include <syslog.h>

#ifndef DEBUG
 #define DEBUG 0
#endif

#define T411_LOG(a, ...) \
  do { if (DEBUG) fprintf(stderr, "\n[DEBUG] " __VA_ARGS__  ); else syslog(a, __VA_ARGS__);  } while (0)

#if DEBUG
#define DUMP_TORRENT(config) {						\
    int i;for(i=0;i < config.nb_torrent;i++) T411_LOG(LOG_DEBUG, "list[%d] : %d %s %d %d \n", i, config.torrents[i].type, config.torrents[i].name, config.torrents[i].season, config.torrents[i].episode);\
  }
#else
#define DUMP_TORRENT(config)
#endif

#if DEBUG
#define DUMP_RESULT(results, nb) {						\
    int i;for(i=0;i < nb;i++) T411_LOG(LOG_DEBUG, "\nid %d\nname %s\nleechers %d\nseeders %d\nsize %d Mo\ncompleted %d\nis_verified %d\n____\n", results[i].id, results[i].name, results[i].leechers, results[i].seeders, results[i].size / O_TO_MO , results[i].completed, results[i].is_verified); \
  }
#else
#define DUMP_RESULT(config, size)
#endif



#endif /* !LOG_H_ */
