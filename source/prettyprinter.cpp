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
		current = current->GetNextNode();
	}
}


void PrettyPrinter::Visit(const TranslationUnit *translationUnit)
{
	PrintList(translationUnit->GetExternalDeclarationList());
}


void PrettyPrinter::Visit(const NamespaceDefinition *namespaceDefinition)
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


void PrettyPrinter::Visit(const EnumDeclaration *enumDeclaration)
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


void PrettyPrinter::Visit(const Enumerator *enumerator)
{
	Tab();
	Print(enumerator->GetName());
	if (mPrintFoldedConstants && enumerator->GetTypeAndValue()->IsValueDefined())
	{
		mWriter.Write(" = " BOND_DECIMAL_FORMAT, enumerator->GetTypeAndValue()->GetIntValue());
	}
	else if (enumerator->GetValue() != 0)
	{
		mWriter.Write(" = ");
		Print(enumerator->GetValue());
	}
	mWriter.Write(",\n");
}


void PrettyPrinter::Visit(const StructDeclaration *structDeclaration)
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


void PrettyPrinter::Visit(const FunctionDefinition *functionDefinition)
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


void PrettyPrinter::Visit(const FunctionPrototype *functionPrototype)
{
	Print(functionPrototype->GetReturnType());
	mWriter.Write(" ");
	Print(functionPrototype->GetName());
	mWriter.Write("(");
	PrintList(functionPrototype->GetParameterList(), ", ");
	mWriter.Write(")");
}


void PrettyPrinter::Visit(const Parameter *parameter)
{
	Print(parameter->GetTypeDescriptor());
	mWriter.Write(" ");
	Print(parameter->GetName());
}


void PrettyPrinter::Visit(const TypeDescriptor *typeDescriptor)
{
	if (typeDescriptor->IsPointerIntrinsicType())
	{
		const TypeDescriptor parent = typeDescriptor->DereferenceType();
		Visit(&parent);
		mWriter.Write(" *");
		if (typeDescriptor->IsConst())
		{
			mWriter.Write(" const");
		}
	}
	else if (typeDescriptor->IsArrayType())
	{
		Visit(typeDescriptor->GetParent());
		mWriter.Write("[");
		PrintList(typeDescriptor->GetLengthExpressionList(), "][");
		mWriter.Write("]");
	}
	else if (typeDescriptor->IsValueType())
	{
		if (typeDescriptor->IsConst())
		{
			mWriter.Write("const ");
		}
		Print(typeDescriptor->GetTypeSpecifier());
	}
}


void PrettyPrinter::Visit(const TypeSpecifier *typeSpecifier)
{
	if (typeSpecifier->GetPrimitiveTypeToken() != 0)
	{
		Print(typeSpecifier->GetPrimitiveTypeToken());
	}
	else
	{
		PrintList(typeSpecifier->GetIdentifier(), "::");
	}
}


void PrettyPrinter::Visit(const NamedInitializer *namedInitializer)
{
	Print(namedInitializer->GetName());

	if (namedInitializer->GetInitializer() != 0)
	{
		mWriter.Write(" = ");
		Print(namedInitializer->GetInitializer());
	}
}


void PrettyPrinter::Visit(const Initializer *initializer)
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


void PrettyPrinter::Visit(const QualifiedIdentifier *identifier)
{
	Print(identifier->GetName());
}


void PrettyPrinter::Visit(const CompoundStatement *compoundStatement)
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


void PrettyPrinter::Visit(const IfStatement *ifStatement)
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


void PrettyPrinter::Visit(const SwitchStatement *switchStatement)
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


void PrettyPrinter::Visit(const SwitchSection *switchSection)
{
	PrintList(switchSection->GetLabelList());
	IncrementTab();
	PrintList(switchSection->GetStatementList());
	DecrementTab();
}


void PrettyPrinter::Visit(const SwitchLabel *switchLabel)
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


void PrettyPrinter::Visit(const WhileStatement *whileStatement)
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


void PrettyPrinter::Visit(const ForStatement *forStatement)
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


void PrettyPrinter::Visit(const JumpStatement *jumpStatement)
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


void PrettyPrinter::Visit(const DeclarativeStatement *declarativeStatement)
{
	Tab();
	Print(declarativeStatement->GetTypeDescriptor());
	mWriter.Write(" ");
	PrintList(declarativeStatement->GetNamedInitializerList(), ", ");
	mWriter.Write(";");
	Newline();
}


void PrettyPrinter::Visit(const ExpressionStatement *expressionStatement)
{
	Tab();
	if (expressionStatement->GetExpression() != 0)
	{
		Print(expressionStatement->GetExpression());
	}
	mWriter.Write(";");
	Newline();
}


