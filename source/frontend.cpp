#include "bond/compiler/compilererror.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/fileloader.h"
#include "bond/stl/algorithm.h"

namespace Bond
{

void FrontEnd::AddInputFile(const HashedString &inputFileName)
{
	if (!ContainsInputFile(inputFileName))
	{
		mInputFileNameList.push_back(inputFileName);
	}
}


bool FrontEnd::ContainsInputFile(const HashedString &inputFileName)
{
	return Find(mInputFileNameList.begin(), mInputFileNameList.end(), inputFileName) != mInputFileNameList.end();
}


void FrontEnd::Analyze()
{
	const size_t numFilesToCompile = mInputFileNameList.size();
	size_t i = 0;
	StringList::Type::const_iterator it = mInputFileNameList.begin();
	while ((it != mInputFileNameList.end()) && !HasErrors())
	{
		FileLoader::Handle scriptHandle = mFileLoader.LoadFile(it->GetString());
		const char *script = reinterpret_cast<const char *>(scriptHandle.Get().mData);
		const size_t length = scriptHandle.Get().mLength;
		TokenStream stream = mLexer.Lex(script, length)->GetTokenStream();

		if (!HasErrors())
		{
			TranslationUnit *translationUnit = mParser.Parse(stream);
			if (!HasErrors())
			{
				translationUnit->SetRequiresCodeGeneration(mRecursiveCompileEnabled || (i < numFilesToCompile));
				const IncludeDirective *includeDirectiveList = translationUnit->GetIncludeDirectiveList();
				while (includeDirectiveList != NULL)
				{
					AddInputFile(includeDirectiveList->GetIncludePath()->GetStringValue());
					includeDirectiveList = NextNode(includeDirectiveList);
				}
			}
		}
		++it;
		++i;
	}

	if (!HasErrors())
	{
		mSemanticAnalyzer.Analyze(mParser.GetTranslationUnitList());
	}
}


bool FrontEnd::HasErrors() const
{
	return
		mLexer.GetErrorBuffer().HasErrors() ||
		mParser.GetErrorBuffer().HasErrors() ||
		mSemanticAnalyzer.GetErrorBuffer().HasErrors();
}

}
