#include "cipher/cipher.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>


namespace sansocks {
  class Aserver
  {
    typedef boost::asio::ip::tcp TCP;
  public:
    Aserver(const std::string& path);
    virtual ~Aserver() {};

    // start the server
    void Start();

  private:
    void ReadConfig(); 
    void DoAccept();
        
    int port_;
    std::string config_path_;
    std::string base64_table_code_;
    std::shared_ptr<Cipher> cipher_ptr_;
    boost::asio::io_service io_service_;
    std::shared_ptr<TCP::acceptor> acceptor_ptr;
  };


  class Session
  {
    enum class State {
      FIRST_PHASE,
	SECOND_PHASE, 
	THIRD_PHASE
	};
    typedef boost::asio::ip::tcp TCP;
  public:
  Session(std::shared_ptr<TCP::socket> client_socket_ptr,
	  std::shared_ptr<TCP::socket> dst_socket_ptr,
	  std::shared_ptr<Cipher> cipher_ptr,
	  std::shared_ptr<TCP::resolver> resolver)
    :client_socket_(client_socket_ptr),
      cipher_ptr_(cipher_ptr),
      resolver_(resolver),
      dst_socket_(dst_socket_ptr) {};
    virtual ~Session();

    void Start();
    
  private:
    void DoRead(std::shared_ptr<TCP::socket> from, std::shared_ptr<TCP::socket> to);
    void DoWrite(std::shared_ptr<TCP::socket> from, std::shared_ptr<TCP::socket> to);
    void DoPrepare();
    void HandleSock5(std::error_code err, std::size_t sz);

    std::shared_ptr<TCP::resolver> resolver_;
    State state_;
    std::string data_in_;
    std::string data_out_;
    std::shared_ptr<TCP::socket> client_socket_;
    std::shared_ptr<TCP::socket> dst_socket_;
    std::shared_ptr<Cipher> cipher_ptr_;
  };
} // namespace sansocks

