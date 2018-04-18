#include "client/client.h"
#include "boost/log/trivial.hpp"

namespace sansocks {
  Client::Client(const std::string& path) : config_path_(path) {
    ReadConfig();

    acceptor_ptr =
      std::shared_ptr<TCP::acceptor>(new TCP::acceptor(ios_,
						       TCP::endpoint(boost::asio::ip::address::from_string("127.0.0.1"),
								     local_port_)));

    for (;;) {
      acceptor_ptr->set_option(TCP::acceptor::reuse_address(true));
      
      auto browser_sock_ptr = std::make_shared<TCP::socket>(ios_);
      acceptor_ptr->accept(*browser_sock_ptr);
      PreparedForWork(browser_sock_ptr);
    }
  }

  void Client::PreparedForWork(std::shared_ptr<TCP::socket> browser_sock_ptr) {
    std::shared_ptr<TCP::socket> server_sock_ptr = ConnectToServer();
    std::thread t1(&Client::TransmitMsg, this, browser_sock_ptr, server_sock_ptr,TransmitType::BROWSER_TO_SERVER);
    std::thread t2(&Client::TransmitMsg, this, server_sock_ptr, browser_sock_ptr,TransmitType::SERVER_TO_BROWSER);
    t1.detach();
    t2.detach();
  }

  void Client::TransmitMsg(std::shared_ptr<TCP::socket> read_sock_ptr, std::shared_ptr<TCP::socket> write_sock_ptr,TransmitType type) {
    boost::system::error_code err;
    std::string data;
    data.resize(1024);
    for (;;) {
      size_t sz = read_sock_ptr->read_some(boost::asio::buffer(data), err);
      
      if (err == boost::asio::error::eof) {
	std::cout << "eof" << std::endl;
	break;
      }
      else if (err)
	throw boost::system::system_error(err);
      
      if (type == TransmitType::BROWSER_TO_SERVER)
	data = cipher_ptr_->Encode(data);
      else
	data = cipher_ptr_->Decode(data);
      std::cout << sz << " " << data << std::endl;
      write_sock_ptr->write_some(boost::asio::buffer(data, sz));
    }
  }

  auto Client::ConnectToServer() -> std::shared_ptr<TCP::socket> {
    auto server_sock_ptr = std::make_shared<TCP::socket>(ios_);
    server_sock_ptr->connect(TCP::endpoint(boost::asio::ip::address::from_string(remote_addr_), remote_port_));
    return server_sock_ptr;
  }

  void Client::ReadConfig() {
    using boost::property_tree::ptree;
    using boost::property_tree::read_json;
    ptree config;
    read_json(config_path_, config);
    local_port_ = config.get<int>("local port");
    remote_port_ = config.get<int>("remote port");
    remote_addr_ = config.get<std::string>("remote addr");
    // TODO(TANGLUN): opimize with std::move
    cipher_ptr_ = std::make_shared<Cipher>
      ((config.get<std::string>("code")));

    // BOOST_LOG_TRIVIAL(debug) << "local port: " << local_port_ << std::endl
    // 			     << "remote addr: " << remote_addr_ << std::endl
    // 			     << "remote port: " << remote_port_ << std::endl
    // 			     << "code: " << config.get<std::string>("code") << std::endl; 
  }
}


int main(int, char *[]) {
  using namespace sansocks;

  Client client("/home/handora/.sansocks.json");
  return 0;
}
