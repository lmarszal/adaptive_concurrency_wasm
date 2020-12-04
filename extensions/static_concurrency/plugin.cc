#include <atomic>

#include "proxy_wasm_intrinsics.h"
#include "plugin.hpp"

class SCPluginRootContext: public PluginRootContext {
public:
  explicit SCPluginRootContext(uint32_t id, StringView root_id): PluginRootContext(id, root_id) {
  }

  uint32_t getLimitInternal() override
  {
    return config_.limit;
  }
};

class SCPluginContext : public PluginContext {
public:
  explicit SCPluginContext(uint32_t id, RootContext* root) : PluginContext(id, root) {
  }
};

static RegisterContextFactory register_PluginContext(CONTEXT_FACTORY(SCPluginContext), ROOT_FACTORY(SCPluginRootContext), "lmarszal.http.static_concurrency_filter");
