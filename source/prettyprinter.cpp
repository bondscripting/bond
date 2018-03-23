#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/parsenodevisitor.h"
#include "bond/io/outputstream.h"
#include "bond/stl/autostack.h"
#include "bond/tools/prettyprinter.h"

namespace Bond
{

class PrettyPrinterCore: private ParseNodeVisitorAdapter
{
public:
	PrettyPrinterCore(OutputStream &stream, PrettyPrinter::Verbosity verbosity, PrettyPrinter::ConstantFolding folding):
		mStream(stream),
		mTabLevel(0),
		mVerbosity(verbosity),
		mConstantFolding(folding),
		mSpace((verbosity == PrettyPrinter::VERBOSITY_MINIMAL) ? "" : " ")
	{}

	virtual ~PrettyPrinterCore() {}

	void Print(const ParseNode *parseNode);
	void PrintList(const ListParseNode *listNode);

	PrettyPrinterCore &operator=(const PrettyPrinterCore &other) = delete;

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
	virtual void Visit(const PropertyofExpression *propertyofExpression);
	virtual void Visit(const ConstantLiteralExpression *constantExpression);
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
	bool IsVerbosityMinimal() const { return mVerbosity == PrettyPrinter::VERBOSITY_MINIMAL; }
	bool ShouldFoldConstants() const { return mConstantFolding == PrettyPrinter::CONSTANT_FOLDING_ON; }
	bool ShouldPrintTabsAndNewlines() const { return mShouldPrintTabsAndNewlines.IsEmpty() || mShouldPrintTabsAndNewlines.GetTop(); }

