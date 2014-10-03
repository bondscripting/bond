#ifndef PRIVATE_BOND_VALUEEVALUATIONPASS_H
#define PRIVATE_BOND_VALUEEVALUATIONPASS_H

#include "private/semanticanalysispass.h"

namespace Bond
{

class TypeAndValue;

class ValueEvaluationPass: public SemanticAnalysisPass
{
public:
	ValueEvaluationPass(CompilerErrorBuffer &errorBuffer, SymbolTable &symbolTable, PointerSize pointerSize):
		SemanticAnalysisPass(errorBuffer, symbolTable),
		mPrevEnumerator(nullptr),
		mPointerSize(pointerSize),
		mHasResolvedItems(false),
		mHasUnresolvedItems(false)
	{}

	virtual ~ValueEvaluationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList) override;

protected:
	using SemanticAnalysisPass::Visit;
	virtual void Visit(EnumDeclaration *enumDeclaration) override;
	virtual void Visit(Enumerator *enumerator) override;
	virtual void Visit(StructDeclaration *structDeclaration) override;
	virtual void Visit(FunctionDefinition *functionDefinition) override;
	virtual void Visit(Parameter *parameter) override;
	virtual void Visit(TypeDescriptor *typeDescriptor) override;
	virtual void Visit(NamedInitializer *namedInitializer) override;
	virtual void Visit(SwitchLabel *switchLabel) override;
	virtual void Visit(DeclarativeStatement *declarativeStatement) override;
	virtual void Visit(ConditionalExpression *conditionalExpression) override;
	virtual void Visit(BinaryExpression *binaryExpression) override;
	virtual void Visit(UnaryExpression *unaryExpression) override;
	virtual void Visit(PostfixExpression *postfixExpression) override;
	virtual void Visit(MemberExpression *memberExpression) override;
	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression) override;
	virtual void Visit(FunctionCallExpression *functionCallExpression) override;
	virtual void Visit(CastExpression *castExpression) override;
	virtual void Visit(SizeofExpression *sizeofExpression) override;
	virtual void Visit(ConstantExpression *constantExpression) override;
	virtual void Visit(IdentifierExpression *identifierExpression) override;
	virtual void Visit(ThisExpression *thisExpression) override;

private:
	void Resolve(TypeAndValue &tav);
	void CheckUnresolved(const TypeAndValue &tav);

	CompilerErrorBuffer mUnresolvedErrorBuffer;
	Enumerator *mPrevEnumerator;
	PointerSize mPointerSize;
	bool mHasResolvedItems;
	bool mHasUnresolvedItems;
};

}

#endif
