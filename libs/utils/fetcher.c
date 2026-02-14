#include "fetcher.h"
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

// Förr hade vi att Init skapar en malloc CurlResponse och returnerar pekaren.
// Kan inte detta ligga på stacken istället?
// Nu har vi att Init tar en pekare till CurlResponse och initierar den.
// Det betyder att anroparen måste skapa CurlResponse på stacke eller heapen själv, sedan kalla denna för att initiera.
int Curl_Initialize(CurlResponse *_Response)
{
    if (!_Response)
    {
        return -1;
    }

    _Response->data = NULL;
    _Response->size = 0;

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
    CURL *curl = curl_easy_init();
    
    if (!curl)
    {
        free(_Response->data);
        return -1;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Curl_WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, _Response);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)_Response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Curl_HTTPGet: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return -2;
    }

    long http_code = 0;
    // Hämta HTTP-responskoden
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200)
    {
        fprintf(stderr, "Curl_HTTPGet: HTTP request failed with code %ld\n", http_code);
        curl_easy_cleanup(curl);
        return -3;
    }

    curl_easy_cleanup(curl);
    return 0;
}

void Curl_Dispose(CurlResponse *_Response)
{
    if (!_Response)
        return;

    if (_Response->data != NULL)
        free(_Response->data);

    //free(_Response->data);
    _Response->data = NULL;
    _Response->size = 0;
}