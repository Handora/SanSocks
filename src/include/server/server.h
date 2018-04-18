#pragma once

#include "cipher/cipher.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

namespace sansocks {
  
  class Server
  {
    typedef boost::asio::ip::tcp TCP;
  public:
    Server(const std::string& path);
    ~Server() {};
    
  private: 
    void ReadConfig();
    void HandleConnection(std::shared_ptr<TCP::socket> );
    
    std::string config_path_;
    int port_;
    std::string base64_table_code_;
    std::shared_ptr<Cipher> cipher_ptr_;
    boost::asio::io_service io_service_;
    boost::asio::signal_set signals_; 
  };

} // namespace sansocks

