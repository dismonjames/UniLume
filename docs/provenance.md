<!-- SPDX-License-Identifier: GPL-2.0-or-later -->

# Legacy source provenance

This document records reproducible provenance evidence for the inherited
source tree. It does not grant a license, reinterpret an existing notice, or
replace legal review.

## Status vocabulary

- **Verified**: direct evidence identifies the file or statement, such as an
  exact byte comparison with the published archive or an embedded notice.
- **Strongly inferred**: multiple pieces of contextual evidence agree, but no
  direct per-file evidence was found.
- **Unresolved**: the available evidence is missing or conflicts, so no exact
  conclusion is claimed.

Provenance and license status are evaluated separately. A file can have
verified provenance while its exact per-file license remains unresolved.

## Reference snapshot

The reference used for this audit is the SourceForge release
[`x-unikey-1.0.4.tar.bz2`](https://sourceforge.net/projects/unikey/files/x-unikey/1.0.4/x-unikey-1.0.4.tar.bz2/download):

```text
SHA-256: aa7dd444853538bcba0f24c4c19692c34d4553a1df213a260c2628a7116b2dd9
Size:    561391 bytes
```

The official [UniKey source page](https://www.unikey.org/source.html) says the
engine was released with the original x-unikey Linux package and links to the
x-unikey source. The SourceForge release identifies the project owner as
`pklong`. The checksum is independently recorded by the
[FreeBSD port](https://www.freshports.org/vietnamese/x-unikey/).

Inside the archive:

- `configure.ac` identifies package `x-unikey`, version `1.0.4`;
- `README` identifies “Unikey Input Method for X-Window” and
  `Copyright (C) 2004 Pham Kim Long`;
- `ChangeLog` records the 1.0.4 release on 9 April 2006;
- `AUTHORS` identifies Pham Kim Long as the main developer;
- `COPYING` is the GNU Library General Public License, version 2.

The UniLume Git history starts with repository-preparation commit
`e73bd0a8b5f77ae749870ab90bddac00f7ed6fd9`; it does not contain the upstream
CVS history. Snapshot identity therefore rests on the published archive and
file comparison, not on reconstructed Git ancestry.

## Path mapping and comparison

The following paths were compared with `cmp` after extracting the reference
archive. “Exact” means identical file bytes.

| UniLume path | x-unikey 1.0.4 path | Result | Provenance |
| --- | --- | --- | --- |
| `src/byteio/` | `src/byteio/` | 4 of 4 files exact | Verified |
| `src/ukengine/` | `src/ukengine/` | 10 exact; 2 documented UniLume modifications | Verified |
| `src/ukinterface/` | `src/ukinterface/` | 2 of 2 files exact | Verified |
| `src/vnconv/` | `src/vnconv/` | 10 exact; 1 documented UniLume modification | Verified |
| `src/platform/legacy/gui/` | `src/gui/` | 6 of 6 files exact | Verified |
| `src/platform/legacy/unikey-gtk/` | `src/unikey-gtk/` | 7 of 7 files exact | Verified |
| `src/platform/legacy/xim/` | `src/xim/` | 13 of 13 files exact | Verified |
| `third_party/imdkit/` | `src/IMdkit/` | 34 of 34 files exact | Verified |
| `docs/legacy/` | `doc/` | 7 exact; 5 differ only in final newline layout | Verified |

The three source files that differ from the archive retain their original
headers and contain explicit UniLume modification notes:

- `src/ukengine/mactab.cpp`: bounded macro serialization and compiler
  compatibility;
- `src/ukengine/usrkeymap.cpp`: explicit standard-library includes;
- `src/vnconv/convert.cpp`: safer temporary-file handling and compiler
  compatibility.

## Headerless inherited groups

Each file below is byte-identical to the published 1.0.4 archive. That
verifies origin but does not create a per-file license grant.

| Files | Archive/build evidence | Provenance | Exact per-file license |
| --- | --- | --- | --- |
| `src/byteio/byteio.cpp`, `byteio.h`, `prehdr.cpp`, `prehdr.h` | Listed together in upstream `src/byteio/Makefile.am` as `libbyteio`; `byteio.cpp` is in the current engine target | Verified | Unresolved |
| `src/ukengine/stdafx.cpp`, `stdafx.h` | Upstream precompiled-header helpers; `stdafx.h` is included by engine sources | Verified | Unresolved |
| `src/platform/legacy/unikey-gtk/dummy.cpp` | Listed in the upstream GTK module; comment says it forces C++ linkage | Verified | Unresolved |
| `src/platform/legacy/unikey-gtk/nls.h` | Listed in the upstream GTK module and provides gettext macros | Verified | Unresolved |
| `src/platform/legacy/xim/dummy.cpp` | Listed in upstream `ukxim`; comment says it forces C++ linkage | Verified | Unresolved |
| `src/platform/legacy/xim/ukopt.c`, `ukopt.h` | Built into both upstream `ukxim` and the optional GTK module | Verified | Unresolved |
| `src/platform/legacy/xim/install.sh`, `install.sed`, `uninstall.sed` | Listed as upstream XIM distribution/install files | Verified | Unresolved |

There is strong package-level evidence of copyleft intent: neighboring
component files contain GPL/LGPL “version 2 or later” notices, the archive
includes Library GPL version 2, and the current official source page states
that source code linked there is under the GNU GPL. Those facts do not resolve
which exact grant applies to each headerless file, or the mismatch between
GPL and Library/Lesser GPL wording. No SPDX identifier or new header should be
added to these files without evidence from the copyright holder or historical
upstream records.

## Legacy platform files with explicit notices

| Files | Evidence | Status |
| --- | --- | --- |
| `src/platform/legacy/gui/*` | Each file embeds Pham Kim Long's LGPL version 2-or-later notice | Verified |
| GTK sources other than `dummy.cpp` and `nls.h` | Embedded Pham Kim Long LGPL version 2-or-later notices; contributor credit retained where present | Verified |
| `src/platform/legacy/xim/optparse.*`, `uksync.*` | Embedded Pham Kim Long LGPL version 2-or-later notices | Verified |
| `src/platform/legacy/xim/IC.c`, `IC.h` | Embedded Sun Microsystems and Hewlett-Packard copyright and permission text | Verified |
| `src/platform/legacy/xim/xim.c` | Embedded Sun/HP permission text followed by Pham Kim Long's LGPL version 2-or-later notice | Verified; both notices must be retained |

The XIM permission text is present in full in the relevant source headers; it
is not merely an unexplained copyright line. This code is retained for
historical reference and is not part of the current CMake build.

## Build-graph findings

The upstream `src/Makefile.am` builds `byteio`, `vnconv`, `ukengine`,
`ukinterface`, `IMdkit`, `xim`, `gui`, and optionally `unikey-gtk`.
Per-component `Makefile.am` files explicitly list all headerless groups above.

The current CMake target builds `src/byteio/byteio.cpp` together with
`vnconv`, `ukengine`, and `ukinterface`. It does not build the historical GUI,
XIM, GTK module, IMdkit, or standalone precompiled-header translation units.
Build inclusion supports provenance and component association; it is not a
substitute for a license notice.

## Remaining evidence needed

The exact per-file license for headerless files remains **Unresolved**.
Resolution requires at least one of:

- an upstream CVS revision containing a per-file notice;
- a release announcement explicitly applying named terms to the complete
  archive and identifying the license version;
- confirmation from the relevant copyright holder;
- legal review reconciling the archive's Library GPL v2 text, neighboring
  “Lesser GPL version 2 or later” headers, and the current website's general
  GPL statement.
