#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/io/textwriter.h"
#include "bond/tools/prettyprinter.h"

namespace Bond
{

void PrettyPrinter::Print(const ParseNode *parseNode)
{
	if (parseNode != NULL)
	{
		parseNode->Accept(*this);
	}
}


void PrettyPrinter::PrintList(const ListParseNode *listNode)
{
	const ListParseNode *current = listNode;
	while (current != NULL)
	{
		Print(current);
		current = current->GetNextNode();
	}
}


void PrettyPrinter::Visit(const TranslationUnit *translationUnit)
{
	PrintList(translationUnit->GetIncludeDirectiveList());
	mWriter.Write("\n");
	PrintList(translationUnit->GetExternalDeclarationList());
}


void PrettyPrinter::Visit(const IncludeDirective *includeDirective)
{
	Tab();
	mWriter.Write("include ");
	Print(includeDirective->GetIncludePath());
	mWriter.Write(";\n");
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


void PrettyPrinter::Visit(const NativeBlock *nativeBlock)
{
	Tab();
	mWriter.Write("native\n");
	Tab();
	mWriter.Write("{\n");
	IncrementTab();
	PrintList(nativeBlock->GetNativeDeclarationList());
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
		mWriter.Write(" = %" BOND_PRId32, enumerator->GetTypeAndValue()->GetIntValue());
	}
	else if (enumerator->GetValue() != NULL)
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

	switch (structDeclaration->GetVariant())
	{
		case StructDeclaration::VARIANT_NATIVE:
			mWriter.Write("native ");
			break;
		case StructDeclaration::VARIANT_REFERENCE:
			mWriter.Write("ref ");
			break;
		default:
			break;
	}

	Print(structDeclaration->GetName());
	mWriter.Write("\n");
	Tab();
	mWriter.Write("{\n");
	IncrementTab();
	PrintList(structDeclaration->GetMemberFunctionList());
	PrintList(structDeclaration->GetMemberVariableList());
	DecrementTab();
	Tab();
	mWriter.Write("};\n");
}


void PrettyPrinter::Visit(const FunctionDefinition *functionDefinition)
{
	Tab();
	Print(functionDefinition->GetPrototype());
	if (functionDefinition->GetBody() != NULL)
	{
		mWriter.Write("\n");
		Print(functionDefinition->GetBody());
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

	if (functionPrototype->IsConst())
	{
		mWriter.Write(" const");
	}
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
		const TypeDescriptor parent = typeDescriptor->GetDereferencedType();
		Visit(&parent);
		mWriter.Write(" *");
		if (typeDescriptor->IsConst())
		{
			mWriter.Write(" const");
		}
	}
	else if (typeDescriptor->IsArrayType())
	{
		const TypeDescriptor value = typeDescriptor->GetArrayElementType();
		Visit(&value);
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
	else if (typeDescriptor->IsNullType())
	{
		mWriter.Write("null");
	}
}


void PrettyPrinter::Visit(const TypeSpecifier *typeSpecifier)
{
	if (typeSpecifier->GetPrimitiveTypeToken() != NULL)
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

	if (namedInitializer->GetInitializer() != NULL)
	{
		mWriter.Write(" = ");
		Print(namedInitializer->GetInitializer());
	}
}


void PrettyPrinter::Visit(const Initializer *initializer)
{
	if (initializer->GetExpression() != NULL)
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
	Tab();
	mWriter.Write("{\n");
	IncrementTab();
	PrintList(compoundStatement->GetStatementList());
	DecrementTab();
	Tab();
	mWriter.Write("}\n");
}


void PrettyPrinter::Visit(const IfStatement *ifStatement)
{
	Tab();
	mWriter.Write("if (");
	Print(ifStatement->GetCondition());
	mWriter.Write(")\n");

	PrintBlockOrStatement(ifStatement->GetThenStatement());

	if (ifStatement->GetElseStatement() != NULL)
	{
		Tab();
		mWriter.Write("else\n");
		PrintBlockOrStatement(ifStatement->GetElseStatement());
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

	if (switchLabel->GetExpression() != NULL)
	{
		mWriter.Write(" ");
		Print(switchLabel->GetExpression());
	}

	mWriter.Write(":\n");
}


void PrettyPrinter::Visit(const WhileStatement *whileStatement)
{
	Tab();
	if (whileStatement->IsDoLoop())
	{
		mWriter.Write("do\n");
		PrintBlockOrStatement(whileStatement->GetBody());
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
		PrintBlockOrStatement(whileStatement->GetBody());
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
	PrintBlockOrStatement(forStatement->GetBody());
}


void PrettyPrinter::Visit(const JumpStatement *jumpStatement)
{
	Tab();
	Print(jumpStatement->GetKeyword());

	const Expression *rhs = jumpStatement->GetRhs(); 
	if (jumpStatement->IsReturn() && (rhs != NULL))
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
	if (expressionStatement->GetExpression() != NULL)
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
		if (!IsTopLevelExpression())
		{
			mWriter.Write("(");
		}
		PrintExpression(conditionalExpression->GetCondition());
		mWriter.Write(" ? ");
		PrintExpression(conditionalExpression->GetTrueExpression());
		mWriter.Write(" : ");
		PrintExpression(conditionalExpression->GetFalseExpression());
		if (!IsTopLevelExpression())
		{
			mWriter.Write(")");
		}
	}
}


void PrettyPrinter::Visit(const BinaryExpression *binaryExpression)
{
	if (!PrintFoldedConstant(binaryExpression))
	{
		if (!IsTopLevelExpression())
		{
			mWriter.Write("(");
		}
		PrintExpression(binaryExpression->GetLhs());
		mWriter.Write(" ");
		Print(binaryExpression->GetOperator());
		mWriter.Write(" ");
		PrintExpression(binaryExpression->GetRhs());
		if (!IsTopLevelExpression())
		{
			mWriter.Write(")");
		}
	}
}


void PrettyPrinter::Visit(const UnaryExpression *unaryExpression)
{
	if (!PrintFoldedConstant(unaryExpression))
	{
		Print(unaryExpression->GetOperator());
		PrintExpression(unaryExpression->GetRhs());
	}
}


void PrettyPrinter::Visit(const PostfixExpression *postfixExpression)
{
	PrintExpression(postfixExpression->GetLhs());
	Print(postfixExpression->GetOperator());
}


void PrettyPrinter::Visit(const MemberExpression *memberExpression)
{
	PrintExpression(memberExpression->GetLhs());
	Print(memberExpression->GetOperator());
	Print(memberExpression->GetMemberName());
}


void PrettyPrinter::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	PrintExpression(arraySubscriptExpression->GetLhs());
	mWriter.Write("[");
	PrintTopLevelExpression(arraySubscriptExpression->GetIndex());
	mWriter.Write("]");
}


void PrettyPrinter::Visit(const FunctionCallExpression *functionCallExpression)
{
	BoolStack::Element topLevelExpressionElement(mTopLevelExpression, true);
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
		PrintTopLevelExpression(castExpression->GetRhs());
		mWriter.Write(")");
	}
}


void PrettyPrinter::Visit(const SizeofExpression *sizeofExpression)
{
	if (!PrintFoldedConstant(sizeofExpression))
	{
		mWriter.Write("sizeof");
		if (sizeofExpression->GetTypeDescriptor() != NULL)
		{
			mWriter.Write("<");
			Print(sizeofExpression->GetTypeDescriptor());
			mWriter.Write(">");
		}
		else
		{
			mWriter.Write("(");
			PrintTopLevelExpression(sizeofExpression->GetRhs());
			mWriter.Write(")");
		}
	}
}


void PrettyPrinter::Visit(const ConstantExpression *constantExpression)
{
	Print(constantExpression->GetValueToken());
}


void PrettyPrinter::Visit(const ThisExpression *thisExpression)
{
	mWriter.Write("this");
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

	if (current != NULL)
	{
		Print(current);
		current = current->GetNextNode();
	}

	while (current != NULL)
	{
		mWriter.Write("%s", separator);
		Print(current);
		current = current->GetNextNode();
	}
}


void PrettyPrinter::Print(const Token *token)
{
	if (token != NULL)
	{
		mWriter.Write("%s", token->GetText());
	}
}


void PrettyPrinter::PrintBlockOrStatement(const ParseNode *parseNode)
{
	if (CastNode<CompoundStatement>(parseNode) != NULL)
	{
		Print(parseNode);
	}
	else
	{
		IncrementTab();
		Print(parseNode);
		DecrementTab();
	}
}


void PrettyPrinter::PrintExpression(const Expression *expression)
{
	BoolStack::Element topLevelExpressionElement(mTopLevelExpression, false);
	Print(expression);
}


void PrettyPrinter::PrintTopLevelExpression(const Expression *expression)
{
	BoolStack::Element topLevelExpressionElement(mTopLevelExpression, true);
	Print(expression);
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
				mWriter.Write("%c", char(tav.GetIntValue()));
				return true;
			case Token::KEY_UCHAR:
				mWriter.Write("%c", static_cast<unsigned char>(tav.GetUIntValue()));
				return true;
			case Token::KEY_INT:
				mWriter.Write("%" BOND_PRId32, tav.GetIntValue());
				return true;
			case Token::KEY_UINT:
				mWriter.Write("%" BOND_PRIu32, tav.GetUIntValue());
				return true;
			case Token::KEY_LONG:
				mWriter.Write("%" BOND_PRId64, tav.GetLongValue());
				return true;
			case Token::KEY_ULONG:
				mWriter.Write("%" BOND_PRId64, tav.GetULongValue());
				return true;
			case Token::KEY_FLOAT:
				mWriter.Write("%" BOND_PRIf32, tav.GetFloatValue());
				return true;
			case Token::KEY_DOUBLE:
				mWriter.Write("%" BOND_PRIf64, tav.GetDoubleValue());
				return true;
			default:
				break;
		}
	}
	return false;
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


}
