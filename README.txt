Bond Toolset
------------

bondc - Command line Bond compiler. Compiles Bond source files into the CBO (compiled bond object)
file format. Optionally generates C++ bindings for native functions callable from Bond code.

Usage: bondc <options> <source files>
  -o <path>   Output CBO file. Defaults to "bond.cbo" if omitted. If -b, -c, -h and -i are
              used and -o is omitted, no CBO file is generated.
  -b <id>     Identifier for native binding collection inserted in generated C++ header and
              source files. Must be used in conjunction with -c, -h and -i.
  -c <path>   Generated C++ source file containing the definition of native function bindings.
              Must be used in conjunction with -b, -h and -i.
  -h <path>   Generated C++ header file containing the declaration of native function bindings.
              Must be used in conjunction with -b, -c and -i.
  -i <path>   Include path inserted in the generated C++ source file to include the generated
              C++ header file. Must be used in conjunction with -b, -c and -h.
  -I <path>   Add the specified path to the list of directories to be searched for included
              Bond files.
  -p32        Generate output that targets platforms having 32 bit pointers.
  -p64        Generate output that targets platforms having 64 bit pointers.



bond - Command line Bond virtual machine. Loads and executes a Bond application compiled in the
CBO file format.

Usage: bond <vm options> - <application options>
  -e <func>   Fully qualified function name to be called as the Bond application's entry point.
              Defaults to "main" if omitted. The function signature must be:
              int <func>(int argc, const char **argv)
  -s <int>    Stack size in kilobytes. Defaults to 64 if omitted.
  -           All remaining command line arguments are passed through to the Bond application.



bondd - Command line Bond disassembler. Loads CBO files and displays their contents in a human
readable format.

Usage: bondd <CBO files>



prettyprint - Command line Bond pretty printer. Parses Bond source files and prints them back out
with consistent formatting.

Usage: prettyprint <options> <source files>
  -f          Fold constant expressions and print out their literal values. Implies -s.
  -p          Print the parse tree rather than Bond source.
  -s          Enable semantic analysis and print out semantic errors.


Building Bond (make)
--------------------

Bond offers a few Makefiles for common platforms. First create a symbolic link to an appropriate
Makefile. Do not use Makefile.common, it is included by the other Makefiles. For example:

ln -s Makefile.gcc Makefile


To build the Bond static library, type:

make


To build the Bond toolset, type:

make tools


To run the unit tests, type:

make test


Building Bond (Visual Studio)
-----------------------------

Bond also offers a Visual Studio solution that is known to work with Visual Studio 2019. Open up
the bond.sln solution with Visual Studio and build.
