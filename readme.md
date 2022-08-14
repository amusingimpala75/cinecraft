# Cinecraft
![Cinecraft logo, a cow](DALL·E 2022-07-30 14.22.05 - 256 x 256 pixel art of a cow.png)
Meaning of the name: popular voxel game implementation, but written in C. Should be pronounced with a hard 'C' sould, akin to a 'k', so that then a pun can be used to make the mascot of the project a cow. Thanks DALL·E for the icon.

### Requirements
As of now, Cinecraft requires a *nix system in order to run, as it requires on pthreads for multithreading and unix sockets for networking. Other than that, all dependencies are pulled in as git submodules.

### Building

``` sh
git clone https://github.com/amusingimpala75/cinecraft.git
cd cinecraft
git submodules update
run "make -j `nproc`"
```
The executable is aptly named 'cinecraft'.

### Running

``` sh
/path/to/executable/cinecraft folder-for-data-storage
```
ie: ./cinecraft test

### Licensing
This project is licensed under the GPL v3, found as the LICENSE file in the root of the project.

### Credits
Written by AmusingImpala75

Uses the following libraries:
- [LibBase64 by devolve](https://sourceforge.net/projects/libb64), licensed under a public domain-ish license which requires attribution
- [tomlc99 by cktan](https://github.com/cktan/tomlc99), licensed under the MIT license
