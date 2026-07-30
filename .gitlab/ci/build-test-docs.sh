#!/bin/bash
# Runs ON the allocated compute node (invoked via
#   flux proxy <jobid> flux run -N 1 bash .gitlab/ci/build-test-docs.sh)
# and executes the CI inside podman containers using the same images as the
# GitHub Actions CI (gcc:12 for build/test, python:3.11 for docs) so GitLab CI
# mimics GitHub CI as closely as possible.
set -ex

# Rootless podman needs node-local storage (overlayfs does not work on NFS
# homes/workspaces). Keep image store + runroot in /var/tmp on the node.
PODMAN_STORE=/var/tmp/$USER/podman-root
PODMAN_RUNROOT=/var/tmp/$USER/podman-run
mkdir -p "$PODMAN_STORE" "$PODMAN_RUNROOT"
PODMAN="podman --root $PODMAN_STORE --runroot $PODMAN_RUNROOT"

# Build + test in the same image the GitHub CI used.
$PODMAN run --rm -v "$PWD:/ws" -w /ws docker.io/library/gcc:12 bash -ec '
  # APT::Sandbox::User=root: rootless podman has no mapped _apt uid, so apts
  # privilege drop fails with "setgroups (22: Invalid argument)".
  apt-get -o APT::Sandbox::User=root update -qq
  apt-get -o APT::Sandbox::User=root install -y -qq cmake
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCPP_LOGGER_ENABLE_TESTING=ON
  cmake --build build -j "$(nproc)"
  ctest --test-dir build --output-on-failure
'

# Docs in the same image the pages job used.
$PODMAN run --rm -v "$PWD:/ws" -w /ws docker.io/library/python:3.11 bash -ec '
  pip install -r docs/requirements.txt
  sphinx-build -b html docs public
'
