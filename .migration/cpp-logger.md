# Migration Plan: cpp-logger

Selected: 2026-07-30. Source: `git@github.com:llnl/cpp-logger.git` (develop). Target: `ssh://git@czgitlab.llnl.gov:7999/dftracer/cpp-logger.git`.

## Findings

- Local checkout exists at project root; `develop` up to date with origin.
- GitLab repo already exists with `main` @ b43016d.
- **No `.github/workflows/`** — no CI to convert. Will author a fresh `.gitlab-ci.yml` (CMake build + ctest, gcc image).
- **No `docs/`** — GitLab Pages skipped.
- `gitlab` remote added and verified (`ls-remote` OK).

## Steps

1. [x] Fetch/pull latest `develop`
2. [x] Add + verify `gitlab` remote
3. [x] Author `.gitlab-ci.yml`: build (cmake, `-DCPP_LOGGER_ENABLE_TESTING=ON` — actual option name) + ctest, on push & MR, image `gcc:12`
4. [x] Local test: cmake configure/build/ctest passes; YAML parses
5. [x] Commit on `gitlab-migration` branch (5a93284)
6. [x] Pushed `develop` (551ca66), tags v0.0.1–v0.0.8, `gitlab-migration` (5a93284) to gitlab
7. [x] Pipeline URL: https://czgitlab.llnl.gov/dftracer/cpp-logger/-/pipelines
8. [ ] User merges `gitlab-migration` → `develop` after green pipeline

## Status log

- 2026-07-30: remote added, plan created.
- 2026-07-30: Test option is `CPP_LOGGER_ENABLE_TESTING` (not `CPP_LOGGER_BUILD_TEST`). Authored `.gitlab-ci.yml` (image gcc:12, apt-installs cmake, configure/build/ctest; rules on push + merge_request_event).
- 2026-07-30: Local validation in scratch build dir: configure and build succeeded (shared lib libcpp-logger.so). `ctest --output-on-failure` printed "No tests were found!!!" with exit=0 — `test/CMakeLists.txt` is empty, so the repo defines no actual tests; CI will still pass. YAML validated with python (`yaml.safe_load` OK).
- 2026-07-30: Created `gitlab-migration` from develop, committed only `.gitlab-ci.yml` as 5a93284 "ci: add GitLab CI for czgitlab migration".
- 2026-07-30: Pushed to gitlab: develop @ 551ca66 (new branch), tags v0.0.1..v0.0.8 (all new), gitlab-migration @ 5a93284 (new branch). No failures. Nothing pushed to origin/GitHub. Local repo returned to `develop`.

## Executed changes (what to undo on revert)

Everything added by this migration — see `.migration/REVERT.md` for the generic procedure:

- `gitlab` remote → `git remote remove gitlab`
- Local + GitLab branch `gitlab-migration` (5a93284, adds only `.gitlab-ci.yml`)
- GitLab repo content: `develop` @ 551ca66, tags v0.0.1–v0.0.8 (mirror only; archive/delete `dftracer/cpp-logger` on czgitlab if desired)
- No GitHub-side changes were made — nothing to restore there. `.gitlab-ci.yml` is NOT on `develop` yet; if it gets merged, `git rm .gitlab-ci.yml` + push to origin reverts it.
