/**
 * server.cpp
 * @author: Handora
 */

#include "server/server.h"

namespace sansocks {
  
  Server::Server(const std::string& path)
    : config_path_(path),
      signals_(io_service_) {
    // read from json 
    ReadConfig();
    
    auto acceptor_ptr
      = std::make_shared<TCP::acceptor>(io_service_,
					TCP::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port_));

    boost::thread accept_thread([&]() {
	// open the thread for accepting from
	// the client and establish a connection
	for ( ; ; ) {
	  auto client_sock_ptr = std::make_shared<TCP::socket>(io_service_);
	  acceptor_ptr->accept(*client_sock_ptr);
	  boost::thread connect_thread(&Server::HandleConnection, this, client_sock_ptr);
	  connect_thread.detach();
	}
      });
    accept_thread.join();
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

    client_sock_ptr->write_some(boost::asio::buffer(cipher_ptr_->Decode(std::string(0x5, 0x0))), err);
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
      dst_ip = std::string(data.begin()+4, data.begin()+8);
      break;
      
    case 0x03:
      // TCP::resolver
    {
      TCP::resolver resolver(io_service_);
    TCP::resolver::query query(std::string(data.begin() + 5, data.end() - 2), "");
      auto address_iterator = resolver.resolve(query);
      dst_ip = (*address_iterator.begin()).host_name();
      break;
    }
      
    case 0x04:
      // TODO(Handora): ipv6 len
      dst_ip = std::string(dst_ip.begin()+4, dst_ip.begin()+20);
      break;
    }

    dst_port = std::string(dst_ip.end()-2, dst_ip.end());
    
    auto dst_sock_ptr = std::make_shared<TCP::socket>(io_service_);
    dst_sock_ptr->connect(TCP::endpoint(boost::asio::ip::address::from_string(dst_ip), stoi(dst_ip)));

    data.resize(1024);

    boost::thread com_thread([&]() {
	std::string data;
	data.resize(1024);
	boost::system::error_code err;
	for ( ; ; ) {
	  size_t sz = dst_sock_ptr->read_some(boost::asio::buffer(data), err);
	  if (err) {
	    if (err != boost::asio::error::eof) {
	      BOOST_LOG_TRIVIAL(error) << "read from client failed " << err.message(); 
	    } 
	    break;
	  }

	  if (sz == 0) {
	    break;
	  }

	  client_sock_ptr->write_some(boost::asio::buffer(cipher_ptr_->Decode(data), sz), err);
	  if (err) {
	    BOOST_LOG_TRIVIAL(error) << "send to dst failed " << err.message();
	    break;
	  }
	}
      });
    
    for ( ; ; ) {
      size_t sz = client_sock_ptr->read_some(boost::asio::buffer(data), err);
      if (err) {
	if (err != boost::asio::error::eof) {
	  BOOST_LOG_TRIVIAL(error) << "read from client failed " << err.message(); 
	} 
	break;
      }

      if (sz == 0) {
	break;
      }

      dst_sock_ptr->write_some(boost::asio::buffer(cipher_ptr_->Decode(data), sz), err);
      if (err) {
	BOOST_LOG_TRIVIAL(error) << "send to dst failed " << err.message();
	break;
      }
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

int main(int, char *[])
{
  using namespace sansocks;

  Server server("/home/handora/.sansocks-server.json");
  
  return 0;
}
