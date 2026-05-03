#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/drogon.h>

class DBInit : public drogon::Plugin<DBInit>
{
  public:
    DBInit() {}
    void initAndStart(const Json::Value &config) override;

    void shutdown() override;
};

