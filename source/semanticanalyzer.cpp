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

class SemanticAnalysisPass: protected ParseNodeTraverser
{
public:
	virtual ~SemanticAnalysisPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	SemanticAnalysisPass(
			ParseErrorBuffer &errorBuffer,
			Allocator &allocator,
			SymbolTable &symbolTable):
		mErrorBuffer(errorBuffer),
		mSymbolTable(symbolTable),
		mAllocator(allocator)
	{}

	virtual void Visit(TranslationUnit *translationUnit);
	virtual void Visit(NamespaceDefinition *namespaceDefinition);
	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(CompoundStatement *compoundStatement);
	virtual void Visit(SwitchSection *switchSection);
	virtual void Visit(ForStatement *forStatement);

	Symbol *GetSymbol(const Token *name);
	Symbol *GetSymbol(const QualifiedIdentifier *identifier);
	Symbol *InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition);
	Symbol *GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition);

	ParseErrorBuffer &mErrorBuffer;

private:
	SemanticAnalysisPass(const SemanticAnalysisPass &other);
	SemanticAnalysisPass &operator=(const SemanticAnalysisPass &other);

	Symbol *GetCurrentScope() { return mScopeStack.GetTop(); }
	Symbol *InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);
	Symbol *CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent);

	ScopeStack mScopeStack;
	SymbolTable &mSymbolTable;
	Allocator &mAllocator;
};


void SemanticAnalysisPass::Analyze(TranslationUnit *translationUnitList)
{
	ParseNodeTraverser::TraverseList(translationUnitList);
}


void SemanticAnalysisPass::Visit(TranslationUnit *translationUnit)
{
	ScopeStack::Element globalScopeElement(mScopeStack, mSymbolTable.GetGlobalScope());
	ParseNodeTraverser::Visit(translationUnit);
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


void SemanticAnalysisPass::Visit(FunctionDefinition *functionDefinition)
{
	//ScopeStack::Element stackElement(mScopeStack, functionDefinition->GetSymbol());
	ParseNodeTraverser::Visit(functionDefinition);
}


void SemanticAnalysisPass::Visit(CompoundStatement *compoundStatement)
{
	//ScopeStack::Element stackElement(mScopeStack, structDeclaration->GetSymbol());
	ParseNodeTraverser::Visit(compoundStatement);
}


void SemanticAnalysisPass::Visit(SwitchSection *switchSection)
{
	//ScopeStack::Element stackElement(mScopeStack, switchSection->GetSymbol());
	ParseNodeTraverser::Visit(switchSection);
}


void SemanticAnalysisPass::Visit(ForStatement *forStatement)
{
	//ScopeStack::Element stackElement(mScopeStack, forStatement->GetSymbol());
	ParseNodeTraverser::Visit(forStatement);
}


Symbol *SemanticAnalysisPass::GetSymbol(const Token *name)
{
	Symbol *parent = GetCurrentScope();
	return parent->FindSymbol(name);
}


Symbol *SemanticAnalysisPass::GetSymbol(const QualifiedIdentifier *identifier)
{
	Symbol *parent = GetCurrentScope();
	return parent->FindSymbol(identifier);
}


Symbol *SemanticAnalysisPass::InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition)
{
	Symbol *parent = GetCurrentScope();
	return InsertSymbol(type, name, definition, parent);
}


Symbol *SemanticAnalysisPass::InsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	Symbol *symbol = parent->FindSymbol(name);

	if (symbol != 0)
	{
		mErrorBuffer.PushError(ParseError::DUPLICATE_SYMBOL, name, symbol->GetName());
	}

	symbol = CreateSymbol(type, name, definition, parent);

	return symbol;
}


Symbol *SemanticAnalysisPass::GetOrInsertSymbol(Symbol::Type type, const Token *name, const ParseNode *definition)
{
	Symbol *parent = GetCurrentScope();
	return GetOrInsertSymbol(type, name, definition, parent);
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
	}

	return symbol;
}


Symbol *SemanticAnalysisPass::CreateSymbol(Symbol::Type type, const Token *name, const ParseNode *definition, Symbol *parent)
{
	Symbol *symbol = new (mAllocator.Alloc<Symbol>()) Symbol(type, name, definition, parent);
	parent->InsertSymbol(symbol);
	return symbol;
}

}


#include "semanticanalysis/typeevaluationpass.cpp"
#include "semanticanalysis/typeandconstantdeclarationpass.cpp"


namespace Bond
{
//------------------------------------------------------------------------------
// ExpressionEvaluator
//------------------------------------------------------------------------------
/*
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
	//virtual void Visit(IdentifierExpression *identifierExpression);
};


void ExpressionEvaluator::Visit(BinaryExpression *binaryExpression)
{
	TypeAndValue tav = binaryExpression->GetTypeAndValue();

	if (!tav.IsTypeDefined() && !tav.IsValueDefined())
	{
		Expression *lhs = binaryExpression->GetLhs();
		Expression *rhs = binaryExpression->GetRhs();
}


void ExpressionEvaluator::Visit(UnaryExpression *unaryExpression)
{
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

			if (rhs.IsValueDefined())
			{
			}

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

	TopLevelTypeEvaluationPass topLevelExpressionTypePass(mErrorBuffer, allocator, *mSymbolTable);
	topLevelExpressionTypePass.Analyze(translationUnitList);

	if (mErrorBuffer.HasErrors())
	{
		return;
	}
}

}
