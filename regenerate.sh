#!/bin/bash

set -e

rm -rf .config generated/ android/device/ android/host/

echo "**** HOST"
cp .config-host .config
NOBUILD=1 scripts/make.sh
mkdir -p android/host/
mv generated android/host/

echo "**** DEVICE"
cp .config-device .config
NOBUILD=1 scripts/make.sh
mkdir -p android/device/
mv generated android/device/

rm .config
