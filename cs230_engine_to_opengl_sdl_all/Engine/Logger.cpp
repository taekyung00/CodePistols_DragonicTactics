/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author TODO Your Name
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Logger.hpp"
#include <iostream>

namespace CS230
{
    // TODO implement class here
    // note the proper way to redirect the rdbuf is `stream.basic_ios<char>::rdbuf(other_stream.rdbuf());`
    // note that we don't need a destructor ~Logger() if all we are doing is closing the streams. The std stream classes impl Rule of 5 and will auto flush & close themselves
}
