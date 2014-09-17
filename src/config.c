#include "config.h"

/**
 * \fn static void ltrim (char* str)
 * \brief Remove trailing whitespace from the left side of the char array
 *
 * \param str char array that we want to trim
 * \return void
 */
static void ltrim (char* str)
{
  char* newstart = str;

  while (isspace(*newstart))
    newstart++;

  if (str != newstart)
    memmove (str, newstart, strlen( newstart) + 1);
}


/**
 * \fn static void extract_torrent_data (char* line, str_t411_config* config)
 * \brief Extract information on torrent thanks to line from config file
 *
 * \param line char array which contain the current parsing line of the configuration file
 * \param config Structure that contains all info from config file
 * \return int, 0 if success else 1
 */
static int extract_torrent_data (char* line, str_t411_config* config)
{
  char* token = NULL;
  int col = 1;
  str_torrent new_torrent;
  char* tmp = NULL;

  //T411_LOG (LOG_DEBUG, "line : %s", line);

  memset (&new_torrent, 0, sizeof (new_torrent));
  token = strtok (line + 1, "\t \n");
  while (token)
  {
    switch (col++)
    {
      case 1:
	if (strncmp (token, "A", 1) == 0)
	  new_torrent.type = ANIMATION;
	else if (strncmp (token, "S", 1) == 0)
	  new_torrent.type = TV_SHOW;
	else
	  goto error;
	break;
      case 2:
	strncpy (new_torrent.name, token, strlen(token) + 1);
	break;
      case 3:
	tmp = token;
	while (tmp && isdigit(*tmp))
	  tmp++;
	if (tmp[0])
	  goto error;
	new_torrent.season = atoi(token);
	break;
      case 4:
	tmp = token;
	while (tmp && isdigit(*tmp))
	  tmp++;
	if (tmp[0])
	  goto error;
	new_torrent.episode = atoi(token);
	break;
      default:
	goto error;
	break;
    }
    token = strtok (NULL, "\t \n");
  }

  if (col != 5)
    goto error;

  if (config->nb_torrent == 0)
      config->torrents = malloc(sizeof (*(config->torrents)) * (POOL_TORRENT));
  else if (config->nb_torrent % 10 == 0)
     config->torrents = realloc(config->torrents, sizeof (*(config->torrents)) * (config->nb_torrent + POOL_TORRENT));

  memcpy (&config->torrents[config->nb_torrent++], &new_torrent, sizeof (str_torrent));

  return 0;

  error:
  T411_LOG (LOG_ERR, "Error during parsing torrent process on %d column", col);
  return 1;
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

int dump_config (str_t411_config* config)
{
  FILE* fd;
  int index;
  char torrent_info[256];
  char type;

  fd = fopen (CONF_FILE, "w+");

  if (fd == NULL)
  {
    perror ("fopen");
    T411_LOG (LOG_ERR, "Not able to open %s !", CONF_FILE);
    return 1;
  }

  fwrite ("username ", 9, 1, fd); fwrite (config->username, strlen(config->username), 1, fd);
  fwrite ("\n", 1, 1, fd);
  fwrite ("password ", 9, 1, fd); fwrite (config->password, strlen(config->password), 1, fd);
  fwrite ("\n", 1, 1, fd);
  fwrite ("mail ", 5, 1, fd); fwrite (config->mail, strlen(config->mail), 1, fd);
  fwrite ("\n", 1, 1, fd);

  for (index = 0; index < config->nb_torrent; index++)
  {
    memset (torrent_info, 0, 256);
    if (config->torrents[index].type == ANIMATION)
      type = 'A';
    else
      type = 'S';
    sprintf (torrent_info, "T %c %s %d %d\n", type, config->torrents[index].name, config->torrents[index].season, config->torrents[index].episode);
    fwrite (torrent_info, strlen(torrent_info), 1, fd);
  }

  fclose (fd);
  T411_LOG (LOG_DEBUG, "Dump config ... Done.\n");

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
  FILE* fd;

  if (!is_existing_conf ())
    return 1;

  fd = fopen (CONF_FILE, "r+");

  if (fd == NULL)
  {
    perror ("fopen");
    T411_LOG (LOG_ERR, "Not able to open %s !", CONF_FILE);
    return 1;
  }

  T411_LOG (LOG_DEBUG, "read config file...\n");

  while (fgets(line, SIZE, fd) != NULL)
  {
    ltrim (line);
    if (line[0] == 'T')
    {
      if (extract_torrent_data (line, config) != 0)
	return 1;
      continue;
    }

    if (line[0] == '#')
    {
      T411_LOG (LOG_DEBUG, "Comment line detected in config file : %s\n", line);
      continue;
    }

    /* truncate endline */
    key = strtok (line, "\t \n");
    if (!key) continue;
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
	fclose (fd);
	return 1;
      }
      strcpy (config->mail, data);
    }
    else
    {
      T411_LOG (LOG_DEBUG, "Unknow key and data in config file : |%s|%s|\n", key, data);
      fclose (fd);
      return 1;
    }
  }

  T411_LOG (LOG_DEBUG, "username : |%s|\n", config->username);
  T411_LOG (LOG_DEBUG, "password : |%s|\n", config->password);
  T411_LOG (LOG_DEBUG, "mail : |%s|\n", config->mail);

  if (!config->password[0] || !config->username[0])
  {
    T411_LOG (LOG_ERR, "Not able to get username/password from  %s !", CONF_FILE);
    fclose (fd);
    return 1;
  }

  if (!config->mail[0])
  {
    T411_LOG (LOG_ERR, "Not able to get mail info from  %s !", CONF_FILE);
    fclose (fd);
    return 1;
  }

  fclose (fd);
  return 0;
}
