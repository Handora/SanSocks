/*
#include "cipher/cipher.h"
#include <boost/asio.hpp>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace server {
  
  class Server
  {
  public:
    Server(const std::string& path);
    ~Server();
    
  private: 
    void ReadConfig();
    std::string config_path_;
    std::string port_;
    std::string base64_table_code_;
    std::string path_;
    boost::asio::io_service io_service_;
    boost::asio::signal_set signals_;
    
  };

} // namespace server

namespace server {
  
  Server::Server(const std::string& path)
    : path_(path),
      signals_(io_service_) {
    using namespace boost::asio;
    
  }

  void Server::ReadConfig() {
    using boost::property_tree::ptree;
    using boost::property_tree::read_json;
    ptree config;
    read_json(config_path_, config);
    port_ = std::move(config.get<std::string>("port"));
    base64_table_code_ = std::move(config.get<std::string>("code"));

    return ;
  }
} // namespace server
*/