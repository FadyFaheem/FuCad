<img src="/.github/images/fucad-banner.png" width="900" alt="FuCad"/>

### A Fusion 360-style fork of FreeCAD

FuCad is a fork of [FreeCAD](https://www.freecad.org), the open-source parametric
3D modeler. It keeps FreeCAD's geometry kernel, document model, Python API and
`.FCStd` file format intact, and reworks the user experience along the lines of
Fusion 360: a unified, task-driven interface instead of a workbench-per-discipline
layout.

Because the core is unchanged, FuCad opens existing FreeCAD documents and runs
existing FreeCAD macros and addons.

[Upstream project](https://www.freecad.org) •
[Upstream documentation](https://wiki.freecad.org) •
[Upstream repository](https://github.com/FreeCAD/FreeCAD)

<img src="/.github/images/partdesign.png" width="800"/>

Status
------

FuCad is an early-stage fork under active development. There are no precompiled
releases yet; build from source.

Compiling
---------

FuCad uses FreeCAD's build system unchanged. See the
[Developers Handbook – Getting Started](https://freecad.github.io/DevelopersHandbook/gettingstarted/)
for platform-specific build instructions and dependencies.

Underlying technology
---------------------

* **OpenCASCADE** — the geometry kernel
* **Coin3D** — Open Inventor-compliant 3D scene representation
* **Python** — broad scripting API, compatible with FreeCAD's
* **Qt** — graphical user interface

Reporting issues
----------------

Report problems with FuCad on the [FuCad issue tracker](https://github.com/FadyFaheem/FuCad/issues).

If you can reproduce the same problem in upstream FreeCAD, please report it to the
[FreeCAD issue tracker](https://github.com/FreeCAD/FreeCAD/issues) instead, so the
fix benefits both projects.

Usage & getting help
--------------------

FuCad's scripting API and core concepts are FreeCAD's, so the FreeCAD documentation
applies:

- [Getting started](https://wiki.freecad.org/Getting_started)
- [Frequent questions](https://wiki.freecad.org/FAQ/en)
- [Scripting](https://wiki.freecad.org/Power_users_hub)
- [Developers Handbook](https://freecad.github.io/DevelopersHandbook/)
- [FreeCAD forum](https://forum.freecad.org)

Please do not report FuCad-specific problems to the FreeCAD forum or issue tracker;
FuCad is not affiliated with or endorsed by the FreeCAD project or the
[FreeCAD Project Association](https://fpa.freecad.org).

License and attribution
-----------------------

FuCad is a derivative work of FreeCAD, Copyright (C) 2001-2026 FreeCAD contributors.

FuCad is free and open-source software licensed under the GNU Lesser General Public
License, version 2.1 or (at your option) any later version (LGPL-2.1-or-later), the
same terms as upstream FreeCAD. See [LICENSE](LICENSE) for the full text and
[src/Doc/LICENSE.html](src/Doc/LICENSE.html) for details on the licenses of bundled
third-party components.

The names "FreeCAD" and the FreeCAD logo belong to the FreeCAD project and are used
here only to identify the upstream work from which FuCad is derived.
