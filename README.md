# adaptive_concurrency_wasm
Gradient2 Adaptive Concurrency Filter WASM plugin for Envoy

## Build (temp)

1) Download envoy sources into `./envoy`
1) Run `make` (build project)
1) Run `kubectl apply -f example/configmap.yaml` (deploy wasm binaries)
1) Add pod annotations to your deployment (see below)
1) Run `kubectl apply -f example/static_concurrency_filter.yaml` (deploy filter)

Pod annotations:
```
sidecar.istio.io/userVolume: '[{"name":"wasmfilters-dir","configMap": {"name": "adaptive-concurrency-wasm"}}]'
sidecar.istio.io/userVolumeMount: '[{"mountPath":"/var/local/lib/wasm-filters","name":"wasmfilters-dir"}]'
sidecar.istio.io/statsInclusionRegexps: http_static_concurrency_filter.*,http_adaptive_concurrency_filter.*
```

To debug WASM run:
`kubectl exec -it $POD  -c istio-proxy -- curl -X POST localhost:15000/logging\?wasm=debug`

## Links
 - https://github.com/Netflix/concurrency-limits/blob/85f7ec4b3d38633a3d3b399073cb9f7796e2ec11/concurrency-limits-core/src/main/java/com/netflix/concurrency/limits/limit/Gradient2Limit.java
 - https://banzaicloud.com/blog/envoy-wasm-filter/
 - https://github.com/proxy-wasm/proxy-wasm-cpp-sdk#docker