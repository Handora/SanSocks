#include "server/aserver.h"

namespace sansocks {
  
  void Session::Start() {
    state_ = State::FIRST_PHASE;
    data_in_.resize(1024);
    data_out_.resize(1024);
    DoPrepare();
  }

  void Session::DoPrepare() {
    client_socket_->async_read_some(boost::asio::buffer(data_in_)
				    , boost::bind(&Session::HandleSock5, this, _1, _2));
  }

  void Session::HandleSock5(std::error_code err, std::size_t sz) {
    // TODO(Handora): handle the error code with utility code
    data_in_ = cipher_ptr_->Decode(data_in_);
    if (state_ == State::FIRST_PHASE) {
      if (err || sz < 3) {
	BOOST_LOG_TRIVIAL(error) << "size: " << sz << std::endl;
	BOOST_LOG_TRIVIAL(error) << "read first phase error hanpened " << err.message();
	return ;
      }

      if (data_in_[0] != 0x5) {
	BOOST_LOG_TRIVIAL(error) << "data[0] is "; 
	BOOST_LOG_TRIVIAL(error) << "first phase structue error hanpened ";
	return ;
      }

      client_socket_->async_write_some(boost::asio::buffer(cipher_ptr_->Encode(std::string{0x5, 0x0}), 2), [this] (std::error_code err, std::size_t) {
	  if (err) {
	    BOOST_LOG_TRIVIAL(error) << "write first phase error hanpened " << err.message();
	    return ;
	  }
	  state_ = State::SECOND_PHASE;
	  DoPrepare();
	});
    } else if (state_ == State::SECOND_PHASE) {
      data_in_ = cipher_ptr_->Decode(data_in_);
      if (err || sz < 7) {
	BOOST_LOG_TRIVIAL(error) << "size: " << sz << std::endl;
	BOOST_LOG_TRIVIAL(error) << "read second phase error hanpened " << err.message();
	return ;
      }
      
      if (data_in_[0] != 0x5 || data_in_[1] !=0x1) {
	BOOST_LOG_TRIVIAL(error) << "second phase structure error hanpened " << err.message();
	return ;
      }

      std::string dst_ip; 
      switch (data_in_[3]) {
      
      case 0x01:
	// TODO(Handora): ipv4 len 
      
	dst_ip.append(std::to_string(static_cast<uint8_t>(data_in_[4])));
	dst_ip.push_back('.');
	dst_ip.append(std::to_string(static_cast<uint8_t>(data_in_[5]))); 
	dst_ip.push_back('.');
	dst_ip.append(std::to_string(static_cast<uint8_t>(data_in_[6]))); 
	dst_ip.push_back('.');
	dst_ip.append(std::to_string(static_cast<uint8_t>(data_in_[7]))); 
	break;
      case 0x03:
	// TCP::resolver
      {
	TCP::resolver::query query(std::string(data_in_.begin() + 5, data_in_.begin() + sz - 2), ""); 
	auto address_iterator = resolver_->resolve(query);
	dst_ip = (*address_iterator.begin()).endpoint().address().to_string(); 
	break;
      }
      
      case 0x04:
	// TODO(Handora): ipv6 len
	char ipv6_str[100];
	sprintf(ipv6_str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
		data_in_[4], data_in_[5],
		data_in_[6], data_in_[7],
		data_in_[8], data_in_[9],
		data_in_[10], data_in_[11],
		data_in_[12], data_in_[13],
		data_in_[14], data_in_[15],
		data_in_[16], data_in_[17],
		data_in_[18], data_in_[19]);
	std::cout << ipv6_str << std::endl;
	dst_ip = std::string(ipv6_str, sizeof(ipv6_str));
	break;
      }

      auto part1 = static_cast<unsigned char>(*(data_in_.begin() + sz - 2)), part2 = static_cast<unsigned char>(*(data_in_.begin() + sz - 1));
      uint16_t dst_port = 256 * part1 + part2;

      boost::system::error_code ec;
      dst_socket_->connect(TCP::endpoint(boost::asio::ip::address::from_string(dst_ip), dst_port), ec);
      if (ec) {
	BOOST_LOG_TRIVIAL(error) << "connect to dst failed";
	return;
      }

      client_socket_->async_write_some(boost::asio::buffer(cipher_ptr_->Encode(std::string{0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}), 10), [this] (std::error_code err, std::size_t) {
	  if (err) {
	    BOOST_LOG_TRIVIAL(error) << "write second phase error hanpened " << err.message();
	    return ;
	  } 

	  DoRead(client_socket_, dst_socket_);
	  DoRead(dst_socket_, client_socket_); 
	});
      
    } else {
      BOOST_LOG_TRIVIAL(error) << "UNknown enum state";
      return ;
    }
  }

  void Session::DoRead(std::shared_ptr<TCP::socket> from, std::shared_ptr<TCP::socket> to) {
    from->async_read_some(boost::asio::buffer(data_in_), [this, &from, &to](boost::system::error_code err, std::size_t sz) {
	if (err) {
	  if (err != boost::asio::error::eof) {
	    BOOST_LOG_TRIVIAL(error) << "read from client failed " << err.message(); 
	  } else {
	    BOOST_LOG_TRIVIAL(debug) << "client eof" << err.message();
	  }
	  dst_socket_->close();
	  return;
	}

	data_in_ = cipher_ptr_->Decode(data_in_);
	BOOST_LOG_TRIVIAL(debug) << "get data from client(" << sz << "): " << std::endl;

	try {
	  to->async_write_some(boost::asio::buffer(data_in_, sz), [this, &from, &to](boost::system::error_code err, std::size_t) {
	      if (err) {
		BOOST_LOG_TRIVIAL(error) << "send to dst failed " << err.message();
		return;
	      }

	      DoRead(from, to);
	    });
	} catch (...) {
	  BOOST_LOG_TRIVIAL(error) << "send to dst failed " << err.message();
	  return;
	}
      });
  }

  Aserver::Aserver(const std::string& path)
    : config_path_(path) {
    ReadConfig();

    acceptor_ptr = std::make_shared<TCP::acceptor>(io_service_, TCP::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port_));
  }

  void Aserver::Start() {
    DoAccept();
  }

  void Aserver::DoAccept() {
    acceptor_ptr->async_accept(
      [this] (std::error_code ec, TCP::socket socket) {
	if (ec) {
          BOOST_LOG_TRIVIAL(error) << "accept from browser error";
	  return ;
	}

	std::make_shared<Session>(std::make_shared<TCP::socket>(std::move(socket)), std::make_shared<TCP::socket>(io_service_), cipher_ptr_, std::make_shared<TCP::resolver>(io_service_))->Start();

	DoAccept();
      });
  }

  void Aserver::ReadConfig() {
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


int main(int , char *[])
{
  using namespace sansocks;
  Aserver server("/home/handora/.sansocks-server.json");
  return 0;
}
