#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/parsenodevisitor.h"
#include "bond/io/textwriter.h"
#include "bond/stl/autostack.h"
#include "bond/tools/prettyprinter.h"

namespace Bond
{

class PrettyPrinterCore: private ParseNodeVisitorAdapter
{
public:
	PrettyPrinterCore(TextWriter &writer, bool printFoldedConstants):
		mWriter(writer),
		mTabLevel(0),
		mPrintFoldedConstants(printFoldedConstants)
	{}

	virtual ~PrettyPrinterCore() {}

	void Print(const ParseNode *parseNode);
	void PrintList(const ListParseNode *listNode);

private:
	virtual void Visit(const TranslationUnit *translationUnit);
	virtual void Visit(const IncludeDirective *includeDirective);
	virtual void Visit(const NamespaceDefinition *namespaceDefinition);
	virtual void Visit(const NativeBlock *nativeBlock);
	virtual void Visit(const EnumDeclaration *enumDeclaration);
	virtual void Visit(const Enumerator *enumerator);
	virtual void Visit(const StructDeclaration *structDeclaration);
	virtual void Visit(const FunctionDefinition *functionDefinition);
	virtual void Visit(const FunctionPrototype *functionPrototype);
	virtual void Visit(const Parameter *parameter);
	virtual void Visit(const TypeDescriptor *typeDescriptor);
	virtual void Visit(const TypeSpecifier *typeSpecifier);
	virtual void Visit(const NamedInitializer *namedInitializer);
	virtual void Visit(const Initializer *initializer);
	virtual void Visit(const QualifiedIdentifier *identifier);
	virtual void Visit(const CompoundStatement *compoundStatement);
	virtual void Visit(const IfStatement *ifStatement);
	virtual void Visit(const SwitchStatement *switchStatement);
	virtual void Visit(const SwitchSection *switchSection);
	virtual void Visit(const SwitchLabel *switchLabel);
	virtual void Visit(const WhileStatement *whileStatement);
	virtual void Visit(const ForStatement *forStatement);
	virtual void Visit(const JumpStatement *jumpStatement);
	virtual void Visit(const DeclarativeStatement *declarativeStatement);
	virtual void Visit(const ExpressionStatement *expressionStatement);
	virtual void Visit(const ConditionalExpression *conditionalExpression);
	virtual void Visit(const BinaryExpression *binaryExpression);
	virtual void Visit(const UnaryExpression *unaryExpression);
	virtual void Visit(const PostfixExpression *postfixExpression);
	virtual void Visit(const MemberExpression *memberExpression);
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression);
	virtual void Visit(const FunctionCallExpression *functionCallExpression);
	virtual void Visit(const CastExpression *castExpression);
	virtual void Visit(const SizeofExpression *sizeofExpression);
	virtual void Visit(const ConstantExpression *constantExpression);
	virtual void Visit(const IdentifierExpression *identifierExpression);
	virtual void Visit(const ThisExpression *thisExpression);

	void PrintList(const ListParseNode *listNode, const char *separator);
	void Print(const Token *token);
	void PrintBlockOrStatement(const ParseNode *parseNode);
	void PrintExpression(const Expression *expression);
	void PrintTopLevelExpression(const Expression *expression);
	bool PrintFoldedConstant(const Expression *expression);
	void IncrementTab() { ++mTabLevel; }
	void DecrementTab() { --mTabLevel; }
	void Tab();
	void Newline();
	bool IsTopLevelExpression() const { return mIsTopLevelExpression.IsEmpty() || mIsTopLevelExpression.GetTop(); }
	bool IsTopLevelId() const { return mIsTopLevelId.IsEmpty() || mIsTopLevelId.GetTop(); }
	bool ShouldPrintTabsAndNewlines() const { return mShouldPrintTabsAndNewlines.IsEmpty() || mShouldPrintTabsAndNewlines.GetTop(); }

	BoolStack mIsTopLevelExpression;
	BoolStack mIsTopLevelId;
	BoolStack mShouldPrintTabsAndNewlines;
	TextWriter &mWriter;
	int mTabLevel;
	bool mPrintFoldedConstants;
};


void PrettyPrinter::Print(const ParseNode *parseNode, TextWriter &writer, bool printFoldedConstants)
{
	PrettyPrinterCore printer(writer, printFoldedConstants);
	printer.Print(parseNode);
}


void PrettyPrinter::PrintList(const ListParseNode *listNode, TextWriter &writer, bool printFoldedConstants)
{
	PrettyPrinterCore printer(writer, printFoldedConstants);
	printer.PrintList(listNode);
}


void PrettyPrinterCore::Print(const ParseNode *parseNode)
{
	if (parseNode != NULL)
	{
		parseNode->Accept(*this);
	}
}


