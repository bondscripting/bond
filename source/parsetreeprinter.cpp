#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/parsenodevisitor.h"
#include "bond/io/outputstream.h"
#include "bond/stl/autostack.h"
#include "bond/tools/parsetreeprinter.h"

namespace Bond
{

class ParseTreePrinterCore: private ParseNodeVisitorAdapter
{
public:
	explicit ParseTreePrinterCore(OutputStream &stream):
		mStream(stream),
		mTabLevel(0)
	{}

	virtual ~ParseTreePrinterCore() {}

	ParseTreePrinterCore(const ParseTreePrinterCore &other) = delete;
	ParseTreePrinterCore &operator=(const ParseTreePrinterCore &other) = delete;

	void Print(const ParseNode *parseNode);
	void PrintList(const ListParseNode *listNode);

private:
	virtual void Visit(const TranslationUnit *translationUnit) override;
	virtual void Visit(const IncludeDirective *includeDirective) override;
	virtual void Visit(const NamespaceDefinition *namespaceDefinition) override;
	virtual void Visit(const NativeBlock *nativeBlock) override;
	virtual void Visit(const EnumDeclaration *enumDeclaration) override;
	virtual void Visit(const Enumerator *enumerator) override;
	virtual void Visit(const StructDeclaration *structDeclaration) override;
	virtual void Visit(const FunctionDefinition *functionDefinition) override;
	virtual void Visit(const FunctionPrototype *functionPrototype) override;
	virtual void Visit(const Parameter *parameter) override;
	virtual void Visit(const TypeDescriptor *typeDescriptor) override;
	virtual void Visit(const TypeSpecifier *typeSpecifier) override;
	virtual void Visit(const NamedInitializer *namedInitializer) override;
	virtual void Visit(const Initializer *initializer) override;
	virtual void Visit(const QualifiedIdentifier *identifier) override;
	virtual void Visit(const CompoundStatement *compoundStatement) override;
	virtual void Visit(const IfStatement *ifStatement) override;
	virtual void Visit(const SwitchStatement *switchStatement) override;
	virtual void Visit(const SwitchSection *switchSection) override;
	virtual void Visit(const SwitchLabel *switchLabel) override;
	virtual void Visit(const WhileStatement *whileStatement) override;
	virtual void Visit(const ForStatement *forStatement) override;
	virtual void Visit(const JumpStatement *jumpStatement) override;
	virtual void Visit(const DeclarativeStatement *declarativeStatement) override;
	virtual void Visit(const ExpressionStatement *expressionStatement) override;
	virtual void Visit(const ConditionalExpression *conditionalExpression) override;
	virtual void Visit(const BinaryExpression *binaryExpression) override;
	virtual void Visit(const UnaryExpression *unaryExpression) override;
	virtual void Visit(const PostfixExpression *postfixExpression) override;
	virtual void Visit(const MemberExpression *memberExpression) override;
	virtual void Visit(const ArraySubscriptExpression *arraySubscriptExpression) override;
	virtual void Visit(const FunctionCallExpression *functionCallExpression) override;
	virtual void Visit(const CastExpression *castExpression) override;
	virtual void Visit(const PropertyofExpression *propertyofExpression) override;
	virtual void Visit(const ConstantLiteralExpression *constantExpression) override;
	virtual void Visit(const IdentifierExpression *identifierExpression) override;
	virtual void Visit(const ThisExpression *thisExpression) override;

	void PrintList(const ListParseNode *listNode, const char *separator);
	void Print(const Token *token);
	void IncrementTab() { ++mTabLevel; }
	void DecrementTab() { --mTabLevel; }
	void Tab();
	void Newline();
	bool IsTopLevelId() const { return mIsTopLevelId.IsEmpty() || mIsTopLevelId.GetTop(); }

