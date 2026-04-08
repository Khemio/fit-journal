#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>

class DBInit : public drogon::Plugin<DBInit>
{
  public:
    DBInit() {}
    /// This method must be called by drogon to initialize and start the plugin.
    /// It must be implemented by the user.
    void initAndStart(const Json::Value &config) override;

    /// This method must be called by drogon to shutdown the plugin.
    /// It must be implemented by the user.
    void shutdown() override;
};

