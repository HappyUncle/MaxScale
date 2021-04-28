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

#include "internal/mariadb_query_manager.hh"

using std::string;

int64_t
MariaDBQueryManager::schedule_query(const std::string& user, const std::string& pw, const std::string& host,
                                    int port, const std::string& query)
{
    int64_t rval = -1;
    if (!m_busy)
    {
        std::lock_guard<std::mutex> lock(m_thread_lock);
        if (m_query_thread.joinable())
        {
            m_query_thread.join();
        }

        if (!m_busy)
        {
            auto query_id = m_next_query_id++;
            {
                std::lock_guard<std::mutex> result_lock(m_results_lock);
                QueryStatus new_status;
                new_status.status = QueryStatus::Status::STILL_RUNNING;
                m_results.insert(std::make_pair(query_id, std::move(new_status)));
            }

            auto run_query = [this, user, pw, host, port, query, query_id] () {
                mxq::MariaDB conn;
                auto& sett = conn.connection_settings();
                sett.timeout = 10;
                sett.user = user;
                sett.password = pw;

                std::unique_ptr<mxq::QueryResult> res;
                string errormsg;

                if (conn.open(host, port))
                {
                    res = conn.query(query);
                }

                if (!res)
                {
                    errormsg = conn.error();
                }

                // Query finished, store result.
                std::lock_guard<std::mutex> result_lock(m_results_lock);
                auto it = m_results.find(query_id);
                if (it != m_results.end())
                {
                    auto& elem = it->second;
                    if (res)
                    {
                        elem.status = QueryStatus::Status::COMPLETE_OK;
                        elem.res = std::move(res);
                    }
                    else
                    {
                        elem.status = QueryStatus::Status::COMPLETE_ERROR;
                        elem.errormsg = std::move(errormsg);
                    }
                }
                m_busy = false;
            };

            m_busy = true;
            m_query_thread = std::thread(run_query);
            rval = query_id;
        }
    }
    return rval;
}

MariaDBQueryManager::QueryStatus MariaDBQueryManager::get_result(int64_t id)
{
    QueryStatus rval;
    std::lock_guard<std::mutex> lock(m_results_lock);
    auto it = m_results.find(id);
    if (it != m_results.end())
    {
        // If query is still running, don't remove the entry from the map.
        if (it->second.status == QueryStatus::Status::STILL_RUNNING)
        {
            rval.status = QueryStatus::Status::STILL_RUNNING;
        }
        else
        {
            rval = std::move(it->second);
            m_results.erase(it);
        }
    }
    return rval;
}

MariaDBQueryManager::~MariaDBQueryManager()
{
    std::lock_guard<std::mutex> lock(m_thread_lock);
    if (m_query_thread.joinable())
    {
        m_query_thread.join();
    }
}
