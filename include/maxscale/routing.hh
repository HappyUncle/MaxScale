/*
 * Copyright (c) 2018 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl11.
 *
 * Change Date: 2024-10-14
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */
#pragma once

/**
 * @file routing.hh - Common definitions and declarations for routers and filters.
 */

#include <maxscale/ccdefs.hh>
#include <maxscale/target.hh>

namespace maxscale
{
/**
 * mxs::Routable is the base type representing the session related data of a particular routing module
 * instance. Implemented by filter and router sessions.
 */
class Routable
{
public:
    virtual ~Routable() = default;

    /**
     * Called when a packet is traveling downstream, towards a backend.
     *
     * @param pPacket Packet to route
     * @return 1 for success, 0 for error
     */
    virtual int32_t routeQuery(GWBUF* pPacket) = 0;

    /**
     * Called when a packet is traveling upstream, towards the client.
     *
     * @param pPacket Packet to route
     * @param down Response source
     * @param reply Reply information
     * @return 1 for success, 0 for error
     */
    virtual int32_t clientReply(GWBUF* pPacket, const mxs::ReplyRoute& down, const mxs::Reply& reply) = 0;
};
}

/**
 * Routing capability type. Indicates what kind of input a router or
 * a filter accepts.
 *
 *       The capability bit ranges are:
 *           0-15:  general capability bits
 *           16-23: router specific bits
 *           24-31: filter specific bits
 *           32-39: authenticator specific bits
 *           40-47: protocol specific bits
 *           48-55: monitor specific bits
 *           56-63: reserved for future use
 *
 * @note The values of the capabilities here *must* be between 0x0000
 *       and 0x8000, that is, bits 0 to 15.
 */
enum mxs_routing_capability_t
{
    /**
     * routeQuery is called with one packet per buffer (currently always on). The buffer is always contiguous.
     *
     * Binary: 0b0000000000000001
     */
    RCAP_TYPE_STMT_INPUT = (1 << 0),

    /**
     * The transaction state and autocommit mode of the session are tracked; implies RCAP_TYPE_STMT_INPUT.
     *
     * Binary: 0b0000000000000011
     */
    RCAP_TYPE_TRANSACTION_TRACKING = (1 << 1) | RCAP_TYPE_STMT_INPUT,

    /**
     * Results are delivered as a set of complete packets. The buffer passed to clientReply can contain
     * multiple packets.
     *
     * Binary: 0b0000000000000100
     */
    RCAP_TYPE_PACKET_OUTPUT = (1 << 2),

    /**
     * Request and response tracking: tells when a response to a query is complete. Implies
     * RCAP_TYPE_STMT_INPUT and RCAP_TYPE_PACKET_OUTPUT.
     *
     * Binary: 0b0000000000001101
     */
    RCAP_TYPE_REQUEST_TRACKING = (1 << 3) | RCAP_TYPE_STMT_INPUT | RCAP_TYPE_PACKET_OUTPUT,

    /**
     * clientReply is called with one packet per buffer. The buffer is always contiguous.
     *
     * Binary: 0b0000000000010000
     */
    RCAP_TYPE_STMT_OUTPUT = (1 << 4),

    /**
     * All result are delivered in one buffer.
     *
     * Binary: 0b0000000000100000
     */
    RCAP_TYPE_RESULTSET_OUTPUT = (1 << 5),

    /**
     * Track session state changes; implies RCAP_TYPE_PACKET_OUTPUT
     *
     * Binary: 0b0000000001000100
     */
    RCAP_TYPE_SESSION_STATE_TRACKING = (1 << 6) | RCAP_TYPE_PACKET_OUTPUT,
};

#define RCAP_TYPE_NONE 0

/**
 * Determines whether a particular capability type is required.
 *
 * @param capabilites The capability bits to be tested.
 * @param type        A particular capability type or a bitmask of types.
 *
 * @return True, if @c type is present in @c capabilities.
 */
static inline bool rcap_type_required(uint64_t capabilities, uint64_t type)
{
    return (capabilities & type) == type;
}
