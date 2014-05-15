# nacl-mruby
**nacl-mruby** is a mruby interpreter works on [Native Client (NaCl)](https://developers.google.com/native-client/).
Ruby code runs on the browser, and you can use Pepper API from Ruby.

__This is ALPHA version.__ Some APIs still not working :cry:


## Live demos
Access these pages from Google Chrome browser.
* [repl](http://devel.notozeki.org/nacl-mruby/repl/): A read-eval-print loop console
* [mandelbrot](http://devel.notozeki.org/nacl-mruby/mandelbrot/): Plotting mandelbrot set


## How to use

### Prerequisites
* Native Client SDK pepper_31 or later
  * See https://developers.google.com/native-client/dev/sdk/download
* depot_tools
  * See http://dev.chromium.org/developers/how-tos/install-depot-tools
* Google Chrome 31 or later
* GNU make

### 1. Preparation
First, set `NACL_SDK_ROOT` environment variable to the path to the Native Client SDK. For example:
```bash
$ export NACL_SDK_ROOT=/Users/notozeki/src/nacl_sdk/pepper_31
```

Second, check out naclports. You need to install [depot_tools](http://dev.chromium.org/developers/how-tos/install-depot-tools) to use `gclient`.
```bash
$ mkdir naclports
$ cd naclports
$ gclient config --name=src  https://chromium.googlesource.com/external/naclports.git
$ gclient sync
```
\# See https://code.google.com/p/naclports/wiki/HowTo_Checkout for more details.

Third, install PCRE library via naclports. PCRE is required in order to compile [mruby-pcre-regexp](https://github.com/mattn/mruby-pcre-regexp) gem later.
```bash
$ cd src
$ ./make_all.sh pcre
```

### 2. Download and build nacl-mruby
Clone nacl-mruby repository into a directory wherever you like. And setup submodules.
```bash
$ git clone git://github.com/notozeki/nacl-mruby.git
$ cd nacl-mruby
$ git submodule init
$ git submodule update
```

Then, run `make`.
```bash
$ cd plugin
$ make
```

Now you have `newlib` directory and nacl-mruby executables in it.

### 3. Run examples
Currently there are three examples:
* examples/simple
* examples/mandelbrot
* examples/repl

To run an example, change directory to the example directory, and run `make` and `make run`.
```bash
$ cd ../examples/mandelbrot
$ make
$ make run
```

Now http server is running in the local, access `localhost:5103` from Google Chrome browser.

### 4. Write your application
(Coming soon...)


## License
under the MIT License:
* http://www.opensource.org/licenses/mit-license.php

Copyright (c) 2014 Yasuhiro KOSEKI
