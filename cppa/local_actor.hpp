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
 * Copyright (C) 2011, 2012                                                   *
 * Dominik Charousset <dominik.charousset@haw-hamburg.de>                     *
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


#ifndef CPPA_CONTEXT_HPP
#define CPPA_CONTEXT_HPP

#include "cppa/actor.hpp"
#include "cppa/behavior.hpp"
#include "cppa/any_tuple.hpp"
#include "cppa/match_expr.hpp"
#include "cppa/exit_reason.hpp"
#include "cppa/partial_function.hpp"
#include "cppa/detail/recursive_queue_node.hpp"

namespace cppa {

// forward declarations
class scheduler;
class local_scheduler;

struct discard_behavior_t { };
struct keep_behavior_t { };

// doxygen doesn't parse anonymous namespaces correctly
#ifndef CPPA_DOCUMENTATION
namespace {
#endif // CPPA_DOCUMENTATION

/**
 * @brief Policy tag that causes {@link event_based_actor::become} to
 *        discard the current behavior.
 * @relates local_actor
 */
constexpr discard_behavior_t discard_behavior = discard_behavior_t();

/**
 * @brief Policy tag that causes {@link event_based_actor::become} to
 *        keep the current behavior available.
 * @relates local_actor
 */
constexpr keep_behavior_t keep_behavior = keep_behavior_t();

#ifndef CPPA_DOCUMENTATION
} // namespace <anonymous>
#endif // CPPA_DOCUMENTATION

/**
 * @brief Base class for local running Actors.
 */
class local_actor : public actor {

    friend class scheduler;

 public:

    /**
     * @brief Causes this actor to subscribe to the group @p what.
     *
     * The group will be unsubscribed if the actor finishes execution.
     * @param what Group instance that should be joined.
     */
    void join(const group_ptr& what);

    /**
     * @brief Causes this actor to leave the group @p what.
     * @param what Joined group that should be leaved.
     * @note Groups are leaved automatically if the Actor finishes
     *       execution.
     */
    void leave(const group_ptr& what);

    /**
     * @brief Finishes execution of this actor.
     *
     * Causes this actor to send an exit message to all of its
     * linked actors, sets its state to @c exited and finishes execution.
     * @param reason Exit reason that will be send to
     *        linked actors and monitors.
     */
    virtual void quit(std::uint32_t reason = exit_reason::normal) = 0;

    /**
     * @brief Removes the first element from the mailbox @p pfun is defined
     *        for and invokes @p pfun with the removed element.
     *        Blocks until a matching message arrives if @p pfun is not
     *        defined for any message in the actor's mailbox.
     * @param pfun A partial function denoting the actor's response to the
     *             next incoming message.
     * @warning You should not call this member function by hand.
     *          Use the {@link cppa::receive receive} function or
     *          the @p become member function in case of event-based actors.
     */
    virtual void dequeue(partial_function& pfun) = 0;

    /**
     * @brief Removes the first element from the mailbox @p bhvr is defined
     *        for and invokes @p bhvr with the removed element.
     *        Blocks until either a matching message arrives if @p bhvr is not
     *        defined for any message in the actor's mailbox or until a
     *        timeout occurs.
     * @param bhvr A partial function with optional timeout denoting the
     *             actor's response to the next incoming message.
     * @warning You should not call this member function by hand.
     *          Use the {@link cppa::receive receive} function or
     *          the @p become member function in case of event-based actors.
     */
    virtual void dequeue(behavior& bhvr) = 0;

    /**
     * @brief Checks whether this actor traps exit messages.
     */
    inline bool trap_exit() const {
        return m_trap_exit;
    }

    /**
     * @brief Enables or disables trapping of exit messages.
     */
    inline void trap_exit(bool new_value) {
        m_trap_exit = new_value;
    }

    /**
     * @brief Checks whether this actor uses the "chained send" optimization.
     */
    inline bool chaining() const {
        return m_chaining;
    }

    /**
     * @brief Enables or disables chained send.
     */
    inline void chaining(bool new_value) {
        m_chaining = m_is_scheduled && new_value;
    }

    /**
     * @brief Returns the last message that was dequeued
     *        from the actor's mailbox.
     * @warning Only set during callback invocation.
     */
    inline any_tuple& last_dequeued() {
        return m_current_node->msg;
    }

    /**
     * @brief Returns the sender of the last dequeued message.
     * @warning Only set during callback invocation.
     * @note Implicitly used by the function {@link cppa::reply}.
     * @see cppa::reply()
     */
    inline actor_ptr& last_sender() {
        return m_current_node->sender;
    }

    /**
     * @brief Adds a unidirectional @p monitor to @p whom.
     *
     * @whom sends a "DOWN" message to this actor as part of its termination.
     * @param whom The actor that should be monitored by this actor.
     * @note Each call to @p monitor creates a new, independent monitor.
     */
    void monitor(actor_ptr whom);

