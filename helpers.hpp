#pragma once

#include <string>
#include <boost/asio.hpp>

/// Helper to get string from streambuf
inline std::string make_string(boost::asio::streambuf& streambuf)
{
  return {boost::asio::buffers_begin(streambuf.data()), 
          boost::asio::buffers_end(streambuf.data())};
}
