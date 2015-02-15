#ifndef PRIVATE_BOND_TYPEEVALUATIONPASS_H
#define PRIVATE_BOND_TYPEEVALUATIONPASS_H

#include "bond/compiler/compilererror.h"
#include "private/semanticanalysispass.h"

namespace Bond
{

// Type descriptors aren't yet fully resolved, so we cannot yet fully validate types in assignments,
// initializers, function calls and so forth, however we can test if the types are convertible. That
// will ensure that the types are sane enough to continue the analysis.
class TypeEvaluationPass: public SemanticAnalysisPass
{
public:
	TypeEvaluationPass(CompilerErrorBuffer &errorBuffer, SymbolTable &symbolTable, PointerSize pointerSize):
		SemanticAnalysisPass(errorBuffer, symbolTable),
		mPointerSize(pointerSize)
	{}

	virtual ~TypeEvaluationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList) override;

protected:
	using SemanticAnalysisPass::Visit;
	virtual void Visit(Enumerator *enumerator) override;
	virtual void Visit(StructDeclaration *structDeclaration) override;
	virtual void Visit(FunctionDefinition *functionDefinition) override;
	virtual void Visit(Parameter *parameter) override;
	virtual void Visit(TypeDescriptor *typeDescriptor) override;
	virtual void Visit(NamedInitializer *namedInitializer) override;
	virtual void Visit(IfStatement *ifStatement) override;
	virtual void Visit(SwitchStatement *switchStatement) override;
	virtual void Visit(SwitchLabel *switchLabel) override;
	virtual void Visit(WhileStatement *whileStatement) override;
	virtual void Visit(ForStatement *forStatement) override;
	virtual void Visit(ConditionalExpression *conditionalExpression) override;
	virtual void Visit(BinaryExpression *binaryExpression) override;
	virtual void Visit(UnaryExpression *unaryExpression) override;
	virtual void Visit(PostfixExpression *postfixExpression) override;
	virtual void Visit(MemberExpression *memberExpression) override;
	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression) override;
	virtual void Visit(FunctionCallExpression *functionCallExpression) override;
	virtual void Visit(CastExpression *castExpression) override;
	virtual void Visit(SizeofExpression *sizeofExpression) override;
	virtual void Visit(ConstantLiteralExpression *constantExpression) override;
	virtual void Visit(IdentifierExpression *identifierExpression) override;
	virtual void Visit(ThisExpression *thisExpression) override;

private:
	typedef AutoStack<const StructDeclaration *> StructStack;
	typedef AutoStack<const FunctionDefinition *> FunctionStack;

	class RecursiveStructAnalyzer: private ParseNodeTraverser
	{
	public:
		RecursiveStructAnalyzer(CompilerErrorBuffer &errorBuffer):
			mErrorBuffer(errorBuffer),
			mTopLevelStruct(nullptr)
		{}

		virtual ~RecursiveStructAnalyzer() {}

		void Analyze(const StructDeclaration *structDeclaration);

	private:
		virtual void Visit(const StructDeclaration *structDeclaration) override;
		virtual void Visit(const FunctionDefinition *functionDefinition) override {}
		virtual void Visit(const DeclarativeStatement *declarativeStatement) override;
		virtual void Visit(const TypeDescriptor *typeDescriptor) override;
		virtual void Visit(const TypeSpecifier *typeSpecifier) override;

		StructStack mStruct;
		CompilerErrorBuffer &mErrorBuffer;
		const StructDeclaration *mTopLevelStruct;
	};

	bool AssertBooleanExpression(const Expression *expression, CompilerError::Type errorType) const;
	bool AssertMost32IntegerExpression(const Expression *expression, CompilerError::Type errorType, const void *arg = nullptr) const;
	bool AssertNonConstExpression(const Token *op);
	bool AssertBooleanOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertIntegerOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertNumericOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertPointerOperand(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertAddressableType(const TypeDescriptor *typeDescriptor, const Token *op);
	bool AssertAssignableType(const TypeDescriptor *typeDescriptor, const Token *op);

	bool AssertConvertibleTypes(
		const TypeDescriptor *fromType,
		const TypeDescriptor *toType,
		const Token *context,
		CompilerError::Type errorType);

	bool AssertComparableTypes(const TypeDescriptor *typeA, const TypeDescriptor *typeB, const Token *op);

	void ValidateInitializer(Initializer *initializer, const TypeDescriptor *typeDescrioptor);

	BoolStack mEnforceConstExpressions;
	StructStack mStruct;
	FunctionStack mFunction;
	PointerSize mPointerSize;
};

}

#endif
