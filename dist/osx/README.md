# Building the macOS bundle

Works on both Intel (`/usr/local` Homebrew) and Apple Silicon
(`/opt/homebrew` Homebrew); `bundle.sh` discovers the prefix at runtime.

Using [Homebrew](http://brew.sh) install build dependencies:

    brew install automake coreutils git
    brew install ffmpeg wxwidgets

If `bundle.sh` fails because a Homebrew dylib references its siblings via
`@loader_path` (occasionally seen with `libbrotli*`), rewrite the offending
references to absolute Homebrew paths so the bundling loop can pick them
up, e.g.:

    BREW_PREFIX=$(brew --prefix)
    install_name_tool -change "@loader_path/libbrotlicommon.1.dylib" "$BREW_PREFIX/lib/libbrotlicommon.1.dylib" "$BREW_PREFIX/lib/libbrotlidec.1.dylib"
    install_name_tool -change "@loader_path/libbrotlicommon.1.dylib" "$BREW_PREFIX/lib/libbrotlicommon.1.dylib" "$BREW_PREFIX/lib/libbrotlienc.1.dylib"

Bundle Spek:

    ./dist/osx/bundle.sh