    /**
     * @brief Removes a monitor from @p whom.
     * @param whom A monitored actor.
     */
    void demonitor(actor_ptr whom);

    // become/unbecome API

    /**
     * @brief Sets the actor's behavior to @p bhvr and discards the
     *        previous behavior.
     * @note The recommended way of using this member function is to pass
     *       a pointer to a member variable.
     * @warning @p bhvr is owned by the caller and must remain valid until
     *          the actor terminates.
     */
    inline void become(discard_behavior_t, behavior* bhvr) {
        do_become(*bhvr, true);
    }

    /**
     * @brief Sets the actor's behavior.
     */
    inline void become(discard_behavior_t, behavior bhvr) {
        do_become(std::move(bhvr), true);
    }

    /**
     * @brief Sets the actor's behavior to @p bhvr and keeps the
     *        previous behavior, so that it can be restored by calling
     *        {@link unbecome()}.
     * @note The recommended way of using this member function is to pass
     *       a pointer to a member variable.
     * @warning @p bhvr is owned by the caller and must remain valid until
     *          the actor terminates.
     */
    inline void become(keep_behavior_t, behavior* bhvr) {
        do_become(*bhvr, false);
    }

    /**
     * @brief Sets the actor's behavior.
     */
    inline void become(keep_behavior_t, behavior bhvr) {
        do_become(std::move(bhvr), false);
    }

    /**
     * @brief Sets the actor's behavior.
     */
    inline void become(behavior bhvr) {
        become(discard_behavior, std::move(bhvr));
    }

    /**
     * @brief Equal to <tt>become(discard_old, bhvr)</tt>.
     */
    inline void become(behavior* bhvr) {
        become(discard_behavior, *bhvr);
    }

    /**
     * @brief Sets the actor's behavior.
     */
    template<typename... Cases, typename... Args>
    inline void become(discard_behavior_t, match_expr<Cases...>&& arg0, Args&&... args) {
        become(discard_behavior, match_expr_convert(std::move(arg0), std::forward<Args>(args)...));
    }

    /**
     * @brief Sets the actor's behavior.
     */
    template<typename... Cases, typename... Args>
    inline void become(keep_behavior_t, match_expr<Cases...>&& arg0, Args&&... args) {
        become(keep_behavior, match_expr_convert(std::move(arg0), std::forward<Args>(args)...));
    }

    /**
     * @brief Sets the actor's behavior.
     */
    template<typename... Cases, typename... Args>
    inline void become(match_expr<Cases...> arg0, Args&&... args) {
        become(discard_behavior, match_expr_convert(std::move(arg0), std::forward<Args>(args)...));
    }

    /**
     * @brief Returns to a previous behavior if available.
     */
    virtual void unbecome() = 0;

    /**
     * @brief Can be overridden to initialize an actor before any
     *        message is handled.
     * @warning Must not call blocking functions such as
     *          {@link cppa::receive receive}.
     * @note Calling {@link become} to set an initial behavior is supported.
     */
    virtual void init();

    /**
     * @brief Can be overridden to perform cleanup code after an actor
     *        finished execution.
     * @warning Must not call any function manipulating the actor's state such
     *          as join, leave, link, or monitor.
     */
    virtual void on_exit();

    // library-internal members and member functions that shall
    // not appear in the documentation

#   ifndef CPPA_DOCUMENTATION

    local_actor(bool is_scheduled = false);

    virtual bool initialized() = 0;

    inline void send_message(channel* whom, any_tuple&& what) {
        whom->enqueue(this, std::move(what));
    }

    inline void send_message(actor* whom, any_tuple&& what) {
        if (m_chaining && !m_chained_actor) {
            if (whom->chained_enqueue(this, std::move(what))) {
                m_chained_actor = whom;
            }
        }
        else {
            whom->enqueue(this, std::move(what));
        }
    }

    inline actor_ptr& chained_actor() {
        return m_chained_actor;
    }

 protected:

    bool m_chaining;
    bool m_trap_exit;
    bool m_is_scheduled;
    actor_ptr m_chained_actor;
    detail::recursive_queue_node m_dummy_node;
    detail::recursive_queue_node* m_current_node;

#   endif // CPPA_DOCUMENTATION

 protected:

    virtual void do_become(behavior&& bhvr, bool discard_old) = 0;

    inline void do_become(const behavior& bhvr, bool discard_old) {
        behavior copy{bhvr};
        do_become(std::move(copy), discard_old);
    }

};

/**
 * @brief A smart pointer to a {@link local_actor} instance.
 * @relates local_actor
 */
typedef intrusive_ptr<local_actor> local_actor_ptr;

} // namespace cppa

#endif // CPPA_CONTEXT_HPP
