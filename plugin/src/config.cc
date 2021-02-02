#include "config.hpp"
#include "json.hpp"
#include "proxy_wasm_intrinsics.h"

using nlohmann::json;

Config load_config(size_t config_buffer_length)
{
    auto conf = getBufferBytes(WasmBufferType::PluginConfiguration, 0, config_buffer_length);
    auto cfg_string = conf->toString();
    Config cfg = Config { 0, 0 };

    // parse from json
    if (!json::accept(cfg_string)) {
        LOG_ERROR("invalid configuration json");
        
    }
    else
    {
        auto j = json::parse(cfg_string, nullptr, false);
        if (j.contains("limit") && j["limit"].is_number_unsigned())
        {
            cfg.limit = j["limit"].get<uint32_t>();
        }
        if (j.contains("window_size") && j["window_size"].is_number_unsigned())
        {
            cfg.window_size = j["window_size"].get<uint32_t>();
        }
        if (j.contains("prometheus_cluster_name"))
        {
            cfg.prometheus_cluster_name = j["prometheus_cluster_name"].get<std::string>();
        }
        if (j.contains("prometheus_longrtt_query"))
        {
            cfg.prometheus_longrtt_query = j["prometheus_longrtt_query"].get<std::string>();
        }
        if (j.contains("prometheus_limit_query"))
        {
            cfg.prometheus_limit_query = j["prometheus_limit_query"].get<std::string>();
        }
    }

    // apply defaults
    if (cfg.limit == 0)
    {
        cfg.limit = config_default_limit;
    }
    if (cfg.window_size == 0)
    {
        cfg.window_size = config_default_window_size;
    }

    return cfg;
}

Config default_config()
{
    return Config { config_default_limit, config_default_window_size };
}
