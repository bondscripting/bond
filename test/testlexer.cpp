#include "framework/testlexerframework.h"

DEFINE_LEXER_TEST(KeywordAndPunctuationTokens, "scripts/lexer_KeywordAndPunctuationTokens.bond")
{
	const Bond::Token::TokenType EXPECTED_TYPES[] =
	{
		Bond::Token::KEY_BOOL,
		Bond::Token::KEY_BREAK,
		Bond::Token::KEY_CHAR,
		Bond::Token::KEY_CASE,
		Bond::Token::KEY_CAST,
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
		Bond::Token::KEY_REF,
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
			("Expected %s but was %s.", Bond::Token::GetTokenName(EXPECTED_TYPES[i]), token->GetTokenName()));
	}

	ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");

	return true;
}


DEFINE_LEXER_TEST(LiteralTokens, "scripts/lexer_LiteralTokens.bond")
{
	const bool EXPECTED_BOOLS[] = { true, false };
	const int NUM_BOOLS = sizeof(EXPECTED_BOOLS) / sizeof(*EXPECTED_BOOLS);

	const Bond::bi32_t EXPECTED_INTS[] = { 98765, 07777, 0x7fffffff, 0xffffffff };
	const int NUM_INTS = sizeof(EXPECTED_INTS) / sizeof(*EXPECTED_INTS);

	const Bond::bu32_t EXPECTED_UINTS[] = { 98765u, 07777u, 0x7fffffffu, 0xffffffffu };
	const int NUM_UINTS = sizeof(EXPECTED_UINTS) / sizeof(*EXPECTED_UINTS);

	const Bond::bi64_t EXPECTED_LONGS[] = { 98765, 07777, 0x7fffffff, 0xffffffff, Bond::BOND_LONG_MAX };
	const int NUM_LONGS = sizeof(EXPECTED_LONGS) / sizeof(*EXPECTED_LONGS);

	const Bond::bu64_t EXPECTED_ULONGS[] = { 98765u, 07777u, 0x7fffffffu, 0xffffffffu, Bond::BOND_ULONG_MAX };
	const int NUM_ULONGS = sizeof(EXPECTED_ULONGS) / sizeof(*EXPECTED_ULONGS);

	const Bond::bf32_t EXPECTED_FLOATS[] = { 3.0f, 3.0f, 3.1f, 15.75f, 1.575E1f, 1575e-2f, 2.5e-3f, 25E-4f, .0075e+2f };
	const int NUM_FLOATS = sizeof(EXPECTED_FLOATS) / sizeof(*EXPECTED_FLOATS);

	const Bond::bf64_t EXPECTED_DOUBLES[] = { 15.75, 1.575E1, 1575e-2, 2.5e-3, 25E-4, .0075e+2 };
	const int NUM_DOUBLES = sizeof(EXPECTED_DOUBLES) / sizeof(*EXPECTED_DOUBLES);

	const char EXPECTED_CHARS[] = { 'z', '\t', '\'', '"', '\"' };
	const int NUM_CHARS = sizeof(EXPECTED_CHARS) / sizeof(*EXPECTED_CHARS);

	Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();

	for (int i = 0; i < NUM_BOOLS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_BOOL == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_BOOL), token->GetTokenName()));

		const bool expected = EXPECTED_BOOLS[i];
		const bool actual = token->GetBoolValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRId32 " but was %" BOND_PRId32 ".", expected, actual));
	}

	const Bond::Token *nullToken = stream.Next();
	ASSERT_FORMAT(Bond::Token::CONST_NULL == nullToken->GetTokenType(),
		("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_NULL), nullToken->GetTokenName()));

	for (int i = 0; i < NUM_INTS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_INT == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_INT), token->GetTokenName()));

		const Bond::bi32_t expected = EXPECTED_INTS[i];
		const Bond::bi32_t actual = token->GetIntValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRId32 " but was %" BOND_PRId32 ".", expected, actual));
	}

	for (int i = 0; i < NUM_UINTS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_UINT == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_UINT), token->GetTokenName()));

		const Bond::bu32_t expected = EXPECTED_UINTS[i];
		const Bond::bu32_t actual = token->GetUIntValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRIu32 " but was %" BOND_PRIu32 ".", expected, actual));
	}

	for (int i = 0; i < NUM_LONGS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_LONG == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_LONG), token->GetTokenName()));

		const Bond::bi64_t expected = EXPECTED_LONGS[i];
		const Bond::bi64_t actual = token->GetLongValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRId64 " but was %" BOND_PRId64 ".", expected, actual));
	}

	for (int i = 0; i < NUM_ULONGS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_ULONG == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_ULONG), token->GetTokenName()));

		const Bond::bu64_t expected = EXPECTED_ULONGS[i];
		const Bond::bu64_t actual = token->GetULongValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRIu64 " but was %" BOND_PRIu64 ".", expected, actual));
	}

	for (int i = 0; i < NUM_FLOATS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_FLOAT == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_FLOAT), token->GetTokenName()));

		const Bond::bf32_t expected = EXPECTED_FLOATS[i];
		const Bond::bf32_t actual = token->GetFloatValue();
		ASSERT_FORMAT((expected >= (actual - 0.0000001f)) && (expected <= (actual + 0.0000001f)),
			("Expected %" BOND_PRIf32 " but was %" BOND_PRIf32 ".", expected, actual));
	}

	for (int i = 0; i < NUM_DOUBLES; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_DOUBLE == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_DOUBLE), token->GetTokenName()));

		const Bond::bf64_t expected = EXPECTED_DOUBLES[i];
		const Bond::bf64_t actual = token->GetDoubleValue();
		ASSERT_FORMAT((expected >= (actual - 0.0000001)) && (expected <= (actual + 0.0000001)),
			("Expected %.16" BOND_PRIf64 " but was %.16" BOND_PRIf64 ".", expected, actual));
	}

	for (int i = 0; i < NUM_CHARS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_CHAR == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_CHAR), token->GetTokenName()));

		const Bond::bi32_t expected = EXPECTED_CHARS[i];
		const Bond::bi32_t actual = token->GetIntValue();
		ASSERT_FORMAT(expected == actual,
			("Expected %" BOND_PRId32 " but was %" BOND_PRId32 ".", expected, actual));
	}

	ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");

	return true;
}


