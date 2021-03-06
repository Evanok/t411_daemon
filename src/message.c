#include "message.h"


/* Array to fetch episode number and index of episode in t411 database */
const int index_episode_tab[] = {0, 937, 938, 939, 940, 941, 942, 943, 944, 946, 947, 948, 949, 950, 951, 952, 954, 953, 955, 956, 957, 958, 959, 960, 961, 962, 963, 964, 965, 966, 967, 1088};

/**
 * \fn int t411_api_select_best_result (str_torrent_result* results, int nb_result)
 * \brief find in torrent list, the best torrent
 *
 * \param results list of torrent
 * \param nb_result number of torrent in the list of result
 * \return int return id of the best torrent else 0
 */
int t411_api_select_best_result (str_torrent_result* results, int nb_result)
{
  int id = 0;
  int index;
  int max_completed = 0;

  for (index = 0; index < nb_result; index++)
  {
    if (results[index].is_verified && results[index].size / O_TO_MO <= TORRENT_MAX_SIZE
	&& results[index].completed >= max_completed)
    {
      max_completed = results[index].completed;
      id = results[index].id;
    }
  }

  T411_LOG (LOG_DEBUG, "Best id is %d\n", id);

  return id;
}


/**
 * \fn int t411_api_download_torrent (int id, str_t411_config* config)
 * \brief Download torrent file from torrent id through t411 api
 *
 * \param id identification number of the torrent
 * \param config Structure that contains torrents info
 * \return int 0 if success else 1
 */
int t411_api_download_torrent (int id, str_t411_config* config)
{
  char url[256];
  char* answer = NULL;
  int fd;
  char name_tmp[32];

  sprintf (url, "%s%s%d", T411_API_URL, T411_API_DOWNLOAD_URL, id);
  answer = process_http_message (url, NULL, config->token);

  if (answer == NULL)
  {
    T411_LOG (LOG_ERR, "Torrent %d not found in t411 database\n", id);
    return 1;
  }

  memset (name_tmp, 0, 32);
  strncpy (name_tmp, "/tmp/t411-XXXXXX", 16);

  fd = mkstemp (name_tmp);

  if (fd < 1)
  {
    T411_LOG (LOG_ERR, "Error during the mkstemp creation... fd : %d\n", fd);
    return 1;
  }

  T411_LOG (LOG_DEBUG, "tmp name is %s\n", name_tmp);

  if (write (fd, answer, strlen(answer)) == -1)
  {
    T411_LOG (LOG_ERR, "Error during the write process in %s\n", name_tmp);
    return 1;
  }

  add_torrent_transmission (name_tmp);

  free (answer);
  unlink (name_tmp);
  return 0;
}

/**
 * \fn int sendmail(const char *to, const char *from, const char *subject, const char *message)
 * \brief Send message though email
 *
 * \param to Email address of the receiver
 * \param from Email address of the sender
 * \param subject Subject of the mail
 * \param message Message of the mail
 * \return int 0 if success else 1
 */
int sendmail(const char *to, const char *from, const char *subject, const char *message)
{
  int retval = 1;
  FILE *mailpipe = popen("/usr/lib/sendmail -t", "w");

  if (mailpipe != NULL)
  {
    fprintf(mailpipe, "To: %s\n", to);
    fprintf(mailpipe, "From: %s\n", from);
    fprintf(mailpipe, "Subject: %s\n\n", subject);
    fwrite(message, 1, strlen(message), mailpipe);
    fwrite(".\n", 1, 2, mailpipe);
    pclose(mailpipe);
    retval = 0;
  }
  else
  {
    T411_LOG (LOG_ERR, "Failed to invoke sendmail");
  }
  return retval;
}

/**
 * \fn static size_t extract_string_from_data (char* data, char* key, char* storage)
 * \brief Extract data that fetch key in http message from t411
 *
 * \param data Pointer on http data
 * \param key Key that we must use to find data in the message
 * \param storage String where we want to store data extrated from http message
 * \return size of extracted data. 0 in error case
 */
static size_t extract_string_from_data (char* data, char* key, char* storage)
{
  size_t len = strlen(data);
  char* token = NULL;
  char localdata[len + 1];
  int find_key = 0;

  strcpy (localdata, data);
  token = strtok (localdata, "\",{}");

  while (token)
  {
    if (find_key)
    {
      strcpy (storage, token);
      return strlen(token);
    }

    if (strncmp (token, key, strlen(key)) == 0)
    {
      token = strtok (NULL, "\",{}");
      if (!token)
	return 0;
      find_key = 1;
    }
    token = strtok (NULL, "\",{}");
  }

  T411_LOG (LOG_ERR, "Failed to find key : \"%s\" in http message", key);

  return 0;
}

