# CA216 Operating Systems: myshell

Myshell is a simple shell implemenation, see `manual/readme.md` for more details

## Build
```
make
```
This will put the `myshell` binary in ./bin

you can also run
```
make clean
```
to remove build files

```
make watch
```
to hot rebuild on file changes (this depends on [entr](https://github.com/eradman/entr) program)

For debug symbols and runtime logging prefix `make` commands with `debug=1` (e.g. `debug=1 make`)
