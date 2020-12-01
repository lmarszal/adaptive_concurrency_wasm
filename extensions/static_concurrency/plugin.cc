#include <atomic>

#include "proxy_wasm_intrinsics.h"
#include "gradient_plugin.hpp"

class PluginRootContext: public GradientPluginRootContext {
public:
  explicit PluginRootContext(uint32_t id, StringView root_id): GradientPluginRootContext(id, root_id) {
  }

  uint32_t getLimit() override
  {
    return initial_limit_.load();
  }
};

class PluginContext : public GradientPluginContext {
public:
  explicit PluginContext(uint32_t id, RootContext* root) : GradientPluginContext(id, root) {
  }
};

static RegisterContextFactory register_PluginContext(CONTEXT_FACTORY(PluginContext), ROOT_FACTORY(PluginRootContext), "lmarszal.http.static_concurrency_filter");