void PrettyPrinter::Visit(const ConditionalExpression *conditionalExpression)
{
	if (!PrintFoldedConstant(conditionalExpression))
	{
		mWriter.Write("(");
		Print(conditionalExpression->GetCondition());
		mWriter.Write(" ? ");
		Print(conditionalExpression->GetTrueExpression());
		mWriter.Write(" : ");
		Print(conditionalExpression->GetFalseExpression());
		mWriter.Write(")");
	}
}


void PrettyPrinter::Visit(const BinaryExpression *binaryExpression)
{
	if (!PrintFoldedConstant(binaryExpression))
	{
		mWriter.Write("(");
		Print(binaryExpression->GetLhs());
		mWriter.Write(" ");
		Print(binaryExpression->GetOperator());
		mWriter.Write(" ");
		Print(binaryExpression->GetRhs());
		mWriter.Write(")");
	}
}


void PrettyPrinter::Visit(const UnaryExpression *unaryExpression)
{
	if (!PrintFoldedConstant(unaryExpression))
	{
		//mWriter.Write("(");
		Print(unaryExpression->GetOperator());
		Print(unaryExpression->GetRhs());
		//mWriter.Write(")");
	}
}


void PrettyPrinter::Visit(const PostfixExpression *postfixExpression)
{
	//mWriter.Write("(");
	Print(postfixExpression->GetLhs());
	Print(postfixExpression->GetOperator());
	//mWriter.Write(")");
}


void PrettyPrinter::Visit(const MemberExpression *memberExpression)
{
	Print(memberExpression->GetLhs());
	Print(memberExpression->GetOperator());
	Print(memberExpression->GetMemberName());
}


void PrettyPrinter::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	Print(arraySubscriptExpression->GetLhs());
	mWriter.Write("[");
	Print(arraySubscriptExpression->GetIndex());
	mWriter.Write("]");
}


void PrettyPrinter::Visit(const FunctionCallExpression *functionCallExpression)
{
	Print(functionCallExpression->GetLhs());
	mWriter.Write("(");
	PrintList(functionCallExpression->GetArgumentList(), ", ");
	mWriter.Write(")");
}


void PrettyPrinter::Visit(const CastExpression *castExpression)
{
	if (!PrintFoldedConstant(castExpression))
	{
		mWriter.Write("cast<");
		Print(castExpression->GetTypeDescriptor());
		mWriter.Write(">(");
		Print(castExpression->GetRhs());
		mWriter.Write(")");
	}
}


void PrettyPrinter::Visit(const SizeofExpression *sizeofExpression)
{
	if (!PrintFoldedConstant(sizeofExpression))
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
}


void PrettyPrinter::Visit(const ConstantExpression *constantExpression)
{
	Print(constantExpression->GetValueToken());
}


void PrettyPrinter::Visit(const IdentifierExpression *identifierExpression)
{
	if (!PrintFoldedConstant(identifierExpression))
	{
		PrintList(identifierExpression->GetIdentifier(), "::");
	}
}


void PrettyPrinter::PrintList(const ListParseNode *listNode, const char *separator)
{
	const ListParseNode *current = listNode;

	if (current != 0)
	{
		Print(current);
		current = current->GetNextNode();
	}

	while (current != 0)
	{
		mWriter.Write("%s", separator);
		Print(current);
		current = current->GetNextNode();
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


bool PrettyPrinter::PrintFoldedConstant(const Expression *expression)
{
	const TypeAndValue &tav = expression->GetTypeAndValue();
	if (mPrintFoldedConstants && tav.IsValueDefined())
	{
		const TypeDescriptor *typeDescriptor = tav.GetTypeDescriptor();
		switch (typeDescriptor->GetPrimitiveType())
		{
			case Token::KEY_BOOL:
				mWriter.Write("%s", tav.GetBoolValue() ? "true" : "false");
				return true;
			case Token::KEY_CHAR:
				mWriter.Write("%c", tav.GetCharValue());
				return true;
			case Token::KEY_FLOAT:
				mWriter.Write(BOND_FLOAT_FORMAT, tav.GetFloatValue());
				return true;
			case Token::KEY_INT:
				mWriter.Write(BOND_DECIMAL_FORMAT, tav.GetIntValue());
				return true;
			case Token::KEY_UINT:
				mWriter.Write(BOND_UDECIMAL_FORMAT, tav.GetUIntValue());
				return true;
			default:
				break;
		}
	}
	return false;
}

}
