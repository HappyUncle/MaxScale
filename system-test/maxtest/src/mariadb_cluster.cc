/*
 * Copyright (c) 2021 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl11.
 *
 * Change Date: 2025-01-25
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */

#include <maxtest/mariadb_cluster.hh>

using std::string;
namespace
{
const string type_mariadb = "mariadb";
}

MariaDBCluster::MariaDBCluster(SharedData& shared, const string& network_config)
    : Mariadb_nodes(shared, "node", "server", network_config)
{
}

bool MariaDBCluster::setup()
{
    return Mariadb_nodes::setup();
}

const std::string& MariaDBCluster::type_string() const
{
    return type_mariadb;
}
