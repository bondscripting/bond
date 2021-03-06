#ifndef BOND_COMPILER_CODEGENERATOR_H
#define BOND_COMPILER_CODEGENERATOR_H

#include "bond/types/types.h"

namespace Bond
{

class Allocator;
class OutputStream;
class CompilerErrorBuffer;
class TranslationUnit;

/// \brief A code generator for the Bond scripting language.
///
/// The CodeGenerator traverses the given parse tree and compiles it to bytecode that can be
/// interpreted by a virtual machine. The bytecode is written to the given OutputStream in the
/// CBO file format.
///
/// \sa ParseNode
/// \ingroup compiler
class CodeGenerator
{
public:
	/// \brief Constructs a CodeGenerator object.
	/// \param allocator Allocator used for temporary memory allocations made by the Generate method.
	/// \param errorBuffer Buffer where error messages are pushed when code generation errors are
	///        encountered.
	/// \param pointerSize Size of pointers used by the target virtual machine that will execute the
	///        Bond bytecode. It is required to generate correct pointer arithmetic instructions for
	///        the target platform.
	CodeGenerator(Allocator &allocator, CompilerErrorBuffer &errorBuffer, PointerSize pointerSize = BOND_NATIVE_POINTER_SIZE):
		mAllocator(allocator),
		mErrorBuffer(errorBuffer),
		mPointerSize(pointerSize)
	{}
	~CodeGenerator() {}

	/// \brief Generates bytecode for a list of parse trees.
	/// \param translationUnitList A linked list of parse trees to be compiled to bytecode.
	/// \param stream OutputStream to which the generated bytecode is written in the CBO file format.
	void Generate(const TranslationUnit *translationUnitList, OutputStream &stream);

private:
	CodeGenerator &operator=(const CodeGenerator &other) = delete;

	Allocator &mAllocator;
	CompilerErrorBuffer &mErrorBuffer;
	PointerSize mPointerSize;
};

}

#endif
