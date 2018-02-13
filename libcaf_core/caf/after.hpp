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

#ifndef CAF_AFTER_HPP
#define CAF_AFTER_HPP

#include <tuple>
#include <type_traits>

#include "caf/timeout_definition.hpp"

namespace caf {

class timeout_definition_builder {
public:
  constexpr timeout_definition_builder(timespan d) : tout_(d) {
    // nop
  }

  template <class F>
  timeout_definition<F> operator>>(F f) const {
    return {tout_, std::move(f)};
  }

private:
  timespan tout_;
};

/// Returns a generator for timeouts.
constexpr timeout_definition_builder after(timespan d) {
  return {d};
}

/// Returns a generator for timeouts.
template <class Rep, class Period>
constexpr timeout_definition_builder
after(std::chrono::duration<Rep, Period> d) {
  return {std::chrono::duration_cast<timespan>(d)};
}

} // namespace caf

#endif // CAF_AFTER_HPP
