#include "bond/autostack.h"
#include "bond/linearallocator.h"
#include "bond/parsenodecounter.h"
#include "bond/parsenodes.h"
#include "bond/parsenodeutil.h"
#include "bond/parsenodetraverser.h"
#include "bond/semanticanalyzer.h"
#include "bond/symboltable.h"
#include <new>

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
		mSymbolTable(symbolTable),
		mAllocator(allocator)
	{}

	Symbol *InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);

	typedef AutoStack<Symbol *> ScopeStack;

	ScopeStack mScopeStack;
	ParseErrorBuffer &mErrorBuffer;
	SymbolTable &mSymbolTable;

private:

	Allocator &mAllocator;
};


Symbol *SymbolTablePopulator::InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	Symbol *symbol = parent->FindSymbol(name);

	if (symbol != 0)
	{
		mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, name, symbol->GetName());
	}

	symbol = CreateSymbol(type, name, definition, parent);
	parent->InsertSymbol(symbol);

	return symbol;
}


Symbol *SymbolTablePopulator::GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	Symbol *symbol = parent->FindSymbol(name);

	if ((symbol != 0) && (symbol->GetType() != type))
	{
		mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, name, symbol->GetName());
		symbol = 0;
	}

	if (symbol == 0)
	{
		symbol = CreateSymbol(type, name, definition, parent);
		parent->InsertSymbol(symbol);
	}

	return symbol;
}


Symbol *SymbolTablePopulator::CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	return new (mAllocator.Alloc<Symbol>()) Symbol(type, name, definition, parent);
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

	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(EnumDeclaration *enumDeclaration);
	virtual void Visit(Enumerator *enumerator);
	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(NamedInitializer *namedInitializer);
};


void TopLevelSymbolPopulator::Populate(TranslationUnit *translationUnitList)
{
	ScopeStack::Element globalScopeElement(mScopeStack, mSymbolTable.GetGlobalScope());
	ParseNodeTraverser::TraverseList(translationUnitList);
}


void TopLevelSymbolPopulator::Visit(NamespaceDefinition *namespaceDefinition)
{
	const Token *name = namespaceDefinition->GetName();
	Symbol *parent = mScopeStack.GetTop();
	Symbol *symbol = GetOrInsertSymbol(Symbol::TYPE_NAMESPACE, name, namespaceDefinition, parent);
	ScopeStack::Element stackElement(mScopeStack, symbol);
	ParseNodeTraverser::Visit(namespaceDefinition);
}


void TopLevelSymbolPopulator::Visit(EnumDeclaration *enumDeclaration)
{
	const Token *name = enumDeclaration->GetName();
	Symbol *parent = mScopeStack.GetTop();
	Symbol *symbol = InsertSymbol(Symbol::TYPE_ENUM, name, enumDeclaration, parent);
	ScopeStack::Element stackElement(mScopeStack, symbol);
	ParseNodeTraverser::Visit(enumDeclaration);
}


void TopLevelSymbolPopulator::Visit(Enumerator *enumerator)
{
	const Token *name = enumerator->GetName();
	Symbol *parent = mScopeStack.GetTop();
	InsertSymbol(Symbol::TYPE_VALUE, name, enumerator, parent);
	parent = parent->GetParent();
	InsertSymbol(Symbol::TYPE_VALUE, name, enumerator, parent);
}


void TopLevelSymbolPopulator::Visit(StructDeclaration *structDeclaration)
{
	const Token *name = structDeclaration->GetName();
	Symbol *parent = mScopeStack.GetTop();
	InsertSymbol(Symbol::TYPE_STRUCT, name, structDeclaration, parent);
}


void TopLevelSymbolPopulator::Visit(FunctionDefinition *functionDefinition)
{
	const FunctionPrototype *currentPrototype = functionDefinition->GetPrototype();
	const Token *name = currentPrototype->GetName();
	Symbol *parent = mScopeStack.GetTop();

	Symbol *symbol = parent->FindSymbol(name);
	if (symbol != 0)
	{
		const FunctionDefinition *previousFunction = CastNode<FunctionDefinition>(symbol->GetDefinition());
		const FunctionPrototype *previousPrototype = previousFunction->GetPrototype();

		if (!TestMatchingFunctionPrototypes(currentPrototype, previousPrototype))
		{
			mErrorBuffer.PushError(ParseError::FUNCTION_PROTOTYPE_MISMATCH, name, symbol->GetName());
		}
		else if (!functionDefinition->IsDeclaration())
		{
			// We've found a definition for a previously declared function. Replace the declaration.
			if (previousFunction->IsDeclaration())
			{
				symbol->SetDefinition(functionDefinition);
			}
			// We have two definitions for the same function.
			else
			{
				mErrorBuffer.PushError(ParseError::DUPLICATE_FUNCTION_DEFINITION, name, symbol->GetName());
			}
		}
	}
	else
	{
		InsertSymbol(Symbol::TYPE_FUNCTION, name, functionDefinition, parent);
	}
}


void TopLevelSymbolPopulator::Visit(NamedInitializer *namedInitializer)
{
	const Token *name = namedInitializer->GetName();
	Symbol *parent = mScopeStack.GetTop();
	InsertSymbol(Symbol::TYPE_VALUE, name, namedInitializer, parent);
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

	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(EnumDeclaration *enumDeclaration);
	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(FunctionDefinition *functionDefinition) {}
	virtual void Visit(DeclarativeStatement *declarativeStatement) {}

private:
	typedef AutoStack<Symbol *> ScopeStack;

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

	const int symbolCount =
		nodeCount.mNamespaceDefinition +
		nodeCount.mEnumDeclaration +
		// Enumerators are added to the enum and to the parent scope of the enum.
		(nodeCount.mEnumerator * 2) +
		nodeCount.mStructDeclaration +
		nodeCount.mFunctionDefinition +
		nodeCount.mParameter +
		nodeCount.mNamedInitializer +
		nodeCount.mCompoundStatement +
		nodeCount.mSwitchSection +
		nodeCount.mForStatement;

	const int size = sizeof(SymbolTable) + (symbolCount * sizeof(Symbol));
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
