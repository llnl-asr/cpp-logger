#!/bin/bash
# Runs inside the shared Flux allocation via `flux proxy <jobid> bash .gitlab/ci/build-test-docs.sh`.
# Module names are hardcoded so nothing depends on environment expansion
# across the flux proxy boundary.
set -ex

module load gcc/11.2.1 python/3.13.2
export CC=gcc CXX=g++

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCPP_LOGGER_ENABLE_TESTING=ON
cmake --build build -j "$(nproc)"
ctest --test-dir build --output-on-failure

python3 -m venv .venv-docs
source .venv-docs/bin/activate
pip install -r docs/requirements.txt
sphinx-build -b html docs public
