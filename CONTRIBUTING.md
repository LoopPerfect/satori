# Contributing to Satori 🎉

Hello! Thanks for taking the time to contribute to Satori. Satori is a community project, and will only succeed through the work of contributors such as yourself. This guide will help you get started.


## Important Resources

Satori lives in a single repository, but it requires a few external components to build:

 1. [Buck Build](https://buckbuild.com) is the build system used by Satori.
 2. [Buckaroo](https://github.com/LoopPerfect/buckaroo) is the package manager used to fetch Satori's dependencies. You can also use Buckaroo to install Satori into your project.


## Getting Help

If you need quick interaction, a good avenue for talking to the developers is [Gitter.im](https://gitter.im/LoopPerfect/satori). If you have a complex problem, reporting an issue is the best route.


## Reporting Issues

Please send all issues to the [issue tracker](https://github.com/LoopPerfect/satori/issues) on GitHub. 😌


### Security

If you have a sensitive issue, such as a security bug, please send an email to security@buckaroo.pm.


## Making a Contribution

The procedure for contributing to Satori is:

 1. Fork [LoopPerfect/satori](https://github.com/LoopPerfect/satori) on GitHub
 2. Make some changes, adding unit-tests where appropriate
 3. Ensure that all tests pass
 4. Make a pull request (usually to `master`)
 5. Bask in the kudos! 👏 👑


## What should I work on?

We endeavour to keep [the issue tracker](https://github.com/LoopPerfect/satori/issues) up-to-date, so that is the best place to start. Keep an eye out for [issues marked "help wanted"](https://github.com/LoopPerfect/satori/labels/help%20wanted).


### First-time Contributor ❤️

First-time contributor? Take a look at the issue tracker for [issues marked "first commit"](https://github.com/LoopPerfect/satori/labels/first%20commit) for smaller, self-contained tasks. We would also be happy to walk you through any of the existing code on [Gitter.im](https://gitter.im/LoopPerfect/satori).


### Your Own Feature

If you would like to contribute a feature that you have thought of, please [create an issue](https://github.com/LoopPerfect/satori/issues) first so that we can ensure the design is in-keeping with the general direction of Satori.


## Environment

Development of Satori requires [Buck](https://buckbuild.com) and [Buckaroo](https://github.com/LoopPerfect/buckaroo/) installations on your system. You will also need a modern C++ compiler.

To fetch the source-code:

```bash=
git clone https://github.com/LoopPerfect/satori.git
cd satori
```

To build the project:

```bash=
buck build :satori
```


### Which branch should I use? 🤔

The convention for branches in Satori is:

 * `master` - the very latest code with the latest features, but not recommended for production use
 * `release/version` - the branch from which a release version is tagged
 * `bugfix/bug` - a branch for fixing a specific bug
 * `feature/widget` - a branch for implementing a specific feature
 * `improvement/widget` - a branch for making a specific improvement, such as refactoring

Most developers should branch from `master` in order to have their changes included in the next release.

If you would like to patch an old release, you should branch off of `release/version`.


## Testing

Satori uses automated testing and [Travis CI](https://travis-ci.org/LoopPerfect/satori) to prevent regressions.

There are two `buck test` targets for Satori:

 * `satori-unit` is for self-contained tests of internal logic to Satori.
 * `satori-integration` is for tests that require external services.

To run the tests:

```bash=
buck test :satori-unit
buck test :satori-integration
```


### Adding a Test

First, determine if your test belongs in `satori-unit` or `satori-integration`. A test in `satori-unit` must not modify the file system or call any external services. Favour `satori-unit` tests where possible.

Next, place your test in the right directory:

 * `satori-unit` tests live under `satori/tests/unit`.
 * `satori-integration` tests live under `satori/tests/integration`.


## Making a Pull Request

Once your submission is ready, you should make a pull request on GitHub to the appropriate branch.

 * If you are implementing a new feature or bug-fix for the next release, then you should `base` your pull request on `master`.

 * If you are making a bug-fix for an old release, you should `base` on the appropriate `release/version` branch.

We review pulls request within 24 hours.


## Releases

Satori releases are semantically versioned Git tags. You can see [the releases on GitHub](https://github.com/LoopPerfect/satori/releases).

Each release has a corresponding branch named `release/version`. This allows `master` to progress beyond the current release, whilst still allowing for easy patching of old versions.

That's all for now! 💖
