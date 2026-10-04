# dwmblocks maintenance

Keep `blocks.h` and `blocks.def.h` consistent when changing the intended default
configuration. Status tests must not invoke real hardware or alter the X session.

## Change-size history (required)

After every modification run `python3 scripts/update_cloc_by_commit.py`.
`scripts/cloc_by_commit.md` is generated locally and gitignored; never stage it. Regenerate after committing or changing
branches to replace WORKTREE with the actual commit row. Do not create commits
without the user's request just to refresh the report.

Check freshness with `python3 scripts/update_cloc_by_commit.py --check`. Test
reporting changes with `python3 scripts/test_cloc_history.py`.

Report latest runtime C/runtime-script totals and net deltas in the final response;
include support C/scripts/build deltas when nonzero. Explicitly report zero measured
change for documentation/reporting-only changes.

Maintain the fixed fork baseline and scope in `scripts/cloc-history.json`.
Upstream is torrinfail/dwmblocks, not an official suckless.org repository. Add
reviewed descriptions for unclear subjects; new commits otherwise use their
subjects. Preserve the single effective blocks header policy and runtime/support
split. WORKTREE includes staged/unstaged and nonignored new source files; do not
attribute those changes to an existing commit.
