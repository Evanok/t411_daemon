#include "config.h"

/**
 * \fn static void extract_torrent_data (char* line, str_t411_config* config)
 * \brief Extract information on torrent thanks to line from config file
 *
 * \param line char array which contain the current parsing line of the configuration file
 * \param config Structure that contains all info from config file
 * \return void
 */
static void extract_torrent_data (char* line, str_t411_config* config)
{
  char* token = NULL;
  int col = 0;


  config=config;
  T411_LOG (LOG_DEBUG, "Current line : |%s|\n", line);

  token = strtok (line, "\t \n");
  while (token)
  {
    switch (col)
    {
      case 0:
	break;
      case 1:
	break;
      case 2:
	break;
    }
    token = strtok (NULL, "\t \n");
  }
}

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
  char* key = NULL;
  char* data = NULL;

  if (!is_existing_conf ())
  {
    return 1;
  }

  config->fd_config = fopen (CONF_FILE, "r+");

  if (config->fd_config == NULL)
  {
    perror ("fopen");
    T411_LOG (LOG_ERR, "Not able to open %s !", CONF_FILE);
    return 1;
  }

  T411_LOG (LOG_DEBUG, "read config file...\n");

  while (fgets(line, SIZE, config->fd_config) != NULL)
  {
    /* truncate endline */
    key = strtok (line, "\t \n");
    if (!key) continue;
    if (key[0] == 'T')
    {
      extract_torrent_data (line, config);
      continue;
    }
    data = strtok (NULL, "\t \n");
    if (!data) continue;
    if (strncmp (key, "username", 8) == 0)
    {
      strcpy (config->username, data);
    }
    else if (strncmp (key, "password", 8) == 0)
    {
      strcpy (config->password, data);
    }
    else if (strncmp (key, "mail", 4) == 0)
    {
      if (strchr (data, '@') == NULL)
      {
	T411_LOG (LOG_ERR, "Wrong format for mail info  %s !", CONF_FILE);
	return 1;
      }
      strcpy (config->mail, data);
    }
    else
    {
      T411_LOG (LOG_DEBUG, "Unknow key and data in config file : |%s|%s|\n", key, data);
    }
  }

  T411_LOG (LOG_DEBUG, "username : |%s|\n", config->username);
  T411_LOG (LOG_DEBUG, "password : |%s|\n", config->password);
  T411_LOG (LOG_DEBUG, "mail : |%s|\n", config->mail);

  if (!config->password[0] || !config->username[0])
  {
    T411_LOG (LOG_ERR, "Not able to get username/password from  %s !", CONF_FILE);
    return 1;
  }

  if (!config->mail[0])
  {
    T411_LOG (LOG_ERR, "Not able to get mail info from  %s !", CONF_FILE);
    return 1;
  }

  return 0;
}
