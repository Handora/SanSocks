#pragma once
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <memory>
#include <thread>
#include "utility\util.h"
#include "Cipher\cipher.h"

namespace sansocks
{
	struct SocketDeleter
	{
		void operator()(boost::asio::ip::tcp::socket* ptr)
		{
			if (ptr != nullptr)
			{
				ptr->close();
				delete ptr;
			}
		}
	};
	class Client
	{
		typedef boost::asio::ip::tcp TCP;
	public:
		Client(const std::string& path = "config.json");
		~Client();
	private:
		void ReadConfig();
		void StartUp();
		void HandleMsg(std::shared_ptr<TCP::socket>);
		int local_port_;
		int remote_port_;
		std::string remote_addr_;
		std::string config_path_;
		std::shared_ptr<Cipher> cipher_ptr;
		boost::asio::io_service ios_;
		std::shared_ptr<TCP::socket> listen_sock_ptr;
		std::shared_ptr<TCP::acceptor> acceptor_ptr;
	};
}