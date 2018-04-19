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
    
    boost::thread accept_thread([&]() {
	// open the thread for accepting from
	// the client and establish a connection
	auto acceptor_ptr
	  = std::make_shared<TCP::acceptor>(io_service_,
					    TCP::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port_));

	acceptor_ptr->set_option(TCP::acceptor::reuse_address(true));
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
    data = cipher_ptr_->Decode(data);
    if (err || sz < 3) {
      BOOST_LOG_TRIVIAL(error) << "size: " << sz << std::endl;
      BOOST_LOG_TRIVIAL(error) << "read first phase error hanpened " << err.message();
      return ;
    }
    
    if (data[0] != 0x5) {
      BOOST_LOG_TRIVIAL(error) << "data[0] is ";
      printf("%d\n", BYTE(data[0]));
      BOOST_LOG_TRIVIAL(error) << "first phase structue error hanpened ";
      return ;
    }

    client_sock_ptr->write_some(boost::asio::buffer(cipher_ptr_->Encode(std::string{0x5, 0x0}), 2), err);
    if (err) {
      BOOST_LOG_TRIVIAL(error) << "write first phase error hanpened " << err.message();
      return ;
    }

    // The second phase
    sz = client_sock_ptr->read_some(boost::asio::buffer(data), err); 
    
    data = cipher_ptr_->Decode(data);
    if (err || sz < 7) {
      BOOST_LOG_TRIVIAL(error) << "size: " << sz << std::endl;
      BOOST_LOG_TRIVIAL(error) << "read second phase error hanpened " << err.message();
      return ;
    }
    if (data[0] != 0x5 || data[1] !=0x1) {
      BOOST_LOG_TRIVIAL(error) << "second phase structure error hanpened " << err.message();
      return ;
    }

    std::string dst_ip; 
    switch (data[3]) {
      
    case 0x01:
      // TODO(Handora): ipv4 len 
      
      dst_ip.append(std::to_string(static_cast<uint8_t>(data[4])));
      dst_ip.push_back('.');
      dst_ip.append(std::to_string(static_cast<uint8_t>(data[5]))); 
      dst_ip.push_back('.');
      dst_ip.append(std::to_string(static_cast<uint8_t>(data[6]))); 
      dst_ip.push_back('.');
      dst_ip.append(std::to_string(static_cast<uint8_t>(data[7]))); 
      break;
    case 0x03:
      // TCP::resolver
    {
      TCP::resolver resolver(io_service_);
      TCP::resolver::query query(std::string(data.begin() + 5, data.begin() + sz - 2), ""); 
      auto address_iterator = resolver.resolve(query);
      dst_ip = (*address_iterator.begin()).endpoint().address().to_string(); 
      break;
    }
      
    case 0x04:
      // TODO(Handora): ipv6 len
      char ipv6_str[100];
      sprintf(ipv6_str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
	      data[4], data[5],
	      data[6], data[7],
	      data[8], data[9],
	      data[10], data[11],
	      data[12], data[13],
	      data[14], data[15],
	      data[16], data[17],
	      data[18], data[19]);
      std::cout << ipv6_str << std::endl;
      dst_ip = std::string(ipv6_str, sizeof(ipv6_str));
      break;
    }


    auto part1 = static_cast<unsigned char>(*(data.begin() + sz - 2)), part2 = static_cast<unsigned char>(*(data.begin() + sz - 1));
    uint16_t dst_port = 256 * part1 + part2; 
    
    auto dst_sock_ptr = std::make_shared<TCP::socket>(io_service_); 
    dst_sock_ptr->connect(TCP::endpoint(boost::asio::ip::address::from_string(dst_ip), dst_port), err);
    if (err) {
      BOOST_LOG_TRIVIAL(error) << "connect to dst failed";
      return;
    }

    client_sock_ptr->write_some(boost::asio::buffer(cipher_ptr_->Encode(std::string{0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}), 10), err);
    if (err) {
      BOOST_LOG_TRIVIAL(error) << "write second phase error hanpened " << err.message();
      return ;
    }
    
    boost::thread client_thread([](std::shared_ptr<TCP::socket> client_sock_ptr, std::shared_ptr<TCP::socket> dst_sock_ptr, std::shared_ptr<Cipher> cipher_ptr) {
	boost::system::error_code err;
	std::string data; 
	data.resize(1024);
	
	for (;;) {
	  err.clear(); 
	  size_t sz = client_sock_ptr->read_some(boost::asio::buffer(data), err); 
	  if (err) {
	    if (err != boost::asio::error::eof) {
	      BOOST_LOG_TRIVIAL(error) << "read from client failed " << err.message(); 
	    } else {
	      BOOST_LOG_TRIVIAL(debug) << "client eof" << err.message();
	    }
	    dst_sock_ptr->close();
	    break;
	  }

	  // TODO(handora): optimize with substr data
	  data = cipher_ptr->Decode(data);
	  BOOST_LOG_TRIVIAL(debug) << "get data from client(" << sz << "): " << std::endl;

	  err.clear();
	  dst_sock_ptr->write_some(boost::asio::buffer(data, sz), err);
	  if (err) {
	    BOOST_LOG_TRIVIAL(error) << "send to dst failed " << err.message();
	    break;
	  }
	}
      }, client_sock_ptr, dst_sock_ptr, cipher_ptr_);

    client_thread.detach();

    boost::thread dst_thread([](std::shared_ptr<TCP::socket> client_sock_ptr, std::shared_ptr<TCP::socket> dst_sock_ptr, std::shared_ptr<Cipher> cipher_ptr) {
	std::string data;
	data.resize(1024);
	boost::system::error_code err;
	
	for ( ; ; ) {
	  err.clear();
	  size_t sz = dst_sock_ptr->read_some(boost::asio::buffer(data), err);
          
	  BOOST_LOG_TRIVIAL(debug) << "get data from dst(" << sz << "): " << std::endl;
	  
	  if (err) {
	    if (err != boost::asio::error::eof) {
	      BOOST_LOG_TRIVIAL(error) << "read from client failed " << err.message(); 
	    } else {
	      BOOST_LOG_TRIVIAL(debug) << "dst eof" << err.message();
	    }
	    client_sock_ptr->close();
	    break;
	  }

	  err.clear();
	  client_sock_ptr->write_some(boost::asio::buffer(cipher_ptr->Encode(data), sz), err);
	  if (err) {
	    BOOST_LOG_TRIVIAL(error) << "send to dst failed " << err.message();
	    dst_sock_ptr->close();
	    break;
	  }
	}
      }, client_sock_ptr, dst_sock_ptr, cipher_ptr_);
    dst_thread.detach();
  }
  
  void Server::ReadConfig() {
    using boost::property_tree::ptree;
    using boost::property_tree::read_json;
    ptree config;
    read_json(config_path_, config);
    port_ = config.get<int>("port");
    // TODO(handora): optimize with std::move
    std::string code = config.get<std::string>("code");
    cipher_ptr_ = std::make_shared<Cipher>(code); 
    BOOST_LOG_TRIVIAL(debug) << "port: " << port_ << std::endl << "code_: " << code << std::endl; 

    return ;
}
} // namespace sansocks

int main(int, char *[])
{
  using namespace sansocks;

  Server server("/home/handora/.sansocks-server.json");
  
  return 0;
}
