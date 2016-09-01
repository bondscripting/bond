#include "framework/testparserframework.h"
#include "bond/compiler/lexer.h"
#include "bond/compiler/parser.h"
#include "bond/io/stdioinputstream.h"
#include "bond/systems/defaultallocator.h"
#include "bond/systems/exception.h"

namespace TestFramework
{

bool RunParserTest(
	Bond::OutputStream &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	ParserValidationFunction *validationFunction)
{
	__ASSERT_FORMAT__(scriptName != nullptr, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != nullptr, logger, assertFile, assertLine, ("Validation function is NULL."));

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	bool result = false;

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
		result = validationFunction(logger, errorBuffer, translationUnit);
	}
	catch (const Bond::Exception &e)
	{
		logger.Print("line %u in %s: %s\n", assertLine, assertFile, e.GetMessage());
	}

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.", lexerAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.", parserAllocator.GetNumAllocations()));

	return result;
}


bool AssertParseNodeCount(
	Bond::OutputStream &logger,
	const char *assertFile,
	int assertLine,
	const Bond::ListParseNode *root,
	const Bond::ParseNodeCount &expected)
{
	Bond::ParseNodeCounter counter;
	counter.CountList(root);
	const Bond::ParseNodeCount &actual = counter.GetCount();

#define ASSERT_COUNT(field)                                                    \
  __ASSERT_FORMAT__(                                                           \
    (expected.m ## field == -1) || (expected.m ## field == actual.m ## field), \
    logger,                                                                    \
    assertFile,                                                                \
    assertLine,                                                                \
    ("Expected " #field " count to be %d, but was %d.",                        \
    expected.m ## field,                                                       \
    actual.m ## field));                                                       \

	ASSERT_COUNT(TranslationUnit);
	ASSERT_COUNT(NamespaceDefinition);
	ASSERT_COUNT(EnumDeclaration);
	ASSERT_COUNT(Enumerator);
	ASSERT_COUNT(StructDeclaration);
	ASSERT_COUNT(FunctionDefinition);
	ASSERT_COUNT(FunctionPrototype);
	ASSERT_COUNT(Parameter);
	ASSERT_COUNT(TypeDescriptor);
	ASSERT_COUNT(TypeSpecifier);
	ASSERT_COUNT(NamedInitializer);
	ASSERT_COUNT(Initializer);
	ASSERT_COUNT(QualifiedIdentifier);
	ASSERT_COUNT(CompoundStatement);
	ASSERT_COUNT(IfStatement);
	ASSERT_COUNT(SwitchStatement);
	ASSERT_COUNT(SwitchSection);
	ASSERT_COUNT(SwitchLabel);
	ASSERT_COUNT(WhileStatement);
	ASSERT_COUNT(ForStatement);
	ASSERT_COUNT(JumpStatement);
	ASSERT_COUNT(DeclarativeStatement);
	ASSERT_COUNT(ExpressionStatement);
	ASSERT_COUNT(ConditionalExpression);
	ASSERT_COUNT(BinaryExpression);
	ASSERT_COUNT(UnaryExpression);
	ASSERT_COUNT(PostfixExpression);
	ASSERT_COUNT(MemberExpression);
	ASSERT_COUNT(ArraySubscriptExpression);
	ASSERT_COUNT(FunctionCallExpression);
	ASSERT_COUNT(CastExpression);
	ASSERT_COUNT(PropertyofExpression);
	ASSERT_COUNT(ConstantLiteralExpression);
	ASSERT_COUNT(IdentifierExpression);

#undef ASSERT_COUNT

	return true;
}

}
