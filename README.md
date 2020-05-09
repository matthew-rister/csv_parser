# CSV Parser

A single-header CSV parser which supports parsing heterogeneous data types.

## Usage

### Homogeneous Data

For a CSV data which consists of homogeneous data, it's sufficient to instantiate a `Csv` with a single template parameter.

```C++
#include <iostream>
#include <sstream>

#include "csv.hpp"

using namespace csv;

int main() {

    std::stringstream data;
    data << "5.1, 3.5, 1.4, 0.2" << std::endl
        << "4.9, 3.0, 1.4, 0.2" << std::endl
        << "4.7, 3.2, 1.3, 0.2";

    const Csv<double> csv{data.str()};
    std::cout << csv.get(2, 3);

    return EXIT_SUCCESS;
}
```

### Heterogeneous Data

For a CSV which consists of heterogeneous data, you can declare the data type for each column as a template parameter.

```C++
#include <iostream>
#include <sstream>

#include "csv.hpp"

using namespace csv;

int main() {

    std::stringstream data;
    data << "a, 3.14, 42, true" << std::endl
        << "b, 1.618, 0, false" << std::endl
        << "c, 2.71, 7, true";

    const Csv<char, double, int32_t, bool> csv{data.str()};
    std::cout << csv.get<double>(0, 2);

    return EXIT_SUCCESS;
}
```

## Build

To build the project, you must have cmake 3 installed and a compiler that supports the C++17 language standard. You can then build from your favorite IDE or by running `cmake -G Ninja . && ninja` from the command line.

## Test

This project uses the [Catch2](https://github.com/catchorg/Catch2) testing library which is included in this repository as a single header-only file. Tests are currently configured to run as part of the main executable after building.
