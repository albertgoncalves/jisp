with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_14.stdenv; } {
    buildInputs = [
        python3
        shellcheck
    ];
    shellHook = ''
        . .shellhook
    '';
}
