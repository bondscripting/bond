#include "bond/compiler/compilererror.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/path.h"
#include "bond/io/streamfactory.h"
#include "bond/stl/utility.h"
#include "bond/systems/exception.h"

namespace Bond
{

void FrontEnd::AddInputFile(const StringView &inputFileName)
{
	if (!ContainsInputFile(inputFileName))
	{
		mInputFileList.emplace_back(inputFileName, nullptr, nullptr);
	}
}


bool FrontEnd::ContainsInputFile(const StringView &inputFileName)
{
	PathBuffer normalized((PathBuffer::allocator_type(&mAllocator)));
	NormalizePath(StringView(), inputFileName, normalized);
	const StringView name(normalized.data(), normalized.size() - 1);

	if (IsLoaded(name))
	{
		return true;
	}

	PathBuffer other((PathBuffer::allocator_type(&mAllocator)));
	for (const InputFile &inputFile: mInputFileList)
	{
		if (inputFile.mIncludePath == nullptr)
		{
			NormalizePath(StringView(), inputFile.mPath, other);
			if (StringView(other.data(), other.size() - 1) == name)
			{
				return true;
			}
		}
	}

	return false;
}


TranslationUnit *FrontEnd::Analyze()
{
	ParseNodeList<TranslationUnit> translationUnitList;
	const size_t numFilesToCompile = mInputFileList.size();
	size_t i = 0;
	InputFileList::const_iterator it = mInputFileList.begin();
	while ((it != mInputFileList.end()) && !HasErrors())
	{
		InputStreamHandle streamHandle;
		const char *fileName = nullptr;
		const OpenResult result = OpenInputFile(*it, streamHandle, fileName);

		if (result == OPEN_RESULT_NOT_FOUND)
		{
			if (it->mIncludePath != nullptr)
			{
				mLexer.GetErrorBuffer().PushError(CompilerError::INCLUDE_FILE_NOT_FOUND, it->mIncludePath);
			}
			else
			{
				mLexer.GetErrorBuffer().PushError(CompilerError::INPUT_FILE_NOT_FOUND, nullptr, fileName);
			}
		}
		else if (result == OPEN_RESULT_OPENED)
		{
			mTokenCollectionStore.emplace_back(mLexer.Lex(fileName, *streamHandle));

			if (!HasErrors())
			{
				TokenStream stream = mTokenCollectionStore.back()->GetTokenStream();
				TranslationUnit *translationUnit = mParser.Parse(stream);
				translationUnitList.Append(translationUnit);
				if (!HasErrors())
				{
					translationUnit->SetRequiresCodeGeneration(i < numFilesToCompile);
					const IncludeDirective *includeDirectiveList = translationUnit->GetIncludeDirectiveList();
					while (includeDirectiveList != nullptr)
					{
						const Token *includePath = includeDirectiveList->GetIncludePath();
						mInputFileList.emplace_back(includePath->GetStringValue(), includePath, fileName);
						includeDirectiveList = NextNode(includeDirectiveList);
					}
				}
			}
		}

		++it;
		++i;
	}

	if (!HasErrors() && !translationUnitList.IsEmpty())
	{
		mSemanticAnalyzer.Analyze(translationUnitList.GetHead());
	}

	return translationUnitList.GetHead();
}


bool FrontEnd::HasErrors() const
{
	return
		mLexer.GetErrorBuffer().HasErrors() ||
		mParser.GetErrorBuffer().HasErrors() ||
		mSemanticAnalyzer.GetErrorBuffer().HasErrors();
}


FrontEnd::OpenResult FrontEnd::OpenInputFile(const InputFile &inputFile, InputStreamHandle &streamHandle, const char *&resolvedName)
{
	PathBuffer candidate((PathBuffer::allocator_type(&mAllocator)));

	// Like a quoted include in C, first try the path relative to the including file's directory.
	if ((inputFile.mIncluderName != nullptr) && !IsAbsolutePath(inputFile.mPath))
	{
		const StringView directory = GetDirectory(inputFile.mIncluderName);
		if (!directory.empty())
		{
			NormalizePath(directory, inputFile.mPath, candidate);
			const OpenResult result = TryOpen(candidate, true, streamHandle, resolvedName);
			if (result != OPEN_RESULT_NOT_FOUND)
			{
				return result;
			}
		}
	}

	NormalizePath(StringView(), inputFile.mPath, candidate);
	const OpenResult result = TryOpen(candidate, false, streamHandle, resolvedName);
	if (result == OPEN_RESULT_NOT_FOUND)
	{
		// Keep the name for the error message.
		resolvedName = StorePath(Bond::move(candidate));
	}
	return result;
}


FrontEnd::OpenResult FrontEnd::TryOpen(PathBuffer &candidate, bool isProbe, InputStreamHandle &streamHandle, const char *&resolvedName)
{
	const StringView name(candidate.data(), candidate.size() - 1);
	if (IsLoaded(name))
	{
		return OPEN_RESULT_ALREADY_LOADED;
	}

	if (isProbe)
	{
		// A missing file is expected when probing, so tolerate factories that throw.
		try
		{
			streamHandle = mStreamFactory.CreateInputStream(candidate.data());
		}
		catch (const Exception &)
		{
		}
	}
	else
	{
		streamHandle = mStreamFactory.CreateInputStream(candidate.data());
	}

	if (!streamHandle)
	{
		return OPEN_RESULT_NOT_FOUND;
	}

	resolvedName = StorePath(Bond::move(candidate));
	mLoadedFileNameList.emplace_back(resolvedName, name.size());
	return OPEN_RESULT_OPENED;
}


bool FrontEnd::IsLoaded(const StringView &fileName) const
{
	for (const StringView &loadedFileName: mLoadedFileNameList)
	{
		if (loadedFileName == fileName)
		{
			return true;
		}
	}
	return false;
}


const char *FrontEnd::StorePath(PathBuffer &&path)
{
	mPathStore.emplace_back(Bond::move(path));
	return mPathStore.back().data();
}

}
