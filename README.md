# Bond Toolset

## Overview

Bond is a small toolset consisting of:

- `bondc` - the command line Bond compiler
- `bond` - the Bond virtual machine
- `bondd` - the Bond disassembler
- `prettyprint` - the Bond pretty printer

## Usage

### `bondc`

Compiles Bond source files into the CBO (compiled bond object) file format. Optionally generates C++ bindings for native functions callable from Bond code.

Usage: <code>bondc &lt;options&gt; &lt;source files&gt;</code>

<table>
  <tbody>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-o&nbsp;&lt;path&gt;</code></td>
      <td>Output CBO file. Defaults to "bond.cbo" if omitted. If <code>-b</code>, <code>-c</code>, <code>-h</code> and <code>-i</code> are used and <code>-o</code> is omitted, no CBO file is generated.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-b&nbsp;&lt;id&gt;</code></td>
      <td>Identifier for native binding collection inserted in generated C++ header and source files. Must be used in conjunction with <code>-c</code>, <code>-h</code> and <code>-i</code>.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-c&nbsp;&lt;path&gt;</code></td>
      <td>Generated C++ source file containing the definition of native function bindings. Must be used in conjunction with <code>-b</code>, <code>-h</code> and <code>-i</code>.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-h&nbsp;&lt;path&gt;</code></td>
      <td>Generated C++ header file containing the declaration of native function bindings. Must be used in conjunction with <code>-b</code>, <code>-c</code> and <code>-i</code>.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-i&nbsp;&lt;path&gt;</code></td>
      <td>Include path inserted in the generated C++ source file to include the generated C++ header file. Must be used in conjunction with <code>-b</code>, <code>-c</code> and <code>-h</code>.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-e&nbsp;&lt;macro&gt;</code></td>
      <td>Export macro applied to the generated C++ header declarations, such as <code>BOND_API</code>. Must be used in conjunction with <code>-b</code>, <code>-c</code>, <code>-h</code> and <code>-i</code>.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-x&nbsp;&lt;header&gt;</code></td>
      <td>Header included by the generated C++ header file to define the export macro. Must be used in conjunction with <code>-b</code>, <code>-c</code>, <code>-h</code> and <code>-i</code>.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-I&nbsp;&lt;path&gt;</code></td>
      <td>Add the specified path to the list of directories to be searched for included Bond files.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-p32</code></td>
      <td>Generate output that targets platforms having 32 bit pointers.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-p64</code></td>
      <td>Generate output that targets platforms having 64 bit pointers.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-v</code></td>
      <td>Print the Bond version and exit.</td>
    </tr>
  </tbody>
</table>

### `bond`

Loads and executes a Bond application compiled in the CBO file format.

Usage: <code>bond &lt;vm options&gt; - &lt;application options&gt;</code>

<table>
  <tbody>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-e&nbsp;&lt;func&gt;</code></td>
      <td>Fully qualified function name to be called as the Bond application's entry point. Defaults to <code>main</code> if omitted. The function signature must be <code>int &lt;func&gt;(int argc, const char **argv)</code>.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-s&nbsp;&lt;int&gt;</code></td>
      <td>Stack size in kilobytes. Defaults to 64 if omitted.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-v</code></td>
      <td>Print the Bond version and exit.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-</code></td>
      <td>All remaining command line arguments are passed through to the Bond application.</td>
    </tr>
  </tbody>
</table>

### `bondd`

Loads CBO files and displays their contents in a human readable format.

Usage: <code>bondd &lt;CBO files&gt;</code>

### `prettyprint`

Parses Bond source files and prints them back out with consistent formatting.

Usage: <code>prettyprint &lt;options&gt; &lt;source files&gt;</code>

<table>
  <tbody>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-f</code></td>
      <td>Fold constant expressions and print out their literal values. Implies <code>-s</code>.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-p</code></td>
      <td>Print the parse tree rather than Bond source.</td>
    </tr>
    <tr>
      <td nowrap style="white-space: nowrap; word-break: keep-all; overflow-wrap: normal;"><code>-s</code></td>
      <td>Enable semantic analysis and print out semantic errors.</td>
    </tr>
  </tbody>
</table>

## Building Bond

### With CMake

Bond can be built with CMake as either a static library (the default) or a shared library. Use a separate build directory for each, since the two produce conflicting library files on some platforms. The tests read the Bond scripts in `scripts/` using relative paths, so the `run_tests` target runs them from the source root.

#### Static library

Configure a build directory with:

```sh
cmake -S . -B build
```

To build the Bond static library, type:

```sh
cmake --build build --target bond_lib
```

To build the Bond toolset, type:

```sh
cmake --build build --target tools
```

To build the unit tests, type:

```sh
cmake --build build --target test
```

To build and run the unit tests, type:

```sh
cmake --build build --target run_tests
```

Add `--config Release` (or `Debug`) to the build commands when using a multi-configuration generator such as Visual Studio.

#### Shared library

Configure a build directory with `BUILD_SHARED_LIBS` turned on:

```sh
cmake -S . -B build-shared -DBUILD_SHARED_LIBS=ON
```

The build targets are the same as for the static library:

```sh
cmake --build build-shared --target tools
cmake --build build-shared --target run_tests
```

The tools and tests link against the shared library, which is placed in:

- `build-shared/lib` as `libbond.so` or `libbond.dylib` on Linux and macOS. The build tree's RPATH lets the tools and tests find it.
- `build-shared/bin` as `bond.dll` on Windows, with its import library in `build-shared/lib`. The DLL is also copied next to each test executable.

On GCC and Clang, shared builds hide all symbols that are not marked with `BOND_API`, so any public symbol missing the macro fails to link just as it would on Windows. Applications that use the shared library must be compiled with the `BOND_SHARED` definition, which CMake supplies automatically to targets that link `bond_lib`. Native bindings generated by `bondc` for use with a shared library should be generated with `-e BOND_API -x bond/conf.h`. Compile the application with the same compiler, standard library and build configuration as the library, because Bond's public types use standard library containers.

### With make

Bond offers a few Makefiles for common platforms. First create a symbolic link to an appropriate Makefile. Do not use `Makefile.common`; it is included by the other Makefiles. For example:

```sh
ln -s Makefile.gcc Makefile
```

To build the Bond static library, type:

```sh
make
```

To build the Bond toolset, type:

```sh
make tools
```

To build and run the unit tests, type:

```sh
make test
```

### With Visual Studio

Bond also offers a Visual Studio solution for Visual Studio 2026. Install the Desktop development with C++ workload, then open `vs/bond/bond.slnx` and build.
