#include "remote_seedbox.h"

/**
 * \fn int add_torrent_transmission (char* file_name)
 * \brief add torrent file to transmission seedbox
 *
 * \param file_name file that contain torrent data to add
 * \return int 0 if success else 1
 */
int add_torrent_transmission (char* file_name)
{
  file_name = file_name;

  T411_LOG (LOG_DEBUG, "New torrent added to transmission seedbox....\n");

  return 0;
}
