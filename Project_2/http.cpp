#include "http.h"

#include <iostream>
#include <curl/curl.h>

static size_t curlWriteCb(char* ptr, size_t size, size_t nmemb, std::string* out) {
    out->append(ptr, size * nmemb);
    return size * nmemb;
}

std::optional<std::string> httpGet(const std::string& url, long timeout) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[HTTP] curl_easy_init() failed\n";
        return std::nullopt;
    }

    std::string body;
    curl_easy_setopt(curl, CURLOPT_URL,           url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,     &body);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,       timeout);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "[HTTP Error] " << curl_easy_strerror(res)
                  << " — URL: " << url.substr(0, 80) << "...\n";
        return std::nullopt;
    }
    return body;
}
