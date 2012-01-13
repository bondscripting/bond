#include "bond/autostack.h"
#include "bond/codegenerator.h"
#include "bond/parsenodes.h"
#include "bond/parsenodetraverser.h"
#include "bond/set.h"
#include "bond/version.h"
#include <stdio.h>

namespace Bond
{

class GeneratorCore: private ParseNodeTraverser
{
public:
	GeneratorCore(Allocator &allocator, const TranslationUnit *translationUnitList, bu32_t pointerSize):
		mStringPool((StringPool::Compare()), (StringPool::Allocator(&allocator))),
		mAllocator(allocator),
		mTranslationUnitList(translationUnitList),
		mPointerSize(pointerSize)
	{}

	void Generate();

private:
	virtual void Visit(const StructDeclaration *structDeclaration);
	virtual void Visit(const FunctionDefinition *functionDefinition);
	virtual void Visit(const NamedInitializer *namedInitializer);

	struct GeneratorResult
	{
		enum Result
		{
			RESULT_NONE,
			RESULT_FRAME_POINTER_OFFSET,
			RESULT_STACK_ADDRESS,
			RESULT_CONSTANT_VALUE,
			RESULT_STACK_VALUE,
		};

		GeneratorResult(): mResult(RESULT_NONE) {}

		TypeAndValue mTav;
		Result mResult;
	};

	typedef AutoStack<const StructDeclaration *> StructStack;
	typedef AutoStack<const FunctionDefinition *> FunctionStack;
	typedef AutoStack<GeneratorResult> ResultStack;
	typedef Set<HashedString> StringPool;

	StringPool::Type mStringPool;
	ResultStack mResult;
	StructStack mStruct;
	FunctionStack mFunction;
	BoolStack mIsTopLevelDeclaration;
	Allocator &mAllocator;
	const TranslationUnit *mTranslationUnitList;
	bu32_t mPointerSize;
};


void CodeGenerator::Generate(const TranslationUnit *translationUnitList)
{
	GeneratorCore generator(mAllocator, translationUnitList, mPointerSize);
	generator.Generate();
}


void GeneratorCore::Generate()
{
	BoolStack::Element isTopLevelDeclarationElement(mIsTopLevelDeclaration, true);
	mStringPool.insert("List");
	Traverse(mTranslationUnitList);

	printf("%p %u\n", mTranslationUnitList, mStringPool.size());
	for (StringPool::Type::const_iterator it = mStringPool.begin(); it != mStringPool.end(); ++it)
	{
		printf("%s\n", it->GetString());
	}
}


void GeneratorCore::Visit(const StructDeclaration *structDeclaration)
{
	if (structDeclaration->GetVariant() == StructDeclaration::VARIANT_BOND)
	{
		BoolStack::Element isTopLevelDeclarationElement(mIsTopLevelDeclaration, false);
		ParseNodeTraverser::Visit(structDeclaration);
	}
}


void GeneratorCore::Visit(const FunctionDefinition *functionDefinition)
{
	BoolStack::Element isTopLevelDeclarationElement(mIsTopLevelDeclaration, false);
	mStringPool.insert("Func");
	mStringPool.insert(functionDefinition->GetName()->GetHashedText());
	ParseNodeTraverser::Visit(functionDefinition);
}


void GeneratorCore::Visit(const NamedInitializer *namedInitializer)
{
	if (mIsTopLevelDeclaration.GetTop())
	{
		mStringPool.insert("Decl");
		mStringPool.insert(namedInitializer->GetName()->GetHashedText());
	}
	ParseNodeTraverser::Visit(namedInitializer);
}

}
