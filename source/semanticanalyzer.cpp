#include "bond/compiler/semanticanalyzer.h"
#include "private/toplevelsymbolpass.h"
#include "private/typeevaluationpass.h"
#include "private/typespecifierresolutionpass.h"
#include "private/validationpass.h"
#include "private/valueevaluationpass.h"


namespace Bond
{

void SemanticAnalyzer::Analyze(TranslationUnit *translationUnitList)
{
	TopLevelSymbolPass topLevelSymbolPass(mErrorBuffer, mSymbolTable);
	topLevelSymbolPass.Analyze(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}

	TypeSpecifierResolutionPass typeSpecifierResolutionPass(mErrorBuffer, mSymbolTable);
	typeSpecifierResolutionPass.Analyze(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}

	TypeEvaluationPass typeEvaluationPass(mErrorBuffer, mSymbolTable, mPointerSize);
	typeEvaluationPass.Analyze(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}

	ValueEvaluationPass valueEvaluationPass(mErrorBuffer, mSymbolTable, mPointerSize);
	valueEvaluationPass.Analyze(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}

	ValidationPass validationPass(mErrorBuffer, mSymbolTable);
	validationPass.Analyze(translationUnitList);
}

}
