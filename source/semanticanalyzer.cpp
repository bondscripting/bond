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

typedef AutoStack<Symbol *> ScopeStack;

//------------------------------------------------------------------------------
// SemanticAnalysisTraverser
//------------------------------------------------------------------------------
class SemanticAnalysisTraverser: protected ParseNodeTraverser
{
public:
	virtual ~SemanticAnalysisTraverser() {}

	void Analyze(TranslationUnit *translationUnitList);

protected:
	SemanticAnalysisTraverser(
			ParseErrorBuffer &errorBuffer,
			Allocator &allocator,
			SymbolTable &symbolTable,
			ScopeStack &scopeStack):
		mErrorBuffer(errorBuffer),
		mSymbolTable(symbolTable),
		mAllocator(allocator),
		mScopeStack(scopeStack)
	{}

	SemanticAnalysisTraverser(const SemanticAnalysisTraverser &other):
		mErrorBuffer(other.mErrorBuffer),
		mSymbolTable(other.mSymbolTable),
		mAllocator(other.mAllocator),
		mScopeStack(other.mScopeStack)
	{}

	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(StructDeclaration *structDeclaration);

	Symbol *GetCurrentScope() { return mScopeStack.GetTop(); }

	Symbol *InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);

	ParseErrorBuffer &mErrorBuffer;

private:

	SymbolTable &mSymbolTable;
	Allocator &mAllocator;
	ScopeStack &mScopeStack;
};


void SemanticAnalysisTraverser::Analyze(TranslationUnit *translationUnitList)
{
	ScopeStack::Element globalScopeElement(mScopeStack, mSymbolTable.GetGlobalScope());
	ParseNodeTraverser::TraverseList(translationUnitList);
}


void SemanticAnalysisTraverser::Visit(NamespaceDefinition *namespaceDefinition)
{
	ScopeStack::Element stackElement(mScopeStack, namespaceDefinition->GetSymbol());
	ParseNodeTraverser::Visit(namespaceDefinition);
}


void SemanticAnalysisTraverser::Visit(StructDeclaration *structDeclaration)
{
	ScopeStack::Element stackElement(mScopeStack, structDeclaration->GetSymbol());
	ParseNodeTraverser::Visit(structDeclaration);
}


Symbol *SemanticAnalysisTraverser::InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
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


Symbol *SemanticAnalysisTraverser::GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
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


Symbol *SemanticAnalysisTraverser::CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	return new (mAllocator.Alloc<Symbol>()) Symbol(type, name, definition, parent);
}


//------------------------------------------------------------------------------
// ExpressionEvaluator
//------------------------------------------------------------------------------
class ExpressionEvaluator: public SemanticAnalysisTraverser
{
public:
	ExpressionEvaluator(const SemanticAnalysisTraverser &other): SemanticAnalysisTraverser(other) {}

	//virtual void Visit(ConditionalExpression *conditionalExpression);
	//virtual void Visit(BinaryExpression *binaryExpression);
	virtual void Visit(UnaryExpression *unaryExpression);
	//virtual void Visit(PostfixExpression *postfixExpression);
	//virtual void Visit(MemberExpression *memberExpression);
	//virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression);
	//virtual void Visit(FunctionCallExpression *functionCallExpression);
	//virtual void Visit(CastExpression *castExpression);
	//virtual void Visit(SizeofExpression *sizeofExpression);
	virtual void Visit(ConstantExpression *constantExpression);
	//virtual void Visit(IdentifierExpression *identifierValue);
};

	/*
void ExpressionEvaluator::Visit(BinaryExpression *binaryExpression)
{
	TypeAndValue tav = binaryExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined() && !tav.IsValueDefined())
	{
		Expression *lhs = binaryExpression->GetLhs();
		Expression *rhs = binaryExpression->GetRhs();
}
	*/

void ExpressionEvaluator::Visit(UnaryExpression *unaryExpression)
{
	/*
	Token::OP_PLUS,
	Token::OP_MINUS,
	Token::OP_MULT,
	Token::OP_INC,
	Token::OP_DEC,
	Token::OP_NOT,
	Token::OP_BIT_AND,
	Token::OP_BIT_NOT,
	*/
	TypeAndValue &tav = unaryExpression->GetTypeAndValue();

	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(unaryExpression);
		const TypeAndValue &rhs = unaryExpression->GetRhs()->GetTypeAndValue();

		if (rhs.IsResolved())
		{
			// TODO: Validate operation on operand type and operand.
			const Token::TokenType op = unaryExpression->GetOperator()->GetTokenType();
			//const Token::TokenType primitiveType = GetPrimitiveType(rhs.GetTypeDescriptor());

			switch (op)
			{
				case Token::OP_PLUS:
					break;
				case Token::OP_MINUS:
					break;
				case Token::OP_MULT:
					break;
				case Token::OP_INC:
					break;
				case Token::OP_DEC:
					break;
				case Token::OP_NOT:
					break;
				case Token::OP_BIT_AND:
					break;
				case Token::OP_BIT_NOT:
					break;
				default:
					break;
			}
			/*
			if (rhs.IsValueDefined())
			{
			}
			*/
			tav.SetTypeDescriptor(rhs.GetTypeDescriptor());
			tav.Resolve();
		}
	}
}


void ExpressionEvaluator::Visit(ConstantExpression *constantExpression)
{
	TypeAndValue &tav = constantExpression->GetTypeAndValue();

	if (!tav.IsResolved())
	{
		const Token *token = constantExpression->GetValueToken();

		switch (token->GetTokenType())
		{
			case Token::CONST_BOOL:
				tav.SetTypeDescriptor(&CONST_BOOL_TYPE_DESCRIPTOR);
				break;
			case Token::CONST_CHAR:
				tav.SetTypeDescriptor(&CONST_CHAR_TYPE_DESCRIPTOR);
				break;
			case Token::CONST_INT:
				tav.SetTypeDescriptor(&CONST_INT_TYPE_DESCRIPTOR);
				break;
			case Token::CONST_UINT:
				tav.SetTypeDescriptor(&CONST_UINT_TYPE_DESCRIPTOR);
				break;
			case Token::CONST_FLOAT:
				tav.SetTypeDescriptor(&CONST_FLOAT_TYPE_DESCRIPTOR);
				break;
			case Token::CONST_STRING:
				tav.SetTypeDescriptor(&CONST_STRING_TYPE_DESCRIPTOR);
				break;
			default:
				break;
		}

		tav.SetValue(token->GetValue());
		tav.Resolve();
	}
}


//------------------------------------------------------------------------------
// SemanticAnalysicsPass
//------------------------------------------------------------------------------
class SemanticAnalysisPass: public SemanticAnalysisTraverser
{
public:
	SemanticAnalysisPass(ParseErrorBuffer &errorBuffer, Allocator &allocator, SymbolTable &symbolTable):
		SemanticAnalysisTraverser(errorBuffer, allocator, symbolTable, mScopeStack)
	{}

private:
	ScopeStack mScopeStack;
};


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
	SemanticAnalysisPass ::Visit(namespaceDefinition);
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
