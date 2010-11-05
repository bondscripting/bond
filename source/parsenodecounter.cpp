#include "bond/parsenodecounter.h"
#include "bond/parsenodes.h"

namespace Bond
{

void ParseNodeCounter::VisitTranslationUnit(const TranslationUnit *translationUnit)
{
	++mCount.mTranslationUnit;
	ConstParseNodeTraverser::VisitTranslationUnit(translationUnit);
}


void ParseNodeCounter::VisitNamespaceDefinition(const NamespaceDefinition *namespaceDefinition)
{
	++mCount.mNamespaceDefinition;
	ConstParseNodeTraverser::VisitNamespaceDefinition(namespaceDefinition);
}


void ParseNodeCounter::VisitEnumDeclaration(const EnumDeclaration *enumDeclaration)
{
	++mCount.mEnumDeclaration;
	ConstParseNodeTraverser::VisitEnumDeclaration(enumDeclaration);
}


void ParseNodeCounter::VisitEnumerator(const Enumerator *enumerator)
{
	++mCount.mEnumerator;
	ConstParseNodeTraverser::VisitEnumerator(enumerator);
}


void ParseNodeCounter::VisitStructDeclaration(const StructDeclaration *structDeclaration)
{
	++mCount.mStructDeclaration;
	ConstParseNodeTraverser::VisitStructDeclaration(structDeclaration);
}


void ParseNodeCounter::VisitFunctionDefinition(const FunctionDefinition *functionDefinition)
{
	++mCount.mFunctionDefinition;
	ConstParseNodeTraverser::VisitFunctionDefinition(functionDefinition);
}


void ParseNodeCounter::VisitFunctionPrototype(const FunctionPrototype *functionPrototype)
{
	++mCount.mFunctionPrototype;
	ConstParseNodeTraverser::VisitFunctionPrototype(functionPrototype);
}


void ParseNodeCounter::VisitParameter(const Parameter *parameter)
{
	++mCount.mParameter;
	ConstParseNodeTraverser::VisitParameter(parameter);
}


void ParseNodeCounter::VisitTypeDescriptor(const TypeDescriptor *typeDescriptor)
{
	++mCount.mTypeDescriptor;
	ConstParseNodeTraverser::VisitTypeDescriptor(typeDescriptor);
}


void ParseNodeCounter::VisitTypeSpecifier(const TypeSpecifier *typeSpecifier)
{
	++mCount.mTypeSpecifier;
	ConstParseNodeTraverser::VisitTypeSpecifier(typeSpecifier);
}


void ParseNodeCounter::VisitQualifiedIdentifier(const QualifiedIdentifier *identifier)
{
	++mCount.mQualifiedIdentifier;
	ConstParseNodeTraverser::VisitQualifiedIdentifier(identifier);
}


void ParseNodeCounter::VisitNamedInitializer(const NamedInitializer *namedInitializer)
{
	++mCount.mNamedInitializer;
	ConstParseNodeTraverser::VisitNamedInitializer(namedInitializer);
}


void ParseNodeCounter::VisitInitializer(const Initializer *initializer)
{
	++mCount.mInitializer;
	ConstParseNodeTraverser::VisitInitializer(initializer);
}


void ParseNodeCounter::VisitCompoundStatement(const CompoundStatement *compoundStatement)
{
	++mCount.mCompoundStatement;
	ConstParseNodeTraverser::VisitCompoundStatement(compoundStatement);
}


void ParseNodeCounter::VisitIfStatement(const IfStatement *ifStatement)
{
	++mCount.mIfStatement;
	ConstParseNodeTraverser::VisitIfStatement(ifStatement);
}


void ParseNodeCounter::VisitSwitchStatement(const SwitchStatement *switchStatement)
{
	++mCount.mSwitchStatement;
	ConstParseNodeTraverser::VisitSwitchStatement(switchStatement);
}


void ParseNodeCounter::VisitSwitchSection(const SwitchSection *switchSection)
{
	++mCount.mSwitchSection;
	ConstParseNodeTraverser::VisitSwitchSection(switchSection);
}


void ParseNodeCounter::VisitSwitchLabel(const SwitchLabel *switchLabel)
{
	++mCount.mSwitchLabel;
	ConstParseNodeTraverser::VisitSwitchLabel(switchLabel);
}


void ParseNodeCounter::VisitWhileStatement(const WhileStatement *whileStatement)
{
	++mCount.mWhileStatement;
	ConstParseNodeTraverser::VisitWhileStatement(whileStatement);
}


void ParseNodeCounter::VisitForStatement(const ForStatement *forStatement)
{
	++mCount.mForStatement;
	ConstParseNodeTraverser::VisitForStatement(forStatement);
}


void ParseNodeCounter::VisitJumpStatement(const JumpStatement *jumpStatement)
{
	++mCount.mJumpStatement;
	ConstParseNodeTraverser::VisitJumpStatement(jumpStatement);
}


void ParseNodeCounter::VisitDeclarativeStatement(const DeclarativeStatement *declarativeStatement)
{
	++mCount.mDeclarativeStatement;
	ConstParseNodeTraverser::VisitDeclarativeStatement(declarativeStatement);
}


void ParseNodeCounter::VisitExpressionStatement(const ExpressionStatement *expressionStatement)
{
	++mCount.mExpressionStatement;
	ConstParseNodeTraverser::VisitExpressionStatement(expressionStatement);
}


void ParseNodeCounter::VisitConditionalExpression(const ConditionalExpression *conditionalExpression)
{
	++mCount.mConditionalExpression;
	ConstParseNodeTraverser::VisitConditionalExpression(conditionalExpression);
}


void ParseNodeCounter::VisitBinaryExpression(const BinaryExpression *binaryExpression)
{
	++mCount.mBinaryExpression;
	ConstParseNodeTraverser::VisitBinaryExpression(binaryExpression);
}


void ParseNodeCounter::VisitUnaryExpression(const UnaryExpression *unaryExpression)
{
	++mCount.mUnaryExpression;
	ConstParseNodeTraverser::VisitUnaryExpression(unaryExpression);
}


void ParseNodeCounter::VisitPostfixExpression(const PostfixExpression *postfixExpression)
{
	++mCount.mPostfixExpression;
	ConstParseNodeTraverser::VisitPostfixExpression(postfixExpression);
}


void ParseNodeCounter::VisitMemberExpression(const MemberExpression *memberExpression)
{
	++mCount.mMemberExpression;
	ConstParseNodeTraverser::VisitMemberExpression(memberExpression);
}


void ParseNodeCounter::VisitArraySubscriptExpression(const ArraySubscriptExpression *arraySubscriptExpression)
{
	++mCount.mArraySubscriptExpression;
	ConstParseNodeTraverser::VisitArraySubscriptExpression(arraySubscriptExpression);
}


void ParseNodeCounter::VisitFunctionCallExpression(const FunctionCallExpression *functionCallExpression)
{
	++mCount.mFunctionCallExpression;
	ConstParseNodeTraverser::VisitFunctionCallExpression(functionCallExpression);
}


void ParseNodeCounter::VisitCastExpression(const CastExpression *castExpression)
{
	++mCount.mCastExpression;
	ConstParseNodeTraverser::VisitCastExpression(castExpression);
}


void ParseNodeCounter::VisitSizeofExpression(const SizeofExpression *sizeofExpression)
{
	++mCount.mSizeofExpression;
	ConstParseNodeTraverser::VisitSizeofExpression(sizeofExpression);
}


void ParseNodeCounter::VisitConstantExpression(const ConstantExpression *constantExpression)
{
	++mCount.mConstantExpression;
	ConstParseNodeTraverser::VisitConstantExpression(constantExpression);
}


void ParseNodeCounter::VisitIdentifierExpression(const IdentifierExpression *identifierExpression)
{
	++mCount.mIdentifierExpression;
	ConstParseNodeTraverser::VisitIdentifierExpression(identifierExpression);
}

}
