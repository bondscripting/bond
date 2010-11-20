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


void ParseNodeTraverser::Visit(TranslationUnit *translationUnit)
{
	TraverseList(translationUnit->GetExternalDeclarationList());
}


void ParseNodeTraverser::Visit(NamespaceDefinition *namespaceDefinition)
{
	TraverseList(namespaceDefinition->GetExternalDeclarationList());
}


void ParseNodeTraverser::Visit(EnumDeclaration *enumDeclaration)
{
	TraverseList(enumDeclaration->GetEnumeratorList());
}


void ParseNodeTraverser::Visit(Enumerator *enumerator)
{
	Traverse(enumerator->GetValue());
}


void ParseNodeTraverser::Visit(StructDeclaration *structDeclaration)
{
	TraverseList(structDeclaration->GetMemberList());
}


void ParseNodeTraverser::Visit(FunctionDefinition *functionDefinition)
{
	Traverse(functionDefinition->GetPrototype());
	Traverse(functionDefinition->GetBody());
}


void ParseNodeTraverser::Visit(FunctionPrototype *functionPrototype)
{
	Traverse(functionPrototype->GetReturnType());
	TraverseList(functionPrototype->GetParameterList());
}


void ParseNodeTraverser::Visit(Parameter *parameter)
{
	Traverse(parameter->GetTypeDescriptor());
}


void ParseNodeTraverser::Visit(TypeDescriptor *typeDescriptor)
{
	Traverse(typeDescriptor->GetTypeSpecifier());
	Traverse(typeDescriptor->GetParent());
	Traverse(typeDescriptor->GetLength());
}


void ParseNodeTraverser::Visit(TypeSpecifier *typeSpecifier)
{
	TraverseList(typeSpecifier->GetIdentifier());
}


void ParseNodeTraverser::Visit(NamedInitializer *namedInitializer)
{
	Traverse(namedInitializer->GetInitializer());
}


void ParseNodeTraverser::Visit(Initializer *initializer)
{
	Traverse(initializer->GetExpression());
	TraverseList(initializer->GetInitializerList());
}


void ParseNodeTraverser::Visit(CompoundStatement *compoundStatement)
{
	TraverseList(compoundStatement->GetStatementList());
}


void ParseNodeTraverser::Visit(IfStatement *ifStatement)
{
	Traverse(ifStatement->GetCondition());
	Traverse(ifStatement->GetThenStatement());
	Traverse(ifStatement->GetElseStatement());
}


void ParseNodeTraverser::Visit(SwitchStatement *switchStatement)
{
	Traverse(switchStatement->GetControl());
	TraverseList(switchStatement->GetSectionList());
}


void ParseNodeTraverser::Visit(SwitchSection *switchSection)
{
	TraverseList(switchSection->GetLabelList());
	TraverseList(switchSection->GetStatementList());
}


void ParseNodeTraverser::Visit(SwitchLabel *switchLabel)
{
	Traverse(switchLabel->GetExpression());
}


void ParseNodeTraverser::Visit(WhileStatement *whileStatement)
{
	Traverse(whileStatement->GetCondition());
	Traverse(whileStatement->GetBody());
}


void ParseNodeTraverser::Visit(ForStatement *forStatement)
{
	Traverse(forStatement->GetInitializer());
	Traverse(forStatement->GetCondition());
	Traverse(forStatement->GetCountingExpression());
	Traverse(forStatement->GetBody());
}


void ParseNodeTraverser::Visit(JumpStatement *jumpStatement)
{
	Traverse(jumpStatement->GetRhs());
}


void ParseNodeTraverser::Visit(DeclarativeStatement *declarativeStatement)
{
	Traverse(declarativeStatement->GetTypeDescriptor());
	TraverseList(declarativeStatement->GetNamedInitializerList());
}


void ParseNodeTraverser::Visit(ExpressionStatement *expressionStatement)
{
	Traverse(expressionStatement->GetExpression());
}


void ParseNodeTraverser::Visit(ConditionalExpression *conditionalExpression)
{
	Traverse(conditionalExpression->GetCondition());
	Traverse(conditionalExpression->GetTrueExpression());
	Traverse(conditionalExpression->GetFalseExpression());
}


void ParseNodeTraverser::Visit(BinaryExpression *binaryExpression)
{
	Traverse(binaryExpression->GetLhs());
	Traverse(binaryExpression->GetRhs());
}


void ParseNodeTraverser::Visit(UnaryExpression *unaryExpression)
{
	Traverse(unaryExpression->GetRhs());
}


void ParseNodeTraverser::Visit(PostfixExpression *postfixExpression)
{
	Traverse(postfixExpression->GetLhs());
}


void ParseNodeTraverser::Visit(MemberExpression *memberExpression)
{
	Traverse(memberExpression->GetLhs());
}


void ParseNodeTraverser::Visit(ArraySubscriptExpression *arraySubscriptExpression)
{
	Traverse(arraySubscriptExpression->GetLhs());
	Traverse(arraySubscriptExpression->GetIndex());
}


void ParseNodeTraverser::Visit(FunctionCallExpression *functionCallExpression)
{
	Traverse(functionCallExpression->GetLhs());
	TraverseList(functionCallExpression->GetArgumentList());
}


void ParseNodeTraverser::Visit(CastExpression *castExpression)
{
	Traverse(castExpression->GetRhs());
}


void ParseNodeTraverser::Visit(SizeofExpression *sizeofExpression)
{
	Traverse(sizeofExpression->GetTypeDescriptor());
	Traverse(sizeofExpression->GetRhs());
}


void ParseNodeTraverser::Visit(IdentifierExpression *identifierExpression)
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


void ConstParseNodeTraverser::Visit(const TranslationUnit *translationUnit)
{
	TraverseList(translationUnit->GetExternalDeclarationList());
}


