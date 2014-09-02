#include "message.h"

static void *myrealloc(void *ptr, size_t size)
{
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

static size_t
WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
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


int get_authentification (CURL *curl, const char* username, const char* password)
{
  char message[256];

  sprintf (message, "username=%s&password=%s", username, password);
  process_message (curl, "auth", message);

  return 0;
}
