#!/usr/bin/bash

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

sh "${SCRIPT_DIR}/clean.sh"
sh "${SCRIPT_DIR}/generate.sh"
sh "${SCRIPT_DIR}/build.sh"
