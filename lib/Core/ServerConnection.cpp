//===-- ServerConnection.cpp-------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ServerConnection.h"
#include <curl/curl.h>
#include <iostream>

using namespace klee;

namespace klee {

void ServerConnection::PostRequest(const std::vector<nlohmann::json> &metrics) {
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  if (curl) {
    nlohmann::json jsonData = metrics;
    std::string jsonString = jsonData.dump();

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(
        curl, CURLOPT_WRITEFUNCTION,
        +[](void *, size_t size, size_t nmemb, void *) -> size_t {
          return size * nmemb;
        });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, nullptr);
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
};

size_t write_data(void *ptr, size_t size, size_t nmemb, void *userdata) {
  std::string *buffer = static_cast<std::string *>(userdata);
  buffer->append(static_cast<char *>(ptr), size * nmemb);
  return size * nmemb;
}

void ServerConnection::getUID() {

  CURL *curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/new-session");

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &UID);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    UID = "ServerNotValid";
    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
              << std::endl;
  }

  curl_easy_cleanup(curl);
}

} // namespace klee