	BoolStack mIsTopLevelExpression;
	BoolStack mIsTopLevelId;
	BoolStack mShouldPrintTabsAndNewlines;
	OutputStream &mStream;
	int mTabLevel;
	PrettyPrinter::Verbosity mVerbosity;
	PrettyPrinter::ConstantFolding mConstantFolding;
	const char *mSpace;
};


void PrettyPrinter::Print(const ParseNode *parseNode, OutputStream &stream, Verbosity verbosity,
	ConstantFolding folding) const
{
	PrettyPrinterCore printer(stream, verbosity, folding);
	printer.Print(parseNode);
}


void PrettyPrinter::PrintList(const ListParseNode *listNode, OutputStream &stream, Verbosity verbosity,
	ConstantFolding folding) const
{
	PrettyPrinterCore printer(stream, verbosity, folding);
	printer.PrintList(listNode);
}


void PrettyPrinterCore::Print(const ParseNode *parseNode)
{
	if (parseNode != nullptr)
	{
		parseNode->Accept(*this);
	}
}


void PrettyPrinterCore::PrintList(const ListParseNode *listNode)
{
	const ListParseNode *current = listNode;
	while (current != nullptr)
	{
		Print(current);
		current = current->GetNextNode();
	}
}


void PrettyPrinterCore::Visit(const TranslationUnit *translationUnit)
{
	PrintList(translationUnit->GetIncludeDirectiveList());
	mStream.Print("\n");
	PrintList(translationUnit->GetExternalDeclarationList());
}


void PrettyPrinterCore::Visit(const IncludeDirective *includeDirective)
{
	Tab();
	mStream.Print("include ");
	Print(includeDirective->GetIncludePath());
	mStream.Print(";\n");
}


void PrettyPrinterCore::Visit(const NamespaceDefinition *namespaceDefinition)
{
	Tab();
	mStream.Print("namespace ");
	Print(namespaceDefinition->GetName());
	mStream.Print("\n");
	Tab();
	mStream.Print("{\n");
	IncrementTab();
	PrintList(namespaceDefinition->GetExternalDeclarationList());
	DecrementTab();
	Tab();
	mStream.Print("}\n");
}


void PrettyPrinterCore::Visit(const NativeBlock *nativeBlock)
{
	Tab();
	mStream.Print("native\n");
	Tab();
	mStream.Print("{\n");
	IncrementTab();
	PrintList(nativeBlock->GetNativeDeclarationList());
	DecrementTab();
	Tab();
	mStream.Print("}\n");
}


void PrettyPrinterCore::Visit(const EnumDeclaration *enumDeclaration)
{
	Tab();
	mStream.Print("enum ");
	Print(enumDeclaration->GetName());
	mStream.Print("\n");
	Tab();
	mStream.Print("{\n");
	IncrementTab();
	PrintList(enumDeclaration->GetEnumeratorList());
	DecrementTab();
	Tab();
	mStream.Print("};\n");
}


void PrettyPrinterCore::Visit(const Enumerator *enumerator)
{
	Tab();
	Print(enumerator->GetName());
	if (ShouldFoldConstants() && enumerator->GetTypeAndValue()->IsValueDefined())
	{
		mStream.Print("%s=%s%" BOND_PRId32, mSpace, enumerator->GetTypeAndValue()->GetIntValue(), mSpace);
	}
	else if (enumerator->GetValue() != nullptr)
	{
		mStream.Print("%s=%s", mSpace, mSpace);
		Print(enumerator->GetValue());
	}
	mStream.Print(",\n");
}


void PrettyPrinterCore::Visit(const StructDeclaration *structDeclaration)
{
	Tab();
	mStream.Print("struct ");
	Print(structDeclaration->GetName());

	const Token *sizeToken = structDeclaration->GetSizeToken();
	if (sizeToken != nullptr)
	{
		const uint32_t size = CastValue(*sizeToken, Token::KEY_UINT).mUInt;
		mStream.Print("<%" BOND_PRIu32, size);

		const Token *alignmentToken = structDeclaration->GetAlignmentToken();
		if (alignmentToken != nullptr)
		{
			const uint32_t alignment = CastValue(*alignmentToken, Token::KEY_UINT).mUInt;
			mStream.Print(", %" BOND_PRIu32, alignment);
		}
		mStream.Print(">");
	}

	if (structDeclaration->GetVariant() != StructDeclaration::VARIANT_NATIVE_STUB)
	{
		mStream.Print("\n");
		Tab();
		mStream.Print("{\n");
		IncrementTab();
		PrintList(structDeclaration->GetMemberFunctionList());
		PrintList(structDeclaration->GetMemberVariableList());
		DecrementTab();
		Tab();
		mStream.Print("}");
	}
	mStream.Print(";\n");
}


void PrettyPrinterCore::Visit(const FunctionDefinition *functionDefinition)
{
	Tab();
	Print(functionDefinition->GetPrototype());
	if (functionDefinition->GetBody() != nullptr)
	{
		mStream.Print("\n");
		Print(functionDefinition->GetBody());
	}
	else
	{
		mStream.Print(";\n");
	}
}


void PrettyPrinterCore::Visit(const FunctionPrototype *functionPrototype)
{
	Print(functionPrototype->GetReturnType());
	mStream.Print(" ");
	Print(functionPrototype->GetName());
	mStream.Print("(");
	PrintList(functionPrototype->GetParameterList(), IsVerbosityMinimal() ? "," : ", ");
	mStream.Print(")");

	if (functionPrototype->IsConst())
	{
		mStream.Print(" const");
	}
}


void PrettyPrinterCore::Visit(const Parameter *parameter)
{
	Print(parameter->GetTypeDescriptor());
	mStream.Print(" ");
	Print(parameter->GetName());
}


void PrettyPrinterCore::Visit(const TypeDescriptor *typeDescriptor)
{
	if (typeDescriptor->IsPointerIntrinsicType())
	{
		const TypeDescriptor parent = typeDescriptor->GetDereferencedType();
		Visit(&parent);
		mStream.Print(" *");
		if (typeDescriptor->IsConst())
		{
			mStream.Print(" const");
		}
	}
	else if (typeDescriptor->IsArrayType())
	{
		const TypeDescriptor value = typeDescriptor->GetArrayElementType();
		Visit(&value);
		mStream.Print("[");
		PrintList(typeDescriptor->GetLengthExpressionList(), "][");
		mStream.Print("]");
	}
	else if (typeDescriptor->IsValueType())
	{
		if (typeDescriptor->IsConst())
		{
			mStream.Print("const ");
		}
		Print(typeDescriptor->GetTypeSpecifier());
	}
	else if (typeDescriptor->IsNullType())
	{
		mStream.Print("null");
	}
}


void PrettyPrinterCore::Visit(const TypeSpecifier *typeSpecifier)
{
	if (typeSpecifier->GetPrimitiveTypeToken() != nullptr)
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

	if (namedInitializer->GetInitializer() != nullptr)
	{
		mStream.Print("%s=%s", mSpace, mSpace);
		Print(namedInitializer->GetInitializer());
	}
}


void PrettyPrinterCore::Visit(const Initializer *initializer)
{
	if (initializer->GetExpression() != nullptr)
	{
		Print(initializer->GetExpression());
	}
	else
	{
		mStream.Print("{%s", mSpace);
		PrintList(initializer->GetInitializerList(), IsVerbosityMinimal() ? "," : ", ");
		mStream.Print("%s}", mSpace);
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
	mStream.Print("{\n");
	IncrementTab();
	PrintList(compoundStatement->GetStatementList());
	DecrementTab();
	Tab();
	mStream.Print("}\n");
}


void PrettyPrinterCore::Visit(const IfStatement *ifStatement)
{
	Tab();
	mStream.Print("if (");
	Print(ifStatement->GetCondition());
	mStream.Print(")\n");

	PrintBlockOrStatement(ifStatement->GetThenStatement());

	if (ifStatement->GetElseStatement() != nullptr)
	{
		Tab();
		mStream.Print("else\n");
		PrintBlockOrStatement(ifStatement->GetElseStatement());
	}
}


void PrettyPrinterCore::Visit(const SwitchStatement *switchStatement)
{
	Tab();
	mStream.Print("switch (");
	Print(switchStatement->GetControl());
	mStream.Print(")\n");

	Tab();
	mStream.Print("{\n");
	IncrementTab();
	PrintList(switchStatement->GetSectionList());
	DecrementTab();
	Tab();
	mStream.Print("}\n");
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

	if (switchLabel->GetExpression() != nullptr)
	{
		mStream.Print(" ");
		Print(switchLabel->GetExpression());
	}

	mStream.Print(":\n");
}


void PrettyPrinterCore::Visit(const WhileStatement *whileStatement)
{
	Tab();
	if (whileStatement->IsDoLoop())
	{
		mStream.Print("do\n");
		PrintBlockOrStatement(whileStatement->GetBody());
		Tab();
		mStream.Print("while (");
		Print(whileStatement->GetCondition());
		mStream.Print(");\n");
	}
	else
	{
		mStream.Print("while (");
		Print(whileStatement->GetCondition());
		mStream.Print(")\n");
		PrintBlockOrStatement(whileStatement->GetBody());
	}
}


void PrettyPrinterCore::Visit(const ForStatement *forStatement)
{
	Tab();
	mStream.Print("for (");
	{
		BoolStack::Element shouldPrintTabsAndNewlinesElement(mShouldPrintTabsAndNewlines, false);
		Print(forStatement->GetInitializer());
	}
	mStream.Print(" ");
	Print(forStatement->GetCondition());
	mStream.Print("; ");
	Print(forStatement->GetCountingExpression());
	mStream.Print(")\n");
	PrintBlockOrStatement(forStatement->GetBody());
}


void PrettyPrinterCore::Visit(const JumpStatement *jumpStatement)
{
	Tab();
	Print(jumpStatement->GetKeyword());

	const Expression *rhs = jumpStatement->GetRhs();
	if (jumpStatement->IsReturn() && (rhs != nullptr))
	{
		mStream.Print(" ");
		Print(rhs);
	}

	mStream.Print(";");
	Newline();
}


void PrettyPrinterCore::Visit(const DeclarativeStatement *declarativeStatement)
{
	Tab();
	Print(declarativeStatement->GetTypeDescriptor());
	mStream.Print(" ");
	PrintList(declarativeStatement->GetNamedInitializerList(), ", ");
	mStream.Print(";");
	Newline();
}


void PrettyPrinterCore::Visit(const ExpressionStatement *expressionStatement)
{
	Tab();
	if (expressionStatement->GetExpression() != nullptr)
	{
		Print(expressionStatement->GetExpression());
	}
	mStream.Print(";");
	Newline();
}


void PrettyPrinterCore::Visit(const ConditionalExpression *conditionalExpression)
{
	if (!PrintFoldedConstant(conditionalExpression))
	{
		if (!IsTopLevelExpression())
		{
			mStream.Print("(");
		}
		PrintExpression(conditionalExpression->GetCondition());
		mStream.Print("%s?%s", mSpace, mSpace);
		PrintExpression(conditionalExpression->GetTrueExpression());
		mStream.Print("%s:%s", mSpace, mSpace);
		PrintExpression(conditionalExpression->GetFalseExpression());
		if (!IsTopLevelExpression())
		{
			mStream.Print(")");
		}
	}
}


void PrettyPrinterCore::Visit(const BinaryExpression *binaryExpression)
{
	if (!PrintFoldedConstant(binaryExpression))
	{
		if (!IsTopLevelExpression())
		{
			mStream.Print("(");
		}
		PrintExpression(binaryExpression->GetLhs());
		mStream.Print(mSpace);
		Print(binaryExpression->GetOperator());
		mStream.Print(mSpace);
		PrintExpression(binaryExpression->GetRhs());
		if (!IsTopLevelExpression())
		{
			mStream.Print(")");
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
	mStream.Print("[");
	PrintTopLevelExpression(arraySubscriptExpression->GetIndex());
	mStream.Print("]");
}


void PrettyPrinterCore::Visit(const FunctionCallExpression *functionCallExpression)
{
	BoolStack::Element topLevelExpressionElement(mIsTopLevelExpression, true);
	Print(functionCallExpression->GetLhs());
	mStream.Print("(");
	PrintList(functionCallExpression->GetArgumentList(), ", ");
	mStream.Print(")");
}


void PrettyPrinterCore::Visit(const CastExpression *castExpression)
{
	if (!PrintFoldedConstant(castExpression))
	{
		mStream.Print("cast<");
		Print(castExpression->GetTargetTypeDescriptor());
		mStream.Print(">(");
		PrintTopLevelExpression(castExpression->GetRhs());
		mStream.Print(")");
	}
}


void PrettyPrinterCore::Visit(const PropertyofExpression *propertyofExpression)
{
	if (!PrintFoldedConstant(propertyofExpression))
	{
		Print(propertyofExpression->GetOperator());
		if (propertyofExpression->GetTargetTypeDescriptor() != nullptr)
		{
			mStream.Print("<");
			Print(propertyofExpression->GetTargetTypeDescriptor());
			mStream.Print(">");
		}
		else
		{
			mStream.Print("(");
			PrintTopLevelExpression(propertyofExpression->GetRhs());
			mStream.Print(")");
		}
	}
}


void PrettyPrinterCore::Visit(const ConstantLiteralExpression *constantExpression)
{
	Print(constantExpression->GetValueToken());
}


void PrettyPrinterCore::Visit(const ThisExpression *thisExpression)
{
	mStream.Print("this");
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

	if (current != nullptr)
	{
		Print(current);
		current = current->GetNextNode();
	}

	while (current != nullptr)
	{
		mStream.Print("%s", separator);
		Print(current);
		current = current->GetNextNode();
	}
}


void PrettyPrinterCore::Print(const Token *token)
{
	if (token != nullptr)
	{
		token->GetText().PrintTo(mStream);
	}
}


void PrettyPrinterCore::PrintBlockOrStatement(const ParseNode *parseNode)
{
	if (CastNode<CompoundStatement>(parseNode) != nullptr)
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
	if (ShouldFoldConstants() && tav.IsValueDefined())
	{
		const TypeDescriptor &typeDescriptor = tav.GetTypeDescriptor();
		switch (typeDescriptor.GetPrimitiveType())
		{
			case Token::KEY_BOOL:
				mStream.Print("%s", tav.GetBoolValue() ? "true" : "false");
				return true;
			case Token::KEY_CHAR:
				mStream.Print("%c", char(tav.GetIntValue()));
				return true;
			case Token::KEY_UCHAR:
				mStream.Print("%c", static_cast<unsigned char>(tav.GetUIntValue()));
				return true;
			case Token::KEY_INT:
				mStream.Print("%" BOND_PRId32, tav.GetIntValue());
				return true;
			case Token::KEY_UINT:
				mStream.Print("%" BOND_PRIu32, tav.GetUIntValue());
				return true;
			case Token::KEY_LONG:
				mStream.Print("%" BOND_PRId64, tav.GetLongValue());
				return true;
			case Token::KEY_ULONG:
				mStream.Print("%" BOND_PRId64, tav.GetULongValue());
				return true;
			case Token::KEY_FLOAT:
				mStream.Print("%f", tav.GetFloatValue());
				return true;
			case Token::KEY_DOUBLE:
				mStream.Print("%f", tav.GetDoubleValue());
				return true;
			default:
				break;
		}
	}
	return false;
}


void PrettyPrinterCore::Tab()
{
	if (ShouldPrintTabsAndNewlines() && !IsVerbosityMinimal())
	{
		for (int i = 0; i < mTabLevel; ++i)
		{
			mStream.Print("\t");
		}
	}
}


void PrettyPrinterCore::Newline()
{
	if (ShouldPrintTabsAndNewlines())
	{
		mStream.Print("\n");
	}
}


}
