#include "client\client.h"

namespace sansocks
{
	Client::Client(const std::string& path) : config_path_(path)
	{
		listen_sock_ptr.reset(new TCP::socket(ios_), SocketDeleter());
		ReadConfig();
		acceptor_ptr = std::shared_ptr<TCP::acceptor>(new TCP::acceptor(ios_,
			TCP::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), local_port_)));
		StartUp();
	}

	void Client::StartUp()
	{
		for (;;)
		{
			auto client_ptr = std::make_shared<TCP::socket>(new TCP::socket(ios_), SocketDeleter());

			acceptor_ptr->accept(*client_ptr);

			std::thread t(&Client::HandleMsg, this, client_ptr);
			t.detach();
		}
	}

	void Client::HandleMsg(std::shared_ptr<TCP::socket> client_ptr)
	{
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
		cipher_ptr = std::make_shared<Cipher>
			(std::move(config.get<std::string>("code")));
	}
}