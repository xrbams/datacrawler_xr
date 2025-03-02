#include "storage.hpp"

mongocxx::instance Database::instance{};
mongocxx::client Database::conn{ mongocxx::uri{"mongodb+srv://xrbams:LmVniuYFmyMpVbGF@xrbams.ni6ob.mongodb.net/?retryWrites=true&w=majority&appName=xrbams"} };

Database::Database() : db(conn["datacrawler_xr"]) {}

void Database::saveData(const json& data){
    try{

        auto collection = db["crawled_data"];
        bsoncxx::document::value doc = bsoncxx::from_json(data.dump());
        collection.insert_one(doc.view());

    }catch(const std::exception& e){
        std::cerr << "MongoDB Error (saveData): " << e.what() << '\n';
    }
    
}

void Database::saveLink(const std::string& url, const std::string& parentUrl) {
    try
    {
        auto collection = db["crawled_links"];
        json linkData = {
            {"url", url},
            {"parent_url", parentUrl},
            {"discovered_at", Downloader::getCurrentTimestamp()}
        };
        bsoncxx::document::value doc = bsoncxx::from_json(linkData.dump());
        collection.insert_one(doc.view());
    }catch(const std::exception& e){
        std::cerr << "MongoDB Error (saveLink): " << e.what() << '\n';
    }
}




/*
void getstorage() {
    try
    {
        // Create an instance.
        mongocxx::instance inst{};
        const auto uri = mongocxx::uri{"mongodb+srv://xrbams:LmVniuYFmyMpVbGF@xrbams.ni6ob.mongodb.net/?retryWrites=true&w=majority&appName=xrbams"};
        // Set the version of the Stable API on the client
        mongocxx::options::client client_options;
        const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
        client_options.server_api_opts(api);
        // Setup the connection and get a handle on the "admin" database.
        mongocxx::client conn{ uri, client_options };
        mongocxx::database db = conn["admin"];
        // Ping the database.
        const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
        db.run_command(ping_cmd.view());
        std::cout << "Pinged your deployment. You successfully connected to MongoDB!" << std::endl;
    }
    catch (const std::exception& e)
    {
        // Handle errors
        std::cout<< "Exception: " << e.what() << std::endl;
    }

} */