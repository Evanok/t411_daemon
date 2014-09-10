#include "message.h"

/**
 * \fn static size_t extract_data (char* data, char* key, char* storage)
 * \brief Extrack data that fetch key in http message from t411
 *
 * \param data Pointer on http data
 * \param key Key that we must use to find data in the message
 * \param storage String where we want to store data extrated from http message
 * \return size of extracted data. 0 in error case
 */
static size_t extract_data (char* data, char* key, char* storage)
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
 * \fn static void* myrealloc (void *ptr, size_t size)
 * \brief Realloc pointer with size
 *
 * \param ptr Pointer to alloc/realloc
 * \param size Size that must use to alloc/realloc
 * \return New adress of the pointer
 */
static void* myrealloc (void *ptr, size_t size)
{
  if(ptr)
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
 * \fn char* process_message (CURL *curl, char* url, char* message, char* token)
 * \brief Generic function used to send data to t411 api
 *
 * \param url Url used to send message to t411. This url contains the hostname of t411 server + name of function to call in t411 api
 * \param message Data sent to t411 api
 * \param token Token needed by t411 api for authorization http header
 * \return String that contains answer from t411 api
 */
char* process_message (char* url, char* message, char* token)
{
  CURLcode res;
  static CURL *curl = NULL;
  char buf_error[512];
  struct MemoryStruct chunk;
  char* full_url =  malloc (sizeof(char) * (strlen(url) + strlen(T411_URL) + 1));
  struct curl_slist *headerlist=NULL;
  char buf[256];

  memset (&chunk, 0, sizeof(chunk));
  sprintf (full_url, T411_URL, url);

  if (curl == NULL)
  {
    curl = curl_easy_init();
    curl_global_init(CURL_GLOBAL_ALL);
  }

  if (!curl)
  {
    T411_LOG (LOG_ERR, "Failed to initialiaze curl module");
    return NULL;
  }

  T411_LOG (LOG_DEBUG, "url : |%s|\n", full_url);
  if (message)
    T411_LOG (LOG_DEBUG, "message : |%s|\n", message);

  if (token)
  {
    sprintf (buf, "Authorization: %s", token);
    headerlist = curl_slist_append(headerlist, buf);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
  }

  curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.125 Safari/537.36");
  curl_easy_setopt(curl, CURLOPT_URL, full_url);
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, buf_error);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT_SECONDS);
  if (message)
  {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(message));
  }
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

  res = curl_easy_perform(curl);
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
 * \fn int get_authentification (CURL *curl, str_t411_config* config)
 * \brief Function used to get token from t411 api
 *
 * \param config Structure that contains username/password. We will also use it to store token info from t411
 * \return 0 on success else 1;
 */
int get_authentification (str_t411_config* config)
{
  char message[256];
  char* answer = NULL;

  sprintf (message, "username=%s&password=%s", config->username, config->password);
  answer = process_message ("auth", message, NULL);

  if (strstr (answer, "error"))
  {
    T411_LOG (LOG_ERR, "Error during authentification process : %s\n", answer);
    return 1;
  }

  // if one of extraction return 0 I will also return to handle error.
  if (extract_data (answer, "uid", config->uuid) == 0 || extract_data (answer, "token", config->token) == 0)
    return 1;

  T411_LOG (LOG_DEBUG, "token : %s\n", config->token);


  /* test token */
  /*
    memset(message, 0, 256);
    sprintf (message, "users/profile/%s", config->uuid);
    answer = process_message (message, NULL, config->token);
  */
  /* end test token*/

  return 0;
}

int looking_for_torrent (str_t411_config* config)
{
  int index;
  CURLcode res;
  static CURL *curl = NULL;
  char buf_error[512];
  struct MemoryStruct chunk;
  char url[256];

  if (curl == NULL)
  {
    curl = curl_easy_init();
    curl_global_init(CURL_GLOBAL_ALL);
  }

  if (!curl)
  {
    T411_LOG (LOG_ERR, "Failed to initialiaze curl module");
    return 1;
  }

  for (index = 0; index < config->nb_torrent; index++)
  {
    memset (&chunk, 0, sizeof(chunk));
    memset (url, 0, 256);
    sprintf (url, T411_HTTP_URL, config->torrents[index].name, config->torrents[index].type, config->torrents[index].episode + INDEX_EPISODE, config->torrents[index].season + INDEX_SEASON, VOSTFR, config->torrents[index].name);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.125 Safari/537.36");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, buf_error);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT_SECONDS);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
      T411_LOG (LOG_ERR, "result != CURLE_OK");
      T411_LOG (LOG_ERR, "Error : |%s|", buf_error);
      return 1;
    }

    if (!chunk.memory)
    {
      T411_LOG (LOG_ERR, "chunk memory is NULL !");
      return 1;
    }

    if (strstr (chunk.memory, "<title>503 Service Temporarily Unavailable</title>"))
    {
      T411_LOG (LOG_ERR, "t411 503 Service Temporarily Unavailable");
      return 1;
    }

    if (strstr (chunk.memory, "<title>503 Service Temporarily Unavailable</title>"))
    {
      T411_LOG (LOG_ERR, "t411 503 Service Temporarily Unavailable");
      return 1;
    }


    //T411_LOG (LOG_DEBUG, "answer : |%s|\n", chunk.memory);

    if (strstr (chunk.memory, "<p class=\"error textcenter\">Aucun R&#233;sultat Aucun<br/> .torrent n'a encore") != NULL)
      T411_LOG (LOG_DEBUG, "Torrent found\n");
    else
      T411_LOG (LOG_DEBUG, "Torrent not found\n");
  }

  return 0;
}

