#include "bond/compiler/compilererror.h"
#include "bond/compiler/frontend.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/parser.h"
#include "bond/compiler/semanticanalyzer.h"
#include "bond/io/streamfactory.h"
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


TranslationUnit *FrontEnd::Analyze()
{
	ParseNodeList<TranslationUnit> translationUnitList;
	const size_t numFilesToCompile = mInputFileNameList.size();
	size_t i = 0;
	StringList::const_iterator it = mInputFileNameList.begin();
	while ((it != mInputFileNameList.end()) && !HasErrors())
	{
		const char *fileName = it->GetString();
		auto streamHandle = mStreamFactory.CreateInputStream(fileName);
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
					AddInputFile(includeDirectiveList->GetIncludePath()->GetStringValue());
					includeDirectiveList = NextNode(includeDirectiveList);
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

}
