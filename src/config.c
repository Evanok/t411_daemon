#include "config.h"

/**
 * \fn static int is_existing_conf (void)
 * \brief Check that config file for the daemon is defined in the current system
 *
 * \param void
 * \return integer. 1 means that the conf file exists. 0 is error.
 */
static int is_existing_conf (void)
{
  struct stat s;
  int err;

  err = stat(CONF_FILE, &s);
  if(-1 == err)
  {
    if(ENOENT == errno)
    {
      /* does not exist */
      T411_LOG (LOG_ERR, "%s does not exist. You must define a configuration file !", CONF_FILE);
      return 0;
    }
    else
    {
      perror("stat");
      T411_LOG (LOG_ERR, "%s perror", CONF_FILE);
      return 0;
    }
  }
  else
  {
    if(S_ISREG(s.st_mode))
      return 1;
  }
  return 0;
}

/**
 * \fn int read_config (str_t411_config* config)
 * \brief Read the configuration file to get username/password and some other information
 *
 * \param config Structure which contains all data needed by the daemon
 * \return Integer. 0 in success else error code.
 */
int read_config (str_t411_config* config)
{
  char line [SIZE];

  if (!is_existing_conf ())
  {
    return -1;
  }

  config->fd_config = fopen (CONF_FILE, "r+");

  if (config->fd_config == NULL)
  {
    perror ("fopen");
    T411_LOG (LOG_ERR, "Not able to open %s !", CONF_FILE);
    return -1;
  }

  T411_LOG (LOG_DEBUG, "read config file...\n");

  while (fgets(line, SIZE, config->fd_config) != NULL)
  {
    /* truncate endline */
    line[strlen(line) - 1] = 0;
    if (strncmp (line, "username", 8) == 0)
      strcpy (config->username, line + 9);
    else if (strncmp (line, "password", 8) == 0)
      strcpy (config->password, line + 9);

    T411_LOG (LOG_DEBUG, "line : |%s|\n", line);
  }

  T411_LOG (LOG_DEBUG, "username : |%s|\n", config->username);
  T411_LOG (LOG_DEBUG, "password : |%s|\n", config->password);

  return 0;
}
