#include "prometheus.hpp"
#include <vector>
#include <string>
#include <cstdio>
#include <algorithm>
#include "json.hpp"
#include "state.hpp"
#include "expavg_measurement.hpp"

using nlohmann::json;

// from https://stackoverflow.com/questions/18307429/encode-decode-url-in-c/35348028
std::string UriEncode(const std::string_view& sSrc)
{
   // don't care about performance (much) - this will be executed only twice
   const char RESERVED[20] = "!#$%&'()*+,/:;=?@[]";
   bool SAFE[256];
   for(int i=0; i<256; i++) {
       SAFE[i] = true;
   }
   for(int i=0; i<20; i++) {
       SAFE[RESERVED[i]] = false;
   }

   const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
   const unsigned char * pSrc = (const unsigned char *)sSrc.cbegin();
   const int SRC_LEN = sSrc.length();
   unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
   unsigned char * pEnd = pStart;
   const unsigned char * const SRC_END = pSrc + SRC_LEN;

   for (; pSrc < SRC_END; ++pSrc)
   {
      if (SAFE[*pSrc]) 
         *pEnd++ = *pSrc;
      else
      {
         // escape this char
         *pEnd++ = '%';
         *pEnd++ = DEC2HEX[*pSrc >> 4];
         *pEnd++ = DEC2HEX[*pSrc & 0x0F];
      }
   }

   std::string sResult((char *)pStart, (char *)pEnd);
   delete [] pStart;
   return sResult;
}

std::vector<double> parseResponseBody(std::string_view body)
{
    std::vector<double> measures;

    if (!json::accept(body))
    {
        LOG_ERROR("prometheus response is not a valid json");
        return measures;
    }
    auto j = json::parse(body, nullptr, false);

    if (!j.contains("status"))
    {
        LOG_ERROR("expected prometheus response to contain status");
        return measures;
    }
    if (j["status"].get<std::string>() != "success")
    {
        LOG_ERROR("expected prometheus status to be success");
        return measures;
    }
    if (!j.contains("data") || !j["data"].contains("result") || !j["data"]["result"].is_array())
    {
        LOG_ERROR("malformed prometheus response - expected data.result to be present and type array");
        return measures;
    }
    auto results = j["data"]["result"];
    auto count = results.size();
    measures.reserve(count);

    for (uint32_t i = 0; i < count; i++)
    {
        auto v = results.at(i);
        if (!v.contains("value") || !v["value"].is_array() || v["value"].size() < 2)
        {
            LOG_ERROR("malformed prometheus response - expected data.result[].value to be present, type array, at least two elements");
            continue;
        }
        auto measure_string = v["value"].at(1).get<std::string>();
        double measure;
        sscanf(measure_string.c_str(), "%lf", &measure);
        if (measure > 0) {
            measures.push_back(measure);
        }
    }

    return measures;
}

double get_median(std::vector<double> values)
{
    std::sort(values.begin(), values.end());
    return values[values.size()/2];
}

void prometheus_query(RootContext* ctx, std::string_view clusterName, std::string_view query, prometheus_callback_fn callback)
{
    auto context_id = ctx->id();
    auto http_call_callback = [context_id, callback](uint32_t _headers, size_t body_size, uint32_t _trailers) {
        // inspired by https://github.com/envoyproxy/envoy/blob/488973532ac72f56473bd2b6f770f0a011bf14a1/test/extensions/filters/http/wasm/test_data/test_async_call_cpp.cc#L32-L49
        if (body_size == 0)
        {
            LOG_ERROR("prometheus query failed");
            return;
        }
        getContext(context_id)->setEffectiveContext();
        auto body = getBufferBytes(WasmBufferType::HttpCallResponseBody, 0, body_size);
        LOG_WARN(body.get()->toString());
        auto values = parseResponseBody(body.get()->toString());
        if (values.size() > 0)
        {
            auto value = get_median(values);
            if (callback != nullptr)
            {
                callback(value);
            }
        }
    };

    auto path = std::string("/api/v1/query?query=") + UriEncode(query);
    ctx->httpCall(clusterName, {{":method", "GET"},{":path", path},{":authority",""}}, nullptr, {}, 500, http_call_callback);
}

void on_longRtt_query_result(double result)
{
    auto state = ExpAvgMeasurement(result, result * 10, 10);
    setExpAvgState(state.toString());
}

void on_limit_query_result(double result)
{
    setLimit((uint32_t)result);
}
