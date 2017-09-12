# Satori

Satori is an HTTP library built on top of libUV. It intends to be the Express / Node.js of the C++ ecosystem.


## Project Status

Satori is in development. Do not use this in production yet!


## TODO:

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
