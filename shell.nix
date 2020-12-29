with import <nixpkgs> {};
pkgsMusl.mkShell {
    buildInputs = [
        clang_10
        cppcheck
        python3
        shellcheck
    ];
    shellHook = ''
        . .shellhook
    '';
}
