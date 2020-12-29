with import <nixpkgs> {};
mkShell {
    buildInputs = [
        clang_10
        cppcheck
        shellcheck
        valgrind
    ];
    shellHook = ''
        . .shellhook
    '';
}
