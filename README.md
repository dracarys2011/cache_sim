# Cache Simulator & Matrix Transposition Optimization

## Overview

This project is part of the CS:APP Cache Lab. It consists of two main components:
1.  **Cache Simulator (`csim`)**: A C program that simulates the behavior of a cache memory. It takes valgrind memory traces as input and simulates the hits, misses, and evictions of a cache with configurable parameters (set associativity, block size, etc.).
2.  **Matrix Transposition (`trans.c`)**: An optimized matrix transposition function capable of minimizing cache misses. It uses blocking techniques to improve cache performance for specific matrix sizes (32x32, 64x64, 61x67).

## Features

### Cache Simulator
-   **Configurable Cache Parameters**: Users can define the number of set index bits (`-s`), lines per set (`-E`), and block offset bits (`-b`).
-   **LRU Replacement Policy**: Implements the Least Recently Used (LRU) policy for evicting cache lines.
-   **Detailed Simulation**: Tracks and reports the number of cache hits, misses, and evictions.
-   **Verbose Mode**: Optional flag (`-v`) to display trace information for every memory access.

### Matrix Transposition
-   **Cache-Aware Optimization**: Uses blocking (tiling) to reduce cache misses during matrix transposition.
-   **Optimized for Specific Sizes**: Contains specialized logic for 32x32, 64x64, and 61x67 matrices.

## Getting Started

### Prerequisites
-   Linux environment (or WSL on Windows)
-   GCC Compiler
-   Make
-   Python (for the driver script)

### Compilation

To compile the project, simply run:

```bash
make
```

This will generate the `csim` executable and other test tools.

To clean up the build files:

```bash
make clean
```

## Usage

### Running the Cache Simulator (`csim`)

The simulator requires several command-line arguments to define the cache structure and the input trace file.

**Syntax:**
```bash
./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>
```

**Options:**
-   `-h`: Print help message.
-   `-v`: Optional verbose flag that displays trace info.
-   `-s <s>`: Number of set index bits (S = 2^s is the number of sets).
-   `-E <E>`: Associativity (number of lines per set).
-   `-b <b>`: Number of block bits (B = 2^b is the block size).
-   `-t <tracefile>`: Name of the valgrind trace to replay.

**Example:**
Run a simulation with 16 sets (s=4), 1 line per set (E=1, direct-mapped), and 16-byte blocks (b=4) on `traces/yi.trace`:

```bash
./csim -s 4 -E 1 -b 4 -t traces/yi.trace
```

### Testing Matrix Transposition

Use the `test-trans` utility to evaluate the correctness and performance of the transposition functions.

**Syntax:**
```bash
./test-trans -M <rows> -N <cols>
```

**Examples:**

Test 32x32 matrix:
```bash
./test-trans -M 32 -N 32
```

Test 64x64 matrix:
```bash
./test-trans -M 64 -N 64
```

Test 61x67 matrix:
```bash
./test-trans -M 61 -N 67
```

## Project Structure

-   `csim.c`: Source code for the cache simulator. Implementing the cache logic, LRU policy, and command-line parsing.
-   `trans.c`: Source code for the matrix transposition functions. Contains `transpose_submit` and helper functions for different block sizes.
-   `cachelab.c` / `cachelab.h`: Helper functions and definitions for the lab (e.g., `printSummary`).
-   `tracegen.c`: Helper program to generate trace files for testing.
-   `traces/`: Directory containing example memory traces.
-   `Makefile`: Build script for the project.
-   `driver.py`: Autograder driver program.

## Results

After running `csim`, the program outputs the counts for hits, misses, and evictions.

Example Output:
```
hits:4 misses:3 evictions:0
```
