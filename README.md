# SystemInterv

## JSON Model Counter

A high-performance C application that reads **large JSON files in streaming mode** and counts how many times each `"model"` value appears.  
Designed for efficiency, low memory footprint, and full compliance with modern C best practices.

---

## Features

- **Streaming JSON parsing** using [YAJL](https://lloyd.github.io/yajl/): handles gigabyte-sized JSON files without loading them into memory.  
- **Fast hash-based counting** via GLib’s `GHashTable`.
- **Arena allocator** integration for YAJL’s memory callbacks — reduces malloc overhead.
- **Command-line interface** with options for input, output, and verbosity.
- **Structured logging** with timestamps and levels (`INFO`, `DEBUG`, etc.).
- **Portable build system** (CMake) with separate `Debug` and `Release` configurations.
- **Ready for profiling and CI** — integrated `build.sh`, `gprof`, and GitHub Actions workflow.

---

## Architecture Overview

src/
├── main.c              → Entry point, command-line handling and orchestration
├── cmdline.c/.h        → CLI parser (getopt-style)
├── log.c/.h            → Logging system with levels and timestamps
├── json_parser.c/.h    → Streaming parser callbacks using YAJL
├── model_counter.c/.h  → Hash table-based model counting
├── arena_alloc.c/.h    → Custom allocator for YAJL
└── error.c/.h          → Error codes and basic error handling

---

## Build Instructions

### Prerequisites

Ubuntu/Debian example:

sudo apt install build-essential cmake pkg-config libyajl-dev libglib2.0-dev

### Build (Debug or Release)

./build.sh Debug    # For debugging (ASan + UBSan)
./build.sh Release  # Optimized build (-O3 -march=native)

Build artifacts are generated in the `build/` directory.

---

## Usage

### Read from file

./App -i big.json

### Read from stdin

cat big.json | ./App

### Enable verbose mode

./App -v -i big.json

### Example output

33329611  SCSI3HD
33345174  SSDLP2
33328579  SSDF1
33332954  RDV2
...

---

## Implementation Notes

### Performance

- Uses YAJL incremental parsing with a 64 KB I/O buffer.
- GLib `GHashTable` stores `(key,len) → count` pairs with custom hash/equal functions.
- Memory allocator uses an **arena system** to minimize fragmentation.
- Profiling via `gprof` shows:
  - ~60 s CPU time in hashing + callbacks.
  - ~8 min wall-clock time due to I/O bottleneck on a 400 MB JSON file.

### Memory Efficiency

- Keeps only unique `"model"` strings in memory.
- Average RSS < 3 MB even with hundreds of millions of entries processed.

---

## Testing & Profiling

### Test with a small JSON file

echo '{"items":[{"model":"A"},{"model":"B"},{"model":"A"}]}' > test.json
./build/App -i test.json

**Expected output:**
2  A
1  B

### Performance profiling (with gprof)

cmake -S . -B build-prof -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE="-pg"
cmake --build build-prof -j$(nproc)
cd build-prof
/usr/bin/time -v ./App -i ../bigf.json
gprof ./App gmon.out > profile.txt

Alternatively:

cmake -S . -B build-prof \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_FLAGS_RELEASE="-pg"

cmake --build build-prof -j"$(nproc)"

---

## Continuous Integration (Optional)

Example GitHub Actions workflow (.github/workflows/ci.yml):

(name, yaml content omitted for brevity)

---

## License

This project is **not open source** and is provided **for interview and evaluation purposes only**.  
All rights are reserved by the author.  
Unauthorized reproduction, distribution, or commercial use of this code is prohibited.

---

## Author’s Notes

This project was built as a low-level performance exercise — focusing on clean architecture, memory safety, and real-world profiling.  
It demonstrates good engineering practices for modern C software:

- Modular design.
- Explicit memory ownership.
- Reproducible builds.
- Profiling-driven optimization.

> “Make it work. Make it right. Make it fast.” — Kent Beck

---

## Future Improvements

- Parallel parsing using worker threads or mmap partitioning.
- Replace GLib hashtable with custom lock-free hash implementation.
- Add JSON schema validation for stricter parsing.
- Optional binary serialization of results.
- Integrate benchmarks with `perf stat` or `valgrind --tool=callgrind`.
- Add Dockerfile for reproducible CI environments.

---

## Author’s Notes

This project was developed as a **technical demonstration** of system-level design, modular C architecture,  
and performance optimization for large-scale data processing.  

It focuses on:
- clean and maintainable code,
- memory-safe design,
- profiling-based performance tuning,
- reproducible builds.

---

Author: Wagner Souza Keller  
Language: C17  
Platform: Linux (tested on Ubuntu 24.04)  
Dependencies: YAJL, GLib, CMake, GCC