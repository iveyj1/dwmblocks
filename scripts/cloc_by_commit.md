# dwmblocks: changes and cloc since the fork

Committed history through `8195c96169d5191f1960bfe41d64e5c4d4e7ca5f`; cloc **2.10**.

## Provenance and method

- Upstream: https://github.com/torrinfail/dwmblocks.
- Original fork baseline: `8cedd220684064f1433749ed2a19a6184c22cf07`.
- Upstream HEAD verified during this review: `8cedd220684064f1433749ed2a19a6184c22cf07`.
- dwmblocks is a companion tool, not a fork of an official suckless.org repository. Baseline verified against torrinfail/dwmblocks as the upstream/local merge-base.
- Inline status shell commands in blocks.h are counted as C configuration lines, not again as script lines. Standalone status helpers are runtime scripts.
- Main tables follow first-parent history, oldest first; dates are author dates. Merge rows include their complete first-parent delta, including conflict resolutions. Side commits are detailed separately, not added again to totals.
- C = runtime .c/.h files; one effective configuration header (tracked config.h/blocks.h, otherwise its .def.h template). Support C = test/diagnostic C sources. Runtime scripts are counted separately from build/test/update scripts.
- cloc code lines exclude blanks/comments. Deltas are signed net changes vs the preceding row, not diff insertion/deletion counts. Zero does not imply no behavior change. Embedded shell strings in C count as C; shell help heredocs follow cloc classification.
- Build = Makefile/config.mk. Docs, terminfo, binaries, images, generated buildinfo.h, unused configuration templates, and reporting files under scripts/ are excluded. --skip-uniqueness prevents duplicate-file suppression.
- Historical counts use Git blobs. WORKTREE, when present, includes staged/unstaged changes and nonignored untracked regular files; delta is vs HEAD, never folded into committed history. Report-only edits do not create a WORKTREE row.
- Requires Python 3.9+, Git and cloc. Regenerate: `python3 scripts/update_cloc_by_commit.py`. Validate freshness: `python3 scripts/update_cloc_by_commit.py --check`.
- Regeneration is offline: upstream provenance records the review-time verification, not a fresh network check.
- Add reviewed descriptions to scripts/cloc-history.json when a commit subject is unclear. Otherwise new commits automatically use their subjects. Keep baseline fixed; do not move it to a later upstream merge-base.

## Runtime changes

| Commit | Date | C code | Δ C | Script code | Δ scripts | Change |
|---|---|---:|---:|---:|---:|---|
| `8cedd22` | 2024-08-23 | 194 | — | 0 | — | torrinfail/dwmblocks upstream fork baseline. |
| `86ff600` | 2026-07-06 | 197 | +3 | 0 | 0 | Track local Void status configuration (blocks.h); comment out unused returnStatus variable; update ignores. |
| `e8ae0fa` | 2026-08-24 | 198 | +1 | 29 | +29 | Add battery charge-rate and Wi-Fi status scripts, local block commands and install integration. |
| `260e2e1` | 2026-08-24 | 198 | 0 | 29 | 0 | Add b build/optional-install script. |
| `2d64e66` | 2026-08-31 | 198 | 0 | 29 | 0 | Document status blocks, helpers and installation. |
| `1d0ad3a` | 2026-09-04 | 198 | 0 | 29 | 0 | Exclude I/O-wait from busy CPU usage; align default block configuration. |
| `fc9740d` | 2026-09-11 | 229 | +31 | 29 | 0 | Bound status assembly, harden delimiters and empty/failing block handling; add C regression tests. |
| `ad44ed0` | 2026-09-13 | 229 | 0 | 28 | -1 | Document Mint/Void setup and remove Wi-Fi helper debug-file write. |
| `8195c96` | 2026-09-13 | 229 | 0 | 28 | 0 | Correct README dependency documentation. |
| `WORKTREE` | uncommitted | 229 | 0 | 28 | 0 | Pending changes vs HEAD: AGENTS.md, README.md, scripts/cloc-history.json, scripts/test_cloc_history.py, scripts/update_cloc_by_commit.py |

## Supporting code and build files

| Commit | Support C | Δ C | Support scripts | Δ scripts | Build | Δ build | Combined code | Δ combined |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| `8cedd22` | 0 | — | 0 | — | 23 | — | 217 | — |
| `86ff600` | 0 | 0 | 0 | 0 | 23 | 0 | 220 | +3 |
| `e8ae0fa` | 0 | 0 | 0 | 0 | 23 | 0 | 250 | +30 |
| `260e2e1` | 0 | 0 | 7 | +7 | 23 | 0 | 257 | +7 |
| `2d64e66` | 0 | 0 | 7 | 0 | 23 | 0 | 257 | 0 |
| `1d0ad3a` | 0 | 0 | 7 | 0 | 23 | 0 | 257 | 0 |
| `fc9740d` | 68 | +68 | 7 | 0 | 27 | +4 | 360 | +103 |
| `ad44ed0` | 68 | 0 | 7 | 0 | 27 | 0 | 359 | -1 |
| `8195c96` | 68 | 0 | 7 | 0 | 27 | 0 | 359 | 0 |
| `WORKTREE` | 68 | 0 | 7 | 0 | 27 | 0 | 359 | 0 |

Combined includes all five counted categories; excluded files remain excluded.

## Baseline → committed HEAD totals

| Category | Code baseline → HEAD (net) | Blank baseline → HEAD | Comment baseline → HEAD |
|---|---:|---:|---:|
| C | 194 → 229 (+35) | 22 → 23 | 9 → 12 |
| Runtime scripts | 0 → 28 (+28) | 0 → 10 | 0 → 1 |
| Support C | 0 → 68 (+68) | 0 → 6 | 0 → 0 |
| Support scripts | 0 → 7 (+7) | 0 → 1 | 0 → 0 |
| Build | 23 → 27 (+4) | 9 → 10 | 4 → 4 |

## Commits integrated by merges

Counts below are each side commit’s snapshot and delta vs its own first parent. These are **not additive** with the main tables.

No post-fork merges.

## Files counted at committed HEAD

- **C:** `blocks.h`, `dwmblocks.c`.
- **Runtime scripts:** `dwmblocks-charge-rate`, `dwmblocks-wifi`.
- **Support C:** `tests/status.c`.
- **Support scripts:** `b`.
- **Build:** `Makefile`.

## Latest change

WORKTREE — Pending changes vs HEAD: AGENTS.md, README.md, scripts/cloc-history.json, scripts/test_cloc_history.py, scripts/update_cloc_by_commit.py
C: 229 (0); Runtime scripts: 28 (0); Support C: 68 (0); Support scripts: 7 (0); Build: 27 (0)

Maintenance: regenerate after each modification and again after committing (or switching branches); include the latest code/script totals and net deltas in the change summary. Reporting-only changes legitimately have zero measured delta.
