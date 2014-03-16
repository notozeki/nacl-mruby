# Write your Native Client applications in Ruby!
nacl-mruby is a [Native Client (NaCl)](https://developers.google.com/native-client/) application development environment on [mruby](https://github.com/mruby/mruby).
You can write NaCl applications in Ruby, and the Ruby code runs on the browser.

__This is ALPHA version.__


## For busy person
Go to http://devel.notozeki.org/nacl-mruby/mandelbrot/ and see the plotting mandelbrot set demo!


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
Currently there are two examples:
* examples/simple
* examples/mandelbrot

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

Copyright (c) 2013 Yasuhiro KOSEKI
