#include "framework/testparserframework.h"
#include "framework/util.h"
#include "bond/defaultallocator.h"
#include "bond/lexer.h"
#include "bond/textwriter.h"

namespace TestFramework
{

static bool RunParserTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const FileData &script,
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

	FileData script = ReadFile(scriptName);
	__ASSERT_FORMAT__(script.length >= 0, logger, assertFile, assertLine,
		("Failed to load file '%s'.", scriptName));

	// Delegate to another function so we can still clean up even if something bails during the test.
	const bool result = RunParserTest(logger, assertFile, assertLine, script, validationFunction);

	DisposeFile(script);

	return result;
}


static bool RunParserTest(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const FileData &script,
	ParserValidationFunction *validationFunction)
{
	bool result = true;

	Bond::DefaultAllocator lexerAllocator;
	Bond::DefaultAllocator parserAllocator;
	{
		Bond::Lexer lexer(lexerAllocator);
		lexer.Lex(script.data, script.length);
		Bond::TokenStream stream = lexer.GetTokenStream();
		Bond::Parser parser(parserAllocator);
		parser.Parse(stream);
		result = validationFunction(logger, parser);
	}

	__ASSERT_FORMAT__(lexerAllocator.GetNumAllocations() == 0, logger, assertFile, assertLine,
		("Lexer leaked %d chunks of memory.\n", parserAllocator.GetNumAllocations()));
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


bool AssertNoParseErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::ParseErrorBuffer &errorBuffer)
{
	if (errorBuffer.HasErrors())
	{
		logger.Write("line %u in %s:\n", assertLine, assertFile);

		for (int i = 0; i < errorBuffer.GetNumErrors(); ++i)
		{
			logger.Write("\t\t");
			const Bond::ParseError *error = errorBuffer.GetError(i);
			error->Print(logger);
			logger.Write("\n");
		}
		return false;
	}

	return true;
}


bool AssertParseErrors(
	Bond::TextWriter &logger,
	const char *assertFile,
	int assertLine,
	const Bond::ParseErrorBuffer &errorBuffer,
	const ExpectedParseError *expectedErrors,
	int numErrors)
{
	const int count = (errorBuffer.GetNumErrors() < numErrors) ? errorBuffer.GetNumErrors() : numErrors;

	for (int i = 0; i < count; ++i)
	{
		const ExpectedParseError *expected = expectedErrors + i;
		const Bond::ParseError *actual = errorBuffer.GetError(i);
		const Bond::Token *context = actual->GetContext();
		const Bond::StreamPos &pos = context->GetStartPos();
		__ASSERT_FORMAT__(expected->errorType == actual->GetType(), logger, assertFile, assertLine,
			("Expected type of error %d to be %s but was %s.", i,
			Bond::ParseError::GetErrorName(expected->errorType),
			actual->GetErrorName()));
		__ASSERT_FORMAT__(expected->context == context->GetTokenType(), logger, assertFile, assertLine,
			("Expected context of error %d to be %s but was %s.", i,
			Bond::Token::GetTokenName(expected->context), context->GetTokenName()));
		__ASSERT_FORMAT__(expected->line == pos.line, logger, assertFile, assertLine,
			("Expected error %d to be on line %d but was on %d.", i, expected->line, pos.line));
	}

	__ASSERT_FORMAT__(errorBuffer.GetNumErrors() == numErrors, logger, assertFile, assertLine,
		("Expected %d errors but found %d.", numErrors, errorBuffer.GetNumErrors()));

	return true;
}

}
