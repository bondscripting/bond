#include "framework/asserts.h"
#include "framework/testlexerframework.h"
#include "bond/types/simplestring.h"

DEFINE_LEXER_TEST(KeywordAndPunctuationTokens, "scripts/lexer_KeywordAndPunctuationTokens.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const Bond::Token::TokenType EXPECTED_TYPES[] =
	{
		Bond::Token::KEY_BOOL,
		Bond::Token::KEY_BREAK,
		Bond::Token::KEY_CASE,
		Bond::Token::KEY_CAST,
		Bond::Token::KEY_CHAR,
		Bond::Token::KEY_CONST,
		Bond::Token::KEY_CONTINUE,
		Bond::Token::KEY_DEFAULT,
		Bond::Token::KEY_DO,
		Bond::Token::KEY_DOUBLE,
		Bond::Token::KEY_ELSE,
		Bond::Token::KEY_ENUM,
		Bond::Token::KEY_FLOAT,
		Bond::Token::KEY_FOR,
		Bond::Token::KEY_IF,
		Bond::Token::KEY_INT,
		Bond::Token::KEY_LONG,
		Bond::Token::KEY_NAMESPACE,
		Bond::Token::KEY_NATIVE,
		Bond::Token::KEY_RETURN,
		Bond::Token::KEY_SHORT,
		Bond::Token::KEY_SIZEOF,
		Bond::Token::KEY_STRUCT,
		Bond::Token::KEY_SWITCH,
		Bond::Token::KEY_THIS,
		Bond::Token::KEY_UCHAR,
		Bond::Token::KEY_UINT,
		Bond::Token::KEY_ULONG,
		Bond::Token::KEY_USHORT,
		Bond::Token::KEY_VOID,
		Bond::Token::KEY_WHILE,
		Bond::Token::OP_PLUS,
		Bond::Token::OP_MINUS,
		Bond::Token::OP_STAR,
		Bond::Token::OP_DIV,
		Bond::Token::OP_MOD,
		Bond::Token::OP_INC,
		Bond::Token::OP_DEC,
		Bond::Token::OP_LT,
		Bond::Token::OP_LTE,
		Bond::Token::OP_GT,
		Bond::Token::OP_GTE,
		Bond::Token::OP_LEFT,
		Bond::Token::OP_RIGHT,
		Bond::Token::OP_EQUAL,
		Bond::Token::OP_NOT_EQUAL,
		Bond::Token::OP_NOT,
		Bond::Token::OP_AND,
		Bond::Token::OP_OR,
		Bond::Token::OP_AMP,
		Bond::Token::OP_BIT_OR,
		Bond::Token::OP_BIT_XOR,
		Bond::Token::OP_BIT_NOT,
		Bond::Token::OP_TERNARY,
		Bond::Token::OP_ARROW,
		Bond::Token::ASSIGN,
		Bond::Token::ASSIGN_RIGHT,
		Bond::Token::ASSIGN_LEFT,
		Bond::Token::ASSIGN_PLUS,
		Bond::Token::ASSIGN_MINUS,
		Bond::Token::ASSIGN_MULT,
		Bond::Token::ASSIGN_DIV,
		Bond::Token::ASSIGN_MOD,
		Bond::Token::ASSIGN_AND,
		Bond::Token::ASSIGN_OR,
		Bond::Token::ASSIGN_XOR,
		Bond::Token::OPAREN,
		Bond::Token::CPAREN,
		Bond::Token::OBRACE,
		Bond::Token::CBRACE,
		Bond::Token::OBRACKET,
		Bond::Token::CBRACKET,
		Bond::Token::PERIOD,
		Bond::Token::SEMICOLON,
		Bond::Token::SCOPE,
		Bond::Token::COLON,
		Bond::Token::COMMA,
	};

	const int NUM_TOKENS = sizeof(EXPECTED_TYPES) / sizeof(*EXPECTED_TYPES);

	Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();

	for (int i = 0; i < NUM_TOKENS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(EXPECTED_TYPES[i] == token->GetTokenType(),
			("Expected %s, but was %s.", Bond::Token::GetTokenName(EXPECTED_TYPES[i]), token->GetTokenName()));
	}

	ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");

	return true;
}


