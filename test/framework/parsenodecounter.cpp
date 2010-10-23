#include "framework/parsenodecounter.h"
#include "bond/parsenodes.h"

namespace TestFramework
{

void ParseNodeCounter::Count(const Bond::ParseNode *parseNode)
{
	if (parseNode != 0)
	{
		parseNode->Accept(*this);
	}
}


void ParseNodeCounter::VisitTranslationUnit(const Bond::TranslationUnit *translationUnit)
{
	++mCount.mTranslationUnit;
	CountList(translationUnit->GetExternalDeclarationList());
}


void ParseNodeCounter::VisitNamespaceDefinition(const Bond::NamespaceDefinition *namespaceDefinition)
{
	++mCount.mNamespaceDefinition;
	CountList(namespaceDefinition->GetExternalDeclarationList());
}


void ParseNodeCounter::VisitEnumDeclaration(const Bond::EnumDeclaration *enumDeclaration)
{
	++mCount.mEnumDeclaration;
	CountList(enumDeclaration->GetEnumeratorList());
}


void ParseNodeCounter::VisitEnumerator(const Bond::Enumerator *enumerator)
{
	++mCount.mEnumerator;
	Count(enumerator->GetValue());
}


void ParseNodeCounter::VisitFunctionDefinition(const Bond::FunctionDefinition *functionDefinition)
{
	++mCount.mFunctionDefinition;
	Count(functionDefinition->GetPrototype());
	Count(functionDefinition->GetBody());
}


void ParseNodeCounter::VisitFunctionPrototype(const Bond::FunctionPrototype *functionPrototype)
{
	++mCount.mFunctionPrototype;
	Count(functionPrototype->GetReturnType());
	CountList(functionPrototype->GetParameterList());
}


void ParseNodeCounter::VisitParameter(const Bond::Parameter *parameter)
{
	++mCount.mParameter;
	Count(parameter->GetTypeDescriptor());
}


void ParseNodeCounter::VisitTypeDescriptor(const Bond::TypeDescriptor *typeDescriptor)
{
	++mCount.mTypeDescriptor;
	Count(typeDescriptor->GetTypeSpecifier());
	Count(typeDescriptor->GetParent());
	Count(typeDescriptor->GetLength());
}


void ParseNodeCounter::VisitTypeSpecifier(const Bond::TypeSpecifier *typeSpecifier)
{
	++mCount.mTypeSpecifier;
	CountList(typeSpecifier->GetIdentifier());
}


void ParseNodeCounter::VisitQualifiedIdentifier(const Bond::QualifiedIdentifier *identifier)
{
	++mCount.mQualifiedIdentifier;
}


void ParseNodeCounter::VisitNamedInitializer(const Bond::NamedInitializer *namedInitializer)
{
	++mCount.mNamedInitializer;
	Count(namedInitializer->GetInitializer());
}


void ParseNodeCounter::VisitInitializer(const Bond::Initializer *initializer)
{
	++mCount.mInitializer;
	Count(initializer->GetExpression());
	CountList(initializer->GetInitializerList());
}


void ParseNodeCounter::VisitCompoundStatement(const Bond::CompoundStatement *compoundStatement)
{
	++mCount.mCompoundStatement;
	CountList(compoundStatement->GetStatementList());
}


void ParseNodeCounter::VisitIfStatement(const Bond::IfStatement *ifStatement)
{
	++mCount.mIfStatement;
	Count(ifStatement->GetCondition());
	Count(ifStatement->GetThenStatement());
	Count(ifStatement->GetElseStatement());
}


void ParseNodeCounter::VisitSwitchStatement(const Bond::SwitchStatement *switchStatement)
{
	++mCount.mSwitchStatement;
	Count(switchStatement->GetControl());
	CountList(switchStatement->GetSectionList());
}


void ParseNodeCounter::VisitSwitchSection(const Bond::SwitchSection *switchSection)
{
	++mCount.mSwitchSection;
	CountList(switchSection->GetLabelList());
	CountList(switchSection->GetStatementList());
}


void ParseNodeCounter::VisitSwitchLabel(const Bond::SwitchLabel *switchLabel)
{
	++mCount.mSwitchLabel;
	Count(switchLabel->GetExpression());
}


void ParseNodeCounter::VisitWhileStatement(const Bond::WhileStatement *whileStatement)
{
	++mCount.mWhileStatement;
	Count(whileStatement->GetCondition());
	Count(whileStatement->GetBody());
}


void ParseNodeCounter::VisitJumpStatement(const Bond::JumpStatement *jumpStatement)
{
	++mCount.mJumpStatement;
	Count(jumpStatement->GetRhs());
}


void ParseNodeCounter::VisitDeclarativeStatement(const Bond::DeclarativeStatement *declarativeStatement)
{
	++mCount.mDeclarativeStatement;
	Count(declarativeStatement->GetTypeDescriptor());
	CountList(declarativeStatement->GetNamedInitializerList());
}


void ParseNodeCounter::VisitExpressionStatement(const Bond::ExpressionStatement *expressionStatement)
{
	++mCount.mExpressionStatement;
	Count(expressionStatement->GetExpression());
}


void ParseNodeCounter::VisitConditionalExpression(const Bond::ConditionalExpression *conditionalExpression)
{
	++mCount.mConditionalExpression;
	Count(conditionalExpression->GetCondition());
	Count(conditionalExpression->GetTrueExpression());
	Count(conditionalExpression->GetFalseExpression());
}


void ParseNodeCounter::VisitBinaryExpression(const Bond::BinaryExpression *binaryExpression)
{
	++mCount.mBinaryExpression;
	Count(binaryExpression->GetLhs());
	Count(binaryExpression->GetRhs());
}


void ParseNodeCounter::VisitUnaryExpression(const Bond::UnaryExpression *unaryExpression)
{
	++mCount.mUnaryExpression;
	Count(unaryExpression->GetRhs());
}


void ParseNodeCounter::VisitPostfixExpression(const Bond::PostfixExpression *postfixExpression)
{
	++mCount.mPostfixExpression;
	Count(postfixExpression->GetLhs());
}


void ParseNodeCounter::VisitMemberExpression(const Bond::MemberExpression *memberExpression)
{
	++mCount.mMemberExpression;
	Count(memberExpression->GetLhs());
}


void ParseNodeCounter::VisitArraySubscriptExpression(const Bond::ArraySubscriptExpression *arraySubscriptExpression)
{
	++mCount.mArraySubscriptExpression;
	Count(arraySubscriptExpression->GetLhs());
	Count(arraySubscriptExpression->GetIndex());
}


void ParseNodeCounter::VisitFunctionCallExpression(const Bond::FunctionCallExpression *functionCallExpression)
{
	++mCount.mFunctionCallExpression;
	Count(functionCallExpression->GetLhs());
	CountList(functionCallExpression->GetArgumentList());
}


void ParseNodeCounter::VisitCastExpression(const Bond::CastExpression *castExpression)
{
	++mCount.mCastExpression;
	Count(castExpression->GetRhs());
}


void ParseNodeCounter::VisitSizeofExpression(const Bond::SizeofExpression *sizeofExpression)
{
	++mCount.mSizeofExpression;
	Count(sizeofExpression->GetTypeDescriptor());
	Count(sizeofExpression->GetRhs());
}


void ParseNodeCounter::VisitConstantExpression(const Bond::ConstantExpression *constantExpression)
{
	++mCount.mConstantExpression;
}


void ParseNodeCounter::VisitIdentifierExpression(const Bond::IdentifierExpression *identifierExpression)
{
	++mCount.mIdentifierExpression;
	CountList(identifierExpression->GetIdentifier());
}


void ParseNodeCounter::CountList(const Bond::ListParseNode *listNode)
{
	const Bond::ListParseNode *current = listNode;
	while (current != 0)
	{
		Count(current);
		current = current->GetNext();
	}
}

}