/**
 * \fn static int is_digit (const char* s)
 * \brief Check taht all char on char array is a digit
 *
 * \param s char of array
 * \return 1 if char array contains only digit else 0
 */
static int is_digit (const char* s)
{
  while (*s && (*s >= 48 && *s <= 57))
    s++;

  if (!*s )
    return 1;
  return 0;
}

/**
 * \fn static size_t extract_int_from_data (char* data, char* key, int* storage)
 * \brief Extract data that fetch key in http message from t411
 *
 * \param data Pointer on http data
 * \param key Key that we must use to find data in the message
 * \param storage uint where we want to store data extrated from http message
 * \return 0 if success else 1
 */
static int extract_int_from_data (char* data, char* key, int* storage)
{
  size_t len = strlen(data);
  char* token = NULL;
  char localdata[len + 1];
  int find_key = 0;

  strcpy (localdata, data);
  token = strtok (localdata, "\",{}");

  while (token)
  {
    if (find_key)
    {
      if (!is_digit (token))
      {
	T411_LOG (LOG_ERR, "Error none digit found in token : %s\n", token);
	return 1;
      }
      *storage = atoi (token);
      return 0;
    }

    if (strncmp (token, key, strlen(key)) == 0)
    {
      token = strtok (NULL, "\",{}");
      if (!token)
	return 1;
      find_key = 1;
    }
    token = strtok (NULL, "\",{}");
  }

  T411_LOG (LOG_ERR, "Failed to find key : \"%s\" in http message", key);

  return 1;
}

/**
 * \fn static void* myrealloc (void *ptr, size_t size)
 * \brief Realloc pointer with size
 *
 * \param ptr Pointer to alloc/realloc
 * \param size Size that must use to alloc/realloc
 * \return New adress of the pointer
 */
static void* myrealloc (void *ptr, size_t size)
{
  if (ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

/**
 * \fn static size_t WriteMemoryCallback (void *ptr, size_t size, size_t nmemb, void *data)
 * \brief This function allow us to write in memory data from libcurl http messages
 *
 * \param ptr Pointer that contain data to write
 * \param size Size of member of the data
 * \param nmemb Number of member in the message
 * \param data Pointer when we want to write data
 * \return Number of bytes written
 */
static size_t WriteMemoryCallback (void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)data;

  mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory)
  {
    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }
  return realsize;
}

/**
 * \fn char* process_http_message (char* url, char* message, char* token)
 * \brief Generic function used to send data to t411 api
 *
 * \param url Url used to request http by libcurl
 * \param message Data sent to t411 api
 * \param token Token needed by t411 api for authorization http header
 * \return String that contains answer from t411 api
 */
char* process_http_message (char* url, char* message, char* token)
{
  CURLcode res;
  CURL *curl = NULL;
  char buf_error[512];
  struct MemoryStruct chunk;
  struct curl_slist *headerlist=NULL;
  char buf[256];

  memset (&chunk, 0, sizeof(chunk));

  curl = curl_easy_init();
  if (!curl)
  {
    T411_LOG (LOG_ERR, "Failed to initialiaze curl module");
    return NULL;
  }

  curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.125 Safari/537.36");
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, buf_error);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT_SECONDS);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);


  T411_LOG (LOG_DEBUG, "url : |%s|\n", url);

  if (token)
  {
    sprintf (buf, "Authorization: %s", token);
    headerlist = curl_slist_append(headerlist, buf);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  if (message)
  {
    T411_LOG (LOG_DEBUG, "message : |%s|\n", message);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(message));
  }

  res = curl_easy_perform(curl);

  curl_slist_free_all (headerlist);
  curl_easy_cleanup(curl);

  if(res != CURLE_OK)
  {
    T411_LOG (LOG_ERR, "result != CURLE_OK");
    T411_LOG (LOG_ERR, "Error : |%s|", buf_error);
    return NULL;
  }

  if (!chunk.memory)
  {
    T411_LOG (LOG_ERR, "chunk memory is NULL !");
    return NULL;
  }

  T411_LOG (LOG_DEBUG, "answer : |%s|\n", chunk.memory);
  return chunk.memory;
}

