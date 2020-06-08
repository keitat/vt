# *vt* => virtual transport

## Introduction : What is *vt*?

*vt* is an active messaging layer that utilizes C++ object virtualization to
manage virtual endpoints with automatic location management. *vt* is directly
built on top of MPI to provide efficient portability across different machine
architectures. Empowered with virtualization, **vt** can automatically perform
dynamic load balancing to schedule scientific applications across diverse
platforms with minimal user input.

*vt* abstracts the concept of a `node`/`rank`/`worker`/`thread` so a program can
be written in terms of virtual entities that are location independent. Thus,
they can be automatically migrated and thereby executed on varying hardware
resources without explicit programmer mapping, location, and communication
management.

## Building

*vt* can be built with `cmake` or built inside a `docker` container.

To build *vt*, one must obtain the following dependencies:

### Optional:

#### If threading is enabled:
  - OpenMP       _or_
  - `std::thread`s (default to from C++ compiler)

#### Required:
  - detector,   (*vt* ecosystem)
  - checkpoint, (*vt* ecosystem)
  - MPI         (mpich/openmpi/mvapich/IBM Spectrum MPI/Cray MPICH/etc.)

### Automatically build dependencies

Assuming MPI is installed and accessible via CC/CXX, the only other dependencies
that are required are checkpoint and detector. The easiest way to get these
built are to clone them inside `vt/lib`:

```bash
$ git clone git@github.com:DARMA-tasking/vt
$ cd vt/lib
$ git clone git@github.com:DARMA-tasking/checkpoint
$ git clone git@github.com:DARMA-tasking/detector
```

With these in `vt/lib`, cmake will automatically build them and stitch them into
*vt*'s linking process.

Instead of running `cmake`, one may invoke the `vt/ci/build_cpp.sh` script which
will run `cmake` for *vt* with environment variables for most configuration
parameters.

#### Environment Variables

| Variable                    | Default Value   | Description |
| ------------------          | --------------- | ----------- |
| `CMAKE_BUILD_TYPE`          | Release         | The `cmake` build type |
| `VT_LB_ENABLED`             | ON              | Enable compile-time load balancing support |
| `VT_TRACE_ENABLED `         | OFF             | Enable compile-time tracing support |
| `VT_TRACE_RUNTIME_ENABLED ` | OFF             | Force tracing on at runtime (used in CI for automatically testing tracing on all tests/examples) |
| `VT_DOXYGEN_ENABLED `       | OFF             | Enable doxygen generation |
| `VT_MIMALLOC_ENABLED `      | OFF             | Enable `mimalloc`, alternative allocator for debugging memory usage/frees/corruption |
| `VT_ASAN_ENABLED `          | OFF             | Enable building with address sanitizer |
| `VT_POOL_ENABLED `          | ON              | Use memory pool in *vt* for message allocation |
| `VT_ZOLTAN_ENABLED `        | OFF             | Build with Zoltan enabled for `ZoltanLB` support |
| `ZOLTAN_DIR `               | (empty)         | Directory pointing to Zoltan installation |
| `VT_MPI_GUARD_ENABLED `     | OFF             | Enable compile-time load balancing support |

With these set, invoke the script with two arguments: the path to the *vt* root directory and the build path. Here's an example assuming that *vt* is cloned into `/usr/src/vt` with trace enabled in debug mode.

**Usage for building:**

```bash
$ vt/ci/build_cpp.sh <full-path-to-vt-source> <full-path-to-build-dir>
```

**Example:**

```bash
$ cd /usr/src
$ git clone git@github.com:DARMA-tasking/vt
$ VT_TRACE_ENABLED=1 CMAKE_BUILD_TYPE=Debug /usr/src/vt/ci/build_cpp.sh /usr/src/vt /usr/build/vt
```

### Building with `docker` containerization

The easiest way to build *vt* is by using `docker` with the available containers that contain the proper compilers, MPI, and all other dependencies. First, install `docker` on the system. On some systems, `docker-compose` might also need to be installed.

The `docker` builds are configured through `docker-compose` to use a shared, cached filesystem mount with the host for `ccache` to enable fast re-builds.

For `docker-compose`, the following variables can be set to configure the build. One may configure the architecture, compiler type and version, Linux distro (ubuntu or alpine), and distro version.

```
# Variables:
#   ARCH={amd64, arm64v8, ...}
#   COMPILER_TYPE={gnu, clang}
#   COMPILER={gcc-5, gcc-6, gcc-7, gcc-8, gcc-9, gcc-10,
#             clang-3.9, clang-4.0, clang-5.0, clang-6.0, clang-7, clang-8,
#             clang-9, clang-10}
#   REPO=lifflander1/vt
#   UBUNTU={18.04, 20.04}
#   ULIMIT_CORE=0
#
# DARMA/vt Configuration Variables:
#   VT_LB=1              # Enable load balancing
#   VT_TRACE=0           # Enable tracing
#   VT_MIMALLOC=0        # Enable mimalloc memory allocator
#   VT_DOCS=0            # Enable doxygen build
#   VT_TRACE_RT=0        # Enable tracing at runtime (for testing)
#   VT_ASAN=0            # Enable address sanitizer
#   BUILD_TYPE=release   # CMake build type
```

With these set, run the following for a non-interactive build:

```bash
$ cd vt
$ docker-compose run -e BUILD_TYPE=debug -e VT_TRACE=1 ubuntu-cpp
```

For an interactive build, where one can build, debug, and run `valgrind`, etc:

```bash
$ cd vt
$ docker-compose run -e BUILD_TYPE=debug -e VT_TRACE=1 ubuntu-cpp-interactive
$ /vt/ci/build_cpp.sh /vt /build
$ /vt/ci/test_cpp.sh /vt /build
```

For more detailed information on configuring the docker build, read the documentation in `vt/docker-compose.yml`.

## Testing

After *vt* is built succesfully, one may invoke the tests several ways. One may run `make test` or `ninja test` (depending on the generator used) or `ctest`, to run all the tests. Alternatively, the tests can be run automatically from the CI script:

```bash
$ vt/ci/test_cpp.sh <full-path-to-vt-source> <full-path-to-build-dir>
```