/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright (C) 2011 - 2017                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#ifndef CAF_SKIP_HPP
#define CAF_SKIP_HPP

#include <functional>

#include "caf/fwd.hpp"

namespace caf {

/// @relates local_actor
/// Default handler function that leaves messages in the mailbox.
/// Can also be used inside custom message handlers to signalize
/// skipping to the runtime.
class skip_t {
public:
  using fun = std::function<result<message> (scheduled_actor* self,
                                             message_view&)>;

  constexpr skip_t() {
    // nop
  }

  constexpr skip_t operator()() const {
    return *this;
  }

  operator fun() const;

private:
  static result<message> skip_fun_impl(scheduled_actor*, message_view&);
};

/// Tells the runtime system to skip a message when used as message
/// handler, i.e., causes the runtime to leave the message in
/// the mailbox of an actor.
constexpr skip_t skip = skip_t{};

} // namespace caf

#endif // CAF_SKIP_HPP