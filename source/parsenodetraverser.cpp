#include "bond/parsenodetraverser.h"
#include "bond/parsenodes.h"

namespace Bond
{

//------------------------------------------------------------------------------
// ParseNodeTraverser
//------------------------------------------------------------------------------

void ParseNodeTraverser::Traverse(ParseNode *parseNode)
{
	if (parseNode != 0)
	{
		parseNode->Accept(*this);
	}
}


void ParseNodeTraverser::TraverseList(ListParseNode *listNode)
{
	ListParseNode *current = listNode;
	while (current != 0)
	{
		ListParseNode *next = current->GetNext();
		Traverse(current);
		current = next;
	}
}


void ParseNodeTraverser::VisitTranslationUnit(TranslationUnit *translationUnit)
{
	TraverseList(translationUnit->GetExternalDeclarationList());
}


void ParseNodeTraverser::VisitNamespaceDefinition(NamespaceDefinition *namespaceDefinition)
{
	TraverseList(namespaceDefinition->GetExternalDeclarationList());
}


void ParseNodeTraverser::VisitEnumDeclaration(EnumDeclaration *enumDeclaration)
{
	TraverseList(enumDeclaration->GetEnumeratorList());
}


void ParseNodeTraverser::VisitEnumerator(Enumerator *enumerator)
{
	Traverse(enumerator->GetValue());
}


void ParseNodeTraverser::VisitStructDeclaration(StructDeclaration *structDeclaration)
{
	TraverseList(structDeclaration->GetMemberList());
}


void ParseNodeTraverser::VisitFunctionDefinition(FunctionDefinition *functionDefinition)
{
	Traverse(functionDefinition->GetPrototype());
	Traverse(functionDefinition->GetBody());
}


void ParseNodeTraverser::VisitFunctionPrototype(FunctionPrototype *functionPrototype)
{
	Traverse(functionPrototype->GetReturnType());
	TraverseList(functionPrototype->GetParameterList());
}


void ParseNodeTraverser::VisitParameter(Parameter *parameter)
{
	Traverse(parameter->GetTypeDescriptor());
}


void ParseNodeTraverser::VisitTypeDescriptor(TypeDescriptor *typeDescriptor)
{
	Traverse(typeDescriptor->GetTypeSpecifier());
	Traverse(typeDescriptor->GetParent());
	Traverse(typeDescriptor->GetLength());
}


void ParseNodeTraverser::VisitTypeSpecifier(TypeSpecifier *typeSpecifier)
{
	TraverseList(typeSpecifier->GetIdentifier());
}


void ParseNodeTraverser::VisitNamedInitializer(NamedInitializer *namedInitializer)
{
	Traverse(namedInitializer->GetInitializer());
}


void ParseNodeTraverser::VisitInitializer(Initializer *initializer)
{
	Traverse(initializer->GetExpression());
	TraverseList(initializer->GetInitializerList());
}


void ParseNodeTraverser::VisitCompoundStatement(CompoundStatement *compoundStatement)
{
	TraverseList(compoundStatement->GetStatementList());
}


void ParseNodeTraverser::VisitIfStatement(IfStatement *ifStatement)
{
	Traverse(ifStatement->GetCondition());
	Traverse(ifStatement->GetThenStatement());
	Traverse(ifStatement->GetElseStatement());
}


void ParseNodeTraverser::VisitSwitchStatement(SwitchStatement *switchStatement)
{
	Traverse(switchStatement->GetControl());
	TraverseList(switchStatement->GetSectionList());
}


void ParseNodeTraverser::VisitSwitchSection(SwitchSection *switchSection)
{
	TraverseList(switchSection->GetLabelList());
	TraverseList(switchSection->GetStatementList());
}


void ParseNodeTraverser::VisitSwitchLabel(SwitchLabel *switchLabel)
{
	Traverse(switchLabel->GetExpression());
}


void ParseNodeTraverser::VisitWhileStatement(WhileStatement *whileStatement)
{
	Traverse(whileStatement->GetCondition());
	Traverse(whileStatement->GetBody());
}


void ParseNodeTraverser::VisitForStatement(ForStatement *forStatement)
{
	Traverse(forStatement->GetInitializer());
	Traverse(forStatement->GetCondition());
	Traverse(forStatement->GetCountingExpression());
	Traverse(forStatement->GetBody());
}


void ParseNodeTraverser::VisitJumpStatement(JumpStatement *jumpStatement)
{
	Traverse(jumpStatement->GetRhs());
}


void ParseNodeTraverser::VisitDeclarativeStatement(DeclarativeStatement *declarativeStatement)
{
	Traverse(declarativeStatement->GetTypeDescriptor());
	TraverseList(declarativeStatement->GetNamedInitializerList());
}


void ParseNodeTraverser::VisitExpressionStatement(ExpressionStatement *expressionStatement)
{
	Traverse(expressionStatement->GetExpression());
}


void ParseNodeTraverser::VisitConditionalExpression(ConditionalExpression *conditionalExpression)
{
	Traverse(conditionalExpression->GetCondition());
	Traverse(conditionalExpression->GetTrueExpression());
	Traverse(conditionalExpression->GetFalseExpression());
}


void ParseNodeTraverser::VisitBinaryExpression(BinaryExpression *binaryExpression)
{
	Traverse(binaryExpression->GetLhs());
	Traverse(binaryExpression->GetRhs());
}


void ParseNodeTraverser::VisitUnaryExpression(UnaryExpression *unaryExpression)
{
	Traverse(unaryExpression->GetRhs());
}


void ParseNodeTraverser::VisitPostfixExpression(PostfixExpression *postfixExpression)
{
	Traverse(postfixExpression->GetLhs());
}


void ParseNodeTraverser::VisitMemberExpression(MemberExpression *memberExpression)
{
	Traverse(memberExpression->GetLhs());
}


void ParseNodeTraverser::VisitArraySubscriptExpression(ArraySubscriptExpression *arraySubscriptExpression)
{
	Traverse(arraySubscriptExpression->GetLhs());
	Traverse(arraySubscriptExpression->GetIndex());
}


void ParseNodeTraverser::VisitFunctionCallExpression(FunctionCallExpression *functionCallExpression)
{
	Traverse(functionCallExpression->GetLhs());
	TraverseList(functionCallExpression->GetArgumentList());
}


void ParseNodeTraverser::VisitCastExpression(CastExpression *castExpression)
{
	Traverse(castExpression->GetRhs());
}


void ParseNodeTraverser::VisitSizeofExpression(SizeofExpression *sizeofExpression)
{
	Traverse(sizeofExpression->GetTypeDescriptor());
	Traverse(sizeofExpression->GetRhs());
}


void ParseNodeTraverser::VisitIdentifierExpression(IdentifierExpression *identifierExpression)
{
	TraverseList(identifierExpression->GetIdentifier());
}


//------------------------------------------------------------------------------
// ConstParseNodeTraverser
//------------------------------------------------------------------------------

void ConstParseNodeTraverser::Traverse(const ParseNode *parseNode)
{
	if (parseNode != 0)
	{
		parseNode->Accept(*this);
	}
}


void ConstParseNodeTraverser::TraverseList(const ListParseNode *listNode)
{
	const ListParseNode *current = listNode;
	while (current != 0)
	{
		const ListParseNode *next = current->GetNext();
		Traverse(current);
		current = next;
	}
}


void ConstParseNodeTraverser::VisitTranslationUnit(const TranslationUnit *translationUnit)
{
	TraverseList(translationUnit->GetExternalDeclarationList());
}


void ConstParseNodeTraverser::VisitNamespaceDefinition(const NamespaceDefinition *namespaceDefinition)
{
	TraverseList(namespaceDefinition->GetExternalDeclarationList());
}


void ConstParseNodeTraverser::VisitEnumDeclaration(const EnumDeclaration *enumDeclaration)
{
	TraverseList(enumDeclaration->GetEnumeratorList());
}


void ConstParseNodeTraverser::VisitEnumerator(const Enumerator *enumerator)
{
	Traverse(enumerator->GetValue());
}


void ConstParseNodeTraverser::VisitStructDeclaration(const StructDeclaration *structDeclaration)
{
	TraverseList(structDeclaration->GetMemberList());
}


void ConstParseNodeTraverser::VisitFunctionDefinition(const FunctionDefinition *functionDefinition)
{
	Traverse(functionDefinition->GetPrototype());
	Traverse(functionDefinition->GetBody());
}


void ConstParseNodeTraverser::VisitFunctionPrototype(const FunctionPrototype *functionPrototype)
{
	Traverse(functionPrototype->GetReturnType());
	TraverseList(functionPrototype->GetParameterList());
}


void ConstParseNodeTraverser::VisitParameter(const Parameter *parameter)
{
	Traverse(parameter->GetTypeDescriptor());
}


void ConstParseNodeTraverser::VisitTypeDescriptor(const TypeDescriptor *typeDescriptor)
{
	Traverse(typeDescriptor->GetTypeSpecifier());
	Traverse(typeDescriptor->GetParent());
	Traverse(typeDescriptor->GetLength());
}


void ConstParseNodeTraverser::VisitTypeSpecifier(const TypeSpecifier *typeSpecifier)
{
	TraverseList(typeSpecifier->GetIdentifier());
}


void ConstParseNodeTraverser::VisitNamedInitializer(const NamedInitializer *namedInitializer)
{
	Traverse(namedInitializer->GetInitializer());
}


void ConstParseNodeTraverser::VisitInitializer(const Initializer *initializer)
{
	Traverse(initializer->GetExpression());
	TraverseList(initializer->GetInitializerList());
}


void ConstParseNodeTraverser::VisitCompoundStatement(const CompoundStatement *compoundStatement)
{
	TraverseList(compoundStatement->GetStatementList());
}


void ConstParseNodeTraverser::VisitIfStatement(const IfStatement *ifStatement)
{
	Traverse(ifStatement->GetCondition());
	Traverse(ifStatement->GetThenStatement());
	Traverse(ifStatement->GetElseStatement());
}


void ConstParseNodeTraverser::VisitSwitchStatement(const SwitchStatement *switchStatement)
{
	Traverse(switchStatement->GetControl());
	TraverseList(switchStatement->GetSectionList());
}


void ConstParseNodeTraverser::VisitSwitchSection(const SwitchSection *switchSection)
{
	TraverseList(switchSection->GetLabelList());
	TraverseList(switchSection->GetStatementList());
}


void ConstParseNodeTraverser::VisitSwitchLabel(const SwitchLabel *switchLabel)
{
	Traverse(switchLabel->GetExpression());
}


void ConstParseNodeTraverser::VisitWhileStatement(const WhileStatement *whileStatement)
{
	Traverse(whileStatement->GetCondition());
	Traverse(whileStatement->GetBody());
}


void ConstParseNodeTraverser::VisitForStatement(const ForStatement *forStatement)
{
	Traverse(forStatement->GetInitializer());
	Traverse(forStatement->GetCondition());
	Traverse(forStatement->GetCountingExpression());
	Traverse(forStatement->GetBody());
}


void ConstParseNodeTraverser::VisitJumpStatement(const JumpStatement *jumpStatement)
{
	Traverse(jumpStatement->GetRhs());
}


void ConstParseNodeTraverser::VisitDeclarativeStatement(const DeclarativeStatement *declarativeStatement)
{
	Traverse(declarativeStatement->GetTypeDescriptor());
	TraverseList(declarativeStatement->GetNamedInitializerList());
}


void ConstParseNodeTraverser::VisitExpressionStatement(const ExpressionStatement *expressionStatement)
{
	Traverse(expressionStatement->GetExpression());
}


void ConstParseNodeTraverser::VisitConditionalExpression(const ConditionalExpression *conditionalExpression)
{
	Traverse(conditionalExpression->GetCondition());
	Traverse(conditionalExpression->GetTrueExpression());
	Traverse(conditionalExpression->GetFalseExpression());
}


void ConstParseNodeTraverser::VisitBinaryExpression(const BinaryExpression *binaryExpression)
{
	Traverse(binaryExpression->GetLhs());
	Traverse(binaryExpression->GetRhs());
}


void ConstParseNodeTraverser::VisitUnaryExpression(const UnaryExpression *unaryExpression)
{
	Traverse(unaryExpression->GetRhs());
}


void ConstParseNodeTraverser::VisitPostfixExpression(const PostfixExpression *postfixExpression)
{
	Traverse(postfixExpression->GetLhs());
}


void ConstParseNodeTraverser::VisitMemberExpression(const MemberExpression *memberExpression)
{
	Traverse(memberExpression->GetLhs());
}


void ConstParseNodeTraverser::VisitArraySubscriptExpression(const ArraySubscriptExpression *arraySubscriptExpression)
{
	Traverse(arraySubscriptExpression->GetLhs());
	Traverse(arraySubscriptExpression->GetIndex());
}


void ConstParseNodeTraverser::VisitFunctionCallExpression(const FunctionCallExpression *functionCallExpression)
{
	Traverse(functionCallExpression->GetLhs());
	TraverseList(functionCallExpression->GetArgumentList());
}


void ConstParseNodeTraverser::VisitCastExpression(const CastExpression *castExpression)
{
	Traverse(castExpression->GetRhs());
}


void ConstParseNodeTraverser::VisitSizeofExpression(const SizeofExpression *sizeofExpression)
{
	Traverse(sizeofExpression->GetTypeDescriptor());
	Traverse(sizeofExpression->GetRhs());
}


void ConstParseNodeTraverser::VisitIdentifierExpression(const IdentifierExpression *identifierExpression)
{
	TraverseList(identifierExpression->GetIdentifier());
}

}
