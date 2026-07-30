# Reverting a Migration (back to GitHub-only)

The migration is additive: it only adds a `gitlab` remote, a `.gitlab-ci.yml` (+ `pages` job), and a `gitlab-migration` branch. Nothing on GitHub is ever changed, so reverting is just removing the additions.

## Per-project revert steps

Run inside `<project>/`:

```bash
# 1. Remove the GitLab remote (local only)
git remote remove gitlab

# 2. Drop the local migration branch
git checkout develop            # or the project's default branch
git branch -D gitlab-migration

# 3. If .gitlab-ci.yml was merged into develop, remove it with a revert commit
git rm .gitlab-ci.yml
git commit -m "ci: remove GitLab CI (revert to GitHub-only)"
git push origin develop         # this is the ONLY push to GitHub a revert needs

# 4. Ensure local develop matches GitHub
git fetch origin && git reset --hard origin/develop   # only if you want to discard unmerged gitlab-only commits
```

## GitLab-side cleanup (optional)

- Delete or archive the repo at `https://czgitlab.llnl.gov/dftracer/<project>` (Settings → General → Advanced). Archiving is safer than deleting.
- Or just delete the `gitlab-migration` branch there and disable CI/CD + Pages in project settings if you want to keep the mirror.

## Notes

- Step 3 is only needed if `.gitlab-ci.yml` reached `develop`; if it only lives on `gitlab-migration`, deleting that branch (local + GitLab) is enough.
- `.migration/<project>.md` records exactly what was pushed (branches, tags, SHAs) — consult it to know what to undo.
- The skill (`.claude/skills/gitlab-migrate/SKILL.md`) and this folder can stay; they have no effect on GitHub.
