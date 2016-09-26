// Copyright Antony Polukhin, 2016.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STACKTRACE_DETAIL_STACKTRACE_WINDOWS_HPP
#define BOOST_STACKTRACE_DETAIL_STACKTRACE_WINDOWS_HPP

#include <boost/config.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#include <boost/stacktrace.hpp>
#include <boost/stacktrace/detail/stacktrace_helpers.hpp>

#include <windows.h>
#include "DbgHelp.h"
#include <WinBase.h>

#if !defined(BOOST_ALL_NO_LIB)
#   define BOOST_LIB_NAME Dbghelp
#   ifdef BOOST_STACKTRACE_DYN_LINK
#       define BOOST_DYN_LINK
#   endif
#   include <boost/config/auto_link.hpp>
#endif

namespace boost { namespace stacktrace { namespace detail {

struct symbol_info_with_stack {
    BOOST_STATIC_CONSTEXPR  std::size_t max_name_length = MAX_SYM_NAME * sizeof(char);
    SYMBOL_INFO symbol;
    char name_part[max_name_length];
};

struct symbol_initialization_structure {
    HANDLE process;

    inline symbol_initialization_structure() BOOST_NOEXCEPT
        : process(GetCurrentProcess())
    {
        SymInitialize(process, 0, true);
    }

    inline ~symbol_initialization_structure() BOOST_NOEXCEPT {
        SymCleanup(process);
    }
};

struct backtrace_holder {
    BOOST_STATIC_CONSTEXPR std::size_t max_size = 100u;

    std::size_t frames_count;
    void* buffer[max_size];

    inline std::size_t size() const BOOST_NOEXCEPT {
        return frames_count;
    }

    inline std::string get_frame(std::size_t frame) const {
        std::string res;

        static symbol_initialization_structure symproc;

        if (frame >= frames_count) {
            return res;
        }

        symbol_info_with_stack s;
        s.symbol.MaxNameLen = symbol_info_with_stack::max_name_length;
        s.symbol.SizeOfStruct = sizeof(SYMBOL_INFO);
        const bool sym_res = !!SymFromAddr(symproc.process, reinterpret_cast<DWORD64>(buffer[frame]), 0, &s.symbol);
        if (sym_res) {
            res = s.symbol.Name;
        }
        return res;
    }
};

}}} // namespace boost::stacktrace::detail

namespace boost { namespace stacktrace {

stacktrace::stacktrace() BOOST_NOEXCEPT {
    new (&impl_) boost::stacktrace::detail::backtrace_holder();
    boost::stacktrace::detail::backtrace_holder& bt = boost::stacktrace::detail::to_bt(impl_);
    bt.frames_count = CaptureStackBackTrace(0, boost::stacktrace::detail::backtrace_holder::max_size, bt.buffer, 0);
}

}} // namespace boost::stacktrace

#endif // BOOST_STACKTRACE_DETAIL_STACKTRACE_WINDOWS_HPP