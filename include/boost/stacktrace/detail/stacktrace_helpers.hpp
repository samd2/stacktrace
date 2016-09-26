// Copyright Antony Polukhin, 2016.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STACKTRACE_DETAIL_STACKTRACE_HELPERS_HPP
#define BOOST_STACKTRACE_DETAIL_STACKTRACE_HELPERS_HPP

#include <boost/config.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_pointer.hpp>

namespace boost { namespace stacktrace { namespace detail {

struct backtrace_holder;

template <class T>
inline boost::stacktrace::detail::backtrace_holder& to_bt(T& data) BOOST_NOEXCEPT {
    return *reinterpret_cast<boost::stacktrace::detail::backtrace_holder*>(&data);
}

template <class T>
inline const boost::stacktrace::detail::backtrace_holder& to_bt(const T& data) BOOST_NOEXCEPT {
    return *reinterpret_cast<const boost::stacktrace::detail::backtrace_holder*>(&data);
}


/*
BOOST_STATIC_CONSTEXPR char to_hex_array[] = "0123456789ABCDEF";

template <class T>
inline boost::array<char, 2 + sizeof(void*) * 2 + 1> to_hex(T addr) BOOST_NOEXCEPT {
    boost::array<char, 2 + sizeof(void*) * 2 + 1> ret = {"0x"};
    ret.back() = '\0';
    BOOST_STATIC_ASSERT_MSG(!boost::is_pointer<T>::value, "");

    const std::size_t s = (
        (addr >> (sizeof(addr) / 2 * 8)) ? sizeof(addr) : sizeof(addr) / 2
    );

    char* out = ret.data() + s * 2 + 1;

    for (std::size_t i = 0; i < s; ++i) {
        const unsigned char tmp_addr = (addr & 0xFFu);
        *out = to_hex_array[tmp_addr & 0xF];
        -- out;
        *out = to_hex_array[tmp_addr >> 4];
        -- out;
        addr >>= 8;
    }

    return ret;
}*/

}}} // namespace boost::stacktrace::detail

#endif // BOOST_STACKTRACE_DETAIL_STACKTRACE_HELPERS_HPP