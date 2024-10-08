#ifndef UTILS_HPP
#define UTILS_HPP

#include "beast.hpp"
#include <iostream>

inline void fail(boost::beast::error_code ec, char const* what) // dont do this
{
    if(ec == net::ssl::error::stream_truncated)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}



#endif // UTILS_HPP

