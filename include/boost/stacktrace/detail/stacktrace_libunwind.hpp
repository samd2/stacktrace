// Copyright Antony Polukhin, 2016.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STACKTRACE_DETAIL_STACKTRACE_LIBUNWIND_HPP
#define BOOST_STACKTRACE_DETAIL_STACKTRACE_LIBUNWIND_HPP

#include <boost/config.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#include <boost/stacktrace.hpp>
#include <boost/stacktrace/detail/stacktrace_helpers.hpp>
#include <boost/core/demangle.hpp>
#include <cstring>
#include <boost/core/no_exceptions_support.hpp>
#include <boost/make_shared.hpp>

#define UNW_LOCAL_ONLY
#include <libunwind.h>

namespace boost { namespace stacktrace { namespace detail {

struct backtrace_holder {
    std::size_t frames_count;
    boost::shared_ptr<std::string[]> frames;

    BOOST_FORCEINLINE backtrace_holder() BOOST_NOEXCEPT
        : frames_count(0)
    {}

    inline std::size_t size() const BOOST_NOEXCEPT {
        return frames_count;
    }

    inline std::string get_frame(std::size_t frame) const {
        if (frame < frames_count) {
            return frames[frame];
        } else {
            return std::string();
        }
    }

    static inline std::string get_frame_impl(unw_cursor_t& cursor) {
        std::string res;
        unw_word_t offp;
        char data[256];
        const int ret = unw_get_proc_name (&cursor, data, sizeof(data) / sizeof(char), &offp);

        if (ret == -UNW_ENOMEM) {
            res.resize(sizeof(data) * 2);
            do {
                const int ret2 = unw_get_proc_name(&cursor, &res[0], res.size(), &offp);
                if (ret2 == -UNW_ENOMEM) {
                    res.resize(res.size() * 2);
                } else if (ret2 == 0) {
                    break;
                } else {
                    res = data;
                    return res;
                }
            } while(1);
        } else if (ret == 0) {
            res = data;
        } else {
            return res;
        }

        boost::core::scoped_demangled_name demangled(res.data());
        if (demangled.get()) {
            res = demangled.get();
        } else {
            res.resize( std::strlen(res.data()) ); // Note: here res is \0 terminated, but size() not equal to strlen
        }

        return res;
    }
};

}}} // namespace boost::stacktrace::detail


namespace boost { namespace stacktrace {

stacktrace::stacktrace() BOOST_NOEXCEPT {
    new (&impl_) boost::stacktrace::detail::backtrace_holder();
    boost::stacktrace::detail::backtrace_holder& bt = boost::stacktrace::detail::to_bt(impl_);

    unw_context_t uc;
    if (unw_getcontext(&uc) != 0) {
        return;
    }

    {   // Counting frames_count
        unw_cursor_t cursor;
        if (unw_init_local(&cursor, &uc) != 0) {
            return;
        }
        while (unw_step(&cursor) > 0) {
            ++ bt.frames_count;
        }
    }

    unw_cursor_t cursor;
    if (unw_init_local(&cursor, &uc) != 0) {
        bt.frames_count = 0;
        return;
    }

    BOOST_TRY {
        bt.frames = boost::make_shared<std::string[]>(bt.frames_count);
        std::size_t i = 0;
        while (unw_step(&cursor) > 0){
            bt.frames[i] = boost::stacktrace::detail::backtrace_holder::get_frame_impl(cursor);
            ++ i;
        }
    } BOOST_CATCH(...) {}
    BOOST_CATCH_END
}

}} // namespace boost::stacktrace

#endif // BOOST_STACKTRACE_DETAIL_STACKTRACE_LIBUNWIND_HPP