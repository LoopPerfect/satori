# Satori

Satori is an HTTP library built on top of libUV. It intends to be the Express / Node.js of the C++ ecosystem.


## Project Status

Satori is in development. Do not use this in production yet!


## Design Goals

 * Great performance, at least as fast as Node.js in all benchmarks
 * Low memory-usage
 * Cross platform (Linux, Windows and macOS)
 * Leverage C++ 14 for terse user-code (similar length to Express apps)
 * Commercial friendly license (MIT)
 * Easy builds and deployment using [Buck](https://buckbuild.com) and [Buckaroo](https://buckaroo.pm) 


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
