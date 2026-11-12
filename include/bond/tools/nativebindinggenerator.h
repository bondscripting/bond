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
/// allows Bond code to call native functions. The generated source file defines a
/// NativeBindingCollection that binds the native functions declared in the translation units,
/// including the getters and setters of native struct members, to C++ functions declared in the
/// generated header file, which the embedder must implement. The header also declares C++
/// equivalents of the enums. Only the TranslationUnits that are flagged as requiring code
/// generation are scanned.
///
/// \sa TranslationUnit, OutputStream
/// \ingroup tools
class BOND_API NativeBindingGenerator
{
public:
	/// \brief Generates C++ native binding source and header output.
	/// \param translationUnitList Linked list of translation units to scan for native declarations.
	/// \param cppStream Output stream receiving generated C++ source definitions.
	/// \param hStream Output stream receiving generated C++ header declarations.
	/// \param collectionName Identifier of the generated native binding collection.
	/// \param includeName Include path written into the generated C++ source file for the header.
	/// \param exportMacro Macro applied to generated declarations, or nullptr or empty for none.
	/// \param exportHeader Header included by the generated header to define the export macro,
	///        or nullptr or empty for none.
	void Generate(
		const TranslationUnit *translationUnitList,
		OutputStream &cppStream,
		OutputStream &hStream,
		const char *collectionName,
		const char *includeName,
		const char *exportMacro = nullptr,
		const char *exportHeader = nullptr);
};

}

#endif
