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
 * \fn void* myrealloc (void *tr, size_t size)
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
static size_t
WriteMemoryCallback (void *ptr, size_t size, size_t nmemb, void *data)
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
 * \fn char* process_message (CURL *curl, char* url, char* message)
 * \brief Generic function used to send data to t411 api
 *
 * \param curl Structure which allow us to send message to t411
 * \param url Url used to send message to t411. This url contains the hostname of t411 server + name of function to call in t411 api
 * \param message Data sent to t411 api
 * \return String that contains answer from t411 api
 */
char* process_message (CURL *curl, char* url, char* message)
{
  CURLcode res;
  char buf_error[512];
  struct MemoryStruct chunk;
  char* full_url =  malloc (sizeof(char) * (strlen(url) + strlen(T411_URL) + 1));
  sprintf (full_url, T411_URL, url);

  chunk.memory = NULL; /* we expect realloc(NULL, size) to work */
  chunk.size = 0;      /* no data at this point */

  if (curl == NULL)
    curl = curl_easy_init();

  if (!curl)
  {
    T411_LOG (LOG_ERR, "Failed to initialiaze curl module");
    return NULL;
  }

  T411_LOG (LOG_DEBUG, "url : |%s|\n", full_url);
  T411_LOG (LOG_DEBUG, "message : |%s|\n", message);

  curl_easy_setopt(curl, CURLOPT_URL, full_url);
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, buf_error);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT_SECONDS);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(message));
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
 * \fn int get_authentification (CURL *curl, const char* username, const char* password)
 * \brief Function used to get token from t411 api
 *
 * \param curl Structure which allow us to send message to t411
 * \param config Structure that contain username/password. We will also use it to store token info from t411
 * \param password Password used to login on t411
 * \return 0 on success else 1;
 */
int get_authentification (CURL *curl, str_t411_config* config)
{
  char message[256];
  char* answer = NULL;

  sprintf (message, "username=%s&password=%s", config->username, config->password);
  answer = process_message (curl, "auth", message);

  if (strstr (answer, "error"))
  {
    T411_LOG (LOG_ERR, "Error during authentification process : %s\n", answer);
    return 1;
  }

  // if one of extraction return 0 I will also return to handle error.
  if (extract_data (answer, "uid", config->uuid) == 0 || extract_data (answer, "token", config->token) == 0)
    return 1;

  return 0;
}
