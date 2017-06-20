#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "helpers.hpp"

using boost::asio::ip::tcp;
using namespace std;

/// Simple client to send messages to server.
int main(int argc, char* argv[])
{
  try
  {
    if (argc != 5)
    {
      std::cerr << "Usage: client <host> <port> <id> <msg>" << std::endl;
      return 1;
    }
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve({ argv[1], argv[2]});
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
    cout << "Connected to the server." << endl;

    // Send message to server.
    boost::system::error_code error;
    boost::asio::streambuf write_buffer;
    std::ostream output(&write_buffer);
    output << argv[3] << "#" << argv[4] << '\n';
    // Delimiter is \n so omit endl at the end of print.
    std::cout << "Writing: " << make_string(write_buffer);
    boost::asio::write(socket, write_buffer);
    if (boost::asio::error::eof != error) {
      throw boost::system::system_error(error); // An error occured.
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