DEFINE_LEXER_TEST(LiteralTokens, "scripts/lexer_LiteralTokens.bond")
{
	ASSERT_NO_COMPILER_ERRORS(errorBuffer);

	const bool EXPECTED_BOOLS[] = { true, false };
	const int NUM_BOOLS = sizeof(EXPECTED_BOOLS) / sizeof(*EXPECTED_BOOLS);

	const int32_t EXPECTED_INTS[] = { 98765, 07777, 0x7fffffff, int32_t(0xffffffff) };
	const int NUM_INTS = sizeof(EXPECTED_INTS) / sizeof(*EXPECTED_INTS);

	const uint32_t EXPECTED_UINTS[] = { 98765u, 07777u, 0x7fffffffu, 0xffffffffu };
	const int NUM_UINTS = sizeof(EXPECTED_UINTS) / sizeof(*EXPECTED_UINTS);

	const int64_t EXPECTED_LONGS[] = { 98765, 07777, 0x7fffffff, 0xffffffff, Bond::BOND_LONG_MAX };
	const int NUM_LONGS = sizeof(EXPECTED_LONGS) / sizeof(*EXPECTED_LONGS);

	const uint64_t EXPECTED_ULONGS[] = { 98765u, 07777u, 0x7fffffffu, 0xffffffffu, Bond::BOND_ULONG_MAX };
	const int NUM_ULONGS = sizeof(EXPECTED_ULONGS) / sizeof(*EXPECTED_ULONGS);

	const float EXPECTED_FLOATS[] = { 3.0f, 3.0f, 3.1f, 15.75f, 1.575E1f, 1575e-2f, 2.5e-3f, 25E-4f, .0075e+2f };
	const int NUM_FLOATS = sizeof(EXPECTED_FLOATS) / sizeof(*EXPECTED_FLOATS);

	const double EXPECTED_DOUBLES[] = { 15.75, 1.575E1, 1575e-2, 2.5e-3, 25E-4, .0075e+2 };
	const int NUM_DOUBLES = sizeof(EXPECTED_DOUBLES) / sizeof(*EXPECTED_DOUBLES);

	const char EXPECTED_CHARS[] = { 'z', '\t', '\'', '"', '\"' };
	const int NUM_CHARS = sizeof(EXPECTED_CHARS) / sizeof(*EXPECTED_CHARS);

	const Bond::SimpleString EXPECTED_STRINGS[] = {
		Bond::SimpleString("string"),
		Bond::SimpleString("\""),
		Bond::SimpleString("\'"),
		Bond::SimpleString("\\"),
		Bond::SimpleString("\x7e \0 \176", 5)
	};
	const int NUM_STRINGS = sizeof(EXPECTED_STRINGS) / sizeof(*EXPECTED_STRINGS);

	Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();

	for (int i = 0; i < NUM_BOOLS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_BOOL == token->GetTokenType(),
			("Expected %s, but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_BOOL), token->GetTokenName()));

		const bool expected = EXPECTED_BOOLS[i];
		const bool actual = token->GetBoolValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", expected, actual));
	}

	const Bond::Token *nullToken = stream.Next();
	ASSERT_FORMAT(Bond::Token::CONST_NULL == nullToken->GetTokenType(),
		("Expected %s, but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_NULL), nullToken->GetTokenName()));

	for (int i = 0; i < NUM_INTS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_INT == token->GetTokenType(),
			("Expected %s, but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_INT), token->GetTokenName()));

		const int32_t expected = EXPECTED_INTS[i];
		const int32_t actual = token->GetIntValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", expected, actual));
	}

	for (int i = 0; i < NUM_UINTS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_UINT == token->GetTokenType(),
			("Expected %s, but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_UINT), token->GetTokenName()));

		const uint32_t expected = EXPECTED_UINTS[i];
		const uint32_t actual = token->GetUIntValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRIu32 ", but was %" BOND_PRIu32 ".", expected, actual));
	}

	for (int i = 0; i < NUM_LONGS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_LONG == token->GetTokenType(),
			("Expected %s, but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_LONG), token->GetTokenName()));

		const int64_t expected = EXPECTED_LONGS[i];
		const int64_t actual = token->GetLongValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRId64 ", but was %" BOND_PRId64 ".", expected, actual));
	}

	for (int i = 0; i < NUM_ULONGS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_ULONG == token->GetTokenType(),
			("Expected %s, but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_ULONG), token->GetTokenName()));

		const uint64_t expected = EXPECTED_ULONGS[i];
		const uint64_t actual = token->GetULongValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRIu64 ", but was %" BOND_PRIu64 ".", expected, actual));
	}

	for (int i = 0; i < NUM_FLOATS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_FLOAT == token->GetTokenType(),
			("Expected %s, but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_FLOAT), token->GetTokenName()));

		const float expected = EXPECTED_FLOATS[i];
		const float actual = token->GetFloatValue();
		ASSERT_FORMAT((actual >= (expected - 0.0000001f)) && (actual <= (expected + 0.0000001f)),
			("Expected %f, but was %f.", expected, actual));
	}

	for (int i = 0; i < NUM_DOUBLES; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_DOUBLE == token->GetTokenType(),
			("Expected %s, but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_DOUBLE), token->GetTokenName()));

		const double expected = EXPECTED_DOUBLES[i];
		const double actual = token->GetDoubleValue();
		ASSERT_FORMAT((expected >= (actual - 0.0000001)) && (expected <= (actual + 0.0000001)),
			("Expected %.16f, but was %.16f.", expected, actual));
	}

	for (int i = 0; i < NUM_CHARS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_CHAR == token->GetTokenType(),
			("Expected %s, but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_CHAR), token->GetTokenName()));

		const int32_t expected = EXPECTED_CHARS[i];
		const int32_t actual = token->GetIntValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRId32 ", but was %" BOND_PRId32 ".", expected, actual));
	}

	for (int i = 0; i < NUM_STRINGS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_STRING == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_STRING), token->GetTokenName()));

		const Bond::SimpleString &expected = EXPECTED_STRINGS[i];
		const Bond::SimpleString actual(token->GetStringValue(), token->GetStringLength());
		ASSERT_FORMAT(expected == actual,
			("Expected \"%s\", but was \"%s\".", expected.GetString(), actual.GetString()));
	}

	ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");

	return true;
}


