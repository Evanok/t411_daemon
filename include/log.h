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


#endif /* !LOG_H_ */
