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
7. [x] Pipeline URL: <https://czgitlab.llnl.gov/dftracer/cpp-logger/-/pipelines>
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

## GitLab Pages (added 2026-07-30)

- Project had no docs, so a minimal Sphinx tree was authored on `gitlab-migration`: `docs/conf.py`, `index.rst`, `introduction.rst` (intro/build/quick-start), `api.rst` (C++ `Logger` + C `clogger` API from the public headers), `docs/requirements.txt`.
- Docs are host-neutral Sphinx — the same source builds on ReadTheDocs (GitHub) with no changes. `conf.py` falls back to alabaster if `sphinx_rtd_theme` is missing.
- `.gitlab-ci.yml` gained `stages: [test, deploy]` and a `pages` job (python:3.11, `sphinx-build -b html docs public`) on `develop` + temporarily `gitlab-migration` (remove the temp rule after merge).
- Local build verified: `python3 -m sphinx -b html docs …` → build succeeded; YAML OK.
- Revert: covered by removing `.gitlab-ci.yml`; optionally `git rm -r docs/` if the authored docs are unwanted, and delete the Pages deployment on czgitlab (see `REVERT.md`).
- 2026-07-30: gitlab-migration merged into gitlab develop (49ef724). New branch corona-ci: CI moved off docker images onto LC corona batch runner (1 node) via inline .corona-batch template (tags [batch, corona], SCHEDULER_PARAMETERS -N 1 -q pdebug -t 60, module-loaded gcc/python, venv for sphinx). Pages temp rule now corona-ci.
- 2026-07-30: CI moved corona → tuolumne per user request, restructured to a SINGLE 1-node allocation: one batch job build-test-docs (cce/20.0.0 CC=cc CXX=CC, cmake+ctest+sphinx in same allocation, public/ artifact); pages is a shell-runner job that republishes the artifact (no allocation). Temp pages rule still corona-ci branch.
- 2026-07-30: CI restructured to explicit Flux allocation flow on tuolumne shell runner: flux batch -N1 (sleep inf placeholder) → JOBID=$(flux job last) → each CI step (configure, build, ctest, sphinx) via flux proxy $JOBID → flux cancel in after_script. Single allocation for the whole pipeline; pages just republishes the artifact.
