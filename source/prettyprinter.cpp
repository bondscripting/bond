#include "bond/parsenodes.h"
#include "bond/prettyprinter.h"
#include "bond/textwriter.h"
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
		mWriter.Write("<invalid>");
	}
}


void PrettyPrinter::PrintList(const ListParseNode *listNode)
{
	const ListParseNode *current = listNode;
	while (current != 0)
	{
		Print(current);
		current = current->GetNext();
	}
}


void PrettyPrinter::VisitTranslationUnit(const TranslationUnit *translationUnit)
{
	PrintList(translationUnit->GetExternalDeclarationList());
}


void PrettyPrinter::VisitNamespaceDefinition(const NamespaceDefinition *namespaceDefinition)
{
	Tab();
	mWriter.Write("namespace ");
	Print(namespaceDefinition->GetName());
	mWriter.Write("\n");
	Tab();
	mWriter.Write("{\n");
	IncrementTab();
	PrintList(namespaceDefinition->GetExternalDeclarationList());
	DecrementTab();
	Tab();
	mWriter.Write("}\n");
}


void PrettyPrinter::VisitEnumDeclaration(const EnumDeclaration *enumDeclaration)
{
	Tab();
	mWriter.Write("enum ");
	Print(enumDeclaration->GetName());
	mWriter.Write("\n");
	Tab();
	mWriter.Write("{\n");
	IncrementTab();
	PrintList(enumDeclaration->GetEnumeratorList());
	DecrementTab();
	Tab();
	mWriter.Write("};\n");
}


void PrettyPrinter::VisitEnumerator(const Enumerator *enumerator)
{
	Tab();
	Print(enumerator->GetName());
	if (enumerator->GetValue() != 0)
	{
		mWriter.Write(" = ");
		Print(enumerator->GetValue());
	}
	mWriter.Write(",\n");
}


void PrettyPrinter::VisitStructDeclaration(const StructDeclaration *structDeclaration)
{
	Tab();
	mWriter.Write("struct ");
	Print(structDeclaration->GetName());
	mWriter.Write("\n");
	Tab();
	mWriter.Write("{\n");
	IncrementTab();
	PrintList(structDeclaration->GetMemberList());
	DecrementTab();
	Tab();
	mWriter.Write("};\n");
}


void PrettyPrinter::VisitFunctionDefinition(const FunctionDefinition *functionDefinition)
{
	Tab();
	Print(functionDefinition->GetPrototype());
	if (functionDefinition->GetBody() != 0)
	{
		mWriter.Write("\n");
		IncrementTab();
		Print(functionDefinition->GetBody());
		DecrementTab();
	}
	else
	{
		mWriter.Write(";\n");
	}
}


void PrettyPrinter::VisitFunctionPrototype(const FunctionPrototype *functionPrototype)
{
	Print(functionPrototype->GetReturnType());
	mWriter.Write(" ");
	Print(functionPrototype->GetName());
	mWriter.Write("(");
	PrintList(functionPrototype->GetParameterList(), ", ");
	mWriter.Write(")");
}


void PrettyPrinter::VisitParameter(const Parameter *parameter)
{
	Print(parameter->GetTypeDescriptor());
	mWriter.Write(" ");
	Print(parameter->GetName());
}


void PrettyPrinter::VisitTypeDescriptor(const TypeDescriptor *typeDescriptor)
{
	switch (typeDescriptor->GetVariant())
	{
		case TypeDescriptor::VARIANT_VALUE:
			if (typeDescriptor->IsConst())
			{
				mWriter.Write("const ");
			}
			Print(typeDescriptor->GetTypeSpecifier());
			break;

		case TypeDescriptor::VARIANT_POINTER:
			VisitTypeDescriptor(typeDescriptor->GetParent());
			mWriter.Write(" *");
			if (typeDescriptor->IsConst())
			{
				mWriter.Write(" const");
			}
			break;

		case TypeDescriptor::VARIANT_ARRAY:
			VisitTypeDescriptor(typeDescriptor->GetParent());
			mWriter.Write(" [");
			Print(typeDescriptor->GetLength());
			mWriter.Write("]");
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
		PrintList(typeSpecifier->GetIdentifier(), "::");
	}
}


