#include "bond/autostack.h"
#include "bond/linearallocator.h"
#include "bond/parsenodecounter.h"
#include "bond/parsenodes.h"
#include "bond/parsenodetraverser.h"
#include "bond/semanticanalyzer.h"
#include "bond/symboltable.h"
#include <new>
#include <string.h>

namespace Bond
{

//------------------------------------------------------------------------------
// SymbolTablePopulator
//------------------------------------------------------------------------------
class SymbolTablePopulator: public ParseNodeTraverser
{
public:
	virtual ~SymbolTablePopulator() {}

protected:
	SymbolTablePopulator(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		mErrorBuffer(errorBuffer),
		mAllocator(allocator),
		mSymbolTable(symbolTable)
	{}

	Symbol *InsertSymbol(SymbolBase::Type type, const Token *name, const ParseNode *definition, Scope *parent);
	Scope *InsertScope(SymbolBase::Type type, const Token *name, const ParseNode *definition, Scope *parent);

	typedef AutoStack<Scope *> ScopeStack;

	ScopeStack mScopeStack;
	ParseErrorBuffer &mErrorBuffer;
	Allocator &mAllocator;
	SymbolTable &mSymbolTable;
};


Symbol *SymbolTablePopulator::InsertSymbol(SymbolBase::Type type, const Token *name, const ParseNode *definition, Scope *parent)
{
	Symbol *symbol = parent->FindSymbol(name);

	if (symbol == 0)
	{
		symbol = new (mAllocator.Alloc<Symbol>()) Symbol(type, name, definition);
		parent->InsertSymbol(symbol);
	}
	else
	{
		if (strcmp(name->GetText(), symbol->GetName()->GetText()) == 0)
		{
			mErrorBuffer.PushError(ParseError::HASH_COLLISION, name, symbol->GetName());
		}
		else
		{
			mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, name, symbol->GetName());
		}
	}

	return symbol;
}


Scope *SymbolTablePopulator::InsertScope(SymbolBase::Type type, const Token *name, const ParseNode *definition, Scope *parent)
{
	Scope *scope = parent->FindScope(name);

	if (scope == 0)
	{
		scope = new (mAllocator.Alloc<Scope>()) Scope(type, name, definition, parent);
		parent->InsertScope(scope);
	}
	else
	{
		if (strcmp(name->GetText(), scope->GetName()->GetText()) == 0)
		{
			mErrorBuffer.PushError(ParseError::HASH_COLLISION, name, scope->GetName());
		}
		else
		{
			mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, name, scope->GetName());
		}
	}

	return scope;
}


//------------------------------------------------------------------------------
// TopLevelSymbolPopulator
//------------------------------------------------------------------------------
class TopLevelSymbolPopulator: public SymbolTablePopulator
{
public:
	TopLevelSymbolPopulator(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		SymbolTablePopulator(errorBuffer, allocator, symbolTable)
	{}

	virtual ~TopLevelSymbolPopulator() {}

	void Populate(TranslationUnit *translationUnitList);

	virtual void VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition);
	virtual void VisitEnumDeclaration(EnumDeclaration *enumDeclaration);
	virtual void VisitEnumerator(Enumerator *enumerator);
	virtual void VisitStructDeclaration(StructDeclaration *structDeclaration);
	virtual void VisitFunctionDefinition(FunctionDefinition *functionDefinition) {}
	virtual void VisitDeclarativeStatement(DeclarativeStatement *declarativeStatement);
};


void TopLevelSymbolPopulator::Populate(TranslationUnit *translationUnitList)
{
	ScopeStack::Element globalScopeElement(mScopeStack, mSymbolTable.GetGlobalScope());
	ParseNodeTraverser::TraverseList(translationUnitList);
}


