/******************************************************************************\
 *           ___        __                                                    *
 *          /\_ \    __/\ \                                                   *
 *          \//\ \  /\_\ \ \____    ___   _____   _____      __               *
 *            \ \ \ \/\ \ \ '__`\  /'___\/\ '__`\/\ '__`\  /'__`\             *
 *             \_\ \_\ \ \ \ \L\ \/\ \__/\ \ \L\ \ \ \L\ \/\ \L\.\_           *
 *             /\____\\ \_\ \_,__/\ \____\\ \ ,__/\ \ ,__/\ \__/.\_\          *
 *             \/____/ \/_/\/___/  \/____/ \ \ \/  \ \ \/  \/__/\/_/          *
 *                                          \ \_\   \ \_\                     *
 *                                           \/_/    \/_/                     *
 *                                                                            *
 * Copyright (C) 2011-2013                                                    *
 * Dominik Charousset <dominik.charousset@haw-hamburg.de>                     *
 * Raphael Hiesgen <raphael.hiesgen@haw-hamburg.de>                           *
 *                                                                            *
 * This file is part of libcppa.                                              *
 * libcppa is free software: you can redistribute it and/or modify it under   *
 * the terms of the GNU Lesser General Public License as published by the     *
 * Free Software Foundation, either version 3 of the License                  *
 * or (at your option) any later version.                                     *
 *                                                                            *
 * libcppa is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 * See the GNU Lesser General Public License for more details.                *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with libcppa. If not, see <http://www.gnu.org/licenses/>.            *
\******************************************************************************/


#ifndef CPPA_OPENCL_COMMAND_HPP
#define CPPA_OPENCL_COMMAND_HPP

#include <vector>
#include <functional>

#include "cppa/actor.hpp"
#include "cppa/opencl/global.hpp"

namespace cppa { namespace opencl {

class command {

 public:

    command* next;

    // bool is_new_job; // if false == call handle_cl_result
    // struct callbacks {
    //     std::function<cl_job_id(cl_command_queue)> enqueue_cl_task;
    //     std::function<void()> handle_cl_result;
    // };
    // union { callbacks cbs; cl_job_id jid; };

    std::function<void(cl_command_queue)> fun;

    actor* sender;

    template<typename T>
    inline command(T f, actor* sender)
        : next(nullptr), fun(f), sender(sender) { }

    inline command() : next(nullptr), sender(nullptr) { }
};

} } // namespace cppa::opencl

#endif // CPPA_OPENCL_COMMAND_HPP
