#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <boost/asio.hpp>

#include "helpers.hpp"

using boost::asio::ip::tcp;
using boost::asio::io_service;
using namespace std;

/// Atomic variable for signaling threads to exit.
atomic<bool> atomic_running;
/// Map use as a counter for messages.
map<string, int> msg_counter;
/// Port number of this server.
unsigned short port = 0;

/// Server thread function, handles incoming messages and counts them in msg_counter.
void server_thread() {
  try
  {
    io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
      tcp::socket socket(io_service);
      acceptor.accept(socket);
      
      // Boost timeouts are quite difficult and not implemented here, so the server would need one more message before quit.
      if (atomic_running.load(memory_order_release) == false) {
        return;
      }

      // No need for extra handling, as clients just send one message and quits.
      boost::asio::streambuf read_buffer;
      size_t bytes_transferred = boost::asio::read_until(socket, read_buffer, '\n');

      // The delimiter is \n so omit endl at the end of the print.
      string message = make_string(read_buffer);
      std::cout << "Read: " << message;
      read_buffer.consume(bytes_transferred); // Remove data that was read.
      
      // Get service name
      string sname = message.substr(0, message.find_first_of('#'));
      sname += '@';
      sname += socket.remote_endpoint().address().to_string();
      cout << "Receieved message from service " << sname << endl;

      // Count messages
      auto res = msg_counter.find(sname);
      if (msg_counter.end() == res) {
        msg_counter.emplace(sname, 1);
      } else {
        ++(res->second);
      }
    }
  }
  catch (exception& e)
  {
    cerr << e.what() << endl;
    return;
  }
}

/// Console thread function, handles console commands to this server (exit, count).
void console_thread() {
  while (1) {
    string input;
    cin >> input;
    if ("exit" == input) {
      atomic_running.store(false, memory_order_release);
      // Send a message to this server to properly close (as listener runs in blocking mode without timeout).
      boost::asio::io_service io_service;
      tcp::resolver resolver(io_service);
      tcp::socket socket(io_service);
      tcp::endpoint this_server(boost::asio::ip::address::from_string("127.0.0.1"), port);
      socket.connect(this_server);
      boost::system::error_code error;
      boost::asio::streambuf write_buffer;
      std::ostream output(&write_buffer);
      output << "quit#quit\n";
      boost::asio::write(socket, write_buffer);
      return;
    } else if ("count" == input) {
      cout << "Count of messages based by services:" << endl;
      for (const auto cnt : msg_counter) {
        cout << "\t" << cnt.first << " : " << cnt.second << endl;
      }
    } else {
      cout << "Invalid command (valid are 'exit' and 'count')." << endl;
    }
  }
}

/// Initialization of both threads.
int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: server <port>" << std::endl;
    return 1;
  }
  port = static_cast<unsigned short>(atoi(argv[1]));
  atomic_running.store(true, memory_order_release);
  cout << "Starting server thread." << endl;
  thread srvr_thread(server_thread);
  cout << "Starting console thread." << endl;
  thread cnsl_thread(console_thread);
  cout << "Server initialized." << endl;
  cnsl_thread.join();
  srvr_thread.join();
  cout << "Server stopped." << endl;
  return 0;
}
