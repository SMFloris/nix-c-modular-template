{ pkgs ? import <nixpkgs> {} }:
pkgs.stdenv.mkDerivation {
    pname = "libimago";
    version = "2.3";

    src = pkgs.fetchFromGitHub {
      owner = "jtsiomb";
      repo  = "libimago";
      rev   = "v2.3";
      hash  = "sha256-ot5inX6CPhpXkFS03P1mirF/I3T/RdFKiJTgWkdddhk=";
    };

    nativeBuildInputs = [ pkgs.pkg-config ];
    buildInputs = [ pkgs.libpng pkgs.libjpeg pkgs.zlib ];

    enableParallelBuilding = true;

    postInstall = ''
      mkdir $out/lib/pkgconfig
      # pkg-config metadata (name is imago2 as used in your Makefile)
      cat > "$out/lib/pkgconfig/imago2.pc" <<EOF
Name: libimago (imago 2)
Description: Lightweight image loading/saving library
Version: 2.3
Requires.private: libpng zlib libjpeg
Libs: -L$out/lib -limago
Libs.private: -lpng -lz -ljpeg
Cflags: -I$out/include
EOF
    '';
}
