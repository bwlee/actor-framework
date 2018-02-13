/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#ifndef CAF_TIMEOUT_DEFINITION_HPP
#define CAF_TIMEOUT_DEFINITION_HPP

#include <functional>
#include <type_traits>

#include "caf/timestamp.hpp"

namespace caf {

namespace detail {

class behavior_impl;

behavior_impl* new_default_behavior(timespan d, std::function<void()> fun);

} // namespace detail

template <class F>
struct timeout_definition {
  static constexpr bool may_have_timeout = true;

  timespan timeout;

  F handler;

  detail::behavior_impl* as_behavior_impl() const {
    return detail::new_default_behavior(timeout, handler);
  }

  timeout_definition() = default;
  timeout_definition(timeout_definition&&) = default;
  timeout_definition(const timeout_definition&) = default;

  timeout_definition(timespan d, F&& f) : timeout(d), handler(std::move(f)) {
    // nop
  }

  template <class U>
  timeout_definition(const timeout_definition<U>& other)
      : timeout(other.timeout),
        handler(other.handler) {
    // nop
  }
};

template <class T>
struct is_timeout_definition : std::false_type {};

template <class T>
struct is_timeout_definition<timeout_definition<T>> : std::true_type {};

using generic_timeout_definition = timeout_definition<std::function<void()>>;

} // namespace caf

#endif // CAF_TIMEOUT_DEFINITION_HPP