void PrettyPrinterCore::PrintList(const ListParseNode *listNode)
{
	const ListParseNode *current = listNode;
	while (current != NULL)
	{
		Print(current);
		current = current->GetNextNode();
	}
}


void PrettyPrinterCore::Visit(const TranslationUnit *translationUnit)
{
	PrintList(translationUnit->GetIncludeDirectiveList());
	mWriter.Write("\n");
	PrintList(translationUnit->GetExternalDeclarationList());
}


void PrettyPrinterCore::Visit(const IncludeDirective *includeDirective)
{
	Tab();
	mWriter.Write("include ");
	Print(includeDirective->GetIncludePath());
	mWriter.Write(";\n");
}


void PrettyPrinterCore::Visit(const NamespaceDefinition *namespaceDefinition)
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


void PrettyPrinterCore::Visit(const NativeBlock *nativeBlock)
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


void PrettyPrinterCore::Visit(const EnumDeclaration *enumDeclaration)
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


void PrettyPrinterCore::Visit(const Enumerator *enumerator)
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


void PrettyPrinterCore::Visit(const StructDeclaration *structDeclaration)
{
	Tab();
	mWriter.Write("struct ");
	Print(structDeclaration->GetName());

	const Token *sizeToken = structDeclaration->GetSizeToken();
	if (sizeToken != NULL)
	{
		const bi32_t size = CastValue(sizeToken->GetValue(), sizeToken->GetTokenType(), Token::CONST_INT).mInt;
		mWriter.Write("<%" BOND_PRId32, size);

		const Token *alignmentToken = structDeclaration->GetAlignmentToken();
		if (alignmentToken != NULL)
		{
			const bi32_t alignment = CastValue(alignmentToken->GetValue(), alignmentToken->GetTokenType(), Token::CONST_INT).mInt;
			mWriter.Write(", %" BOND_PRId32, alignment);
		}
		mWriter.Write(">");
	}

	if (structDeclaration->GetVariant() != StructDeclaration::VARIANT_NATIVE_STUB)
	{
		mWriter.Write("\n");
		Tab();
		mWriter.Write("{\n");
		IncrementTab();
		PrintList(structDeclaration->GetMemberFunctionList());
		PrintList(structDeclaration->GetMemberVariableList());
		DecrementTab();
		Tab();
		mWriter.Write("}");
	}
	mWriter.Write(";\n");
}


void PrettyPrinterCore::Visit(const FunctionDefinition *functionDefinition)
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


void PrettyPrinterCore::Visit(const FunctionPrototype *functionPrototype)
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


void PrettyPrinterCore::Visit(const Parameter *parameter)
{
	Print(parameter->GetTypeDescriptor());
	mWriter.Write(" ");
	Print(parameter->GetName());
}


void PrettyPrinterCore::Visit(const TypeDescriptor *typeDescriptor)
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


void PrettyPrinterCore::Visit(const TypeSpecifier *typeSpecifier)
{
	if (typeSpecifier->GetPrimitiveTypeToken() != NULL)
	{
		Print(typeSpecifier->GetPrimitiveTypeToken());
	}
	else
	{
		Print(typeSpecifier->GetIdentifier());
	}
}


void PrettyPrinterCore::Visit(const NamedInitializer *namedInitializer)
{
	Print(namedInitializer->GetName());

	if (namedInitializer->GetInitializer() != NULL)
	{
		mWriter.Write(" = ");
		Print(namedInitializer->GetInitializer());
	}
}


void PrettyPrinterCore::Visit(const Initializer *initializer)
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


void PrettyPrinterCore::Visit(const QualifiedIdentifier *identifier)
{
	if (IsTopLevelId())
	{
		BoolStack::Element topLevelIdElement(mIsTopLevelId, false);
		PrintList(identifier, "::");
	}
	else
	{
		Print(identifier->GetName());
	}
}


void PrettyPrinterCore::Visit(const CompoundStatement *compoundStatement)
{
	Tab();
	mWriter.Write("{\n");
	IncrementTab();
	PrintList(compoundStatement->GetStatementList());
	DecrementTab();
	Tab();
	mWriter.Write("}\n");
}


void PrettyPrinterCore::Visit(const IfStatement *ifStatement)
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


void PrettyPrinterCore::Visit(const SwitchStatement *switchStatement)
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


void PrettyPrinterCore::Visit(const SwitchSection *switchSection)
{
	PrintList(switchSection->GetLabelList());
	IncrementTab();
	PrintList(switchSection->GetStatementList());
	DecrementTab();
}


void PrettyPrinterCore::Visit(const SwitchLabel *switchLabel)
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


