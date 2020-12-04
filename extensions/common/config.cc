#include "config.hpp"
#include "nlohmann_json.hpp"
#include "proxy_wasm_intrinsics.h"

using nlohmann::json;

const char* config_default_vm_name = "adaptive_concurrency_filter";

void to_json(json& j, const Config& c) {
    j = json{{"limit", c.limit}, {"window_size", c.window_size}, {"vm_id", c.vm_name}};
}

void from_json(const json& j, Config& c) {
    j.at("limit").get_to(c.limit);
    j.at("window_size").get_to(c.window_size);
    j.at("vm_name").get_to(c.vm_name);
}

Config load_config()
{
    auto cfg_string = getConfiguration()->toString();
    Config cfg;

    // parse from json
    if (!json::accept(cfg_string)) {
        LOG_ERROR("invalid configuration json");
        cfg = Config { 0, 0 };
    }
    else
    {
        auto j = json::parse(cfg_string, nullptr, false);
        cfg = j.get<Config>();
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
    if (cfg.vm_name.length() <= 0)
    {
        cfg.vm_name = config_default_vm_name;
    }

    return cfg;
}

Config default_config()
{
    return Config { config_default_limit, config_default_window_size, config_default_vm_name };
}
