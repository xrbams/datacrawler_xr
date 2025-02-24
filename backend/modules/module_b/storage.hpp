// Module B: Storage

/*
- Receive parsed text & metadata from Module A.
- Store in a MongoDB.
- Enable fast querying & retrieval for later analysis.
- Data Serialization: Protocol Buffers or JSON (RapidJSON).
- Thread Safety: Use mutex for thread-safe writes.

*/

#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <iostream>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace std;

void getstorage(); 

#endif