#include "framework/parsenodecounter.h"
#include "bond/parsenodes.h"

namespace TestFramework
{

void ParseNodeCounter::VisitTranslationUnit(const Bond::TranslationUnit *translationUnit)
{
	++mCount.mTranslationUnit;
	ConstParseNodeTraverser::VisitTranslationUnit(translationUnit);
}


void ParseNodeCounter::VisitNamespaceDefinition(const Bond::NamespaceDefinition *namespaceDefinition)
{
	++mCount.mNamespaceDefinition;
	ConstParseNodeTraverser::VisitNamespaceDefinition(namespaceDefinition);
}


void ParseNodeCounter::VisitEnumDeclaration(const Bond::EnumDeclaration *enumDeclaration)
{
	++mCount.mEnumDeclaration;
	ConstParseNodeTraverser::VisitEnumDeclaration(enumDeclaration);
}


void ParseNodeCounter::VisitEnumerator(const Bond::Enumerator *enumerator)
{
	++mCount.mEnumerator;
	ConstParseNodeTraverser::VisitEnumerator(enumerator);
}


void ParseNodeCounter::VisitStructDeclaration(const Bond::StructDeclaration *structDeclaration)
{
	++mCount.mStructDeclaration;
	ConstParseNodeTraverser::VisitStructDeclaration(structDeclaration);
}


void ParseNodeCounter::VisitFunctionDefinition(const Bond::FunctionDefinition *functionDefinition)
{
	++mCount.mFunctionDefinition;
	ConstParseNodeTraverser::VisitFunctionDefinition(functionDefinition);
}


void ParseNodeCounter::VisitFunctionPrototype(const Bond::FunctionPrototype *functionPrototype)
{
	++mCount.mFunctionPrototype;
	ConstParseNodeTraverser::VisitFunctionPrototype(functionPrototype);
}


void ParseNodeCounter::VisitParameter(const Bond::Parameter *parameter)
{
	++mCount.mParameter;
	ConstParseNodeTraverser::VisitParameter(parameter);
}


void ParseNodeCounter::VisitTypeDescriptor(const Bond::TypeDescriptor *typeDescriptor)
{
	++mCount.mTypeDescriptor;
	ConstParseNodeTraverser::VisitTypeDescriptor(typeDescriptor);
}


void ParseNodeCounter::VisitTypeSpecifier(const Bond::TypeSpecifier *typeSpecifier)
{
	++mCount.mTypeSpecifier;
	ConstParseNodeTraverser::VisitTypeSpecifier(typeSpecifier);
}


void ParseNodeCounter::VisitQualifiedIdentifier(const Bond::QualifiedIdentifier *identifier)
{
	++mCount.mQualifiedIdentifier;
	ConstParseNodeTraverser::VisitQualifiedIdentifier(identifier);
}


void ParseNodeCounter::VisitNamedInitializer(const Bond::NamedInitializer *namedInitializer)
{
	++mCount.mNamedInitializer;
	ConstParseNodeTraverser::VisitNamedInitializer(namedInitializer);
}


void ParseNodeCounter::VisitInitializer(const Bond::Initializer *initializer)
{
	++mCount.mInitializer;
	ConstParseNodeTraverser::VisitInitializer(initializer);
}


void ParseNodeCounter::VisitCompoundStatement(const Bond::CompoundStatement *compoundStatement)
{
	++mCount.mCompoundStatement;
	ConstParseNodeTraverser::VisitCompoundStatement(compoundStatement);
}


void ParseNodeCounter::VisitIfStatement(const Bond::IfStatement *ifStatement)
{
	++mCount.mIfStatement;
	ConstParseNodeTraverser::VisitIfStatement(ifStatement);
}


void ParseNodeCounter::VisitSwitchStatement(const Bond::SwitchStatement *switchStatement)
{
	++mCount.mSwitchStatement;
	ConstParseNodeTraverser::VisitSwitchStatement(switchStatement);
}


void ParseNodeCounter::VisitSwitchSection(const Bond::SwitchSection *switchSection)
{
	++mCount.mSwitchSection;
	ConstParseNodeTraverser::VisitSwitchSection(switchSection);
}


void ParseNodeCounter::VisitSwitchLabel(const Bond::SwitchLabel *switchLabel)
{
	++mCount.mSwitchLabel;
	ConstParseNodeTraverser::VisitSwitchLabel(switchLabel);
}


void ParseNodeCounter::VisitWhileStatement(const Bond::WhileStatement *whileStatement)
{
	++mCount.mWhileStatement;
	ConstParseNodeTraverser::VisitWhileStatement(whileStatement);
}


void ParseNodeCounter::VisitForStatement(const Bond::ForStatement *forStatement)
{
	++mCount.mForStatement;
	ConstParseNodeTraverser::VisitForStatement(forStatement);
}


void ParseNodeCounter::VisitJumpStatement(const Bond::JumpStatement *jumpStatement)
{
	++mCount.mJumpStatement;
	ConstParseNodeTraverser::VisitJumpStatement(jumpStatement);
}


void ParseNodeCounter::VisitDeclarativeStatement(const Bond::DeclarativeStatement *declarativeStatement)
{
	++mCount.mDeclarativeStatement;
	ConstParseNodeTraverser::VisitDeclarativeStatement(declarativeStatement);
}


void ParseNodeCounter::VisitExpressionStatement(const Bond::ExpressionStatement *expressionStatement)
{
	++mCount.mExpressionStatement;
	ConstParseNodeTraverser::VisitExpressionStatement(expressionStatement);
}


void ParseNodeCounter::VisitConditionalExpression(const Bond::ConditionalExpression *conditionalExpression)
{
	++mCount.mConditionalExpression;
	ConstParseNodeTraverser::VisitConditionalExpression(conditionalExpression);
}


void ParseNodeCounter::VisitBinaryExpression(const Bond::BinaryExpression *binaryExpression)
{
	++mCount.mBinaryExpression;
	ConstParseNodeTraverser::VisitBinaryExpression(binaryExpression);
}


void ParseNodeCounter::VisitUnaryExpression(const Bond::UnaryExpression *unaryExpression)
{
	++mCount.mUnaryExpression;
	ConstParseNodeTraverser::VisitUnaryExpression(unaryExpression);
}


void ParseNodeCounter::VisitPostfixExpression(const Bond::PostfixExpression *postfixExpression)
{
	++mCount.mPostfixExpression;
	ConstParseNodeTraverser::VisitPostfixExpression(postfixExpression);
}


void ParseNodeCounter::VisitMemberExpression(const Bond::MemberExpression *memberExpression)
{
	++mCount.mMemberExpression;
	ConstParseNodeTraverser::VisitMemberExpression(memberExpression);
}


void ParseNodeCounter::VisitArraySubscriptExpression(const Bond::ArraySubscriptExpression *arraySubscriptExpression)
{
	++mCount.mArraySubscriptExpression;
	ConstParseNodeTraverser::VisitArraySubscriptExpression(arraySubscriptExpression);
}


void ParseNodeCounter::VisitFunctionCallExpression(const Bond::FunctionCallExpression *functionCallExpression)
{
	++mCount.mFunctionCallExpression;
	ConstParseNodeTraverser::VisitFunctionCallExpression(functionCallExpression);
}


void ParseNodeCounter::VisitCastExpression(const Bond::CastExpression *castExpression)
{
	++mCount.mCastExpression;
	ConstParseNodeTraverser::VisitCastExpression(castExpression);
}


void ParseNodeCounter::VisitSizeofExpression(const Bond::SizeofExpression *sizeofExpression)
{
	++mCount.mSizeofExpression;
	ConstParseNodeTraverser::VisitSizeofExpression(sizeofExpression);
}


void ParseNodeCounter::VisitConstantExpression(const Bond::ConstantExpression *constantExpression)
{
	++mCount.mConstantExpression;
	ConstParseNodeTraverser::VisitConstantExpression(constantExpression);
}


void ParseNodeCounter::VisitIdentifierExpression(const Bond::IdentifierExpression *identifierExpression)
{
	++mCount.mIdentifierExpression;
	ConstParseNodeTraverser::VisitIdentifierExpression(identifierExpression);
}

}
