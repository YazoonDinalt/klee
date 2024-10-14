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
#include <string>
#include <vector>

namespace klee {

/// TimingSolver - A simple class that collects delta statistics
    class ServerConnection {
        public:
          std::string url;

        public:
          void PostRequest(const std::vector<nlohmann::json> &metrics);
    };
} // namespace klee

#endif