# bt

wip bittorent client in c++. 

## dependencies 
- [{fmt}](https://fmt.dev)
- [doctest](https://github.com/doctest/doctest)
- [recursive-variant](https://github.com/codeinred/recursive-variant)
  - version of `std::variant` that allows for sum type-ish behavior

## build
```sh
cmake -S . -B build
cmake --build build
```

## run
```sh
./build/bt <torrent file>
```
there are a number of example torrent files in `examples/`

## test
```sh
./build/test
```