/**
 * \fn int t411_get_authentification (str_t411_config* config)
 * \brief Function used to get token from t411 api
 *
 * \param config Structure that contains username/password. We will also use it to store token info from t411
 * \return 0 on success else 1;
 */
int t411_get_authentification (str_t411_config* config)
{
  char message[256];
  char* answer = NULL;
  char* url = NULL;

  sprintf (message, "username=%s&password=%s", config->username, config->password);

  url = malloc (sizeof(char) * (strlen(T411_API_URL) + strlen(T411_API_AUTH_URL) + 1));
  sprintf (url, "%s%s", T411_API_URL, T411_API_AUTH_URL);

  answer = process_http_message (url, message, NULL);

  if (answer == NULL || strstr (answer, "error")) goto error;

  // if one of extraction return 0 I will also return to handle error.
  if (extract_string_from_data (answer, "uid", config->uuid) == 0 || extract_string_from_data (answer, "token", config->token) == 0)
    goto error;

  T411_LOG (LOG_DEBUG, "token : %s\n", config->token);


  /* test token */
  /*
    free (answer);
    answer = process_http_message ("http://www.t411.me/public/index.php?_url=/users/profile/94588399", NULL, config->token);
  */
  /* end test token*/

  free (answer);
  free (url);

  return 0;

  error:

  T411_LOG (LOG_ERR, "Error during authentification process");

  if (answer)
  {
    T411_LOG (LOG_ERR, "<%s>", answer);
    free (answer);
  }
  free (url);

  return 1;
}

/**
 * \fn int t411_html_search_torrent_from_config (str_t411_config* config)
 * \brief For each torrent of config structure, request the torrent though http search
 *
 * \param config Structure that contains torrents info
 * \return 0 on success else 1;
 */
int t411_html_search_torrent_from_config (str_t411_config* config)
{
  int index;
  char url[256];
  char* answer = NULL;

  for (index = 0; index < config->nb_torrent; index++)
  {
    memset (url, 0, 256);
    sprintf (url, T411_HTTP_URL, config->torrents[index].name, config->torrents[index].type, config->torrents[index].episode + INDEX_EPISODE, config->torrents[index].season + INDEX_SEASON, VOSTFR, config->torrents[index].name);

    answer = process_http_message (url, NULL, NULL);

    if (strstr (answer, "<title>503 Service Temporarily Unavailable</title>"))
    {
      T411_LOG (LOG_ERR, "t411 503 Service Temporarily Unavailable");
      return 1;
    }

    if (strstr (answer, "<title>503 Service Temporarily Unavailable</title>"))
    {
      T411_LOG (LOG_ERR, "t411 503 Service Temporarily Unavailable");
      return 1;
    }

    if (strstr (answer, "<p class=\"error textcenter\">Aucun R&#233;sultat Aucun<br/> .torrent n'a encore") == NULL)
    {
      T411_LOG (LOG_DEBUG, "Torrent found\n");
      t411_html_extract_torrent_info (answer, config);
    }
    else
    {
      T411_LOG (LOG_DEBUG, "Torrent not found\n");
    }

    sleep (5);
    free (answer);
  }

  return 0;
}

/**
 * \fn int t411_api_search_torrent_from_config (str_t411_config* config)
 * \brief For each torrent of config structure, request the torrent though http search
 *
 * \param config Structure that contains torrents info
 * \return 0 on success else 1;
 */
