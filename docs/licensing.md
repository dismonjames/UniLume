# Licensing map

This document records what is present in the imported x-unikey 1.0.4
snapshot; it is not a new license grant and is not legal advice.

| Path | Evidence in the repository | Working classification |
| --- | --- | --- |
| `src/vnconv/` | Most source headers say GPL version 2 or later | GPL-2.0-or-later |
| `src/ukinterface/` | Headers say GPL version 2 or later | GPL-2.0-or-later |
| `src/ukengine/keycons.h` | Header says GPL version 2 or later | GPL-2.0-or-later |
| Most of `src/ukengine/` | Headers say “GNU Lesser General Public License”, version 2 or later | LGPL-2.0-or-later, subject to wording note below |
| Most of `src/platform/legacy/` | Headers say LGPL version 2 or later | LGPL-2.0-or-later where stated |
| `third_party/imdkit/` | Per-file DEC/HP/Sun/Fujitsu/X Consortium permission notices | Separate permissive terms; retain each notice |
| New CMake/test/docs files | SPDX headers or repository notice | GPL-2.0-or-later |

## Important uncertainties

`COPYING` is the unmodified GNU **Library** General Public License version 2,
dated June 1991. Several source headers instead call their terms the GNU
**Lesser** General Public License “version 2”. “Lesser” became the official
name at version 2.1. The version number, the “or later” language, and the
bundled `COPYING` support the common `LGPL-2.0-or-later` classification, but
the naming mismatch should be reviewed by the maintainer or legal counsel.

The following inherited groups include files without an individual license
header: `src/byteio/`, several precompiled-header placeholders, and parts of
`src/platform/legacy/`. They appear inside the x-unikey source snapshot and
build graph, but this repository does not invent a per-file license for them.

Some legacy XIM files contain copyright notices whose full permission text is
not in the first header block. Their provenance and exact terms should also be
verified before restoring that build.

## Distribution consequence

The current `unilume_engine` target includes `vnconv` and `ukinterface`, whose
headers state GPL-2.0-or-later. The combined target must therefore be treated
as GPL-2.0-or-later even though much of the underlying engine states LGPL
terms. This does not relicense separable inherited files.

`LICENSE` supplies GPL version 2 and `COPYING` preserves the bundled Library
GPL version 2. Per-file third-party notices remain controlling where
applicable.