DEFINE_LEXER_TEST(InvalidTokens, "scripts/lexer_InvalidTokens.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::INVALID_INT, Bond::Token::INVALID, 1},
		{Bond::CompilerError::INVALID_OCTAL_INT, Bond::Token::INVALID, 1},
		{Bond::CompilerError::INVALID_HEX_INT, Bond::Token::INVALID, 1},
		{Bond::CompilerError::INVALID_FLOAT, Bond::Token::INVALID, 2},
		{Bond::CompilerError::INVALID_FLOAT, Bond::Token::INVALID, 2},
		{Bond::CompilerError::INVALID_FLOAT, Bond::Token::INVALID, 2},
		{Bond::CompilerError::INVALID_FLOAT, Bond::Token::INVALID, 2},
		{Bond::CompilerError::INVALID_FLOAT, Bond::Token::INVALID, 2},
		{Bond::CompilerError::EMPTY_CHARACTER_CONSTANT, Bond::Token::INVALID, 3},
		{Bond::CompilerError::MULTI_CHARACTER_CONSTANT, Bond::Token::INVALID, 3},
		{Bond::CompilerError::INVALID_ESCAPE, Bond::Token::INVALID, 3},
		{Bond::CompilerError::MULTI_CHARACTER_CONSTANT, Bond::Token::INVALID, 3},
		{Bond::CompilerError::INVALID_ESCAPE, Bond::Token::INVALID, 3},
		{Bond::CompilerError::MISSING_HEX_ESCAPE, Bond::Token::INVALID, 4},
		{Bond::CompilerError::MISSING_HEX_ESCAPE, Bond::Token::INVALID, 4},
		{Bond::CompilerError::OCTAL_ESCAPE_RANGE, Bond::Token::INVALID, 5},
		{Bond::CompilerError::HEX_ESCAPE_RANGE, Bond::Token::INVALID, 5},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_LEXER_TEST(UnterminatedCharacter, "scripts/lexer_UnterminatedCharacter.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::UNTERMINATED_CHARACTER, Bond::Token::INVALID, 1},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_LEXER_TEST(UnterminatedString, "scripts/lexer_UnterminatedString.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::UNTERMINATED_STRING, Bond::Token::INVALID, 1},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_LEXER_TEST(UnterminatedComment, "scripts/lexer_UnterminatedComment.bond")
{
	const TestFramework::ExpectedCompilerError EXPECTED_ERRORS[] =
	{
		{Bond::CompilerError::UNTERMINATED_COMMENT, Bond::Token::INVALID, 3},
	};

	const int NUM_ERRORS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	ASSERT_COMPILER_ERRORS(errorBuffer, EXPECTED_ERRORS, NUM_ERRORS);

	return true;
}


DEFINE_LEXER_TEST(EndOfStream1, "scripts/lexer_EndOfStream1.bond")
{
	Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();

	ASSERT_FORMAT(stream.GetLength() == 2, ("Expected 2 tokens but found %d.", stream.GetLength()));

	stream.SetPosition(1);
	const Bond::Token *token = stream.Next();
	Bond::StreamPos pos = token->GetStartPos();
	ASSERT_FORMAT((pos.line == 3) && (pos.column == 2),
		("Expected EOF at line 3 column 2 but was at line %d column %d.", pos.line, pos.column));

	return true;
}


DEFINE_LEXER_TEST(EndOfStream2, "scripts/lexer_EndOfStream2.bond")
{
	Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();

	ASSERT_FORMAT(stream.GetLength() == 1, ("Expected 1 token but found %d.", stream.GetLength()));

	const Bond::Token *token = stream.Next();
	Bond::StreamPos pos = token->GetStartPos();
	ASSERT_FORMAT((pos.line == 1) && (pos.column == 2),
		("Expected EOF at line 1 column 2 but was at line %d column %d.", pos.line, pos.column));

	return true;
}


#define TEST_ITEMS                       \
  TEST_ITEM(KeywordAndPunctuationTokens) \
  TEST_ITEM(LiteralTokens)               \
  TEST_ITEM(InvalidTokens)               \
  TEST_ITEM(UnterminatedCharacter)       \
  TEST_ITEM(UnterminatedString)          \
  TEST_ITEM(UnterminatedComment)         \
  TEST_ITEM(EndOfStream1)                \
  TEST_ITEM(EndOfStream2)                \


RUN_TESTS(Lexer, TEST_ITEMS)
