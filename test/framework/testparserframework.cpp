#include "framework/testparserframework.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/lexer.h"

namespace TestFramework
{

static bool RunParserTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::FileData &script,
	ParserValidationFunction *validationFunction);


bool RunParserTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const char *scriptName,
	ParserValidationFunction *validationFunction)
{
	__ASSERT_FORMAT__(scriptName != 0, logger, assertFile, assertLine, ("Script name is NULL."));
	__ASSERT_FORMAT__(validationFunction != 0, logger, assertFile, assertLine, ("Validation function is NULL."));

	Bond::DefaultAllocator allocator;
	Bond::DefaultFileLoader fileLoader(allocator);
	Bond::FileData script = fileLoader.LoadFile(scriptName);
	__ASSERT_FORMAT__(script.mValid, logger, assertFile, assertLine,
		("Failed to load file '%s'.", scriptName));

	const bool result = RunParserTest(logger, assertFile, assertLine, script, validationFunction);

	fileLoader.DisposeFile(script);

	return result;
}


static bool RunParserTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::FileData &script,
	ParserValidationFunction *validationFunction)
{
	bool result = true;

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	{
		Bond::CompilerErrorBuffer errorBuffer;
		Bond::Lexer lexer(lexerAllocator, errorBuffer);
		lexer.Lex(reinterpret_cast<const char *>(script.mData), script.mLength);
		Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();
		Bond::Parser parser(parserAllocator, errorBuffer);
		if (!errorBuffer.HasErrors())
		{
			parser.Parse(stream);
		}
		result = validationFunction(logger, errorBuffer, parser);
	}

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.\n", lexerAllocator.GetNumAllocations()));
	__ASSERT_FORMAT__(parserAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Parser leaked %d chunks of memory.\n", parserAllocator.GetNumAllocations()));

	return result;
}


bool AssertParseNodeCount(
	Bond::TextWriter &logger,
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
	ASSERT_COUNT(SizeofExpression);
	ASSERT_COUNT(ConstantExpression);
	ASSERT_COUNT(IdentifierExpression);

#undef ASSERT_COUNT

	return true;
}

}
