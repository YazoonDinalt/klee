//===-- ServerConnection.h---------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_SERVERCONNECTION_H
#define KLEE_SERVERCONNECTION_H

#include "nlohmann/json.hpp"
#include <curl/curl.h>

namespace klee {

class ServerConnection {

public:
  ServerConnection(const std::string &serverUrl);
  ~ServerConnection();
  void PostRequest(const std::vector<nlohmann::json> &metrics);
  void getUIDFromServer();
  std::string getUID();
  void setUID(const std::string &newUID);

private:
  std::string url;
  CURL *curl;
  std::string UID;
};
} // namespace klee

#endif
