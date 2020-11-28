#!/bin/bash

cat <<- EOM
apiVersion: v1
kind: ConfigMap
metadata:
  name: adaptive-concurrency-wasm
binaryData:
EOM

for FILE in "$@" ; do
    CONTENT=`base64 ${FILE}`
    FILENAME=`basename ${FILE}`
    echo "  ${FILENAME}: ${CONTENT}"
done
