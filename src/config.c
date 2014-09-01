#include "config.h"

int is_existing_conf (void)
{
  struct stat s;
  int err;

  err = stat(CONF_FILE, &s);
  if(-1 == err)
  {
    if(ENOENT == errno)
    {
      /* does not exist */
      syslog (LOG_ERR, "%s does not exist. You must define a configuration file !", CONF_FILE);
      return 0;
    }
    else
    {
      perror("stat");
      syslog (LOG_ERR, "%s perror", CONF_FILE);
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
    syslog (LOG_ERR, "Not able to open %s !", CONF_FILE);
    return -1;
  }

  PRINT_DEBUG ("[DEBUG] read config file...\n");

  while (fgets(line, SIZE, config->fd_config) != NULL)
  {
    /* truncate endline */
    line[strlen(line) - 1] = 0;
    if (strncmp (line, "username", 8) == 0)
      strcpy (config->username, line + 9);
    else if (strncmp (line, "password", 8) == 0)
      strcpy (config->password, line + 9);

    PRINT_DEBUG ("[DEBUG] line : |%s|\n", line);
  }

  PRINT_DEBUG ("[DEBUG] username : |%s|\n", config->username);
  PRINT_DEBUG ("[DEBUG] password : |%s|\n", config->password);

  return 0;
}
