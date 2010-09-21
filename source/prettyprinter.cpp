#include "bond/parsenodes.h"
#include "bond/prettyprinter.h"
#include <stdio.h>

namespace Bond
{

void PrettyPrinter::Print(const ParseNode *parseNode)
{
	if (parseNode != 0)
	{
		parseNode->Accept(*this);
	}
	else
	{
		Print("<invalid>");
	}
}


void PrettyPrinter::VisitTranslationUnit(const TranslationUnit *translationUnit)
{
	PrintExternalDeclarationList(translationUnit->GetExternalDeclarationList());
}


void PrettyPrinter::VisitNamespaceDefinition(const NamespaceDefinition *namespaceDefinition)
{
	Tab();
	Print("namespace ");
	Print(namespaceDefinition->GetName());
	Print("\n");
	Tab();
	Print("{\n");
	IncrementTab();
	PrintExternalDeclarationList(namespaceDefinition->GetExternalDeclarationList());
	DecrementTab();
	Tab();
	Print("}\n");
}


void PrettyPrinter::VisitEnumDeclaration(const EnumDeclaration *enumDeclaration)
{
	Tab();
	Print("enum ");
	Print(enumDeclaration->GetName());
	Print("\n");
	Tab();
	Print("{\n");
	IncrementTab();
	PrintEnumeratorList(enumDeclaration->GetEnumeratorList());
	DecrementTab();
	Tab();
	Print("};\n");
}


void PrettyPrinter::VisitEnumerator(const Enumerator *enumerator)
{
	Tab();
	Print(enumerator->GetName());
	if (enumerator->GetValue() != 0)
	{
		Print(" = ");
		Print(enumerator->GetValue());
	}
	Print(",\n");
}


void PrettyPrinter::VisitFunctionDefinition(const FunctionDefinition *functionDefinition)
{
	Tab();
	Print(functionDefinition->GetPrototype());
	if (functionDefinition->GetBody() != 0)
	{
		Print("\n");
		IncrementTab();
		Print(functionDefinition->GetBody());
		DecrementTab();
	}
	else
	{
		Print(";\n");
	}
}


void PrettyPrinter::VisitFunctionPrototype(const FunctionPrototype *functionPrototype)
{
	Print(functionPrototype->GetReturnType());
	Print(" ");
	Print(functionPrototype->GetName());
	Print("(");
	PrintParameterList(functionPrototype->GetParameterList());
	Print(")");
}


void PrettyPrinter::VisitParameter(const Parameter *parameter)
{
	Print(parameter->GetTypeDescriptor());
	Print(" ");
	Print(parameter->GetName());
}


void PrettyPrinter::VisitTypeDescriptor(const TypeDescriptor *typeDescriptor)
{
	switch (typeDescriptor->GetDescriptor())
	{
		case TypeDescriptor::DESC_VALUE:
			if (typeDescriptor->IsConst())
			{
				Print("const ");
			}
			Print(typeDescriptor->GetTypeSpecifier());
			break;

		case TypeDescriptor::DESC_POINTER:
			VisitTypeDescriptor(typeDescriptor->GetParent());
			Print(" *");
			if (typeDescriptor->IsConst())
			{
				Print(" const");
			}
			break;

		case TypeDescriptor::DESC_ARRAY:
			VisitTypeDescriptor(typeDescriptor->GetParent());
			Print(" [");
			Print(typeDescriptor->GetLength());
			Print("]");
			break;
	}
}


void PrettyPrinter::VisitTypeSpecifier(const TypeSpecifier *typeSpecifier)
{
	if (typeSpecifier->GetPrimitiveType() != 0)
	{
		Print(typeSpecifier->GetPrimitiveType());
	}
	else
	{
		PrintQualifiedIdentifier(typeSpecifier->GetIdentifier());
	}
}


void PrettyPrinter::VisitQualifiedIdentifier(const QualifiedIdentifier *identifier)
{
	Print(identifier->GetName());
}


void PrettyPrinter::VisitCompoundStatement(const CompoundStatement *compoundStatement)
{
	DecrementTab();
	Tab();
	Print("{\n");
	IncrementTab();
	PrintStatementList(compoundStatement->GetStatementList());
	DecrementTab();
	Tab();
	Print("}\n");
	IncrementTab();
}


void PrettyPrinter::VisitIfStatement(const IfStatement *ifStatement)
{
	Tab();
	Print("if (");
	Print(ifStatement->GetCondition());
	Print(")\n");

	IncrementTab();
	Print(ifStatement->GetThenStatement());
	DecrementTab();

	if (ifStatement->GetElseStatement() != 0)
	{
		Tab();
		Print("else\n");

		IncrementTab();
		Print(ifStatement->GetElseStatement());
		DecrementTab();
	}
}


void PrettyPrinter::VisitWhileStatement(const WhileStatement *whileStatement)
{
	Tab();
	if (whileStatement->GetForm() == WhileStatement::FORM_DO_WHILE)
	{
		Print("do\n");
		IncrementTab();
		Print(whileStatement->GetBody());
		DecrementTab();
		Tab();
		Print("while (");
		Print(whileStatement->GetCondition());
		Print(");\n");
	}
	else
	{
		Print("while (");
		Print(whileStatement->GetCondition());
		Print(")\n");
		IncrementTab();
		Print(whileStatement->GetBody());
		DecrementTab();
	}
}


void PrettyPrinter::VisitConditionalExpression(const ConditionalExpression *conditionalExpression)
{
	Print("(");
	Print(conditionalExpression->GetCondition());
	Print(" ? ");
	Print(conditionalExpression->GetTrueExpression());
	Print(" : ");
	Print(conditionalExpression->GetFalseExpression());
	Print(")");
}


void PrettyPrinter::VisitBinaryExpression(const BinaryExpression *binaryExpression)
{
	Print("(");
	Print(binaryExpression->GetLhs());
	Print(" ");
	Print(binaryExpression->GetOperator());
	Print(" ");
	Print(binaryExpression->GetRhs());
	Print(")");
}


void PrettyPrinter::VisitUnaryExpression(const UnaryExpression *unaryExpression)
{
	//Print("(");
	Print(unaryExpression->GetOperator());
	Print(unaryExpression->GetRhs());
	//Print(")");
}


void PrettyPrinter::VisitPostfixExpression(const PostfixExpression *postfixExpression)
{
	//Print("(");
	Print(postfixExpression->GetLhs());
	Print(postfixExpression->GetOperator());
	//Print(")");
}


void PrettyPrinter::VisitMemberExpression(const MemberExpression *memberExpression)
{
	Print(memberExpression->GetLhs());
	Print(memberExpression->GetOperator());
	Print(memberExpression->GetMemberName());
}


void PrettyPrinter::VisitArraySubscriptExpression(const ArraySubscriptExpression *arraySubscriptExpression)
{
	Print(arraySubscriptExpression->GetLhs());
	Print("[");
	Print(arraySubscriptExpression->GetIndex());
	Print("]");
}


void PrettyPrinter::VisitFunctionCallExpression(const FunctionCallExpression *functionCallExpression)
{
	Print(functionCallExpression->GetLhs());
	Print("(");
	PrintArgumentList(functionCallExpression->GetArgumentList());
	Print(")");
}


void PrettyPrinter::VisitCastExpression(const CastExpression *castExpression)
{
	Print("(");
	Print(castExpression->GetTypeDescriptor());
	Print(") ");
	Print(castExpression->GetRhs());
}


void PrettyPrinter::VisitSizeofExpression(const SizeofExpression *sizeofExpression)
{
	Print("sizeof");
	if (sizeofExpression->GetTypeDescriptor() != 0)
	{
		Print("(");
		Print(sizeofExpression->GetTypeDescriptor());
		Print(")");
	}
	else
	{
		Print(sizeofExpression->GetRhs());
	}
}


void PrettyPrinter::VisitConstantExpression(const ConstantExpression *constantExpression)
{
	Print(constantExpression->GetValue());
}


void PrettyPrinter::VisitIdentifierExpression(const IdentifierExpression *identifierExpression)
{
	PrintQualifiedIdentifier(identifierExpression->GetIdentifier());
}


void PrettyPrinter::PrintExternalDeclarationList(const ExternalDeclaration *declarationList)
{
	const ExternalDeclaration *current = declarationList;
	while (current != 0)
	{
		Print(current);
		current = current->GetNext();
	}
}


void PrettyPrinter::PrintEnumeratorList(const Enumerator *enumeratorList)
{
	const Enumerator *current = enumeratorList;
	while (current != 0)
	{
		Print(current);
		current = current->GetNext();
	}
}


void PrettyPrinter::PrintParameterList(const Parameter *parameterList)
{
	const Parameter *current = parameterList;

	if (current != 0)
	{
		Print(current);
		current = current->GetNext();
	}

	while (current != 0)
	{
		Print(", ");
		Print(current);
		current = current->GetNext();
	}
}


void PrettyPrinter::PrintArgumentList(const Expression *argumentList)
{
	const Expression *current = argumentList;

	if (current != 0)
	{
		Print(current);
		current = current->GetNext();
	}

	while (current != 0)
	{
		Print(", ");
		Print(current);
		current = current->GetNext();
	}
}


void PrettyPrinter::PrintQualifiedIdentifier(const QualifiedIdentifier *identifier)
{
	const QualifiedIdentifier *current = identifier;

	if (current != 0)
	{
		Print(current);
		current = current->GetNext();
	}

	while (current != 0)
	{
		Print("::");
		Print(current);
		current = current->GetNext();
	}
}


void PrettyPrinter::PrintStatementList(const Statement *statementList)
{
	const Statement *current = statementList;
	while (current != 0)
	{
		Print(current);
		current = current->GetNext();
	}
}


void PrettyPrinter::Tab()
{
	for (int i = 0; i < mTabLevel; ++i)
	{
		Print("\t");
	}
}


void PrettyPrinter::Print(const char *text)
{
	// TODO: output to appropriate place.
	printf("%s", text);
}


void PrettyPrinter::Print(const Token *token)
{
	if (token != 0)
	{
		Print(token->GetText());
	}
}

}
