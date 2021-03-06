# Satori

An HTTP library built on top of libUV.

[![Travis](https://img.shields.io/travis/LoopPerfect/satori.svg)](https://travis-ci.org/LoopPerfect/satori) [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/LoopPerfect/satori/master/license)


## What is Satori?

 - An asyncronous IO framework
 - A C++ wrapper of libuv
 - An attempt to build the Express / Node.js of the C++ ecosystem


## Project Status

Satori is in development. Do not use this in production *quite* yet!


## Design Goals

 * Great performance, at least as fast as Node.js in all benchmarks
 * Low memory-usage
 * Cross platform (Linux, Windows and macOS)
 * Leverage C++ 14 for terse user-code (similar length to Express apps)
 * Commercial friendly license (MIT)
 * Easy builds and dependency management using [Buck](https://buckbuild.com) and [Buckaroo](https://buckaroo.pm)
 * Zero dependencies at deployment-time


## TODO

 * Path-to-regex
 * Router
 * Async file-system API
 * Benchmarks
 * Kafel integration for secure route-handling
 * ... and probably lots more!


## Building

You will need [Buck](https://buckbuild.com) and [Buckaroo](https://buckaroo.pm).

```
# Fetch dependencies
buckaroo install

# Build the library
buck build :satori

# Run the example
buck run :main
```


## Examples

There are many sample projects in the [examples](./examples) folder. 
