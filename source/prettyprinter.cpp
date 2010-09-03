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
	Print(namespaceDefinition->GetName()->GetText());
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
	Print(enumDeclaration->GetName()->GetText());
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
	Print(enumerator->GetName()->GetText());
	if (enumerator->GetValue() != 0)
	{
		Print(" = ");
		Print(enumerator->GetValue());
	}
	Print(",\n");
}


void PrettyPrinter::VisitTypeDescriptor(const TypeDescriptor *typeDescriptor)
{
	// TODO
}


void PrettyPrinter::VisitQualifiedIdentifier(const QualifiedIdentifier *identifier)
{
	Print(identifier->GetName()->GetText());
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
	Print(binaryExpression->GetOperator()->GetText());
	Print(" ");
	Print(binaryExpression->GetRhs());
	Print(")");
}


void PrettyPrinter::VisitUnaryExpression(const UnaryExpression *unaryExpression)
{
	Print("(");
	Print(unaryExpression->GetOperator()->GetText());
	Print(unaryExpression->GetRhs());
	Print(")");
}


void PrettyPrinter::VisitPostfixExpression(const PostfixExpression *postfixExpression)
{
	Print("(");
	Print(postfixExpression->GetLhs());
	Print(postfixExpression->GetOperator()->GetText());
	Print(")");
}


void PrettyPrinter::VisitMemberExpression(const MemberExpression *memberExpression)
{
	Print(memberExpression->GetLhs());
	Print(memberExpression->GetOperator()->GetText());
	Print(memberExpression->GetMemberName()->GetText());
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
	// TODO
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
		Print("(");
	}
	else
	{
		Print(sizeofExpression->GetRhs());
	}
}


void PrettyPrinter::VisitConstantExpression(const ConstantExpression *constantExpression)
{
	Print(constantExpression->GetValue()->GetText());
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

}
