# Typhoon

A WIP Bittorent client in C++ 20.

## dependencies

- Managed with VCPKG, see [vcpkg.json](vcpkg.json)
- [{fmt}](https://fmt.dev) (String Formatting)
- [doctest](https://github.com/doctest/doctest) (Testing)
- [asio](https://think-async.com/Asio/) (Networking)
- [ada](https://ada-url.com/) (URL Parser)
- [recursive-variant](https://github.com/codeinred/recursive-variant) (Hacky sum-types in C++)

## build

```sh
cmake -S . -B build
cmake --build build
```

## run

```sh
./build/typhoon <torrent file>
```

there are a number of example torrent files in `examples/`

## test

```sh
./build/test
```

