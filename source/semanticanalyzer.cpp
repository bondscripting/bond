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
// SemanticAnalysisPass
//------------------------------------------------------------------------------
class SemanticAnalysisPass: protected ParseNodeTraverser
{
public:
	virtual ~SemanticAnalysisPass() {}

	void Analyze(TranslationUnit *translationUnitList);

protected:
	SemanticAnalysisPass(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		mErrorBuffer(errorBuffer),
		mSymbolTable(symbolTable),
		mAllocator(allocator)
	{}

	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(StructDeclaration *structDeclaration);

	Symbol *GetCurrentScope() { return mScopeStack.GetTop(); }

	Symbol *InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);

	typedef AutoStack<Symbol *> ScopeStack;

	ParseErrorBuffer &mErrorBuffer;

private:

	ScopeStack mScopeStack;
	SymbolTable &mSymbolTable;
	Allocator &mAllocator;
};


void SemanticAnalysisPass::Analyze(TranslationUnit *translationUnitList)
{
	ScopeStack::Element globalScopeElement(mScopeStack, mSymbolTable.GetGlobalScope());
	ParseNodeTraverser::TraverseList(translationUnitList);
}


void SemanticAnalysisPass::Visit(NamespaceDefinition *namespaceDefinition)
{
	ScopeStack::Element stackElement(mScopeStack, namespaceDefinition->GetSymbol());
	ParseNodeTraverser::Visit(namespaceDefinition);
}


void SemanticAnalysisPass::Visit(StructDeclaration *structDeclaration)
{
	ScopeStack::Element stackElement(mScopeStack, structDeclaration->GetSymbol());
	ParseNodeTraverser::Visit(structDeclaration);
}


Symbol *SemanticAnalysisPass::InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
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


Symbol *SemanticAnalysisPass::GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
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


Symbol *SemanticAnalysisPass::CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	return new (mAllocator.Alloc<Symbol>()) Symbol(type, name, definition, parent);
}


//------------------------------------------------------------------------------
// TypeAndConstantDeclarationPass
//------------------------------------------------------------------------------
class TypeAndConstantDeclarationPass: public SemanticAnalysisPass
{
public:
	TypeAndConstantDeclarationPass(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, allocator, symbolTable)
	{}

	virtual ~TypeAndConstantDeclarationPass() {}

	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(EnumDeclaration *enumDeclaration);
	virtual void Visit(Enumerator *enumerator);
	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(FunctionDefinition *functionDefinition) {}
	virtual void Visit(NamedInitializer *namedInitializer);
};


void TypeAndConstantDeclarationPass::Visit(NamespaceDefinition *namespaceDefinition)
{
	const Token *name = namespaceDefinition->GetName();
	Symbol *parent = GetCurrentScope();
	Symbol *symbol = GetOrInsertSymbol(Symbol::TYPE_NAMESPACE, name, namespaceDefinition, parent);
	namespaceDefinition->SetSymbol(symbol);
	SemanticAnalysisPass::Visit(namespaceDefinition);
}


void TypeAndConstantDeclarationPass::Visit(EnumDeclaration *enumDeclaration)
{
	const Token *name = enumDeclaration->GetName();
	Symbol *parent = GetCurrentScope();
	InsertSymbol(Symbol::TYPE_ENUM, name, enumDeclaration, parent);
	ParseNodeTraverser::Visit(enumDeclaration);
}


void TypeAndConstantDeclarationPass::Visit(Enumerator *enumerator)
{
	const Token *name = enumerator->GetName();
	Symbol *parent = GetCurrentScope();
	InsertSymbol(Symbol::TYPE_CONSTANT, name, enumerator, parent);
}


void TypeAndConstantDeclarationPass::Visit(StructDeclaration *structDeclaration)
{
	const Token *name = structDeclaration->GetName();
	Symbol *parent = GetCurrentScope();
	Symbol *symbol = InsertSymbol(Symbol::TYPE_STRUCT, name, structDeclaration, parent);
	structDeclaration->SetSymbol(symbol);
}

/*
void TypeAndConstantDeclarationPass::Visit(FunctionDefinition *functionDefinition)
{
	const FunctionPrototype *currentPrototype = functionDefinition->GetPrototype();
	const Token *name = currentPrototype->GetName();
	Symbol *parent = GetCurrentScope();

	Symbol *symbol = parent->FindSymbol(name);
	if (symbol != 0)
	{
		const FunctionDefinition *previousFunction = CastNode<FunctionDefinition>(symbol->GetDefinition());
		const FunctionPrototype *previousPrototype = previousFunction->GetPrototype();

		if (!AreMatchingFunctionPrototypes(parent, currentPrototype, previousPrototype))
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
*/

void TypeAndConstantDeclarationPass::Visit(NamedInitializer *namedInitializer)
{
	// TODO: Assert that type is a valid top-level constant type.
	// Assert that the expression is a constant expression.
	const Token *name = namedInitializer->GetName();
	Symbol *parent = GetCurrentScope();
	InsertSymbol(Symbol::TYPE_CONSTANT, name, namedInitializer, parent);
}


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
		//(nodeCount.mEnumerator * 2) +
		nodeCount.mEnumerator +
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

	Analyze(translationUnitList, linearAllocator);
}


void SemanticAnalyzer::Analyze(TranslationUnit *translationUnitList, Allocator &allocator)
{
	// Add all type declarations to the symbol table first, since they can be used prior to their declaration
	// in other typed declarations (e.g. function return type and parameter types).
	TypeAndConstantDeclarationPass typeAndConstantPass(mErrorBuffer, allocator, *mSymbolTable);
	typeAndConstantPass.Analyze(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}
}

}
