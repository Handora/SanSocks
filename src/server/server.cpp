/**
 * server.cpp
 * @author: Handora
 */

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
    ~Server();
    
  private: 
    void ReadConfig();
    void HandleConnection(std::shared_ptr<TCP::socket> );
    
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

    // open the thread for accepting from the client and establish a
    // connection
    boost::thread accept_thread([&]() {
	// TODO(Handora): How to handle the deletor for socket
	auto client_sock_ptr = std::make_shared<TCP::socket>(new TCP::socket(ios_));
	acceptor_ptr->accept(*client_sock_ptr);
	boost::thread connect_thread(&Server::HandleConnection, this, client_sock_ptr);
	connect_thread.detach();
      });
    accept_thread.detach();
  }

  void Server::HandleConnection(std::shared_ptr<TCP::socket> client_sock_ptr) {

    boost::system::error_code err;
    std::string data;
    data.resize(1024);

    // 
    // The first phase
    //
    size_t sz = client_sock_ptr->read_some(boost::asio::buffer(data), err);
    data = cipher_ptr_->Encode(data);
    if (err || sz < 3) {
      BOOST_LOG_TRIVIAL(error) << "read first phase error hanpened " << err.message();
      return ;
    }
    
    if (data[0] != 0x5) {
      BOOST_LOG_TRIVIAL(error) << "first phase structue error hanpened " << err.message();
    }

    client_sock_ptr->write_some(cipher_ptr_->Decode(std::string(0x5, 0x0)), err);
    if (err) {
      BOOST_LOG_TRIVIAL(error) << "write first phase error hanpened " << err.message();
      return ;
    }

    // The second phase
    sz = client_sock_ptr->read_some(boost::asio::buffer(data), err); 
    
    data = cipher_ptr_->Encode(data);
    if (sz || sz < 7) {
      BOOST_LOG_TRIVIAL(error) << "read second phase error hanpened " << err.message();
      return ;
    }
    if (data[0] != 0x5 || data[1] !=0x1) {
      BOOST_LOG_TRIVIAL(error) << "second phase structure error hanpened " << err.message();
      return ;
    }

    std::string dst_ip;
    std::string dst_port;
    switch (data[3]) {
    case 0x01:
      // TODO(Handora): ipv4 len
      dst_ip = std::string(dst_ip.begin()+4, dst_ip.begin()+8);

    case 0x03:
      // TCP::resolver
      
    case 0x04:
      // TODO(Handora): ipv6 len
      dst_ip = std::string(dst_ip.begin()+4, dst_ip.begin()+20);
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
