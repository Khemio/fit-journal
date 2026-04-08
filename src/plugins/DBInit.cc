#include "DBInit.h"

using namespace drogon;

/* 
    Application of settings to SQLite has to happen per connection. 
    If there is more then 1 connection, Drogon chooses connection randomly to execute queries.
    To apply settings to all connections:
    1. Block all connections with transactions.
    2. Make one query with one setting to each connection (for N connections make N identical queries).
    3. Release transactions.
    4. Repeat steps 1-3 for each setting.
*/

void DBInit::initAndStart(const Json::Value &config)
{
    auto client = app().getDbClient();
    if (client) {
        try {
                client->execSqlSync("PRAGMA foreign_keys = ON;");
                LOG_INFO << "Foreign keys are enabled now." ;
        } catch (const orm::DrogonDbException &e) {
            LOG_ERROR << "Error executing PRAGMA foreign_keys = ON: " << e.base().what();
        }
    } else {
        LOG_ERROR << "Could not get DBClient on DBInit plugin start";
    }
}

void DBInit::shutdown() 
{
    
}
