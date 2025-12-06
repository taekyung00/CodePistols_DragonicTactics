<<<<<<< HEAD
﻿/**
=======
/**
>>>>>>> main
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "Engine.h"
#include "Logger.h"
#include <filesystem>
#include <source_location>
#include <sstream>
#include <utility>

namespace details
{
<<<<<<< HEAD
    template <typename... Messages>
    [[noreturn]] void internal_throw_report_and_throw_error(std::source_location caller_location, Messages&&... more_messages)
    {
        std::ostringstream error_message_stream;
        error_message_stream << '{' << std::filesystem::path(caller_location.file_name()).filename().string() << "}(" << caller_location.line() << ") : " << caller_location.function_name() << '\n';
        (error_message_stream << ... << std::forward<Messages>(more_messages));
        const auto message_string = error_message_stream.str();
        Engine::GetLogger().LogError(message_string);
        throw std::runtime_error{ message_string };
    }
=======
  template <typename... Messages>
  [[noreturn]] void internal_throw_report_and_throw_error(std::source_location caller_location, Messages&&... more_messages)
  {
	std::ostringstream error_message_stream;
	error_message_stream << '{' << std::filesystem::path(caller_location.file_name()).filename().string() << "}(" << caller_location.line() << ") : " << caller_location.function_name() << '\n';
	(error_message_stream << ... << std::forward<Messages>(more_messages));
	const auto message_string = error_message_stream.str();
	Engine::GetLogger().LogError(message_string);
	throw std::runtime_error{ message_string };
  }
>>>>>>> main
}

template <typename M1>
[[noreturn]] void throw_error_message(M1&& m1, std::source_location caller_location = std::source_location::current())
{
<<<<<<< HEAD
    details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1));
=======
  details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1));
>>>>>>> main
}

template <typename M1, typename M2>
[[noreturn]] void throw_error_message(M1&& m1, M2&& m2, std::source_location caller_location = std::source_location::current())
{
<<<<<<< HEAD
    details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2));
=======
  details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2));
>>>>>>> main
}

template <typename M1, typename M2, typename M3>
[[noreturn]] void throw_error_message(M1&& m1, M2&& m2, M3&& m3, std::source_location caller_location = std::source_location::current())
{
<<<<<<< HEAD
    details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3));
=======
  details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3));
>>>>>>> main
}

template <typename M1, typename M2, typename M3, typename M4>
[[noreturn]] void throw_error_message(M1&& m1, M2&& m2, M3&& m3, M4&& m4, std::source_location caller_location = std::source_location::current())
{
<<<<<<< HEAD
    details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4));
=======
  details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4));
>>>>>>> main
}

template <typename M1, typename M2, typename M3, typename M4, typename M5>
[[noreturn]] void throw_error_message(M1&& m1, M2&& m2, M3&& m3, M4&& m4, M5&& m5, std::source_location caller_location = std::source_location::current())
{
<<<<<<< HEAD
    details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5));
=======
  details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5));
>>>>>>> main
}

template <typename M1, typename M2, typename M3, typename M4, typename M5, typename M6>
[[noreturn]] void throw_error_message(M1&& m1, M2&& m2, M3&& m3, M4&& m4, M5&& m5, M6&& m6, std::source_location caller_location = std::source_location::current())
{
<<<<<<< HEAD
    details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6));
=======
  details::internal_throw_report_and_throw_error(caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6));
>>>>>>> main
}

template <typename M1, typename M2, typename M3, typename M4, typename M5, typename M6, typename M7>
[[noreturn]] void throw_error_message(M1&& m1, M2&& m2, M3&& m3, M4&& m4, M5&& m5, M6&& m6, M7&& m7, std::source_location caller_location = std::source_location::current())
{
<<<<<<< HEAD
    details::internal_throw_report_and_throw_error(
        caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6), std::forward<M7>(m7));
=======
  details::internal_throw_report_and_throw_error(
	caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6), std::forward<M7>(m7));
>>>>>>> main
}

template <typename M1, typename M2, typename M3, typename M4, typename M5, typename M6, typename M7, typename M8>
[[noreturn]] void throw_error_message(M1&& m1, M2&& m2, M3&& m3, M4&& m4, M5&& m5, M6&& m6, M7&& m7, M8&& m8, std::source_location caller_location = std::source_location::current())
{
<<<<<<< HEAD
    details::internal_throw_report_and_throw_error(
        caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6), std::forward<M7>(m7),
        std::forward<M8>(m8));
=======
  details::internal_throw_report_and_throw_error(
	caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6), std::forward<M7>(m7), std::forward<M8>(m8));
>>>>>>> main
}

template <typename M1, typename M2, typename M3, typename M4, typename M5, typename M6, typename M7, typename M8, typename M9>
[[noreturn]] void throw_error_message(M1&& m1, M2&& m2, M3&& m3, M4&& m4, M5&& m5, M6&& m6, M7&& m7, M8&& m8, M9&& m9, std::source_location caller_location = std::source_location::current())
{
<<<<<<< HEAD
    details::internal_throw_report_and_throw_error(
        caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6), std::forward<M7>(m7), std::forward<M8>(m8),
        std::forward<M9>(m9));
=======
  details::internal_throw_report_and_throw_error(
	caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6), std::forward<M7>(m7), std::forward<M8>(m8),
	std::forward<M9>(m9));
>>>>>>> main
}

template <typename M1, typename M2, typename M3, typename M4, typename M5, typename M6, typename M7, typename M8, typename M9, typename M10>
[[noreturn]] void
<<<<<<< HEAD
    throw_error_message(M1&& m1, M2&& m2, M3&& m3, M4&& m4, M5&& m5, M6&& m6, M7&& m7, M8&& m8, M9&& m9, M10&& m10, std::source_location caller_location = std::source_location::current())
{
    details::internal_throw_report_and_throw_error(
        caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6), std::forward<M7>(m7), std::forward<M8>(m8),
        std::forward<M9>(m9), std::forward<M10>(m10));
=======
  throw_error_message(M1&& m1, M2&& m2, M3&& m3, M4&& m4, M5&& m5, M6&& m6, M7&& m7, M8&& m8, M9&& m9, M10&& m10, std::source_location caller_location = std::source_location::current())
{
  details::internal_throw_report_and_throw_error(
	caller_location, std::forward<M1>(m1), std::forward<M2>(m2), std::forward<M3>(m3), std::forward<M4>(m4), std::forward<M5>(m5), std::forward<M6>(m6), std::forward<M7>(m7), std::forward<M8>(m8),
	std::forward<M9>(m9), std::forward<M10>(m10));
>>>>>>> main
}
