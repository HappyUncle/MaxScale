/*
 * Copyright (c) 2019 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl11.
 *
 * Change Date: 2025-03-24
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */

#pragma once

#include <maxscale/ccdefs.hh>
#include <atomic>
#include <map>
#include <mutex>
#include <thread>
#include <maxsql/mariadb_connector.hh>

class MariaDBQueryManager
{
public:
    ~MariaDBQueryManager();

    int64_t schedule_query(const std::string& user, const std::string& pw, const std::string& host,
                           int port, const std::string& query);

    struct QueryStatus
    {
        enum class Status {NOT_EXIST, STILL_RUNNING, COMPLETE_OK, COMPLETE_ERROR};
        Status status {Status::NOT_EXIST};

        std::string errormsg;
        std::unique_ptr<mxq::QueryResult> res;
    };
    QueryStatus get_result(int64_t id);

private:
    std::atomic_int64_t m_next_query_id {0};

    std::mutex m_thread_lock;
    std::atomic_bool m_busy {false};
    std::thread m_query_thread;

    std::mutex m_results_lock;
    std::map<int64_t, QueryStatus> m_results;
};