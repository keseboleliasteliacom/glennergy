#include "Fetcher.h"
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

int Curl_Initialize(CurlResponse *_Response)
{
    CurlResponse *response = (CurlResponse *)malloc(sizeof(CurlResponse));

    if (response == NULL)
    {
        return -1;
    }

    response->data = NULL;
    response->size = 0;

    return 0;
}

size_t Curl_WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t real_size = size * nmemb;
    CurlResponse *response = (CurlResponse *)userp;

    char *ptr = (char *)realloc(response->data, response->size + real_size + 1);
    if (ptr == NULL)
    {
        return 0;
    }

    response->data = ptr;
    memcpy(&(response->data[response->size]), contents, real_size);
    response->size += real_size;
    response->data[response->size] = '\0';

    return real_size;
}

int Curl_HTTPGet(CurlResponse *_Response, char *url)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl)
    {
        free(_Response->data);
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Curl_WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)_Response);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Curl_HTTPGet: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(_Response->data);
        _Response->data = NULL;
        free(_Response);
        _Response = NULL;
        curl_easy_cleanup(curl);
        return -2;
    }

    curl_easy_cleanup(curl);
    return 0;
}

void Curl_Dispose(CurlResponse *_Response)
{
    if (_Response == NULL)
        return;

    if (_Response->data != NULL)
        free(_Response->data);

    free(_Response);
    _Response = NULL;
}