void ConstParseNodeTraverser::Visit(const NamespaceDefinition *namespaceDefinition)
{
	TraverseList(namespaceDefinition->GetExternalDeclarationList());
}


void ConstParseNodeTraverser::Visit(const EnumDeclaration *enumDeclaration)
{
	TraverseList(enumDeclaration->GetEnumeratorList());
}


void ConstParseNodeTraverser::Visit(const Enumerator *enumerator)
{
	Traverse(enumerator->GetValue());
}


void ConstParseNodeTraverser::Visit(const StructDeclaration *structDeclaration)
{
	TraverseList(structDeclaration->GetMemberList());
}


void ConstParseNodeTraverser::Visit(const FunctionDefinition *functionDefinition)
{
	Traverse(functionDefinition->GetPrototype());
	Traverse(functionDefinition->GetBody());
}


void ConstParseNodeTraverser::Visit(const FunctionPrototype *functionPrototype)
{
	Traverse(functionPrototype->GetReturnType());
	TraverseList(functionPrototype->GetParameterList());
}


void ConstParseNodeTraverser::Visit(const Parameter *parameter)
{
	Traverse(parameter->GetTypeDescriptor());
}


void ConstParseNodeTraverser::Visit(const TypeDescriptor *typeDescriptor)
{
	Traverse(typeDescriptor->GetTypeSpecifier());
	Traverse(typeDescriptor->GetParent());
	Traverse(typeDescriptor->GetLength());
}


void ConstParseNodeTraverser::Visit(const TypeSpecifier *typeSpecifier)
{
	TraverseList(typeSpecifier->GetIdentifier());
}


void ConstParseNodeTraverser::Visit(const NamedInitializer *namedInitializer)
{
	Traverse(namedInitializer->GetInitializer());
}


void ConstParseNodeTraverser::Visit(const Initializer *initializer)
{
	Traverse(initializer->GetExpression());
	TraverseList(initializer->GetInitializerList());
}


void ConstParseNodeTraverser::Visit(const CompoundStatement *compoundStatement)
{
	TraverseList(compoundStatement->GetStatementList());
}


void ConstParseNodeTraverser::Visit(const IfStatement *ifStatement)
{
	Traverse(ifStatement->GetCondition());
	Traverse(ifStatement->GetThenStatement());
	Traverse(ifStatement->GetElseStatement());
}


void ConstParseNodeTraverser::Visit(const SwitchStatement *switchStatement)
{
	Traverse(switchStatement->GetControl());
	TraverseList(switchStatement->GetSectionList());
}


void ConstParseNodeTraverser::Visit(const SwitchSection *switchSection)
{
	TraverseList(switchSection->GetLabelList());
	TraverseList(switchSection->GetStatementList());
}


void ConstParseNodeTraverser::Visit(const SwitchLabel *switchLabel)
{
	Traverse(switchLabel->GetExpression());
}


void ConstParseNodeTraverser::Visit(const WhileStatement *whileStatement)
{
	Traverse(whileStatement->GetCondition());
	Traverse(whileStatement->GetBody());
}


void ConstParseNodeTraverser::Visit(const ForStatement *forStatement)
{
	Traverse(forStatement->GetInitializer());
	Traverse(forStatement->GetCondition());
	Traverse(forStatement->GetCountingExpression());
	Traverse(forStatement->GetBody());
}


void ConstParseNodeTraverser::Visit(const JumpStatement *jumpStatement)
{
	Traverse(jumpStatement->GetRhs());
}


void ConstParseNodeTraverser::Visit(const DeclarativeStatement *declarativeStatement)
{
	Traverse(declarativeStatement->GetTypeDescriptor());
	TraverseList(declarativeStatement->GetNamedInitializerList());
}


void ConstParseNodeTraverser::Visit(const ExpressionStatement *expressionStatement)
{
	Traverse(expressionStatement->GetExpression());
}


void ConstParseNodeTraverser::Visit(const ConditionalExpression *conditionalExpression)
{
	Traverse(conditionalExpression->GetCondition());
	Traverse(conditionalExpression->GetTrueExpression());
	Traverse(conditionalExpression->GetFalseExpression());
}


void ConstParseNodeTraverser::Visit(const BinaryExpression *binaryExpression)
{
	Traverse(binaryExpression->GetLhs());
	Traverse(binaryExpression->GetRhs());
}


void ConstParseNodeTraverser::Visit(const UnaryExpression *unaryExpression)
{
	Traverse(unaryExpression->GetRhs());
}


void ConstParseNodeTraverser::Visit(const PostfixExpression *postfixExpression)
{
	Traverse(postfixExpression->GetLhs());
}


void ConstParseNodeTraverser::Visit(const MemberExpression *memberExpression)
{
	Traverse(memberExpression->GetLhs());
}


void ConstParseNodeTraverser::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	Traverse(arraySubscriptExpression->GetLhs());
	Traverse(arraySubscriptExpression->GetIndex());
}


void ConstParseNodeTraverser::Visit(const FunctionCallExpression *functionCallExpression)
{
	Traverse(functionCallExpression->GetLhs());
	TraverseList(functionCallExpression->GetArgumentList());
}


void ConstParseNodeTraverser::Visit(const CastExpression *castExpression)
{
	Traverse(castExpression->GetRhs());
}


void ConstParseNodeTraverser::Visit(const SizeofExpression *sizeofExpression)
{
	Traverse(sizeofExpression->GetTypeDescriptor());
	Traverse(sizeofExpression->GetRhs());
}


void ConstParseNodeTraverser::Visit(const IdentifierExpression *identifierExpression)
{
	TraverseList(identifierExpression->GetIdentifier());
}

}
