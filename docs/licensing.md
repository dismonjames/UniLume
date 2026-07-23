# Licensing map

This document records what is present in the imported x-unikey 1.0.4
snapshot; it is not a new license grant and is not legal advice.

The file-level comparison, upstream path mapping, checksums, and status
definitions supporting this map are recorded in
[provenance.md](provenance.md).

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

The provenance of the following headerless groups is **Verified** by exact
comparison with the published x-unikey 1.0.4 archive:

- all four files under `src/byteio/`;
- `src/ukengine/stdafx.cpp` and `stdafx.h`;
- the GTK/XIM `dummy.cpp` helpers and GTK `nls.h`;
- XIM `ukopt.c`, `ukopt.h`, and the install/uninstall scripts.

Their exact per-file license remains **Unresolved**. Package-level context
strongly indicates copyleft intent, but it does not establish whether each
file is GPL, Library/Lesser GPL, version 2 only, or version 2-or-later. No
license header has been added.

The legacy `IC.c`, `IC.h`, and the initial block of `xim.c` contain complete
Sun Microsystems/Hewlett-Packard permission text, not copyright-only notices.
`xim.c` additionally contains Pham Kim Long's LGPL version 2-or-later notice.
These embedded terms are **Verified** and must remain intact.

The official UniKey source page currently describes linked source code as GNU
GPL, while the 1.0.4 archive contains GNU Library GPL version 2 and many files
say GNU Lesser GPL version 2 or later. This evidence does not justify silently
choosing one exact license for headerless files.

## Distribution consequence

The current `unilume_engine` target includes `vnconv` and `ukinterface`, whose
headers state GPL-2.0-or-later. The combined target must therefore be treated
as GPL-2.0-or-later even though much of the underlying engine states LGPL
terms. This does not relicense separable inherited files.

`LICENSE` supplies GPL version 2 and `COPYING` preserves the bundled Library
GPL version 2. Per-file third-party notices remain controlling where
applicable.

## Release gate

Before enabling or distributing the historical GUI/XIM/GTK targets, resolve
the headerless-file questions listed in `docs/provenance.md` or obtain legal
review. The current engine target continues to be distributed under
GPL-2.0-or-later because it includes files with explicit
GPL-2.0-or-later notices; that combined-target conclusion does not relicense
separable inherited files.