int t411_api_search_torrent_from_config (str_t411_config* config)
{
  int torrent_index = 0;
  char url[256];
  char* answer = NULL;
  int first_try = 1;

  while(torrent_index < config->nb_torrent)
  {
    memset (url, 0, 256);
    if (first_try)
    {
      sprintf (url, "%s%s%s&cid=%d&term[%d][]=%d&term[%d][]=%d&term[%d][]=%d", T411_API_URL, T411_API_TORRENT_SEARCH, config->torrents[torrent_index].name, config->torrents[torrent_index].type, TERM_LANGAGE, VOSTFR, TERM_SEASON, config->torrents[torrent_index].season + INDEX_SEASON, TERM_EPISODE, index_episode_tab[config->torrents[torrent_index].episode]);
      first_try = 0;
    }
    else
    {
      /* On second try, try to get next season info ! */
      sprintf (url, "%s%s%s&cid=%d&term[%d][]=%d&term[%d][]=%d&term[%d][]=%d", T411_API_URL, T411_API_TORRENT_SEARCH, config->torrents[torrent_index].name, config->torrents[torrent_index].type, TERM_LANGAGE, VOSTFR, TERM_SEASON, config->torrents[torrent_index].season + INDEX_SEASON + 1, TERM_EPISODE, INDEX_EPISODE + 1);
      first_try = 1;
      torrent_index++;
    }

    answer = process_http_message (url, NULL, config->token);

    if (answer == NULL)
    {
      T411_LOG (LOG_ERR, "t411 503 Service Temporarily Unavailable");
      return 1;
    }

    if (strstr (answer, "\"total\":0") == NULL)
    {
      T411_LOG (LOG_DEBUG, "Torrent found\n");
      int nb_result = 0;
      int result_index;
      str_torrent_result* results;
      int id;

      extract_int_from_data(answer, "total", &nb_result);
      T411_LOG (LOG_DEBUG, "nb_result : %d\n", nb_result);
      results = malloc (sizeof (*results) * nb_result);

      char* tmp = strstr (answer, "[");
      for (result_index = 0; tmp && result_index < nb_result; result_index++)
      {
	tmp = strstr (tmp, "{");
	if (!tmp)
	  break;
	extract_int_from_data (tmp, "id", &(results[result_index].id));
	extract_string_from_data (tmp, "name", results[result_index].name);
	extract_int_from_data (tmp, "seeders", &(results[result_index].seeders));
	extract_int_from_data (tmp, "leechers", &(results[result_index].leechers));
	extract_int_from_data (tmp, "size", &(results[result_index].size));
	extract_int_from_data (tmp, "times_completed", &(results[result_index].completed));
	extract_int_from_data (tmp, "isVerified", &(results[result_index].is_verified));
	tmp = strstr (tmp, "}");
	if (!tmp)
	  break;
      }
      DUMP_RESULT (results, nb_result);

      id = t411_api_select_best_result (results, nb_result);
      if (id)
	t411_api_download_torrent (id, config);

      free (results);

      /* torrent found we can try to get next ! */

      if (id && first_try)
      {
	config->torrents[torrent_index].episode++;
      }
      else if (id)
      {
	config->torrents[torrent_index].season++;
	config->torrents[torrent_index].episode = 2;;
      }

      first_try = 1;
      torrent_index++;
    }
    else
    {
      if (!first_try)
	T411_LOG (LOG_DEBUG, "Torrent not found\n");
    }
    sleep (5);
    free (answer);
  }

  return 0;
}


/**
 * \fn int t411_html_extract_torrent_info (char* data, str_t411_config* config)
 * \brief Extract torrent url and download torrent
 *
 * \param data array of char which contains html code from search result
 * \param config structure that will allow us to know token and update torrent info
 * \return 0 on success else 1;
 */
int t411_html_extract_torrent_info (char* data, str_t411_config* config)
{
  char* name_token = data;
  char* id_token = NULL;
  char download_url[256];
  char id[32];
  char* answer = NULL;

  name_token = strstr(data, T411_TORRENT_TOKEN);
  if (name_token == NULL)
    goto error;

  while (name_token)
  {
    id_token = strstr(name_token, T411_ID_TOKEN);
    if (id_token == NULL)
      goto error;

    name_token = strstr(id_token + strlen(T411_ID_TOKEN), "\"");

    if (name_token == NULL)
      goto error;

    strncpy (id, id_token + strlen(T411_ID_TOKEN), (name_token - (id_token + strlen(T411_ID_TOKEN))));
    T411_LOG (LOG_DEBUG, "Torrent id : |%s|\n", id);

    /* TODO THERE*/
    /* extract from html page info like size + number of person who download it */
    /* + link of torrent page */
    /* get url for download torrent */
    /* -> NOT MANDATORY WE CAN PERHAPS*/
    /* update episode + season number to get next ! */
    /* Must also add + 1 to episode id in config */

    sprintf (download_url, "%s%s%s", T411_API_URL, T411_API_GETDETAIL_URL, id);
    answer = process_http_message (download_url, NULL, config->token);

    T411_LOG (LOG_WARNING, "answer : |%s|\n", answer);

    name_token = strstr(name_token, T411_TORRENT_TOKEN);
  }

  return 0;

  error:
  T411_LOG (LOG_ERR, "Error during parsing of html result from t411 search page");
  return 1;
}
