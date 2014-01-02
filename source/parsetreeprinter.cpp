#include "bond/compiler/parsenodes.h"
#include "bond/compiler/parsenodeutil.h"
#include "bond/compiler/parsenodevisitor.h"
#include "bond/io/textwriter.h"
#include "bond/stl/autostack.h"
#include "bond/tools/parsetreeprinter.h"

namespace Bond
{

class ParseTreePrinterCore: private ParseNodeVisitorAdapter
{
public:
	ParseTreePrinterCore(TextWriter &writer):
		mWriter(writer),
		mTabLevel(0)
	{}

	virtual ~ParseTreePrinterCore() {}

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
	void IncrementTab() { ++mTabLevel; }
	void DecrementTab() { --mTabLevel; }
	void Tab();
	void Newline();
	bool IsTopLevelId() const { return mIsTopLevelId.IsEmpty() || mIsTopLevelId.GetTop(); }

	BoolStack mIsTopLevelId;
	TextWriter &mWriter;
	int mTabLevel;
};


void ParseTreePrinter::Print(const ParseNode *parseNode, TextWriter &writer)
{
	ParseTreePrinterCore printer(writer);
	printer.Print(parseNode);
}


void ParseTreePrinter::PrintList(const ListParseNode *listNode, TextWriter &writer)
{
	ParseTreePrinterCore printer(writer);
	printer.PrintList(listNode);
}


void ParseTreePrinterCore::Print(const ParseNode *parseNode)
{
	if (parseNode != NULL)
	{
		parseNode->Accept(*this);
	}
}


void ParseTreePrinterCore::PrintList(const ListParseNode *listNode)
{
	const ListParseNode *current = listNode;
	while (current != NULL)
	{
		Print(current);
		current = current->GetNextNode();
	}
}


void ParseTreePrinterCore::Visit(const TranslationUnit *translationUnit)
{
	Tab();
	mWriter.Write("TranslationUnit\n");
	IncrementTab();
	PrintList(translationUnit->GetIncludeDirectiveList());
	PrintList(translationUnit->GetExternalDeclarationList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const IncludeDirective *includeDirective)
{
	Tab();
	mWriter.Write("IncludeDirective: ");
	Print(includeDirective->GetIncludePath());
	Newline();
}


void ParseTreePrinterCore::Visit(const NamespaceDefinition *namespaceDefinition)
{
	Tab();
	mWriter.Write("NamespaceDefinition ");
	Print(namespaceDefinition->GetName());
	Newline();
	IncrementTab();
	PrintList(namespaceDefinition->GetExternalDeclarationList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const NativeBlock *nativeBlock)
{
	Tab();
	mWriter.Write("NativeBlock\n");
	IncrementTab();
	PrintList(nativeBlock->GetNativeDeclarationList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const EnumDeclaration *enumDeclaration)
{
	Tab();
	mWriter.Write("EnumDeclaration: ");
	Print(enumDeclaration->GetName());
	Newline();
	IncrementTab();
	PrintList(enumDeclaration->GetEnumeratorList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const Enumerator *enumerator)
{
	Tab();
	mWriter.Write("Enumerator: ");
	Print(enumerator->GetName());
	Newline();
	IncrementTab();
	Print(enumerator->GetValue());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const StructDeclaration *structDeclaration)
{
	Tab();
	mWriter.Write("StructDeclaration: ");
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

	Newline();
	IncrementTab();
	PrintList(structDeclaration->GetMemberFunctionList());
	PrintList(structDeclaration->GetMemberVariableList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const FunctionDefinition *functionDefinition)
{
	Tab();
	mWriter.Write("FunctionDefinition\n");
	IncrementTab();
	Print(functionDefinition->GetPrototype());
	Print(functionDefinition->GetBody());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const FunctionPrototype *functionPrototype)
{
	Tab();
	mWriter.Write("FunctionPrototype: ");
	Print(functionPrototype->GetName());
	Newline();
	IncrementTab();
	Tab();
	mWriter.Write("(return type)\n");
	Print(functionPrototype->GetReturnType());
	PrintList(functionPrototype->GetParameterList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const Parameter *parameter)
{
	Tab();
	mWriter.Write("Parameter: ");
	Print(parameter->GetName());
	Newline();
	IncrementTab();
	Print(parameter->GetTypeDescriptor());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const TypeDescriptor *typeDescriptor)
{
	Tab();
	mWriter.Write("TypeDescriptor\n");
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
*/
}


void ParseTreePrinterCore::Visit(const TypeSpecifier *typeSpecifier)
{
	Tab();
	mWriter.Write("TypeSpecifier: ");
	if (typeSpecifier->GetPrimitiveTypeToken() != NULL)
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
	mWriter.Write("NamedInitializer: ");
	Print(namedInitializer->GetName());
	Newline();
	IncrementTab();
	Print(namedInitializer->GetInitializer());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const Initializer *initializer)
{
	Tab();
	mWriter.Write("Initializer\n");
	IncrementTab();
	Print(initializer->GetExpression());
	PrintList(initializer->GetInitializerList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const QualifiedIdentifier *identifier)
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


void ParseTreePrinterCore::Visit(const CompoundStatement *compoundStatement)
{
	Tab();
	mWriter.Write("CompoundStatement\n");
	IncrementTab();
	PrintList(compoundStatement->GetStatementList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const IfStatement *ifStatement)
{
	Tab();
	mWriter.Write("IfStatement\n");
	IncrementTab();
	Tab();
	mWriter.Write("(condition)\n");
	Print(ifStatement->GetCondition());
	Tab();
	mWriter.Write("(body)\n");
	Print(ifStatement->GetThenStatement());
	if (ifStatement->GetElseStatement() != NULL)
	{
		Tab();
		mWriter.Write("(else)\n");
		Print(ifStatement->GetElseStatement());
	}
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const SwitchStatement *switchStatement)
{
	Tab();
	mWriter.Write("SwitchStatement\n");
	Tab();
	mWriter.Write("(control)\n");
	Print(switchStatement->GetControl());
	IncrementTab();
	PrintList(switchStatement->GetSectionList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const SwitchSection *switchSection)
{
	Tab();
	mWriter.Write("SwitchSection\n");
	IncrementTab();
	if (switchSection->GetLabelList() != NULL)
	{
		Tab();
		mWriter.Write("(labels)\n");
		PrintList(switchSection->GetLabelList());
	}
	if (switchSection->GetStatementList() != NULL)
	{
		Tab();
		mWriter.Write("(statements)\n");
		PrintList(switchSection->GetStatementList());
	}
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const SwitchLabel *switchLabel)
{
	Tab();
	mWriter.Write("SwitchLabel: ");
	Print(switchLabel->GetLabel());
	Newline();
	IncrementTab();
	Print(switchLabel->GetExpression());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const WhileStatement *whileStatement)
{
	Tab();
	mWriter.Write("WhileStatement: %s\n", whileStatement->IsDoLoop() ? "do" : "while");
	IncrementTab();
	Tab();
	mWriter.Write("(condition)\n");
	Print(whileStatement->GetCondition());
	Tab();
	mWriter.Write("(body)\n");
	Print(whileStatement->GetBody());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const ForStatement *forStatement)
{
	Tab();
	mWriter.Write("ForStatement\n");
	IncrementTab();
	if (forStatement->GetInitializer() != NULL)
	{
		Tab();
		mWriter.Write("(initializer)\n");
		Print(forStatement->GetInitializer());
	}
	if (forStatement->GetCondition() != NULL)
	{
		Tab();
		mWriter.Write("(condition)\n");
		Print(forStatement->GetCondition());
	}
	if (forStatement->GetCountingExpression() != NULL)
	{
		Tab();
		mWriter.Write("(counting)\n");
		Print(forStatement->GetCountingExpression());
	}
	mWriter.Write("(body)\n");
	Print(forStatement->GetBody());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const JumpStatement *jumpStatement)
{
	Tab();
	mWriter.Write("JumpStatement: ");
	Print(jumpStatement->GetKeyword());
	Newline();
	IncrementTab();
	Print(jumpStatement->GetRhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const DeclarativeStatement *declarativeStatement)
{
	Tab();
	mWriter.Write("DeclarativeStatement\n");
	IncrementTab();
	Print(declarativeStatement->GetTypeDescriptor());
	PrintList(declarativeStatement->GetNamedInitializerList());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const ExpressionStatement *expressionStatement)
{
	Tab();
	mWriter.Write("ExpressionStatement\n");
	IncrementTab();
	Print(expressionStatement->GetExpression());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const ConditionalExpression *conditionalExpression)
{
	Tab();
	mWriter.Write("ConditionalExpression\n");
	IncrementTab();
	Tab();
	mWriter.Write("(condition)\n");
	Print(conditionalExpression->GetCondition());
	Tab();
	mWriter.Write("(true expression)\n");
	Print(conditionalExpression->GetTrueExpression());
	Tab();
	mWriter.Write("(false expression)\n");
	Print(conditionalExpression->GetFalseExpression());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const BinaryExpression *binaryExpression)
{
	Tab();
	mWriter.Write("BinaryExpression: ");
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
	mWriter.Write("UnaryExpression: ");
	Print(unaryExpression->GetOperator());
	Newline();
	IncrementTab();
	Print(unaryExpression->GetRhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const PostfixExpression *postfixExpression)
{
	Tab();
	mWriter.Write("PostfixExpression: ");
	Print(postfixExpression->GetOperator());
	Newline();
	IncrementTab();
	Print(postfixExpression->GetLhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const MemberExpression *memberExpression)
{
	Tab();
	mWriter.Write("MemberExpression: ");
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
	mWriter.Write("ArraySubscriptExpression\n");
	IncrementTab();
	Tab();
	mWriter.Write("(lhs)\n");
	Print(arraySubscriptExpression->GetLhs());
	Tab();
	mWriter.Write("(index)\n");
	Print(arraySubscriptExpression->GetIndex());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const FunctionCallExpression *functionCallExpression)
{
	Tab();
	mWriter.Write("FunctionCallExpression\n");
	IncrementTab();
	Tab();
	mWriter.Write("(function)\n");
	Print(functionCallExpression->GetLhs());
	if (functionCallExpression->GetArgumentList() != NULL)
	{
		Tab();
		mWriter.Write("(arguments)\n");
		PrintList(functionCallExpression->GetArgumentList());
	}
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const CastExpression *castExpression)
{
	Tab();
	mWriter.Write("CastExpression\n");
	IncrementTab();
	Tab();
	mWriter.Write("(type)\n");
	Print(castExpression->GetTypeDescriptor());
	Tab();
	mWriter.Write("(expression)\n");
	Print(castExpression->GetRhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const SizeofExpression *sizeofExpression)
{
	Tab();
	mWriter.Write("SizeofExpression\n");
	IncrementTab();
	Print(sizeofExpression->GetTypeDescriptor());
	Print(sizeofExpression->GetRhs());
	DecrementTab();
}


void ParseTreePrinterCore::Visit(const ConstantExpression *constantExpression)
{
	Tab();
	mWriter.Write("CastExpression: ");
	Print(constantExpression->GetValueToken());
	Newline();
}


void ParseTreePrinterCore::Visit(const ThisExpression *thisExpression)
{
	Tab();
	mWriter.Write("ThisExpression\n");
}


void ParseTreePrinterCore::Visit(const IdentifierExpression *identifierExpression)
{
	Tab();
	mWriter.Write("IdentifierExpression: ");
	Print(identifierExpression->GetIdentifier());
	Newline();
}


void ParseTreePrinterCore::PrintList(const ListParseNode *listNode, const char *separator)
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


void ParseTreePrinterCore::Print(const Token *token)
{
	if (token != NULL)
	{
		mWriter.Write("%s", token->GetText());
	}
}


void ParseTreePrinterCore::Tab()
{
	for (int i = 0; i < mTabLevel; ++i)
	{
		mWriter.Write("\t");
	}
}


void ParseTreePrinterCore::Newline()
{
	mWriter.Write("\n");
}

}
