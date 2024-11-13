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

using json = nlohmann::json;

namespace klee {

class ServerConnection {
public:
  std::string url;
  std::string UID;

public:
  void PostRequest(const std::vector<json> &metrics);
  void getUID();
};
} // namespace klee

#endif