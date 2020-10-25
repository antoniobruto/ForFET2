<table>
  <tr>
    <th>Ubuntu</th>
    <th>OS X</th>
    <th>Coverage</th>
    <th>Builds/UnitTests</th>
    <th>Coverity Scan</th>
  </tr>
  <tr>
    <td><a href="https://travis-ci.org/dreal/dreal3"><img src="https://travis-ci.org/dreal/dreal3.svg?branch=master" title="Ubuntu 12.04 LTS 64bit, g++-4.9 | clang++-3.8"/></a></td>
    <td><a href="https://travis-ci.org/soonhokong/dReal-osx"><img src="https://travis-ci.org/soonhokong/dReal-osx.svg?branch=master" title="Mac OS X 10.8.2, g++-4.9"/></a></td>
    <td><a href='https://coveralls.io/r/dreal/dreal3'><img src='https://coveralls.io/repos/dreal/dreal3/badge.svg?branch=master%0A' alt='Coverage Status' /></a></td>
    <td><a href="http://borel.modck.cs.cmu.edu/CDash/index.php?project=dReal"><img src="https://dreal.github.io/images/cdash.svg"/></a></td>
    <td><a href="https://scan.coverity.com/projects/2152"><img alt="Coverity Scan Build Status" src="https://scan.coverity.com/projects/2152/badge.svg"/></a></td>
  </tr>
</table>


dReal is an automated reasoning tool. Please visit [http://dreal.github.io][dReal] for more information.

[dReal]: http://dreal.github.io


Install
========
Please check the [releases page][release] to download latest static binaries.

[release]: https://github.com/dreal/dreal3/releases


Build from Source
==================

 - [Ubuntu 12.04 LTS (using g++-4.9)][ubuntu-gcc]
 - [Ubuntu 12.04 LTS (using clang++-3.8)][ubuntu-clang]
 - [OS X 10.9/10.8 (using g++-4.9)][osx-gcc]

[ubuntu-gcc]: doc/ubuntu-gcc.md
[ubuntu-clang]: doc/ubuntu-clang.md
[osx-gcc]: doc/osx-gcc.md


Required Packages
=================

 - C++11-compatible compiler ([g++(>=4.8)][gcc], [clang++(>=3.3)][clang])
 - [autoconf][autoconf], [automake][automake], [bison][bison],
   [cmake(>=3.1.3)][cmake], [flex][flex], [git][git], [libtool][libtool], [make][make],
   [pkg-config][pkg-config]
 - [ocaml (>=4.01.0)][ocaml], [opam][opam], [ocaml-findlib][ocaml-findlib], [ocaml-batteries (>=2.0)][ocaml-batteries]

[automake]: http://www.gnu.org/software/automake
[autoconf]: http://www.gnu.org/software/autoconf
[make]: http://www.gnu.org/software/make
[libtool]: http://www.gnu.org/software/libtool
[gcc]: https://gcc.gnu.org/projects/cxx-status.html
[git]: http://git-scm.com
[clang]: http://clang.llvm.org/cxx_status.html
[cmake]:http://www.cmake.org/cmake/resources/software.html
[bison]: http://www.gnu.org/software/bison
[flex]: http://flex.sourceforge.net
[ocaml]: http://ocaml.org
[opam]: http://opam.ocamlpro.com
[ocaml-findlib]: http://projects.camlcity.org/projects/findlib.html
[ocaml-batteries]: http://batteries.forge.ocamlcore.org
[pkg-config]: http://www.freedesktop.org/wiki/Software/pkg-config


Documentations
==============
 - [How to contribute to dReal](CONTRIBUTING.md)
 - [Git commit message convention](doc/commit_convention.md)
 - [How to use ODE visualization][ode-vis]
 - [How to use dReal in Windows using Docker](doc/windows-docker.md)

[ode-vis]: doc/ode-visualization.md
