# 🧱 Nix C/C++ Module Template

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

## 🎯 Goals

1. ✅ Use **Make** and existing **Nix** utilities (Make is eternal).
2. ✅ Modules expose a **pkg-config** interface.

   * If one doesn’t, you can patch it via Nix easily.
3. ✅ Modules can depend on other modules.
4. ✅ Pull external modules from **any source** (`nixpkgs`, `git`, `svn`, `tar`, etc).
5. ✅ Link modules however you want.
6. ✅ Cross-compile for **any platform**.
7. ✅ Run tests for both **main app** and **modules**.
8. ✅ `nix-shell` gives you a full dev environment instantly.
9. ✅ One command builds everything.
10. ✅ Clean, hierarchical folder structure.
11. ✅ **clangd** works out of the box via `bear`.

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
│   └── external-module.nix
├── include/
│   └── header.h
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

* Build dependencies (via `nixPkgs` and `importedPkgs`)
* Internal modules (recursively loaded)
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

### `external-module.nix`

Defines an **external module derivation**.
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

You can import it in your main `default.nix` and treat it like any other nixpkgs module.

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

Builds everything, producing the final binary in `result/bin/`.

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

## ⚡ Quickstart

```bash
git clone https://github.com/smfloris/nix-c-modular-template
cd nix-cpp-modular-template
nix-shell
make
```

---

## 🧠 Summary

This template gives you:

* Clean modular structure
* Seamless dependency management via Nix
* Clangd + Bear integration
* Reproducible, portable builds
* Freedom to compose, link, and test however you want

> **Build once, run everywhere — deterministically.**