void TopLevelSymbolPopulator::VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition)
{
	const Token *name = namespaceDefinition->GetName();

	Scope *parentScope = mScopeStack.GetTop();
	Scope *currentScope = parentScope->FindScope(name);

	if (currentScope == 0)
	{
		currentScope = new (mAllocator.Alloc<Scope>()) Scope(SymbolBase::TYPE_NAMESPACE, name, namespaceDefinition, parentScope);
		parentScope->InsertScope(currentScope);
	}
	else
	{
		if (currentScope->GetType() != SymbolBase::TYPE_NAMESPACE)
		{
			mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, name, currentScope->GetName());
		}
	}

	ScopeStack::Element currentScopeElement(mScopeStack, currentScope);
	ParseNodeTraverser::VisitNamespaceDefinition(namespaceDefinition);
}


void TopLevelSymbolPopulator::VisitEnumDeclaration(EnumDeclaration *enumDeclaration)
{
	const Token *name = enumDeclaration->GetName();
	Scope *parent = mScopeStack.GetTop();
	Scope *scope = InsertScope(SymbolBase::TYPE_ENUM, name, enumDeclaration, parent);
	ScopeStack::Element stackElement(mScopeStack, scope);
	ParseNodeTraverser::VisitEnumDeclaration(enumDeclaration);
}


void TopLevelSymbolPopulator::VisitEnumerator(Enumerator *enumerator)
{
	const Token *name = enumerator->GetName();
	Scope *parent = mScopeStack.GetTop();
	InsertSymbol(SymbolBase::TYPE_VALUE, name, enumerator, parent);
	parent = parent->GetParent();
	InsertSymbol(SymbolBase::TYPE_VALUE, name, enumerator, parent);
}


void TopLevelSymbolPopulator::VisitStructDeclaration(StructDeclaration *structDeclaration)
{
	const Token *name = structDeclaration->GetName();
	Scope *parent = mScopeStack.GetTop();
	InsertScope(SymbolBase::TYPE_STRUCT, name, structDeclaration, parent);
}


void TopLevelSymbolPopulator::VisitDeclarativeStatement(DeclarativeStatement *declarativeStatement)
{
	/*
	const Token *name = declarativeStatement->GetName();
	Scope *parent = mScopeStack.GetTop();
	InsertSymbol(SymbolBase::TYPE_VALUE, name, enumerator, parent);
	*/
}


//------------------------------------------------------------------------------
// SymbolPopulator
//------------------------------------------------------------------------------
/*
class SymbolPopulator: ParseNodeTraverser
{
public:
	SymbolPopulator(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		mErrorBuffer(errorBuffer),
		mAllocator(allocator),
		mSymbolTable(symbolTable)
	{}

	virtual ~SymbolPopulator() {}

	void Populate();

	virtual void VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition);
	virtual void VisitEnumDeclaration(EnumDeclaration *enumDeclaration);
	virtual void VisitStructDeclaration(StructDeclaration *structDeclaration);
	virtual void VisitFunctionDefinition(FunctionDefinition *functionDefinition) {}
	virtual void VisitDeclarativeStatement(DeclarativeStatement *declarativeStatement) {}

private:
	typedef AutoStack<Scope *> ScopeStack;

	ScopeStack mScopeStack;
	ParseErrorBuffer &mErrorBuffer;
	Allocator &mAllocator;
	SymbolTable &mSymbolTable;
};
*/

//------------------------------------------------------------------------------
// SemanticAnalyser
//------------------------------------------------------------------------------
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

	const int symbolCount = 0;
		(nodeCount.mEnumerator * 2) +
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

	PopulateSymbolTable(translationUnitList, linearAllocator);
}


void SemanticAnalyzer::PopulateSymbolTable(TranslationUnit *translationUnitList, Allocator &allocator)
{
	// Add all type declarations to the symbol table first, since they can be used prior to their declaration
	// in other typed declarations (e.g. function return type and parameter types).
	TopLevelSymbolPopulator typePopulator(mErrorBuffer, allocator, *mSymbolTable);
	typePopulator.Populate(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}
}

}
