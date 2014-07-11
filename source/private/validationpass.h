#ifndef PRIVATE_BOND_VALIDATIONPASS_H
#define PRIVATE_BOND_VALIDATIONPASS_H

#include "private/semanticanalysispass.h"

namespace Bond
{

class ValidationPass: public SemanticAnalysisPass
{
public:
	ValidationPass(CompilerErrorBuffer &errorBuffer, SymbolTable &symbolTable, PointerSize pointerSize):
		SemanticAnalysisPass(errorBuffer, symbolTable),
		mPointerSize(pointerSize)
	{}

	virtual ~ValidationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	virtual void Visit(FunctionDefinition *functionDefinition);
	virtual void Visit(FunctionPrototype *functionPrototype);
	virtual void Visit(Parameter *parameter);
	virtual void Visit(IfStatement *ifStatement);
	virtual void Visit(SwitchStatement *switchStatement);
	virtual void Visit(SwitchSection *switchSection);
	virtual void Visit(SwitchLabel *switchLabel);
	virtual void Visit(WhileStatement *whileStatement);
	virtual void Visit(ForStatement *forStatement);
	virtual void Visit(JumpStatement *jumpStatement);
	virtual void Visit(DeclarativeStatement *declarativeStatement);
	virtual void Visit(ExpressionStatement *expressionStatement);
	virtual void Visit(BinaryExpression *binaryExpression);

private:
	typedef AutoStack<const TypeDescriptor *> TypeStack;
	typedef AutoStack<const FunctionDefinition *> FunctionStack;
	typedef AutoStack<ResolvedSwitchLabel *> SwitchLabelStack;

	void AssertReachableCode(const ParseNode *node);
	size_t GetJumpTargetId();

	BoolStack mHasDefaultLabel;
	BoolStack mEndsWithJump;
	BoolStack mHasReturn;
	BoolStack mIsInLoop;
	BoolStack mIsInSwitch;
	SizeStack mNextJumpTargetId;
	SizeStack mSwitchJumpTargetId;
	TypeStack mReturnType;
	FunctionStack mFunction;
	PointerSize mPointerSize;
	SwitchLabelStack mSwitchLabelList;
};

}

#endif
