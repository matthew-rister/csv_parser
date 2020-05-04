# CSV Parser

## Usage

### Homogeneous Data

```C++
#include "csv.hpp"

using namespace csv;

std::stringstream data;
data << "5.1, 3.5, 1.4, 0.2"
    << "4.9, 3.0, 1.4, 0.2"
    << "4.7, 3.2, 1.3, 0.2"

const Csv<double> csv{data};

const auto sepal_length = csv.Get(0, 0)
const auto petal_width = csv.Get(2, 3);
```

### Heterogeneous Data

```C++
#include "csv.hpp"

using namespace csv;

std::stringstream data;
data << "a, 3.14, 42, true"
    << "b, 1.618, 0, false"
    << "c, 2.71, 7, true";

const Csv<char, double, int32_t, bool> csv{data};

const auto meaning_of_life = csv.Get<int32_t>(0, 2);
const auto e = csv.Get<double>(2, 1);
```

## Build

To build the project, you must have cmake 3 installed and a compiler that supports the C++17 language standard. You can then build from your favorite IDE or by running `cmake . && make` from the command line.

## Test

This project uses the [Catch2](https://github.com/catchorg/Catch2) testing library which is included in this repository as a single header-only file. Tests are currently configured to run as part of the main executable after building.
