#ifndef BOND_COMPILER_FRONTEND_H
#define BOND_COMPILER_FRONTEND_H

#include "bond/conf.h"
#include "bond/compiler/lexer.h"
#include "bond/io/streamfactory.h"
#include "bond/stl/list.h"
#include "bond/stl/stringview.h"
#include "bond/stl/vector.h"

namespace Bond
{

class Parser;
class SemanticAnalyzer;
class Token;
class TranslationUnit;

/// \brief A compiler front end for the Bond scripting language.
///
/// The FrontEnd manages the interactions between the compiler's front end components, namely a
/// Lexer, a Parser and a SemanticAnalyzer.
///
/// The FrontEnd also manages an input file list, a list of Bond source files that are required to
/// be loaded, parsed and analyzed. Files can be explicitly added to the input file list via the
/// AddInputFile method. Files are also implicitly added to the list when include directives in
/// the Bond source files are parsed. One parse tree, rooted with a TranslationUnit, is generated
/// for each Bond source file. Only the TranslationUnits for the files that were explicitly added
/// to the input file list are flagged as requiring code generation, in the order they were added.
///
/// File names are normalized lexically by NormalizePath (see bond/io/path.h): backslashes become
/// forward slashes, "." segments and repeated separators are removed, and "dir/.." pairs are
/// collapsed. A file is loaded at most
/// once, so an include directive that resolves to the normalized name of a file that is already
/// loaded has no effect. Symbolic links and case differences are not detected, so two names for
/// the same file that differ in those respects load it twice.
///
/// Like a quoted include in C, a relative include path is first resolved against the directory of
/// the including file. If no such file exists, the path is passed to the StreamFactory as written,
/// which may resolve it in its own way (e.g. against the working directory or a search path). Note
/// that a file found through a search path is identified by the name that was passed to the
/// StreamFactory, not the full path of the file. The name of the file that was opened is the file
/// name reported by its Tokens. Those names are owned by the FrontEnd, so the FrontEnd must outlive
/// any use of the Tokens' file names.
///
/// A file that cannot be opened is reported as an INCLUDE_FILE_NOT_FOUND or INPUT_FILE_NOT_FOUND
/// error in the Lexer's CompilerErrorBuffer, and analysis stops.
///
/// When analysis is complete, the list of generated TranslationUnits is returned.
///
/// File loading is delegated to an object that implements the StreamFactory interface so that no
/// assumptions about where the source files reside are made (e.g. disk, network, database,
/// proprietary asset store, etc). The StreamFactory should return a null handle when a file does
/// not exist. A Bond::Exception thrown by the StreamFactory is ignored when probing the including
/// file's directory, but otherwise propagates to the caller.
///
/// Global variables whose values cannot be determined at compile time are initialized by static
/// initializers. Within a CBO, they run in the order the files were added to the input file list,
/// and in declaration order within each file. Included files that are not themselves input files
/// do not contribute static initializers. See CodeSegment::CallStaticInitializers for the order
/// across CBOs.
///
/// \sa CompilerErrorBuffer, FrontEnd, Parser, SemanticAnalyzer, TranslationUnit
/// \ingroup compiler
class BOND_API FrontEnd
{
public:
	/// \brief Constructs a FrontEnd object.
	/// \param allocator The allocator used for the FrontEnd's internal data structures.
	/// \param tokenCollectionStore Storage for the generated TokenCollections..
	/// \param lexer The Lexer used to scan the Bond source files.
	/// \param parser The Parser used to parse the Bond source files.
	/// \param semanticAnalyzer The SemanticAnalyzer used to analyze the Bond source files.
	/// \param streamFactory The StreamFactory responsible for loading the Bond source files.
	FrontEnd(
			Allocator &allocator,
			TokenCollectionStore &tokenCollectionStore,
			Lexer &lexer,
			Parser &parser,
			SemanticAnalyzer &semanticAnalyzer,
			StreamFactory &streamFactory):
		mAllocator(allocator),
		mInputFileList(InputFileList::allocator_type(&allocator)),
		mPathStore(PathStore::allocator_type(&allocator)),
		mLoadedFileNameList(StringList::allocator_type(&allocator)),
		mTokenCollectionStore(tokenCollectionStore),
		mLexer(lexer),
		mParser(parser),
		mSemanticAnalyzer(semanticAnalyzer),
		mStreamFactory(streamFactory)
	{}

	FrontEnd(const FrontEnd &other) = delete;
	FrontEnd &operator=(const FrontEnd &other) = delete;

	/// \brief Adds the name of a Bond source file to the input file list, unless a file with the
	/// same normalized name has already been added.
	/// \param inputFileName The name of the file to be loaded. Since StringViews are not deep
	/// copied, the lifetime of the string must at least span the lifetime of the FrontEnd.
	void AddInputFile(const StringView &inputFileName);

	/// \brief Returns whether a file with the same normalized name as the given one has been
	/// explicitly added to the input file list or has been loaded.
	/// \param inputFileName The name of the file to be tested.
	bool ContainsInputFile(const StringView &inputFileName);

	/// \brief Performs loading, parsing and semantic analysis of all source files in the input file
	/// list, as well as those files referenced via include directives.
	/// returns A parse tree of ParseNodes rooted with a list of TranslationUnits.
	TranslationUnit *Analyze();

	/// \brief Returns whether any errors occurred whilst analyzing the source code.
	bool HasErrors() const;

private:
	struct InputFile
	{
		InputFile(const StringView &path, const Token *includePath, const char *includerName):
			mPath(path),
			mIncludePath(includePath),
			mIncluderName(includerName)
		{}

		// The path as it was given, before normalization.
		StringView mPath;

		// The include directive's path token, or null for an explicit input file.
		const Token *mIncludePath;

		// The resolved name of the including file, or null for an explicit input file.
		const char *mIncluderName;
	};

	enum OpenResult
	{
		OPEN_RESULT_OPENED,
		OPEN_RESULT_ALREADY_LOADED,
		OPEN_RESULT_NOT_FOUND
	};

	typedef List<InputFile> InputFileList;
	typedef Vector<char> PathBuffer;
	typedef List<PathBuffer> PathStore;
	typedef List<StringView> StringList;

	OpenResult OpenInputFile(const InputFile &inputFile, InputStreamHandle &streamHandle, const char *&resolvedName);
	OpenResult TryOpen(PathBuffer &candidate, bool isProbe, InputStreamHandle &streamHandle, const char *&resolvedName);
	bool IsLoaded(const StringView &fileName) const;
	const char *StorePath(PathBuffer &&path);

	Allocator &mAllocator;
	InputFileList mInputFileList;

	// Owns the resolved names of the files, which are referenced by their Tokens.
	PathStore mPathStore;

	StringList mLoadedFileNameList;
	TokenCollectionStore &mTokenCollectionStore;
	Lexer &mLexer;
	Parser &mParser;
	SemanticAnalyzer &mSemanticAnalyzer;
	StreamFactory &mStreamFactory;
};

}

#endif
