{
  description = "discordbot";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    libcurl.url = ./libcurl;
  };

  outputs = { self, nixpkgs, libcurl}: {
    packages.x86_64-linux.default = let
      pkgs = import nixpkgs {system="x86_64-linux";};
      curllib = import libcurl {nixpkgs=pkgs;};
    in pkgs.stdenv.mkDerivation {
      buildInputs=with pkgs; [openssl wget cjson];
      name="oceanbot";
      src=./src;
      buildPhase=''
        export cjson=${pkgs.cjson}
        export libcurl=${curllib}
        make
      '';
      installPhase=''
        mkdir $out
        mv request.out $out
        mv cacert.pem $out
        #mv .env.sh $out
      '';
    };
    devShells.x86_64-linux.default = let
      pkgs = import nixpkgs {system="x86_64-linux";};
      curllib = import libcurl {nixpkgs=pkgs;};
    in pkgs.stdenv.mkDerivation {
      buildInputs=with pkgs; [openssl wget cjson];
      name="oceanbot";
      src=./src;
      buildPhase=''
        export cjson=${pkgs.cjson}
        export libcurl=${curllib}
        make
      '';
      installPhase=''
        mkdir $out
        #cp -r build $out
        mv request.out $out
        mv cacert.pem $out
        mv .env.sh $out
      '';

    };
  };
}
