#!/bin/bash

# Ensure any error leads to script termination
set -e

echo "Starting build process with BUILD_TYPE=${BUILD_TYPE}..."

make conan-profile
make conan-build BUILD_TYPE=$BUILD_TYPE

echo "Build completed, setting permissions..."
chmod -R +x /app/build

echo "Permissions set, script completed."
