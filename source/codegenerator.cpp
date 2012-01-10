#include "bond/autostack.h"
#include "bond/codegenerator.h"
#include "bond/parsenodes.h"
#include "bond/parsenodetraverser.h"
#include "bond/set.h"
#include "bond/version.h"

namespace Bond
{

typedef Set<HashedString> StringPool;

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
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(NamedInitializer *namedInitializer);

	StringPool::Type mStringPool;
	BoolStack mAddDeclarations;
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
	BoolStack::Element addDeclarationsElement(mAddDeclarations, true);
	mStringPool.insert("List");
	Traverse(mTranslationUnitList);
}


void GeneratorCore::Visit(FunctionDefinition *functionDefinition)
{
	BoolStack::Element addDeclarationsElement(mAddDeclarations, false);
	mStringPool.insert("Func");
	mStringPool.insert(functionDefinition->GetName()->GetHashedText());
	ParseNodeTraverser::Visit(functionDefinition);
}


void GeneratorCore::Visit(NamedInitializer *namedInitializer)
{
	if (mAddDeclarations.GetTop())
	{
		mStringPool.insert("Decl");
		mStringPool.insert(namedInitializer->GetName()->GetHashedText());
	}
	ParseNodeTraverser::Visit(namedInitializer);
}

}
