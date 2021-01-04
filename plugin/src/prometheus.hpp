#pragma once
#include <string_view>
#include <functional>
#include "proxy_wasm_intrinsics.h"

using prometheus_callback_fn = std::function<void(double)>;
void prometheus_query(RootContext* ctx, std::string_view clusterName, std::string_view query, prometheus_callback_fn callback);

void on_longRtt_query_result(double result);
void on_limit_query_result(double result);
