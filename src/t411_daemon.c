#include "t411_daemon.h"
#include "config.h"
#include "message.h"

void signal_handler(int sig)
{
  switch(sig)
  {
    case SIGTERM:
    case SIGKILL:
      /* finalize the server */
      syslog (LOG_INFO, "Daemon terminated by %d.", sig);
      closelog();
      exit(EXIT_SUCCESS);
      break;
  }
}

static void daemonize (char* name)
{
  pid_t pid;

  pid = fork();

  if (pid < 0)
    exit(EXIT_FAILURE);

  if (pid > 0)
    exit(EXIT_SUCCESS);

  if (setsid() < 0)
    exit(EXIT_FAILURE);

  //TODO: Implement a working signal handler */
  signal(SIGCHLD, SIG_IGN);
  signal(SIGHUP, SIG_IGN);
  signal(SIGTERM, signal_handler);
  signal(SIGKILL, signal_handler);

  pid = fork();

  if (pid < 0)
    exit(EXIT_FAILURE);

  if (pid > 0)
    exit(EXIT_SUCCESS);

  umask(0);

  /* Change the working directory to the root directory */
  /* or another appropriated directory */
  chdir(WORKING_DIR);

  /* Close all open file descriptors */
  if (!DEBUG)
  {
    for (int x = sysconf(_SC_OPEN_MAX); x > 0; x--)
      close (x);
  }

  openlog (name, LOG_PID, LOG_DAEMON);
}

int main (int argc __attribute__((__unused__)), char* argv[])
{
  int err = 0;
  str_t411_config config;

  daemonize (argv[0]);
  syslog (LOG_INFO, "%s daemon started.", argv[0]);

  /* init */
  memset (&config, 0, sizeof(str_t411_config));

  err = read_config (&config);
  if (err) goto error;

  err = get_authentificaton (config.username, config.password);
  if (err) goto error;

  process_message ("auth", config.username, config.password);


  /* The Big Loop */
  while (1)
  {
    syslog (LOG_INFO, "%s is running...", argv[0]);
    sleep(30); /* wait 30 seconds */
  }


  error:
  syslog (LOG_INFO, "%s daemon terminated.", argv[0]);
  closelog();

  return err;
}
