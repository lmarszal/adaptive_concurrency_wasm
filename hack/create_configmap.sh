#!/bin/bash

cat <<- EOM
apiVersion: v1
kind: ConfigMap
metadata:
  name: adaptive-concurrency-wasm
binaryData:
EOM

CONTENT=`base64 ${2}`
FILENAME=${1}
echo "  ${FILENAME}: ${CONTENT}"
