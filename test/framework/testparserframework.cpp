#include "framework/testparserframework.h"
#include "framework/asserts.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/io/stdioinputstream.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"

namespace TestFramework
{

void RunParserTest(
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	ParserValidationFunction *validationFunction)
{
	Assert(scriptName != nullptr, assertFile, assertLine, "Script name is NULL.");
	Assert(validationFunction != nullptr, assertFile, assertLine, "Validation function is NULL.");

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	AllocationLeakGuard<Bond::DefaultAllocator> lexerLeakGuard(lexerAllocator, "Lexer", assertFile, assertLine);
	AllocationLeakGuard<Bond::DefaultAllocator> parserLeakGuard(parserAllocator, "Parser", assertFile, assertLine);

	try
	{
		Bond::StdioInputStream scriptStream(scriptName);
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		auto tokenCollectionHandle = lexer.Lex(scriptName, scriptStream);
		Bond::ParseNodeStore parseNodeStore((Bond::ParseNodeStore::allocator_type(&parserAllocator)));
		Bond::Parser parser(parserAllocator, errorBuffer, parseNodeStore);
		Bond::TranslationUnit *translationUnit = nullptr;
		if (!errorBuffer.HasErrors())
		{
			Bond::TokenStream stream = tokenCollectionHandle->GetTokenStream();
			translationUnit = parser.Parse(stream);
		}
		validationFunction(errorBuffer, translationUnit);
	}
	catch (const Bond::Exception &e)
	{
		throw AssertionFailure(assertFile, assertLine, "%s", e.GetMessage());
	}
}


void AssertParseNodeCount(
	const char *assertFile,
	int assertLine,
	const Bond::ListParseNode *parseNode,
	const Bond::ParseNodeCount &expected)
{
	Bond::ParseNodeCounter counter;
	counter.CountList(parseNode);
	const Bond::ParseNodeCount &actual = counter.GetCount();

	struct CountField
	{
		const char *name;
		int Bond::ParseNodeCount::*member;
	};

	static const CountField FIELDS[] =
	{
		{"TranslationUnit", &Bond::ParseNodeCount::mTranslationUnit},
		{"IncludeDirective", &Bond::ParseNodeCount::mIncludeDirective},
		{"NamespaceDefinition", &Bond::ParseNodeCount::mNamespaceDefinition},
		{"NativeBlock", &Bond::ParseNodeCount::mNativeBlock},
		{"EnumDeclaration", &Bond::ParseNodeCount::mEnumDeclaration},
		{"Enumerator", &Bond::ParseNodeCount::mEnumerator},
		{"StructDeclaration", &Bond::ParseNodeCount::mStructDeclaration},
		{"FunctionDefinition", &Bond::ParseNodeCount::mFunctionDefinition},
		{"FunctionPrototype", &Bond::ParseNodeCount::mFunctionPrototype},
		{"Parameter", &Bond::ParseNodeCount::mParameter},
		{"TypeDescriptor", &Bond::ParseNodeCount::mTypeDescriptor},
		{"TypeSpecifier", &Bond::ParseNodeCount::mTypeSpecifier},
		{"NamedInitializer", &Bond::ParseNodeCount::mNamedInitializer},
		{"Initializer", &Bond::ParseNodeCount::mInitializer},
		{"QualifiedIdentifier", &Bond::ParseNodeCount::mQualifiedIdentifier},
		{"CompoundStatement", &Bond::ParseNodeCount::mCompoundStatement},
		{"IfStatement", &Bond::ParseNodeCount::mIfStatement},
		{"SwitchStatement", &Bond::ParseNodeCount::mSwitchStatement},
		{"SwitchSection", &Bond::ParseNodeCount::mSwitchSection},
		{"SwitchLabel", &Bond::ParseNodeCount::mSwitchLabel},
		{"WhileStatement", &Bond::ParseNodeCount::mWhileStatement},
		{"ForStatement", &Bond::ParseNodeCount::mForStatement},
		{"JumpStatement", &Bond::ParseNodeCount::mJumpStatement},
		{"UsingDeclaration", &Bond::ParseNodeCount::mUsingDeclaration},
		{"UsingDirective", &Bond::ParseNodeCount::mUsingDirective},
		{"DeclarativeStatement", &Bond::ParseNodeCount::mDeclarativeStatement},
		{"ExpressionStatement", &Bond::ParseNodeCount::mExpressionStatement},
		{"ConditionalExpression", &Bond::ParseNodeCount::mConditionalExpression},
		{"BinaryExpression", &Bond::ParseNodeCount::mBinaryExpression},
		{"UnaryExpression", &Bond::ParseNodeCount::mUnaryExpression},
		{"PostfixExpression", &Bond::ParseNodeCount::mPostfixExpression},
		{"MemberExpression", &Bond::ParseNodeCount::mMemberExpression},
		{"ArraySubscriptExpression", &Bond::ParseNodeCount::mArraySubscriptExpression},
		{"FunctionCallExpression", &Bond::ParseNodeCount::mFunctionCallExpression},
		{"CastExpression", &Bond::ParseNodeCount::mCastExpression},
		{"PropertyofExpression", &Bond::ParseNodeCount::mPropertyofExpression},
		{"ConstantLiteralExpression", &Bond::ParseNodeCount::mConstantLiteralExpression},
		{"IdentifierExpression", &Bond::ParseNodeCount::mIdentifierExpression},
		{"ThisExpression", &Bond::ParseNodeCount::mThisExpression},
		{"EmptyExpression", &Bond::ParseNodeCount::mEmptyExpression},
	};

	for (const CountField &field : FIELDS)
	{
		const int expectedCount = expected.*field.member;
		const int actualCount = actual.*field.member;
		Assert((expectedCount == -1) || (expectedCount == actualCount), assertFile, assertLine,
			"Expected %s count to be %d, but was %d.", field.name, expectedCount, actualCount);
	}
}

}
