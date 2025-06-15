{nixpkgs ? import <nixpkgs>{}, unittesting ? false}: {
  packages.x86_64-linux.default = let
    makeflags = if unittesting then "unittest" else "";
    pkgs = nixpkgs;
  in pkgs.stdenv.mkDerivation {
    #nativebuildInputs=with pkgs; [openssl wget];
    buildInputs=with pkgs; [openssl wget] + [./libcurl/default.nix];
    name="oceanbot";
    src=./src;
    buildPhase=''
      ls
      export flags="-I ${libcurl}/include -L${libcurl}/lib"
      make ${makeflags}
    '';
    installPhase=''
      mkdir  $out
      mv cacert.pem $out
      mv data.json $out
      mv request.out $out
    '';
  };

}
