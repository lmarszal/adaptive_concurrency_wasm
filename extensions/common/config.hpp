#pragma once

#include <stdint.h>
#include <string>

const uint32_t config_default_limit = 5;
const uint32_t config_default_window_size = 5000;

struct Config
{
    uint32_t limit;
    uint32_t window_size;
    std::string prometheus_cluster_name;
    std::string prometheus_longrtt_query;
    std::string prometheus_limit_query;
};

Config load_config();

Config default_config();