void PrettyPrinterCore::Visit(const WhileStatement *whileStatement)
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


void PrettyPrinterCore::Visit(const ForStatement *forStatement)
{
	Tab();
	mWriter.Write("for (");
	{
		BoolStack::Element shouldPrintTabsAndNewlinesElement(mShouldPrintTabsAndNewlines, false);
		Print(forStatement->GetInitializer());
	}
	mWriter.Write(" ");
	Print(forStatement->GetCondition());
	mWriter.Write("; ");
	Print(forStatement->GetCountingExpression());
	mWriter.Write(")\n");
	PrintBlockOrStatement(forStatement->GetBody());
}


void PrettyPrinterCore::Visit(const JumpStatement *jumpStatement)
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


void PrettyPrinterCore::Visit(const DeclarativeStatement *declarativeStatement)
{
	Tab();
	Print(declarativeStatement->GetTypeDescriptor());
	mWriter.Write(" ");
	PrintList(declarativeStatement->GetNamedInitializerList(), ", ");
	mWriter.Write(";");
	Newline();
}


void PrettyPrinterCore::Visit(const ExpressionStatement *expressionStatement)
{
	Tab();
	if (expressionStatement->GetExpression() != NULL)
	{
		Print(expressionStatement->GetExpression());
	}
	mWriter.Write(";");
	Newline();
}


void PrettyPrinterCore::Visit(const ConditionalExpression *conditionalExpression)
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


void PrettyPrinterCore::Visit(const BinaryExpression *binaryExpression)
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


void PrettyPrinterCore::Visit(const UnaryExpression *unaryExpression)
{
	if (!PrintFoldedConstant(unaryExpression))
	{
		Print(unaryExpression->GetOperator());
		PrintExpression(unaryExpression->GetRhs());
	}
}


void PrettyPrinterCore::Visit(const PostfixExpression *postfixExpression)
{
	PrintExpression(postfixExpression->GetLhs());
	Print(postfixExpression->GetOperator());
}


void PrettyPrinterCore::Visit(const MemberExpression *memberExpression)
{
	PrintExpression(memberExpression->GetLhs());
	Print(memberExpression->GetOperator());
	Print(memberExpression->GetMemberName());
}


void PrettyPrinterCore::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	PrintExpression(arraySubscriptExpression->GetLhs());
	mWriter.Write("[");
	PrintTopLevelExpression(arraySubscriptExpression->GetIndex());
	mWriter.Write("]");
}


void PrettyPrinterCore::Visit(const FunctionCallExpression *functionCallExpression)
{
	BoolStack::Element topLevelExpressionElement(mIsTopLevelExpression, true);
	Print(functionCallExpression->GetLhs());
	mWriter.Write("(");
	PrintList(functionCallExpression->GetArgumentList(), ", ");
	mWriter.Write(")");
}


void PrettyPrinterCore::Visit(const CastExpression *castExpression)
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


void PrettyPrinterCore::Visit(const SizeofExpression *sizeofExpression)
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


void PrettyPrinterCore::Visit(const ConstantExpression *constantExpression)
{
	Print(constantExpression->GetValueToken());
}


void PrettyPrinterCore::Visit(const ThisExpression *thisExpression)
{
	mWriter.Write("this");
}


void PrettyPrinterCore::Visit(const IdentifierExpression *identifierExpression)
{
	if (!PrintFoldedConstant(identifierExpression))
	{
		Print(identifierExpression->GetIdentifier());
	}
}


void PrettyPrinterCore::PrintList(const ListParseNode *listNode, const char *separator)
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


void PrettyPrinterCore::Print(const Token *token)
{
	if (token != NULL)
	{
		mWriter.Write("%s", token->GetText());
	}
}


void PrettyPrinterCore::PrintBlockOrStatement(const ParseNode *parseNode)
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


void PrettyPrinterCore::PrintExpression(const Expression *expression)
{
	BoolStack::Element topLevelExpressionElement(mIsTopLevelExpression, false);
	Print(expression);
}


void PrettyPrinterCore::PrintTopLevelExpression(const Expression *expression)
{
	BoolStack::Element topLevelExpressionElement(mIsTopLevelExpression, true);
	Print(expression);
}


bool PrettyPrinterCore::PrintFoldedConstant(const Expression *expression)
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


void PrettyPrinterCore::Tab()
{
	if (ShouldPrintTabsAndNewlines())
	{
		for (int i = 0; i < mTabLevel; ++i)
		{
			mWriter.Write("\t");
		}
	}
}


void PrettyPrinterCore::Newline()
{
	if (ShouldPrintTabsAndNewlines())
	{
		mWriter.Write("\n");
	}
}


}
