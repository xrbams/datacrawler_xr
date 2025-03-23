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
#include <mongocxx/uri.hpp>
#include <mongocxx/pool.hpp>
#include <nlohmann/json.hpp>
#include <mutex>

#include "../module_a/downloader.hpp"

using namespace std;
using json = nlohmann::json;
using namespace mongocxx;


// void getstorage();
extern mongocxx::pool mongo_pool;

class Database
{
private:
    static mongocxx::instance instance;  // ✅ Singleton MongoDB instance
    // static mongocxx::client conn;        // ✅ Persistent client connection
    mongocxx::pool::entry conn = mongo_pool.acquire();
    mongocxx::database db;
public:
    Database();
    ~Database()  = default;

    void saveData(const json& data);
    void saveLink(const std::string& url, const std::string& parentUrl);
};



#endif