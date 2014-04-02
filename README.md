# nacl-mruby
nacl-mruby is a mruby interpreter works on [Native Client (NaCl)](https://developers.google.com/native-client/).
Ruby code runs on the browser, and you can use Pepper API from Ruby.

__This is ALPHA version.__


## Live demos
Access these pages from Google Chrome browser.
* [repl](http://devel.notozeki.org/nacl-mruby/repl/): A read-eval-print loop console
* [mandelbrot](http://devel.notozeki.org/nacl-mruby/mandelbrot/): Plotting mandelbrot set


## How to use

### Prerequisites
* Native Client SDK pepper 31 or later
  * See https://developers.google.com/native-client/dev/sdk/download
* Google Chrome 31 or later
* GNU make

### 1. Download
```bash
$ git clone git://github.com/notozeki/nacl-mruby.git
$ cd nacl-mruby
$ git submodule init
$ git submodule update
```

### 2. Build
First, set `NACL_SDK_ROOT` environment variable to the path to the Native Client SDK. For example:
```bash
$ export NACL_SDK_ROOT=/Users/notozeki/src/nacl_sdk/pepper_31
```

Then, make.
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
