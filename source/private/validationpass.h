#ifndef PRIVATE_BOND_VALIDATIONPASS_H
#define PRIVATE_BOND_VALIDATIONPASS_H

#include "private/semanticanalysispass.h"

namespace Bond
{

class ValidationPass: public SemanticAnalysisPass
{
public:
	ValidationPass(CompilerErrorBuffer &errorBuffer, SymbolTable &symbolTable):
		SemanticAnalysisPass(errorBuffer, symbolTable)
	{}

	virtual ~ValidationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList) override;

protected:
	using SemanticAnalysisPass::Visit;
	virtual void Visit(FunctionDefinition *functionDefinition) override;
	virtual void Visit(FunctionPrototype *functionPrototype) override;
	virtual void Visit(Parameter *parameter) override;
	virtual void Visit(NamedInitializer *namedInitializer) override;
	virtual void Visit(IfStatement *ifStatement) override;
	virtual void Visit(SwitchStatement *switchStatement) override;
	virtual void Visit(SwitchSection *switchSection) override;
	virtual void Visit(SwitchLabel *switchLabel) override;
	virtual void Visit(WhileStatement *whileStatement) override;
	virtual void Visit(ForStatement *forStatement) override;
	virtual void Visit(JumpStatement *jumpStatement) override;
	virtual void Visit(DeclarativeStatement *declarativeStatement) override;
	virtual void Visit(ExpressionStatement *expressionStatement) override;
	virtual void Visit(BinaryExpression *binaryExpression) override;
	virtual void Visit(UnaryExpression *unaryExpression) override;
	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression) override;
	virtual void Visit(FunctionCallExpression *functionCallExpression) override;

private:
	typedef AutoStack<const TypeDescriptor *> TypeStack;
	typedef AutoStack<const FunctionDefinition *> FunctionStack;
	typedef AutoStack<ResolvedSwitchLabel *> SwitchLabelStack;

	void ValidateInitializer(Initializer *initializer, const TypeDescriptor &typeDescrioptor);
	void AssertReachableCode(const ParseNode *node);
	size_t GetJumpTargetId();

	bool AssertAssignableTypes(
		const TypeAndValue &fromTav,
		const TypeDescriptor &toType,
		const Token *context,
		CompilerError::Type errorType,
		bool ignoreConst = false);

	BoolStack mHasDefaultLabel;
	BoolStack mEndsWithJump;
	BoolStack mHasReturn;
	BoolStack mIsInLoop;
	BoolStack mIsInSwitch;
	SizeStack mNextJumpTargetId;
	SizeStack mSwitchJumpTargetId;
	TypeStack mReturnType;
	FunctionStack mFunction;
	SwitchLabelStack mSwitchLabelList;
};

}

#endif