DEFINE_LEXER_TEST(InvalidTokens, "scripts/lexer_InvalidTokens.bond")
{
	const Bond::Token::ErrorType EXPECTED_ERRORS[] =
	{
		Bond::Token::INVALID_INT,
		Bond::Token::INVALID_OCTAL_INT,
		Bond::Token::INVALID_HEX_INT,
		Bond::Token::INVALID_FLOAT,
		Bond::Token::INVALID_FLOAT,
		Bond::Token::INVALID_FLOAT,
		Bond::Token::INVALID_FLOAT,
		Bond::Token::INVALID_FLOAT,
		Bond::Token::EMPTY_CHARACTER_CONSTANT,
		Bond::Token::MULTI_CHARACTER_CONSTANT,
		Bond::Token::INVALID_ESCAPE,
		Bond::Token::MULTI_CHARACTER_CONSTANT,
		Bond::Token::INVALID_ESCAPE,
	};

	const int NUM_TOKENS = sizeof(EXPECTED_ERRORS) / sizeof(*EXPECTED_ERRORS);

	Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();

	for (int i = 0; i < NUM_TOKENS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::INVALID == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::INVALID), token->GetTokenName()));
		ASSERT_FORMAT(EXPECTED_ERRORS[i] == token->GetErrorType(),
			("Expected %s but was %s.", Bond::Token::GetErrorName(EXPECTED_ERRORS[i]), token->GetErrorName()));
	}

	ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");

	return true;
}


DEFINE_LEXER_TEST(UnterminatedCharacter, "scripts/lexer_UnterminatedCharacter.bond")
{
	Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();

	const Bond::Token *token = stream.Next();
	ASSERT_FORMAT(Bond::Token::INVALID == token->GetTokenType(),
		("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::INVALID), token->GetTokenName()));
	ASSERT_FORMAT(Bond::Token::UNTERMINATED_CHARACTER == token->GetErrorType(),
		("Expected %s but was %s.", Bond::Token::GetErrorName(Bond::Token::UNTERMINATED_CHARACTER), token->GetErrorName()));

	ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");

	return true;
}


DEFINE_LEXER_TEST(UnterminatedString, "scripts/lexer_UnterminatedString.bond")
{
	Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();

	const Bond::Token *token = stream.Next();
	ASSERT_FORMAT(Bond::Token::INVALID == token->GetTokenType(),
		("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::INVALID), token->GetTokenName()));
	ASSERT_FORMAT(Bond::Token::UNTERMINATED_STRING == token->GetErrorType(),
		("Expected %s but was %s.", Bond::Token::GetErrorName(Bond::Token::UNTERMINATED_STRING), token->GetErrorName()));

	ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");

	return true;
}


DEFINE_LEXER_TEST(UnterminatedComment, "scripts/lexer_UnterminatedComment.bond")
{
	Bond::TokenStream stream = lexer.GetTokenCollectionList()->GetTokenStream();

	const Bond::Token *token = stream.Next();
	ASSERT_FORMAT(Bond::Token::INVALID == token->GetTokenType(),
		("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::INVALID), token->GetTokenName()));
	ASSERT_FORMAT(Bond::Token::UNTERMINATED_COMMENT == token->GetErrorType(),
		("Expected %s but was %s.", Bond::Token::GetErrorName(Bond::Token::UNTERMINATED_COMMENT), token->GetErrorName()));

	ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");

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


// TODO: Test identifiers
// TODO: Test valid string literals
// TODO: Test valid comments

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
