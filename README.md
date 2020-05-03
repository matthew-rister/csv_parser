# CSV Parser

## Usage

```C++
#include <iostream>

#include "csv.hpp"

using namespace csv;

const std::string data{"a, 3.14, 42, true\nb, 2.72, 0, false"};
const Csv<char, double, int, bool> csv{data};
const auto pi = csv.Get<double>>(0, 1);
std::cout << csv.ToString() << std::endl;
```

## Build

To build the project, you must have cmake 3 installed and a compiler that supports the C++17 language standard. You can then build from your favorite IDE or by running `cmake . && make` from the command line.

## Test

This project uses the [Catch2](https://github.com/catchorg/Catch2) testing library which is included in this repository as a single header-only file. Tests are currently configured to run as part of the main executable after building.
