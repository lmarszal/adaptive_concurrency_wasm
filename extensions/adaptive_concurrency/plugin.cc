#include <atomic>

#include "proxy_wasm_intrinsics.h"
#include "plugin.hpp"

class ACPluginRootContext: public PluginRootContext {
public:
  explicit ACPluginRootContext(uint32_t id, StringView root_id): PluginRootContext(id, root_id) {
  }
};

class ACPluginContext : public PluginContext {
public:
  explicit ACPluginContext(uint32_t id, RootContext* root) : PluginContext(id, root) {
  }
};

static RegisterContextFactory register_PluginContext(CONTEXT_FACTORY(ACPluginContext), ROOT_FACTORY(ACPluginRootContext), "lmarszal.http.adaptive_concurrency_filter");
