#ifndef BOND_TOOLS_NATIVEBINDINGGENERATOR_H
#define BOND_TOOLS_NATIVEBINDINGGENERATOR_H

#include "bond/conf.h"

namespace Bond
{

class OutputStream;
class TranslationUnit;

/// \brief A generator for C++ native binding declarations and definitions.
///
/// The NativeBindingGenerator scans parsed Bond translation units and emits C++ binding glue that
/// allows Bond code to call native functions.
///
/// \sa TranslationUnit, OutputStream
/// \ingroup tools
class NativeBindingGenerator
{
public:
	/// \brief Generates C++ native binding source and header output.
	/// \param translationUnitList Linked list of translation units to scan for native declarations.
	/// \param cppStream Output stream receiving generated C++ source definitions.
	/// \param hStream Output stream receiving generated C++ header declarations.
	/// \param collectionName Identifier of the generated native binding collection.
	/// \param includeName Include path written into the generated C++ source file for the header.
	void Generate(
		const TranslationUnit *translationUnitList,
		OutputStream &cppStream,
		OutputStream &hStream,
		const char *collectionName,
		const char *includeName);
};

}

#endif