	BoolStack mIsTopLevelId;
	OutputStream &mStream;
	int mTabLevel;
};


void ParseTreePrinter::Print(const ParseNode *parseNode, OutputStream &stream)
{
	ParseTreePrinterCore printer(stream);
	printer.Print(parseNode);
}


void ParseTreePrinter::PrintList(const ListParseNode *listNode, OutputStream &stream)
{
	ParseTreePrinterCore printer(stream);
	printer.PrintList(listNode);
}


void ParseTreePrinterCore::Print(const ParseNode *parseNode)
{
	if (parseNode != nullptr)
	{
		parseNode->Accept(*this);
	}
}


void ParseTreePrinterCore::PrintList(const ListParseNode *listNode)
{
	const ListParseNode *current = listNode;
	while (current != nullptr)
	{
		Print(current);
		current = current->GetNextNode();
	}
}


void ParseTreePrinterCore::Visit(const TranslationUnit *translationUnit)
{
	Tab();
	mStream.Print("TranslationUnit\n");
	IncrementTab();
	PrintList(translationUnit->GetIncludeDirectiveList());
	PrintList(translationUnit->GetExternalDeclarationList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const IncludeDirective *includeDirective)
{
	Tab();
	mStream.Print("IncludeDirective: ");
	Print(includeDirective->GetIncludePath());
	Newline();
}


void ParseTreePrinterCore::Visit(const NamespaceDefinition *namespaceDefinition)
{
	Tab();
	mStream.Print("NamespaceDefinition ");
	Print(namespaceDefinition->GetName());
	Newline();
	IncrementTab();
	PrintList(namespaceDefinition->GetExternalDeclarationList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const NativeBlock *nativeBlock)
{
	Tab();
	mStream.Print("NativeBlock\n");
	IncrementTab();
	PrintList(nativeBlock->GetNativeDeclarationList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const EnumDeclaration *enumDeclaration)
{
	Tab();
	mStream.Print("EnumDeclaration: ");
	Print(enumDeclaration->GetName());
	Newline();
	IncrementTab();
	PrintList(enumDeclaration->GetEnumeratorList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const Enumerator *enumerator)
{
	Tab();
	mStream.Print("Enumerator: ");
	Print(enumerator->GetName());
	Newline();
	IncrementTab();
	Print(enumerator->GetValue());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const StructDeclaration *structDeclaration)
{
	Tab();
	mStream.Print("StructDeclaration: ");
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

	Newline();
	IncrementTab();
	PrintList(structDeclaration->GetMemberFunctionList());
	PrintList(structDeclaration->GetMemberVariableList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const FunctionDefinition *functionDefinition)
{
	Tab();
	mStream.Print("FunctionDefinition\n");
	IncrementTab();
	Print(functionDefinition->GetPrototype());
	Print(functionDefinition->GetBody());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const FunctionPrototype *functionPrototype)
{
	Tab();
	mStream.Print("FunctionPrototype: ");
	Print(functionPrototype->GetName());
	Newline();
	IncrementTab();
	Tab();
	mStream.Print("(return type)\n");
	Print(functionPrototype->GetReturnType());
	PrintList(functionPrototype->GetParameterList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const Parameter *parameter)
{
	Tab();
	mStream.Print("Parameter: ");
	Print(parameter->GetName());
	Newline();
	IncrementTab();
	Print(parameter->GetTypeDescriptor());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const TypeDescriptor *typeDescriptor)
{
	Tab();
	mStream.Print("TypeDescriptor\n");
	IncrementTab();
	Print(typeDescriptor->GetTypeSpecifier());
	PrintList(typeDescriptor->GetLengthExpressionList());
	Print(typeDescriptor->GetParent());
	DecrementTab();

/*
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
*/
}


void ParseTreePrinterCore::Visit(const TypeSpecifier *typeSpecifier)
{
	Tab();
	mStream.Print("TypeSpecifier: ");
	if (typeSpecifier->GetPrimitiveTypeToken() != nullptr)
	{
		Print(typeSpecifier->GetPrimitiveTypeToken());
	}
	else
	{
		Print(typeSpecifier->GetIdentifier());
	}
	Newline();
}


void ParseTreePrinterCore::Visit(const NamedInitializer *namedInitializer)
{
	Tab();
	mStream.Print("NamedInitializer: ");
	Print(namedInitializer->GetName());
	Newline();
	IncrementTab();
	Print(namedInitializer->GetInitializer());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const Initializer *initializer)
{
	Tab();
	mStream.Print("Initializer\n");
	IncrementTab();
	Print(initializer->GetExpression());
	PrintList(initializer->GetInitializerList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const QualifiedIdentifier *identifier)
{
	if (IsTopLevelId())
	{
		auto topLevelIdElement = mIsTopLevelId.Push(false);
		PrintList(identifier, "::");
	}
	else
	{
		Print(identifier->GetName());
	}
}


void ParseTreePrinterCore::Visit(const CompoundStatement *compoundStatement)
{
	Tab();
	mStream.Print("CompoundStatement\n");
	IncrementTab();
	PrintList(compoundStatement->GetStatementList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const IfStatement *ifStatement)
{
	Tab();
	mStream.Print("IfStatement\n");
	IncrementTab();
	Tab();
	mStream.Print("(condition)\n");
	Print(ifStatement->GetCondition());
	Tab();
	mStream.Print("(body)\n");
	Print(ifStatement->GetThenStatement());
	if (ifStatement->GetElseStatement() != nullptr)
	{
		Tab();
		mStream.Print("(else)\n");
		Print(ifStatement->GetElseStatement());
	}
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const SwitchStatement *switchStatement)
{
	Tab();
	mStream.Print("SwitchStatement\n");
	Tab();
	mStream.Print("(control)\n");
	Print(switchStatement->GetControl());
	IncrementTab();
	PrintList(switchStatement->GetSectionList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const SwitchSection *switchSection)
{
	Tab();
	mStream.Print("SwitchSection\n");
	IncrementTab();
	if (switchSection->GetLabelList() != nullptr)
	{
		Tab();
		mStream.Print("(labels)\n");
		PrintList(switchSection->GetLabelList());
	}
	if (switchSection->GetStatementList() != nullptr)
	{
		Tab();
		mStream.Print("(statements)\n");
		PrintList(switchSection->GetStatementList());
	}
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const SwitchLabel *switchLabel)
{
	Tab();
	mStream.Print("SwitchLabel: ");
	Print(switchLabel->GetLabel());
	Newline();
	IncrementTab();
	Print(switchLabel->GetExpression());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const WhileStatement *whileStatement)
{
	Tab();
	mStream.Print("WhileStatement: %s\n", whileStatement->IsDoLoop() ? "do" : "while");
	IncrementTab();
	Tab();
	mStream.Print("(condition)\n");
	Print(whileStatement->GetCondition());
	Tab();
	mStream.Print("(body)\n");
	Print(whileStatement->GetBody());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const ForStatement *forStatement)
{
	Tab();
	mStream.Print("ForStatement\n");
	IncrementTab();
	if (forStatement->GetInitializer() != nullptr)
	{
		Tab();
		mStream.Print("(initializer)\n");
		Print(forStatement->GetInitializer());
	}
	if (forStatement->GetCondition() != nullptr)
	{
		Tab();
		mStream.Print("(condition)\n");
		Print(forStatement->GetCondition());
	}
	if (forStatement->GetCountingExpression() != nullptr)
	{
		Tab();
		mStream.Print("(counting)\n");
		Print(forStatement->GetCountingExpression());
	}
	mStream.Print("(body)\n");
	Print(forStatement->GetBody());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const JumpStatement *jumpStatement)
{
	Tab();
	mStream.Print("JumpStatement: ");
	Print(jumpStatement->GetKeyword());
	Newline();
	IncrementTab();
	Print(jumpStatement->GetRhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const DeclarativeStatement *declarativeStatement)
{
	Tab();
	mStream.Print("DeclarativeStatement\n");
	IncrementTab();
	Print(declarativeStatement->GetTypeDescriptor());
	PrintList(declarativeStatement->GetNamedInitializerList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const ExpressionStatement *expressionStatement)
{
	Tab();
	mStream.Print("ExpressionStatement\n");
	IncrementTab();
	Print(expressionStatement->GetExpression());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const ConditionalExpression *conditionalExpression)
{
	Tab();
	mStream.Print("ConditionalExpression\n");
	IncrementTab();
	Tab();
	mStream.Print("(condition)\n");
	Print(conditionalExpression->GetCondition());
	Tab();
	mStream.Print("(true expression)\n");
	Print(conditionalExpression->GetTrueExpression());
	Tab();
	mStream.Print("(false expression)\n");
	Print(conditionalExpression->GetFalseExpression());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const BinaryExpression *binaryExpression)
{
	Tab();
	mStream.Print("BinaryExpression: ");
	Print(binaryExpression->GetOperator());
	Newline();
	IncrementTab();
	Print(binaryExpression->GetLhs());
	Print(binaryExpression->GetRhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const UnaryExpression *unaryExpression)
{
	Tab();
	mStream.Print("UnaryExpression: ");
	Print(unaryExpression->GetOperator());
	Newline();
	IncrementTab();
	Print(unaryExpression->GetRhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const PostfixExpression *postfixExpression)
{
	Tab();
	mStream.Print("PostfixExpression: ");
	Print(postfixExpression->GetOperator());
	Newline();
	IncrementTab();
	Print(postfixExpression->GetLhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const MemberExpression *memberExpression)
{
	Tab();
	mStream.Print("MemberExpression: ");
	Print(memberExpression->GetOperator());
	Print(memberExpression->GetMemberName());
	Newline();
	IncrementTab();
	Print(memberExpression->GetLhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const ArraySubscriptExpression *arraySubscriptExpression)
{
	Tab();
	mStream.Print("ArraySubscriptExpression\n");
	IncrementTab();
	Tab();
	mStream.Print("(lhs)\n");
	Print(arraySubscriptExpression->GetLhs());
	Tab();
	mStream.Print("(index)\n");
	Print(arraySubscriptExpression->GetIndex());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const FunctionCallExpression *functionCallExpression)
{
	Tab();
	mStream.Print("FunctionCallExpression\n");
	IncrementTab();
	Tab();
	mStream.Print("(function)\n");
	Print(functionCallExpression->GetLhs());
	if (functionCallExpression->GetArgumentList() != nullptr)
	{
		Tab();
		mStream.Print("(arguments)\n");
		PrintList(functionCallExpression->GetArgumentList());
	}
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const CastExpression *castExpression)
{
	Tab();
	mStream.Print("CastExpression\n");
	IncrementTab();
	Tab();
	mStream.Print("(type)\n");
	Print(castExpression->GetTargetTypeDescriptor());
	Tab();
	mStream.Print("(expression)\n");
	Print(castExpression->GetRhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const PropertyofExpression *propertyofExpression)
{
	Tab();
	mStream.Print("PropertyofExpression: ");
	Print(propertyofExpression->GetOperator());
	Newline();
	IncrementTab();
	Print(propertyofExpression->GetTargetTypeDescriptor());
	Print(propertyofExpression->GetRhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const ConstantLiteralExpression *constantExpression)
{
	Tab();
	mStream.Print("ConstantLiteralExpression: ");
	Print(constantExpression->GetValueToken());
	Newline();
}


void ParseTreePrinterCore::Visit(const ThisExpression *thisExpression)
{
	Tab();
	mStream.Print("ThisExpression\n");
}


void ParseTreePrinterCore::Visit(const IdentifierExpression *identifierExpression)
{
	Tab();
	mStream.Print("IdentifierExpression: ");
	Print(identifierExpression->GetIdentifier());
	Newline();
}


void ParseTreePrinterCore::PrintList(const ListParseNode *listNode, const char *separator)
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


void ParseTreePrinterCore::Print(const Token *token)
{
	if (token != nullptr)
	{
		token->GetText().PrintTo(mStream);
	}
}


void ParseTreePrinterCore::Tab()
{
	for (int i = 0; i < mTabLevel; ++i)
	{
		mStream.Print("\t");
	}
}


void ParseTreePrinterCore::Newline()
{
	mStream.Print("\n");
}

}
