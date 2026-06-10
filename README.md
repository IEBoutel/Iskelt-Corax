# Iskelt Corax
A chess engine

## Overview

Iskelt Corax is a chess engine built from scratch in C++. It comprises a chess board implementation, an optimal move determination algorithm, and a command line interface.

Iskelt Corax is currently in an early stage of development and likely still contains glitches and other flaws.

## Documentation

Documentation can be found [here](meta/docs.md).

## Benchmarks

The following benchmarks are for the latest version and were calculated on an AMD Ryzen 9950HX platform in the starting position. Benchmarks are kept for each version [here](meta/benchmark_history.md).

| Metric | Value |
| ------ | ----- |
| Perft(7) | 2m27s |
| Move search depth in 1s (excl. quiesence) | 10 plies |
| Move search depth in 5s (excl. quiesence) | 11 plies |

## Versions

The latest version is v0.1.0

Versions of Iskelt Corax take the form "v{major}.{minor}.{micro}". The major version increments on significant changes. The minor version increments upon the addition of feautures. The micro version increments upon bug fixes and other such changes that do not affect the intended behaviour.

## Licensing

Iskelt Corax is licensed under the GNU GPL v3.0 license.