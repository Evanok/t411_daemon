#include "t411_daemon.h"
#include "config.h"
#include "message.h"

#include <sys/file.h>

/**
 * \fn void signal_handler(int sig)
 * \brief Handle signal received by t411 daemon
 *
 * \param sig Integer which identify the signal
 * \return void
 */
void signal_handler(int sig)
{
  switch(sig)
  {
    case SIGTERM:
      /* finalize the server */
      T411_LOG (LOG_INFO, "Daemon terminated by %d.", sig);
      closelog();
      exit(EXIT_SUCCESS);
      break;
  }
}

/**
 * \fn static void singleton (void)
 * \brief Allow to be sure that only one occurence of this daemon is running
 *
 * \param void
 * \return void
 */
static void singleton (void)
{
  int fd;
  char str[32];

  fd = open(FILE_LOCK, O_RDWR | O_CREAT, 0640);
  if (fd < 0)
  {
    T411_LOG (LOG_ERR, "Cannot open %s", FILE_LOCK);
    exit (1);
  }
  if (flock(fd, LOCK_EX | LOCK_NB) < 0)
  {
    T411_LOG (LOG_INFO, "Cannot lock %s, only one instance of t411 daemon can be run", FILE_LOCK);
    exit (1);
  }
  sprintf(str, "%d\n", getpid());
  if (write(fd, str, strlen(str)) <= 0)
  {
    T411_LOG (LOG_INFO, "Cannot write in %s", FILE_LOCK);
    exit (1);
  }
  T411_LOG (LOG_DEBUG, "Check on singleton : Success.");
}

/**
 * \fn static void daemonize (char* name)
 * \brief This function handle all the process to turn on my process as linux daemon
 *
 * \param name Name of the current process
 * \return void
 */
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
  signal(SIGKILL, signal_handler);
  signal(SIGTERM, signal_handler);

  pid = fork();

  if (pid < 0)
    exit(EXIT_FAILURE);

  if (pid > 0)
    exit(EXIT_SUCCESS);

  umask(027);

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

  /* Check that we are running a single copy thanks to file locking feature */
  singleton ();

  daemonize (argv[0]);
  T411_LOG (LOG_INFO, "%s daemon started.", argv[0]);

  /* init */
  memset (&config, 0, sizeof(str_t411_config));

  err = read_config (&config);
  if (err) goto error;

  /* DEBUG */
  /*
  dump_torrent (config);
  */

  T411_LOG (LOG_DEBUG, "<p class=\"error textcenter\">Aucun R&#233;sultat Aucun<br/> .torrent n'a encore\n");

  /* global init for curl*/
  curl_global_init(CURL_GLOBAL_ALL);

  err = get_authentification (&config);
  if (err) goto error;

  T411_LOG (LOG_INFO, "%s is running...", argv[0]);

  /* The Big Loop */
  while (1)
  {
    looking_for_torrent(&config);
    return 0;
    T411_LOG (LOG_INFO, "Next pooling in %d ...", LOOP_POOLING);
    sleep(LOOP_POOLING); /* wait 30 seconds */
  }


  error:
  T411_LOG (LOG_INFO, "%s daemon terminated.", argv[0]);
  closelog();

  T411_LOG (LOG_INFO, "Process return code : %d", err);

  return err;
}
