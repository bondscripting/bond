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

void FrontEnd::AddInputFile(const SimpleString &inputFileName)
{
	if (!ContainsInputFile(inputFileName))
	{
		mInputFileNameList.push_back(inputFileName);
	}
}


bool FrontEnd::ContainsInputFile(const SimpleString &inputFileName)
{
	return find(mInputFileNameList.begin(), mInputFileNameList.end(), inputFileName) != mInputFileNameList.end();
}


void FrontEnd::Analyze()
{
	const size_t numFilesToCompile = mInputFileNameList.size();
	size_t i = 0;
	StringList::Type::const_iterator it = mInputFileNameList.begin();
	while ((it != mInputFileNameList.end()) && !HasErrors())
	{
		const char *fileName = it->GetString();
		auto scriptHandle = mFileLoader.LoadFile(fileName);
		const char *script = reinterpret_cast<const char *>(scriptHandle.Get().mData);
		const size_t length = scriptHandle.Get().mLength;
		mTokenCollectionList.push_back(mLexer.Lex(fileName, script, length));

		if (!HasErrors())
		{
			TokenStream stream = mTokenCollectionList.back()->GetTokenStream();
			TranslationUnit *translationUnit = mParser.Parse(stream);
			if (!HasErrors())
			{
				translationUnit->SetRequiresCodeGeneration(i < numFilesToCompile);
				const IncludeDirective *includeDirectiveList = translationUnit->GetIncludeDirectiveList();
				while (includeDirectiveList != nullptr)
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
