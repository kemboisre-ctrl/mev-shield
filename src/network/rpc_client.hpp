#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#pragma once
#include <curl/curl.h>
#include <string>
#include <rapidjson/document.h>
#include "common/logger.hpp"

namespace mev_shield {

class SimpleRPCClient {
public:
    SimpleRPCClient(const std::string& http_url) : http_url_(http_url) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }
    
    ~SimpleRPCClient() {
        curl_global_cleanup();
    }
    
    rapidjson::Document get_transaction(const std::string& tx_hash) {
        std::string params = R"([")" + tx_hash + R"("])";
        std::string response = json_rpc_call("eth_getTransactionByHash", params);
        
        rapidjson::Document doc;
        doc.Parse(response.c_str());
        return doc;
    }
    
    std::string get_gas_price() {
        std::string response = json_rpc_call("eth_gasPrice");
        
        rapidjson::Document doc;
        doc.Parse(response.c_str());
        if (doc.HasMember("result") && doc["result"].IsString()) {
            return doc["result"].GetString();
        }
        return "0x0";
    }

private:
    std::string http_url_;
    
    std::string json_rpc_call(const std::string& method, const std::string& params = "[]") {
        rapidjson::Document request;
        request.SetObject();
        rapidjson::Document::AllocatorType& allocator = request.GetAllocator();
        
        request.AddMember("jsonrpc", "2.0", allocator);
        request.AddMember("method", rapidjson::Value(method.c_str(), allocator), allocator);
        request.AddMember("id", 1, allocator);
        
        rapidjson::Document params_doc;
        params_doc.Parse(params.c_str());
        if (!params_doc.HasParseError()) {
            request.AddMember("params", params_doc, allocator);
        }
        
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        request.Accept(writer);
        
        return http_post(buffer.GetString());
    }
    
    std::string http_post(const std::string& data) {
        CURL* curl = curl_easy_init();
        std::string response;
        
        if (!curl) {
            return "{}";
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, http_url_.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
        
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        
        CURLcode res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            response = "{\"error\": \"" + std::string(curl_easy_strerror(res)) + "\"}";
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        
        return response;
    }
    
    static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* response) {
        size_t total_size = size * nmemb;
        response->append(static_cast<char*>(contents), total_size);
        return total_size;
    }
};

} // namespace mev_shield