void PrettyPrinter::VisitNamedInitializer(const NamedInitializer *namedInitializer)
{
	Print(namedInitializer->GetName());

	if (namedInitializer->GetInitializer() != 0)
	{
		mWriter.Write(" = ");
		Print(namedInitializer->GetInitializer());
	}
}


void PrettyPrinter::VisitInitializer(const Initializer *initializer)
{
	if (initializer->GetExpression() != 0)
	{
		Print(initializer->GetExpression());
	}
	else
	{
		mWriter.Write("{ ");
		PrintList(initializer->GetInitializerList(), ", ");
		mWriter.Write(" }");
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
	mWriter.Write("{\n");
	IncrementTab();
	PrintList(compoundStatement->GetStatementList());
	DecrementTab();
	Tab();
	mWriter.Write("}\n");
	IncrementTab();
}


void PrettyPrinter::VisitIfStatement(const IfStatement *ifStatement)
{
	Tab();
	mWriter.Write("if (");
	Print(ifStatement->GetCondition());
	mWriter.Write(")\n");

	IncrementTab();
	Print(ifStatement->GetThenStatement());
	DecrementTab();

	if (ifStatement->GetElseStatement() != 0)
	{
		Tab();
		mWriter.Write("else\n");

		IncrementTab();
		Print(ifStatement->GetElseStatement());
		DecrementTab();
	}
}


void PrettyPrinter::VisitSwitchStatement(const SwitchStatement *switchStatement)
{
	Tab();
	mWriter.Write("switch (");
	Print(switchStatement->GetControl());
	mWriter.Write(")\n");

	Tab();
	mWriter.Write("{\n");
	IncrementTab();
	PrintList(switchStatement->GetSectionList());
	DecrementTab();
	Tab();
	mWriter.Write("}\n");
}


void PrettyPrinter::VisitSwitchSection(const SwitchSection *switchSection)
{
	PrintList(switchSection->GetLabelList());
	IncrementTab();
	PrintList(switchSection->GetStatementList());
	DecrementTab();
}


void PrettyPrinter::VisitSwitchLabel(const SwitchLabel *switchLabel)
{
	Tab();
	Print(switchLabel->GetLabel());

	if (switchLabel->GetExpression() != 0)
	{
		mWriter.Write(" ");
		Print(switchLabel->GetExpression());
	}

	mWriter.Write(":\n");
}


void PrettyPrinter::VisitWhileStatement(const WhileStatement *whileStatement)
{
	Tab();
	if (whileStatement->GetVariant() == WhileStatement::VARIANT_DO_WHILE)
	{
		mWriter.Write("do\n");
		IncrementTab();
		Print(whileStatement->GetBody());
		DecrementTab();
		Tab();
		mWriter.Write("while (");
		Print(whileStatement->GetCondition());
		mWriter.Write(");\n");
	}
	else
	{
		mWriter.Write("while (");
		Print(whileStatement->GetCondition());
		mWriter.Write(")\n");
		IncrementTab();
		Print(whileStatement->GetBody());
		DecrementTab();
	}
}


void PrettyPrinter::VisitForStatement(const ForStatement *forStatement)
{
	Tab();
	mWriter.Write("for (");
	SuppressTabsAndNewlines();
	Print(forStatement->GetInitializer());
	PrintTabsAndNewlines();
	mWriter.Write(" ");
	Print(forStatement->GetCondition());
	mWriter.Write("; ");
	Print(forStatement->GetCountingExpression());
	mWriter.Write(")\n");
	IncrementTab();
	Print(forStatement->GetBody());
	DecrementTab();
}


void PrettyPrinter::VisitJumpStatement(const JumpStatement *jumpStatement)
{
	Tab();
	const Token *op = jumpStatement->GetOperator();
	Print(op);

	const Expression *rhs = jumpStatement->GetRhs(); 
	if ((op->GetTokenType() == Token::KEY_RETURN) && (rhs != 0))
	{
		mWriter.Write(" ");
		Print(rhs);
	}

	mWriter.Write(";");
	Newline();
}


void PrettyPrinter::VisitDeclarativeStatement(const DeclarativeStatement *declarativeStatement)
{
	Tab();
	Print(declarativeStatement->GetTypeDescriptor());
	mWriter.Write(" ");
	PrintList(declarativeStatement->GetNamedInitializerList(), ", ");
	mWriter.Write(";");
	Newline();
}


void PrettyPrinter::VisitExpressionStatement(const ExpressionStatement *expressionStatement)
{
	Tab();
	if (expressionStatement->GetExpression() != 0)
	{
		Print(expressionStatement->GetExpression());
	}
	mWriter.Write(";");
	Newline();
}


void PrettyPrinter::VisitConditionalExpression(const ConditionalExpression *conditionalExpression)
{
	mWriter.Write("(");
	Print(conditionalExpression->GetCondition());
	mWriter.Write(" ? ");
	Print(conditionalExpression->GetTrueExpression());
	mWriter.Write(" : ");
	Print(conditionalExpression->GetFalseExpression());
	mWriter.Write(")");
}


void PrettyPrinter::VisitBinaryExpression(const BinaryExpression *binaryExpression)
{
	mWriter.Write("(");
	Print(binaryExpression->GetLhs());
	mWriter.Write(" ");
	Print(binaryExpression->GetOperator());
	mWriter.Write(" ");
	Print(binaryExpression->GetRhs());
	mWriter.Write(")");
}


void PrettyPrinter::VisitUnaryExpression(const UnaryExpression *unaryExpression)
{
	//mWriter.Write("(");
	Print(unaryExpression->GetOperator());
	Print(unaryExpression->GetRhs());
	//mWriter.Write(")");
}


void PrettyPrinter::VisitPostfixExpression(const PostfixExpression *postfixExpression)
{
	//mWriter.Write("(");
	Print(postfixExpression->GetLhs());
	Print(postfixExpression->GetOperator());
	//mWriter.Write(")");
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
	mWriter.Write("[");
	Print(arraySubscriptExpression->GetIndex());
	mWriter.Write("]");
}


void PrettyPrinter::VisitFunctionCallExpression(const FunctionCallExpression *functionCallExpression)
{
	Print(functionCallExpression->GetLhs());
	mWriter.Write("(");
	PrintList(functionCallExpression->GetArgumentList(), ", ");
	mWriter.Write(")");
}


void PrettyPrinter::VisitCastExpression(const CastExpression *castExpression)
{
	mWriter.Write("(");
	Print(castExpression->GetTypeDescriptor());
	mWriter.Write(") ");
	Print(castExpression->GetRhs());
}


void PrettyPrinter::VisitSizeofExpression(const SizeofExpression *sizeofExpression)
{
	mWriter.Write("sizeof");
	if (sizeofExpression->GetTypeDescriptor() != 0)
	{
		mWriter.Write("<");
		Print(sizeofExpression->GetTypeDescriptor());
		mWriter.Write(">");
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
	PrintList(identifierExpression->GetIdentifier(), "::");
}


void PrettyPrinter::PrintList(const ListParseNode *listNode, const char *separator)
{
	const ListParseNode *current = listNode;

	if (current != 0)
	{
		Print(current);
		current = current->GetNext();
	}

	while (current != 0)
	{
		mWriter.Write("%s", separator);
		Print(current);
		current = current->GetNext();
	}
}


void PrettyPrinter::Tab()
{
	if (mPrintTabsAndNewlines)
	{
		for (int i = 0; i < mTabLevel; ++i)
		{
			mWriter.Write("\t");
		}
	}
}


void PrettyPrinter::Newline()
{
	if (mPrintTabsAndNewlines)
	{
		mWriter.Write("\n");
	}
}


void PrettyPrinter::Print(const Token *token)
{
	if (token != 0)
	{
		mWriter.Write("%s", token->GetText());
	}
}

}
