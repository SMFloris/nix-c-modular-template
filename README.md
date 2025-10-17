# 🧱 Nix C/C++ Modular Template

A **modular C/C++ project template** built around **Nix** — designed for reproducibility, modularity, and simplicity. It lets you write structured C/C++ applications with **internal or external modules** using **Nix** as your package manager.

You can pull dependencies from **GitHub, nixpkgs, tarballs, or any other source**. Forget about dependency hell — Nix already solved that.

---

## 🚀 Why

C/C++ dependency management is notoriously bad. Existing package managers are clunky and inconsistent across systems.
With Nix, you can have **fully reproducible builds** that work the same on any machine — Linux, macOS, or even WSL.

This template exists because there was **no clean, elegant way** to start a modular C/C++ project using Nix.
It’s intended to be:

* **Simple:** Just `make` and `nix`.
* **Modular:** Internal and external modules treated the same.
* **Deterministic:** Every build is deterministic.

---

## ⚡ Example Overview

In one simple swoop, this example does the following:

* downloads, builds and links [libimago](https://github.com/jtsiomb/libimago) from a github repository
* uses raylib from nixpkgs and links it
* uses an internal module (rgridlayout) and statically links it
* both main app and internal module use raylib -> don't worry nix makes sure it is only downloaded once

The example itself, all it does is some simple binary-image analysis on pngs.
Notice that both the main app and internal module have the same layout. This is intended. You can thus, make all your code reusable by using the same template over and over again.
You can even use git submodules so that your modules can live in their own separate repositories.

```bash
$: > git clone https://github.com/smfloris/nix-c-modular-template
$: > cd nix-c-modular-template
$: > nix-shell
🚀 C23 dev shell ready (release, Bear + clangd)
Tip: bear -- make -j16
🔍 Checking pkg-config libraries...
✅ raylib found.
✅ imago2 found.
$nix-shell: > make
==> Building modules/rgridlayout/
==> NIXMODE=nix-shell, building modules/rgridlayout/
🔍 Checking pkg-config libraries...
✅ raylib found.
make[1]: Entering directory '/home/flow/Projects/nix-c-template/modules/rgridlayout'
ar rcs build/librgridlayout.a build/rgridlayout.o
✅ Built static library: build/librgridlayout.a
make[1]: Leaving directory '/home/flow/Projects/nix-c-template/modules/rgridlayout'
gcc build/binary_images.o build/main.o modules/rgridlayout//build/librgridlayout.a  -lm -L/nix/store/v172zs5wdv59gqjpmkl5mrz4530x1g8j-raylib-5.5/lib -L/nix/store/fyylpf9wcvagszxs3jzyi4il8aygpvd2-libimago-2.3/lib -lraylib -limago -o build/binary-image-analyse
✅ Built executable: build/binary-image-analyse
$nix-shell: > ./build/binary-image-analyse ./img1.png ./img2.png ./img3.png
```

![output](https://private-user-images.githubusercontent.com/3417174/501736501-27c0ecd8-6671-4a7e-83a2-03255d1afe26.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NjA1NjU3OTAsIm5iZiI6MTc2MDU2NTQ5MCwicGF0aCI6Ii8zNDE3MTc0LzUwMTczNjUwMS0yN2MwZWNkOC02NjcxLTRhN2UtODNhMi0wMzI1NWQxYWZlMjYucG5nP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI1MTAxNSUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNTEwMTVUMjE1ODEwWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9NmZhYzI0NmE0MWNmYTJmZTNiOTg5MmRjNWU5OGIxZTJjMTg2Mzk2OGIxNzQ4OGNhNDUxNWYxYjRkYmEwNGQxNSZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QifQ.SYXU4Ck1ZeGpwBdjzlba9NynMAlsRGIZZoSP3CiRW4o)

---

## Quickstart

Copy or clone this repo.

Follow the folder/file structure. At a minimum, you need:

```bash
├── modules/
├── include/
│   └── header.h
├── src/
│   └── main.c
├── Makefile
├── .nix-build.json
└── default.nix
```

Any other module you want, has the same file/folder structure.

---

## 🗂️ Folder Structure

```bash
.
├── modules/
│   ├── module1/
│   │   ├── include/...
│   │   ├── src/...
│   │   ├── modules/...
│   │   ├── Makefile
│   │   ├── .nix-build.json
│   │   ├── default.nix
│   └── libimago.nix
├── include/
│   └── binary_images.h
├── src/
│   └── main.c
├── Makefile
├── .nix-build.json
└── default.nix
```

---

## ⚙️ File Overview

### `default.nix` — The Bread and Butter

This is the Nix entry point.
It defines:

* Build dependencies (via `nixPkgs` and `importedPkgs` from .nix-build.json)
* Internal modules (recursively loaded - same template)
* Compiler/linker flags from `.nix-build.json`
* A reproducible derivation using `stdenv.mkDerivation`

It also provides a **development shell** with:

* `bear` for clangd compilation database
* Automatic `pkg-config` validation

See the full `default.nix` in this template.

---

### `.nix-build.json` — The Build Heart

Defines your project’s **metadata, build flags, and dependencies**:

```json
{
  "name": "binary-image-analyse",
  "nixPkgs": ["raylib"],
  "pkgConfigNames": ["raylib", "imago2"],
  "makeFlags": {
    "release": {
      "cflags": "-std=c23 -O2 -Wall -Wextra -Wpedantic",
      "ldlibs": "-lm",
      "cppflags": ""
    },
    "debug": {
      "cflags": "-std=c23 -O0 -g3 -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fno-omit-frame-pointer",
      "ldlibs": "-lm",
      "cppflags": ""
    }
  }
}
```

Here, `raylib` is from nixpkgs and `imago2` is a library built from a github repo.

---

### `libimago.nix` - external sources made simple

Defines an **external module derivation**.

You can do the same with any other external source.
Example:

```nix
{ pkgs ? import <nixpkgs> {} }:
pkgs.fetchFromGitHub {
  owner = "someuser";
  repo = "cool-lib";
  rev = "abcdef123456";
  sha256 = "sha256-value";
}
```

---

### `Makefile`

Responsible for:

* Recursive builds of all modules
* Linking via `pkg-config`
* Supporting both **debug** and **release** modes

Each module Makefile should produce a `.a` or `.so` library.

---

## 🧩 Development Workflow

### 🧱 Build

```bash
nix-build
```

Builds everything, including any modules, producing the final binary in `result/bin/`.

---

### 💻 Development Shell

```bash
nix-shell
```

Drops you into a shell with:

* All dependencies
* `clangd` + `bear` ready for autocompletion
* Proper compiler flags loaded automatically

Use `bear` for clangd support:

```bash
bear -- make -j$(nproc)
```

---

### 🧪 Testing

Each module and the main app can define `make test` targets.
When you run:

```bash
make test
```

Nix ensures all test dependencies are available and runs all module-level tests recursively.

---

## 🔗 Example Module Dependency

If your main app depends on an internal module and an external one:

```nix
{
  "name": "image-tool",
  "nixPkgs": ["raylib"],
  "pkgConfigNames": ["raylib", "imago2"]
}
```

Then:

* `imago2` lives under `./modules/libimago.nix`
* `raylib` is pulled from `nixpkgs`

Nix builds them both, exposes their headers and pkg-config metadata, and links automatically.

---

## 🌍 Cross Compilation

`TODO: docs`

---

## 🧠 Summary

This template gives you:

* Clean modular structure
* Seamless dependency management via Nix
* Clangd + Bear integration
* Reproducible, portable builds
* Freedom to compose, link, and test however you want

> **Build once, run everywhere — deterministically.**


