#ifndef BOND_EXPRESSIONTYPEEVALUATOR_H
#define BOND_EXPRESSIONTYPEEVALUATOR_H

#include "bond/semanticanalysistraverser.h"

namespace Bond
{

class ExpressionTypeEvaluator: public SemanticAnalysisTraverser
{
public:
	ExpressionTypeEvaluator(const SemanticAnalysisTraverser &other):
		SemanticAnalysisTraverser(other),
		mMadeChanges(false)
	{}

	virtual void Visit(ConditionalExpression *conditionalExpression);
	//virtual void Visit(BinaryExpression *binaryExpression);
	virtual void Visit(UnaryExpression *unaryExpression);
	virtual void Visit(PostfixExpression *postfixExpression);
	//virtual void Visit(MemberExpression *memberExpression);
	//virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression);
	//virtual void Visit(FunctionCallExpression *functionCallExpression);
	virtual void Visit(CastExpression *castExpression);
	virtual void Visit(SizeofExpression *sizeofExpression);
	virtual void Visit(ConstantExpression *constantExpression);
	//virtual void Visit(IdentifierExpression *identifierValue);

private:
	void AssertBooleanType(const TypeDescriptor *descriptor, const Token *op);
	void AssertIntegerType(const TypeDescriptor *descriptor, const Token *op);
	void AssertNumericType(const TypeDescriptor *descriptor, const Token *op);

	bool mMadeChanges;
};

}

#endif
