{ pkgs ? import <nixpkgs> {}
, mode ? "bin"
, debug ? false
}:
let
  config = builtins.fromJSON (builtins.readFile ./.nix-build.json);

  importedPkgs = let
    sources = builtins.readDir ./modules;
    nixFiles = builtins.filter (name: builtins.match ".*\\.nix" name != null) (builtins.attrNames sources);
  in 
    map (name: import (./modules + "/${name}") { inherit pkgs; }) nixFiles;

  importedModules = if pkgs.lib.inNixShell then [] else
    let
      entries     = builtins.readDir ./modules;
      dirs        = builtins.filter (n: entries.${n} == "directory") (builtins.attrNames entries);
      withDefault = builtins.filter (n: builtins.pathExists (./modules + "/${n}/default.nix")) dirs;
    in map (n: import (./modules + "/${n}/default.nix") { inherit pkgs; mode = "lib"; }) withDefault;

  moduleDirs = builtins.concatStringsSep " " (map (m: "${m}") importedModules);

  name = config.name;
  nixPkgs = builtins.map (pkg: pkgs.${pkg}) config.nixPkgs;
  pkgConfigNames = config.pkgConfigNames;

  buildMode = if debug then "debug" else "release";
  buildCfg = config.makeFlags.${buildMode} or {};
  cflags   = buildCfg.cflags or "";
  ldlibs   = buildCfg.ldlibs or "";
  cppflags = buildCfg.cppflags or "";

in
  assert config ? name && config ? nixPkgs && config ? pkgConfigNames;

  pkgs.stdenv.mkDerivation {
    name = name;
    nativeBuildInputs = with pkgs; [ pkg-config ] ++ pkgs.lib.optionals pkgs.lib.inNixShell [ bear clang-tools ];
    buildInputs = nixPkgs ++ importedPkgs ++ importedModules;
    src = ./.;

    makeFlags = [
      "NAME=${name}"
      "MODE=${mode}"
      "NIXMODE=nix-build"
      "PKGS=${builtins.concatStringsSep "," pkgConfigNames}"
      "MODULE_DIRS=${moduleDirs}"
    ] ++ pkgs.lib.optionals (cflags != "") [ "CFLAGS=${cflags}" ]
      ++ pkgs.lib.optionals (ldlibs != "") [ "LDLIBS=${ldlibs}" ]
      ++ pkgs.lib.optionals (cppflags != "") [ "CPPFLAGS=${cppflags}" ];

    installFlags = [ "PREFIX=${placeholder "out"}" ];

    shellHook = ''
      export NIXMODE=nix-shell
      export NAME="${name}"
      export MODE="${mode}"
      export PKGS="${builtins.concatStringsSep "," pkgConfigNames}"
      export BUILD_MODE="${buildMode}"
      ${if cflags   != "" then ''export CFLAGS="${cflags}"'' else ""}
      ${if ldlibs   != "" then ''export LDLIBS="${ldlibs}"'' else ""}
      ${if cppflags != "" then ''export CPPFLAGS="${cppflags}"'' else ""}

      echo "🚀 C23 dev shell ready (${buildMode}, Bear + clangd)"
      echo " > Tip: bear -- make -j$(nproc)"
      echo "🔍 Checking pkg-config libraries..."
      for lib in ${builtins.concatStringsSep " " pkgConfigNames}; do
        if ! pkg-config --exists "$lib"; then
          echo "❌ pkg-config: library '$lib' not found." >&2
        else
          echo "✅ $lib found."
        fi
      done
    '';
  }
