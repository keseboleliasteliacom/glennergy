/**
 * @file fetcher.cpp
 * @brief Implementation of HTTP fetcher.
 *
 * @ingroup MeteoCppModule
 */

#include "fetcher.hpp"
#include <curl/curl.h>

namespace glennergy {

//curl_guard runs only at creation and deletion
/**
 * @brief RAII guard for global curl initialization.
 *
 * @note Memory ownership:
 * - Static lifetime object
 * - Initializes libcurl globally once
 *
 * @warning Relies on static initialization order
 */
struct curl_guard {
    curl_guard()  { curl_global_init(CURL_GLOBAL_DEFAULT); }
    ~curl_guard() { curl_global_cleanup(); }
};

static curl_guard global_curl;

/**
 * @brief Construct fetcher instance.
 */
fetcher::fetcher()
    : curl_(curl_easy_init(), curl_easy_cleanup) {}

/**
 * @brief Write callback for libcurl.
 */
size_t fetcher::write_cb(char *data, size_t size, size_t nmemb, std::string *out) {
    out->append(data, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Perform HTTP GET request.
 */
std::string fetcher::get(const std::string &url) {
    std::string response;

    if (curl_) {
        curl_easy_setopt(curl_.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_.get(), CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl_.get(), CURLOPT_WRITEDATA, &response);
        curl_easy_perform(curl_.get());
    }

    // Suggestion: Check curl_easy_perform return value for error handling

    return response;
}

} // namespace glennergy