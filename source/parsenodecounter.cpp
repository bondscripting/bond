#include "bond/parsenodecounter.h"
#include "bond/parsenodes.h"

namespace Bond
{

void ParseNodeCounter::Visit(const TranslationUnit *translationUnit)
{
	++mCount.mTranslationUnit;
	ParseNodeTraverser::Visit(translationUnit);
}


void ParseNodeCounter::Visit(const NamespaceDefinition *namespaceDefinition)
{
	++mCount.mNamespaceDefinition;
	ParseNodeTraverser::Visit(namespaceDefinition);
}


void ParseNodeCounter::Visit(const EnumDeclaration *enumDeclaration)
{
	++mCount.mEnumDeclaration;
	ParseNodeTraverser::Visit(enumDeclaration);
}


void ParseNodeCounter::Visit(const Enumerator *enumerator)
{
	++mCount.mEnumerator;
	ParseNodeTraverser::Visit(enumerator);
}


void ParseNodeCounter::Visit(const StructDeclaration *structDeclaration)
{
	++mCount.mStructDeclaration;
	ParseNodeTraverser::Visit(structDeclaration);
}


void ParseNodeCounter::Visit(const FunctionDefinition *functionDefinition)
{
	++mCount.mFunctionDefinition;
	ParseNodeTraverser::Visit(functionDefinition);
}


void ParseNodeCounter::Visit(const FunctionPrototype *functionPrototype)
{
	++mCount.mFunctionPrototype;
	ParseNodeTraverser::Visit(functionPrototype);
}


void ParseNodeCounter::Visit(const Parameter *parameter)
{
	++mCount.mParameter;
	ParseNodeTraverser::Visit(parameter);
}


void ParseNodeCounter::Visit(const TypeDescriptor *typeDescriptor)
{
	++mCount.mTypeDescriptor;
	ParseNodeTraverser::Visit(typeDescriptor);
}


void ParseNodeCounter::Visit(const TypeSpecifier *typeSpecifier)
{
	++mCount.mTypeSpecifier;
	ParseNodeTraverser::Visit(typeSpecifier);
}


void ParseNodeCounter::Visit(const QualifiedIdentifier *identifier)
{
	++mCount.mQualifiedIdentifier;
	ParseNodeTraverser::Visit(identifier);
}


void ParseNodeCounter::Visit(const NamedInitializer *namedInitializer)
{
	++mCount.mNamedInitializer;
	ParseNodeTraverser::Visit(namedInitializer);
}


void ParseNodeCounter::Visit(const Initializer *initializer)
{
	++mCount.mInitializer;
	ParseNodeTraverser::Visit(initializer);
}


void ParseNodeCounter::Visit(const CompoundStatement *compoundStatement)
{
	++mCount.mCompoundStatement;
	ParseNodeTraverser::Visit(compoundStatement);
}


void ParseNodeCounter::Visit(const IfStatement *ifStatement)
{
	++mCount.mIfStatement;
	ParseNodeTraverser::Visit(ifStatement);
}


void ParseNodeCounter::Visit(const SwitchStatement *switchStatement)
{
	++mCount.mSwitchStatement;
	ParseNodeTraverser::Visit(switchStatement);
}


void ParseNodeCounter::Visit(const SwitchSection *switchSection)
{
	++mCount.mSwitchSection;
	ParseNodeTraverser::Visit(switchSection);
}


void ParseNodeCounter::Visit(const SwitchLabel *switchLabel)
{
	++mCount.mSwitchLabel;
	ParseNodeTraverser::Visit(switchLabel);
}


void ParseNodeCounter::Visit(const WhileStatement *whileStatement)
{
	++mCount.mWhileStatement;
	ParseNodeTraverser::Visit(whileStatement);
}


void ParseNodeCounter::Visit(const ForStatement *forStatement)
{
	++mCount.mForStatement;
	ParseNodeTraverser::Visit(forStatement);
}


void ParseNodeCounter::Visit(const JumpStatement *jumpStatement)
{
	++mCount.mJumpStatement;
	ParseNodeTraverser::Visit(jumpStatement);
}


void ParseNodeCounter::Visit(const DeclarativeStatement *declarativeStatement)
{
	++mCount.mDeclarativeStatement;
	ParseNodeTraverser::Visit(declarativeStatement);
}


void ParseNodeCounter::Visit(const ExpressionStatement *expressionStatement)
{
	++mCount.mExpressionStatement;
	ParseNodeTraverser::Visit(expressionStatement);
}


void ParseNodeCounter::Visit(const ConditionalExpression *conditionalExpression)
{
	++mCount.mConditionalExpression;
	ParseNodeTraverser::Visit(conditionalExpression);
}


void ParseNodeCounter::Visit(const BinaryExpression *binaryExpression)
{
	++mCount.mBinaryExpression;
	ParseNodeTraverser::Visit(binaryExpression);
}


void ParseNodeCounter::Visit(const UnaryExpression *unaryExpression)
{
	++mCount.mUnaryExpression;
	ParseNodeTraverser::Visit(unaryExpression);
}


void ParseNodeCounter::Visit(const PostfixExpression *postfixExpression)
{
	++mCount.mPostfixExpression;
	ParseNodeTraverser::Visit(postfixExpression);
}


void ParseNodeCounter::Visit(const MemberExpression *memberExpression)
{
	++mCount.mMemberExpression;
	ParseNodeTraverser::Visit(memberExpression);
}


void ParseNodeCounter::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	++mCount.mArraySubscriptExpression;
	ParseNodeTraverser::Visit(arraySubscriptExpression);
}


void ParseNodeCounter::Visit(const FunctionCallExpression *functionCallExpression)
{
	++mCount.mFunctionCallExpression;
	ParseNodeTraverser::Visit(functionCallExpression);
}


void ParseNodeCounter::Visit(const CastExpression *castExpression)
{
	++mCount.mCastExpression;
	ParseNodeTraverser::Visit(castExpression);
}


void ParseNodeCounter::Visit(const SizeofExpression *sizeofExpression)
{
	++mCount.mSizeofExpression;
	ParseNodeTraverser::Visit(sizeofExpression);
}


void ParseNodeCounter::Visit(const ConstantExpression *constantExpression)
{
	++mCount.mConstantExpression;
	ParseNodeTraverser::Visit(constantExpression);
}


void ParseNodeCounter::Visit(const IdentifierExpression *identifierExpression)
{
	++mCount.mIdentifierExpression;
	ParseNodeTraverser::Visit(identifierExpression);
}


void ParseNodeCounter::Visit(const EmptyExpression *emptyExpression)
{
	++mCount.mEmptyExpression;
	ParseNodeTraverser::Visit(emptyExpression);
}

}
