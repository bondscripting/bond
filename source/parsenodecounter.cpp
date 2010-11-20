#include "bond/parsenodecounter.h"
#include "bond/parsenodes.h"

namespace Bond
{

void ParseNodeCounter::Visit(const TranslationUnit *translationUnit)
{
	++mCount.mTranslationUnit;
	ConstParseNodeTraverser::Visit(translationUnit);
}


void ParseNodeCounter::Visit(const NamespaceDefinition *namespaceDefinition)
{
	++mCount.mNamespaceDefinition;
	ConstParseNodeTraverser::Visit(namespaceDefinition);
}


void ParseNodeCounter::Visit(const EnumDeclaration *enumDeclaration)
{
	++mCount.mEnumDeclaration;
	ConstParseNodeTraverser::Visit(enumDeclaration);
}


void ParseNodeCounter::Visit(const Enumerator *enumerator)
{
	++mCount.mEnumerator;
	ConstParseNodeTraverser::Visit(enumerator);
}


void ParseNodeCounter::Visit(const StructDeclaration *structDeclaration)
{
	++mCount.mStructDeclaration;
	ConstParseNodeTraverser::Visit(structDeclaration);
}


void ParseNodeCounter::Visit(const FunctionDefinition *functionDefinition)
{
	++mCount.mFunctionDefinition;
	ConstParseNodeTraverser::Visit(functionDefinition);
}


void ParseNodeCounter::Visit(const FunctionPrototype *functionPrototype)
{
	++mCount.mFunctionPrototype;
	ConstParseNodeTraverser::Visit(functionPrototype);
}


void ParseNodeCounter::Visit(const Parameter *parameter)
{
	++mCount.mParameter;
	ConstParseNodeTraverser::Visit(parameter);
}


void ParseNodeCounter::Visit(const TypeDescriptor *typeDescriptor)
{
	++mCount.mTypeDescriptor;
	ConstParseNodeTraverser::Visit(typeDescriptor);
}


void ParseNodeCounter::Visit(const TypeSpecifier *typeSpecifier)
{
	++mCount.mTypeSpecifier;
	ConstParseNodeTraverser::Visit(typeSpecifier);
}


void ParseNodeCounter::Visit(const QualifiedIdentifier *identifier)
{
	++mCount.mQualifiedIdentifier;
	ConstParseNodeTraverser::Visit(identifier);
}


void ParseNodeCounter::Visit(const NamedInitializer *namedInitializer)
{
	++mCount.mNamedInitializer;
	ConstParseNodeTraverser::Visit(namedInitializer);
}


void ParseNodeCounter::Visit(const Initializer *initializer)
{
	++mCount.mInitializer;
	ConstParseNodeTraverser::Visit(initializer);
}


void ParseNodeCounter::Visit(const CompoundStatement *compoundStatement)
{
	++mCount.mCompoundStatement;
	ConstParseNodeTraverser::Visit(compoundStatement);
}


void ParseNodeCounter::Visit(const IfStatement *ifStatement)
{
	++mCount.mIfStatement;
	ConstParseNodeTraverser::Visit(ifStatement);
}


void ParseNodeCounter::Visit(const SwitchStatement *switchStatement)
{
	++mCount.mSwitchStatement;
	ConstParseNodeTraverser::Visit(switchStatement);
}


void ParseNodeCounter::Visit(const SwitchSection *switchSection)
{
	++mCount.mSwitchSection;
	ConstParseNodeTraverser::Visit(switchSection);
}


void ParseNodeCounter::Visit(const SwitchLabel *switchLabel)
{
	++mCount.mSwitchLabel;
	ConstParseNodeTraverser::Visit(switchLabel);
}


void ParseNodeCounter::Visit(const WhileStatement *whileStatement)
{
	++mCount.mWhileStatement;
	ConstParseNodeTraverser::Visit(whileStatement);
}


void ParseNodeCounter::Visit(const ForStatement *forStatement)
{
	++mCount.mForStatement;
	ConstParseNodeTraverser::Visit(forStatement);
}


void ParseNodeCounter::Visit(const JumpStatement *jumpStatement)
{
	++mCount.mJumpStatement;
	ConstParseNodeTraverser::Visit(jumpStatement);
}


void ParseNodeCounter::Visit(const DeclarativeStatement *declarativeStatement)
{
	++mCount.mDeclarativeStatement;
	ConstParseNodeTraverser::Visit(declarativeStatement);
}


void ParseNodeCounter::Visit(const ExpressionStatement *expressionStatement)
{
	++mCount.mExpressionStatement;
	ConstParseNodeTraverser::Visit(expressionStatement);
}


void ParseNodeCounter::Visit(const ConditionalExpression *conditionalExpression)
{
	++mCount.mConditionalExpression;
	ConstParseNodeTraverser::Visit(conditionalExpression);
}


void ParseNodeCounter::Visit(const BinaryExpression *binaryExpression)
{
	++mCount.mBinaryExpression;
	ConstParseNodeTraverser::Visit(binaryExpression);
}


void ParseNodeCounter::Visit(const UnaryExpression *unaryExpression)
{
	++mCount.mUnaryExpression;
	ConstParseNodeTraverser::Visit(unaryExpression);
}


void ParseNodeCounter::Visit(const PostfixExpression *postfixExpression)
{
	++mCount.mPostfixExpression;
	ConstParseNodeTraverser::Visit(postfixExpression);
}


void ParseNodeCounter::Visit(const MemberExpression *memberExpression)
{
	++mCount.mMemberExpression;
	ConstParseNodeTraverser::Visit(memberExpression);
}


void ParseNodeCounter::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	++mCount.mArraySubscriptExpression;
	ConstParseNodeTraverser::Visit(arraySubscriptExpression);
}


void ParseNodeCounter::Visit(const FunctionCallExpression *functionCallExpression)
{
	++mCount.mFunctionCallExpression;
	ConstParseNodeTraverser::Visit(functionCallExpression);
}


void ParseNodeCounter::Visit(const CastExpression *castExpression)
{
	++mCount.mCastExpression;
	ConstParseNodeTraverser::Visit(castExpression);
}


void ParseNodeCounter::Visit(const SizeofExpression *sizeofExpression)
{
	++mCount.mSizeofExpression;
	ConstParseNodeTraverser::Visit(sizeofExpression);
}


void ParseNodeCounter::Visit(const ConstantExpression *constantExpression)
{
	++mCount.mConstantExpression;
	ConstParseNodeTraverser::Visit(constantExpression);
}


void ParseNodeCounter::Visit(const IdentifierExpression *identifierExpression)
{
	++mCount.mIdentifierExpression;
	ConstParseNodeTraverser::Visit(identifierExpression);
}

}
