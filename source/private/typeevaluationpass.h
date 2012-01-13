#ifndef PRIVATE_BOND_TYPEEVALUATIONPASS_H
#define PRIVATE_BOND_TYPEEVALUATIONPASS_H

#include "bond/parseerror.h"
#include "private/semanticanalysispass.h"

namespace Bond
{

// Type descriptors aren't yet fully resolved, so we cannot yet fully validate types in assignments,
// initializers, function calls and so forth, however we can test if the types are convertible. That
// will ensure that the types are sane enough to continue the analysis.
class TypeEvaluationPass: public SemanticAnalysisPass
{
public:
	TypeEvaluationPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable)
	{}

	virtual ~TypeEvaluationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	virtual void Visit(Enumerator *enumerator);
	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(Parameter *parameter);
	virtual void Visit(TypeDescriptor *typeDescriptor);
	virtual void Visit(NamedInitializer *namedInitializer);
	virtual void Visit(IfStatement *ifStatement);
	virtual void Visit(SwitchStatement *switchStatement);
	virtual void Visit(SwitchLabel *switchLabel);
	virtual void Visit(WhileStatement *whileStatement);
	virtual void Visit(ForStatement *forStatement);
	virtual void Visit(ConditionalExpression *conditionalExpression);
	virtual void Visit(BinaryExpression *binaryExpression);
	virtual void Visit(UnaryExpression *unaryExpression);
	virtual void Visit(PostfixExpression *postfixExpression);
	virtual void Visit(MemberExpression *memberExpression);
	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression);
	virtual void Visit(FunctionCallExpression *functionCallExpression);
	virtual void Visit(CastExpression *castExpression);
	virtual void Visit(SizeofExpression *sizeofExpression);
	virtual void Visit(ConstantExpression *constantExpression);
	virtual void Visit(IdentifierExpression *identifierExpression);
	virtual void Visit(ThisExpression *thisExpression);

private:
	typedef AutoStack<const StructDeclaration *> StructStack;
	typedef AutoStack<const FunctionDefinition *> FunctionStack;

	class RecursiveStructAnalyzer: private ParseNodeTraverser
	{
	public:
		RecursiveStructAnalyzer(ParseErrorBuffer &errorBuffer):
			mErrorBuffer(errorBuffer),
			mTopLevelStruct(NULL)
		{}

		virtual ~RecursiveStructAnalyzer() {}

		void Analyze(const StructDeclaration *structDeclaration);

	private:
		virtual void Visit(const StructDeclaration *structDeclaration);
		virtual void Visit(const FunctionDefinition *functionDefinition) {}
		virtual void Visit(const DeclarativeStatement *declarativeStatement);
		virtual void Visit(const TypeDescriptor *typeDescriptor);
		virtual void Visit(const TypeSpecifier *typeSpecifier);

		StructStack mStruct;
		ParseErrorBuffer &mErrorBuffer;
		const StructDeclaration *mTopLevelStruct;
	};

	bool AssertBooleanExpression(const Expression *expression, ParseError::Type errorType) const;
	bool AssertIntegerExpression(const Expression *expression, ParseError::Type errorType, const void *arg = NULL) const;
	bool AssertNonConstExpression(const Token *op);
	bool AssertBooleanOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertIntegerOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertNumericOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertPointerOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertLValueType(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertAssignableType(const TypeDescriptor *typeDescriptor, const Token *op);

	bool AssertConvertibleTypes(
		const TypeDescriptor *fromType,
		const TypeDescriptor *toType,
		const Token *context,
		ParseError::Type errorType);

	bool AssertComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB, const Token *op);

	void ValidateInitializer(
		const Token *name,
		const Initializer *initializer,
		const TypeDescriptor *typeDescrioptor);

	BoolStack mAddNamedInitializers;
	BoolStack mEnforceConstExpressions;
	BoolStack mEnforceConstDeclarations;
	StructStack mStruct;
	FunctionStack mFunction;
};

}

#endif
