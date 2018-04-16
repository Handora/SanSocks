#include "client/client.h"

namespace sansocks
{
	Client::Client(const std::string& path) : config_path_(path)
	{
		ReadConfig();
		acceptor_ptr = std::shared_ptr<TCP::acceptor>(new TCP::acceptor(ios_,
			TCP::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), local_port_)));
		for (;;)
		{
			auto browser_sock_ptr = std::make_shared<TCP::socket>(new TCP::socket(ios_), SocketDeleter());
			acceptor_ptr->accept(*browser_sock_ptr);
			std::thread t(&Client::ReadFromBrowser, this, browser_sock_ptr);
			t.detach();
		}
	}

	void Client::ReadFromBrowser(std::shared_ptr<TCP::socket> browser_sock_ptr)
	{
		std::shared_ptr<TCP::socket> server_sock_ptr = ConnectToServer();
		boost::system::error_code err;
		for (;;)
		{
			std::string data;
			data.resize(1024);
			size_t sz = browser_sock_ptr->read_some(boost::asio::buffer(data), err);
			data = cipher_ptr_->Encode(data);
			std::thread t(&Client::CommuniteWithServer, this, server_sock_ptr, browser_sock_ptr,data);
			t.detach();
			if (err == boost::asio::error::eof)
				break;
			else if (err)
				throw boost::system::system_error(err);
		}
	}

	void Client::CommuniteWithServer(std::shared_ptr<TCP::socket> server_sock_ptr,std::shared_ptr<TCP::socket> browser_scok_ptr, std::string wait_send,size_t sz)
	{
		boost::system::error_code err;
		for (;;)
		{
			server_sock_ptr->write_some(boost::asio::buffer(wait_send, sz));
			std::string data;
			data.resize(1024);
			size_t sz = server_sock_ptr->read_some(boost::asio::buffer(data));
			std::thread t(&Client::ReplyToBrowser, this, browser_scok_ptr, data, sz);
			t.detach();
		}
	}

	auto Client::ConnectToServer() -> std::shared_ptr<TCP::socket>
	{
		auto server_sock_ptr = std::make_shared<TCP::socket>(new TCP::socket(ios_),SocketDeleter());
		server_sock_ptr->connect(TCP::endpoint(boost::asio::ip::address::from_string(remote_addr_), remote_port_));
		return server_sock_ptr;
	}

	void Client::ReplyToBrowser(std::shared_ptr<TCP::socket> broswer_sock_ptr, std::string wait_send, size_t sz)
	{
		wait_send = cipher_ptr_->Decode(std::string(wait_send.begin(), wait_send.begin() + sz));
		broswer_sock_ptr->write_some(boost::asio::buffer(wait_send, sz));
	}

	void Client::ReadConfig()
	{
		using boost::property_tree::ptree;
		using boost::property_tree::read_json;
		ptree config;
		read_json(config_path_, config);
		local_port_ = config.get<int>("local port");
		remote_port_ = config.get<int>("remote port");
		remote_addr_ = config.get<std::string>("remote addr");
		cipher_ptr_ = std::make_shared<Cipher>
		  (std::move(config.get<std::string>("code")));
	}
}
