tty2json
========

Dump ttyrec files to a JSON data structure for playing in a browser (term.js)

A small program for dumping the contents of files saved with 
[ttyrec](http://0xcc.net/ttyrec/) to JSON.

# Building
The project can be built with CMake if wanted, but it's overkill. The project
can be compiled manually with gcc or clang as follows:

``` console
$ gcc tty2json.c -o tty2json
```

``` console
$ clang tty2json.c -o tty2json
```
# Example

``` console
$ ./tty2json hello.rec > hello.json
```

# Notes
I wrote a small javascript library to playback ttyrec recordings in the browser
it requires the recording to be dumped to JSON in the format produced by this
program. The JS player can be found 
[here](https://github.com/dkcampbell/teletypeplayer.js).

