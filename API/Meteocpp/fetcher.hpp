/**
 * @file fetcher.hpp
 * @brief HTTP fetcher wrapper using libcurl.
 *
 * @defgroup MeteoCppModule MeteoCpp Module
 *
 * @details
 * Provides a minimal RAII-based wrapper around libcurl for performing HTTP GET requests.
 */

#pragma once

#include <memory>
#include <string>

typedef void CURL;

namespace glennergy {

/**
 * @brief Custom deleter for CURL handles.
 *
 * @note Memory ownership:
 * - Used by std::unique_ptr to ensure proper cleanup of CURL resources.
 */
using curl_deleter = void(*)(CURL *);

/**
 * @brief Unique pointer wrapper for CURL handle.
 *
 * @note Memory ownership:
 * - Owns the CURL handle exclusively.
 * - Automatically cleaned up via curl_easy_cleanup.
 */
using curl_ptr = std::unique_ptr<CURL, curl_deleter>;

/**
 * @brief HTTP fetcher class using libcurl.
 *
 * @note Memory ownership:
 * - Owns CURL handle via RAII
 * - No manual cleanup required by caller
 */
class fetcher {
public:
    /**
     * @brief Construct a fetcher instance.
     *
     * @post CURL handle initialized if possible
     */
    fetcher();

    /**
     * @brief Perform HTTP GET request.
     *
     * @param[in] url Target URL
     * @return Response body as string (empty if failure)
     *
     * @pre url must not be empty
     * @post Returns raw response data
     *
     * @warning No explicit error handling (empty string indicates failure)
     * @note Blocking network call
     */
    std::string get(const std::string &url);

private:
    /**
     * @brief libcurl write callback.
     *
     * @param[in] data Incoming data buffer
     * @param[in] size Size of each element
     * @param[in] nmemb Number of elements
     * @param[out] out Output string buffer
     *
     * @return Number of bytes processed
     *
     * @note Appends raw response data to string
     */
    static size_t write_cb(char *data, size_t size, size_t nmemb, std::string *out);

    curl_ptr curl_; /**< CURL handle */
};

} // namespace glennergy