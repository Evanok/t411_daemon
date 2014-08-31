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

/*
int t411_auth (const char* user, const char* password)
{

}
*/

int process_message (char* url, char** arguments)
{
  CURL *curl;
  CURLcode res;
  struct MemoryStruct chunk;
  char* message =  malloc (sizeof(char) * (strlen(url) + strlen(T411_URL) + 1));

  arguments = arguments;

  chunk.memory = NULL; /* we expect realloc(NULL, size) to work */
  chunk.size = 0;      /* no data at this point */

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();

  sprintf (message, T411_URL, url);
  PRINT_DEBUG ("[DEBUG] message : |%s|\n", message);
  PRINT_DEBUG ("[DEBUG] arguments : |%s|\n", "username=Evanok&password=evangelius");

  if(curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, message);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT_SECONDS);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "username=Evanok&password=evangelius");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen("username=Evanok&password=evangelius"));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
    {
      if (chunk.memory)
	free(chunk.memory);
    }

    if (chunk.memory)
    {
      PRINT_DEBUG ("[DEBUG] answer : |%s|\n", chunk.memory);
    }

    curl_easy_cleanup(curl);
  }

  return 0;
}
