/**
 * server
 */

#include "cipher/cipher.h"
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>

namespace sansocks {
  
  class Server
  {
    typedef boost::asio::ip::tcp TCP;
  public:
    Server(const std::string& path);
    ~Server();
    
  private: 
    void ReadConfig();
    std::string config_path_;
    int port_;
    std::string base64_table_code_;
    std::shared_ptr<Cipher> cipher_ptr_;
    boost::asio::io_service io_service_;
    boost::asio::signal_set signals_;
    boost::asio::io_service ios_;
  };

} // namespace sansocks

namespace sansocks {
  
  Server::Server(const std::string& path)
    : config_path_(path),
      signals_(io_service_) {
    using namespace boost::asio;
    ReadConfig();
    
    auto acceptor_ptr
      = std::make_shared<TCP::acceptor>(ios_,
					TCP::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port_));

    for (;;) {
      auto browser_sock_ptr 
	}
  }

  void Server::ReadConfig() {
    using boost::property_tree::ptree;
    using boost::property_tree::read_json;
    ptree config;
    read_json(config_path_, config);
    port_ = std::move(config.get<int>("port")); 
    cipher_ptr_ = std::make_shared<Cipher>(std::move(config.get<std::string>("code")));

    return ;
  }
} // namespace sansocks
