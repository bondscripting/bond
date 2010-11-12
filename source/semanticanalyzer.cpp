#include "bond/autostack.h"
#include "bond/linearallocator.h"
#include "bond/parsenodecounter.h"
#include "bond/parsenodes.h"
#include "bond/parsenodetraverser.h"
#include "bond/semanticanalyzer.h"
#include "bond/symboltable.h"
#include <new>

namespace Bond
{

//------------------------------------------------------------------------------
// GlobalSymbolPopulator
//------------------------------------------------------------------------------
class GlobalSymbolPopulator: ParseNodeTraverser
{
public:
	GlobalSymbolPopulator(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		mErrorBuffer(errorBuffer),
		mAllocator(allocator),
		mSymbolTable(symbolTable)
	{}

	virtual ~GlobalSymbolPopulator() {}

	void Populate();

	virtual void VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition);
	virtual void VisitEnumDeclaration(EnumDeclaration *enumDeclaration);
	virtual void VisitEnumerator(Enumerator *enumerator);
	virtual void VisitStructDeclaration(StructDeclaration *structDeclaration);
	virtual void VisitFunctionDefinition(FunctionDefinition *functionDefinition);
	virtual void VisitDeclarativeStatement(DeclarativeStatement *declarativeStatement);

private:
	typedef AutoStack<Scope *> ScopeStack;

	ScopeStack mScopeStack;
	ParseErrorBuffer &mErrorBuffer;
	Allocator &mAllocator;
	SymbolTable &mSymbolTable;
};


void GlobalSymbolPopulator::Populate()
{
	ScopeStack::Element globalScopeElement(mScopeStack, mSymbolTable.GetGlobalScope());
}


void GlobalSymbolPopulator::VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition)
{
	const Token *name = namespaceDefinition->GetName();

	Scope *parentScope = mScopeStack.GetTop();
	Scope *namespaceScope = parentScope->FindScope(name);

	if (namespaceScope == 0)
	{
		namespaceScope = new (mAllocator.Alloc<Scope>()) Scope(name, namespaceDefinition, parentScope);
		parentScope->InsertScope(namespaceScope);
	}

	ScopeStack::Element namespaceScopeElement(mScopeStack, namespaceScope);
	ParseNodeTraverser::VisitNamespaceDefinition(namespaceDefinition);
}


void GlobalSymbolPopulator::VisitEnumDeclaration(EnumDeclaration *enumDeclaration)
{
}


void GlobalSymbolPopulator::VisitEnumerator(Enumerator *enumerator)
{
}


void GlobalSymbolPopulator::VisitStructDeclaration(StructDeclaration *structDeclaration)
{
}


void GlobalSymbolPopulator::VisitFunctionDefinition(FunctionDefinition *functionDefinition)
{
}


void GlobalSymbolPopulator::VisitDeclarativeStatement(DeclarativeStatement *declarativeStatement)
{
}



SemanticAnalyzer::~SemanticAnalyzer()
{
	Dispose();
}


void SemanticAnalyzer::Dispose()
{
	mErrorBuffer.Reset();
	mAllocator.Free(mSymbolTable);
	mSymbolTable = 0;
}


void SemanticAnalyzer::Analyze(TranslationUnit *translationUnitList)
{
	Dispose();

	// Tally symbol table space requirements.
	ParseNodeCounter counter;
	counter.CountList(translationUnitList);
	const ParseNodeCount &nodeCount = counter.GetCount();

	const int symbolCount =
		nodeCount.mEnumerator +
		nodeCount.mFunctionDefinition +
		nodeCount.mParameter +
		nodeCount.mNamedInitializer +
		nodeCount.mCompoundStatement;

	const int scopeCount =
		nodeCount.mNamespaceDefinition +
		nodeCount.mEnumDeclaration +
		nodeCount.mStructDeclaration +
		nodeCount.mSwitchSection +
		nodeCount.mForStatement;

	const int size = sizeof(SymbolTable) + (symbolCount * sizeof(Symbol)) + (scopeCount * sizeof(Scope));
	char *buffer = mAllocator.Alloc<char>(size);
	LinearAllocator linearAllocator(buffer, size);

	mSymbolTable = new (linearAllocator.Alloc<SymbolTable>()) SymbolTable();
}


void SemanticAnalyzer::PopulateSymbolTable(Allocator &allocator)
{
	// Add all global symbols to the symbol table first, since they can be used prior to their declaration.
}

}
