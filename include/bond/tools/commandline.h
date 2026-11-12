#ifndef BOND_TOOLS_COMMANDLINE_H
#define BOND_TOOLS_COMMANDLINE_H

#include "bond/conf.h"
#include "bond/systems/allocator.h"

namespace Bond
{

/// \brief Provides a program's command line arguments encoded in UTF-8.
///
/// On Windows, the narrow argv passed to main is encoded in the ANSI code page, so non-ASCII
/// arguments are altered or lost. CommandLineArgs retrieves the UTF-16 command line from the
/// operating system and converts each argument to UTF-8. If that fails, it falls back to the
/// given argv. On other platforms, argv is already UTF-8 and is returned unchanged.
///
/// \ingroup tools
class BOND_API CommandLineArgs
{
public:
	/// \brief Constructs a CommandLineArgs object.
	/// \param allocator Allocator used for the converted arguments. It must outlive this object.
	/// \param argc The argument count passed to main.
	/// \param argv The argument vector passed to main.
	CommandLineArgs(Allocator &allocator, int argc, const char *argv[]);

	CommandLineArgs(const CommandLineArgs &other) = delete;
	CommandLineArgs &operator=(const CommandLineArgs &other) = delete;

	/// \brief Returns the number of arguments, including the program name.
	int GetArgc() const { return mArgc; }

	/// \brief Returns the UTF-8 encoded, null-terminated argument vector. It remains valid for
	/// the lifetime of this object.
	const char **GetArgv() const { return mArgv; }

private:
	Allocator::Handle<char> mBuffer;
	const char **mArgv;
	int mArgc;
};


/// \brief Makes a Windows console display UTF-8 output for the lifetime of this object.
///
/// The constructor switches the console's output code page to UTF-8. The destructor flushes
/// stdout and stderr, then restores the original code page, since the code page belongs to the
/// console and would otherwise persist after the program exits. Console input is not affected.
/// On other platforms, this class does nothing.
///
/// This is intended for applications that own their console, such as the Bond tools. A library
/// embedding Bond should leave the console configuration to its host application.
///
/// \ingroup tools
class BOND_API Utf8ConsoleOutput
{
public:
	/// \brief Switches the console's output code page to UTF-8.
	Utf8ConsoleOutput();

	/// \brief Flushes stdout and stderr and restores the console's original output code page.
	~Utf8ConsoleOutput();

	Utf8ConsoleOutput(const Utf8ConsoleOutput &other) = delete;
	Utf8ConsoleOutput &operator=(const Utf8ConsoleOutput &other) = delete;

private:
	unsigned mOriginalCodePage;
};

}

#endif
