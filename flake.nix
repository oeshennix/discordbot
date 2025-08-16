{
  description = "discordbot";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    libcurl.url = ./libcurl;
  };

  outputs = { self, nixpkgs, libcurl}:
    let
      tomlinfo = builtins.fromTOML(builtins.readFile ./info.toml);
    in
  {
    packages.x86_64-linux.default = let
      pkgs = import nixpkgs {system="x86_64-linux";};
      curllib = import libcurl {nixpkgs=pkgs;};
    in pkgs.stdenv.mkDerivation {
      cjson=pkgs.cjson;
      #ncurses=pkgs.ncurses;
      libcurl=curllib;

      buildInputs=with pkgs; [openssl wget cjson ncurses];
      name="oceanbot";
      src=./src;
      buildPhase=''
        make
      '';
      installPhase=''
        mkdir $out
        mv request.out $out
        mv cacert.pem $out
        #mv .env.sh $out
      '';
    };
  };
}
