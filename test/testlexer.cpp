#include "framework/testlexerframework.h"
#include "bond/defaultallocator.h"

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
		Bond::Token::KEY_ELSE,
		Bond::Token::KEY_ENUM,
		Bond::Token::KEY_FLOAT,
		Bond::Token::KEY_FOR,
		Bond::Token::KEY_IF,
		Bond::Token::KEY_INT,
		Bond::Token::KEY_NAMESPACE,
		Bond::Token::KEY_RETURN,
		Bond::Token::KEY_SIZEOF,
		Bond::Token::KEY_STRUCT,
		Bond::Token::KEY_SWITCH,
		Bond::Token::KEY_UINT,
		Bond::Token::KEY_VOID,
		Bond::Token::KEY_WHILE,
		Bond::Token::OP_PLUS,
		Bond::Token::OP_MINUS,
		Bond::Token::OP_MULT,
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
		Bond::Token::OP_BIT_AND,
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

	Bond::TokenStream stream = lexer.GetTokenStream();

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

	const Bond::int_t EXPECTED_INTS[] = { 98765, 07777, 0x7fffffff, 0xffffffff };
	const int NUM_INTS = sizeof(EXPECTED_INTS) / sizeof(*EXPECTED_INTS);

	const Bond::uint_t EXPECTED_UINTS[] = { 98765u, 07777u, 0x7fffffffu, 0xffffffffu };
	const int NUM_UINTS = sizeof(EXPECTED_UINTS) / sizeof(*EXPECTED_UINTS);

	const Bond::float_t EXPECTED_FLOATS[] = { 15.75f, 1.575E1f, 1575e-2f, 2.5e-3f, 25E-4f, .0075e+2f };
	const int NUM_FLOATS = sizeof(EXPECTED_FLOATS) / sizeof(*EXPECTED_FLOATS);

	const char EXPECTED_CHARS[] = { 'z', '\t', '\'', '"', '\"' };
	const int NUM_CHARS = sizeof(EXPECTED_CHARS) / sizeof(*EXPECTED_CHARS);

	Bond::TokenStream stream = lexer.GetTokenStream();

	for (int i = 0; i < NUM_BOOLS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_BOOL == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_BOOL), token->GetTokenName()));

		const bool expected = EXPECTED_BOOLS[i];
		const bool actual = token->GetBoolValue();
		ASSERT_FORMAT(expected == actual,
			("Expected " BOND_DECIMAL_FORMAT " but was " BOND_DECIMAL_FORMAT ".", expected, actual));
	}

	for (int i = 0; i < NUM_INTS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_INT == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_INT), token->GetTokenName()));

		const Bond::int_t expected = EXPECTED_INTS[i];
		const Bond::int_t actual = token->GetIntValue();
		ASSERT_FORMAT(expected == actual,
			("Expected " BOND_DECIMAL_FORMAT " but was " BOND_DECIMAL_FORMAT ".", expected, actual));
	}

	for (int i = 0; i < NUM_UINTS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_UINT == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_UINT), token->GetTokenName()));

		const Bond::uint_t expected = EXPECTED_UINTS[i];
		const Bond::uint_t actual = token->GetUIntValue();
		ASSERT_FORMAT(expected == actual,
			("Expected " BOND_UDECIMAL_FORMAT " but was " BOND_UDECIMAL_FORMAT ".", expected, actual));
	}

	for (int i = 0; i < NUM_FLOATS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_FLOAT == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_FLOAT), token->GetTokenName()));

		const Bond::float_t expected = EXPECTED_FLOATS[i];
		const Bond::float_t actual = token->GetFloatValue();
		ASSERT_FORMAT(expected == actual,
			("Expected " BOND_FLOAT_FORMAT " but was " BOND_FLOAT_FORMAT ".", expected, actual));
	}

	for (int i = 0; i < NUM_CHARS; ++i)
	{
		const Bond::Token *token = stream.Next();
		ASSERT_FORMAT(Bond::Token::CONST_CHAR == token->GetTokenType(),
			("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::CONST_CHAR), token->GetTokenName()));

		const char expected = EXPECTED_CHARS[i];
		const char actual = token->GetCharValue();
		ASSERT_FORMAT(expected == actual,
			("Expected " BOND_DECIMAL_FORMAT " but was " BOND_DECIMAL_FORMAT ".", expected, actual));
	}

	ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");

	return true;
}


DEFINE_TEST(InvalidTokens)
{
	const char TEXT[] =
		"98765g 07797 0xfg9\n"
		"1.2blah3e4 1.2eblah 1.2e+blah 1.2e3blah 1.2e\n"
		"'' 'ab' '\\p' 'a\\p' \"a\\pb\"\n";

	const int LENGTH = sizeof(TEXT) / sizeof(*TEXT) - 1;

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

	Bond::DefaultAllocator allocator;
	{
		Bond::Lexer lexer(allocator);
		lexer.Lex(TEXT, LENGTH);
		Bond::TokenStream stream = lexer.GetTokenStream();

		for (int i = 0; i < NUM_TOKENS; ++i)
		{
			const Bond::Token *token = stream.Next();
			ASSERT_FORMAT(Bond::Token::INVALID == token->GetTokenType(),
				("Expected %s but was %s.", Bond::Token::GetTokenName(Bond::Token::INVALID), token->GetTokenName()));
			ASSERT_FORMAT(EXPECTED_ERRORS[i] == token->GetErrorType(),
				("Expected %s but was %s.", Bond::Token::GetErrorName(EXPECTED_ERRORS[i]), token->GetErrorName()));
		}

		ASSERT_MESSAGE(stream.Next()->GetTokenType() == Bond::Token::END, "Expected end of stream.");
	}

	ASSERT_FORMAT(allocator.GetNumAllocations() == 0, ("%d leaked chunks.", allocator.GetNumAllocations()));

	return true;
}


DEFINE_LEXER_TEST(UnterminatedCharacter, "scripts/lexer_UnterminatedCharacter.bond")
{
	Bond::TokenStream stream = lexer.GetTokenStream();

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
	Bond::TokenStream stream = lexer.GetTokenStream();

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
	Bond::TokenStream stream = lexer.GetTokenStream();

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
	Bond::TokenStream stream = lexer.GetTokenStream();

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
	Bond::TokenStream stream = lexer.GetTokenStream();

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
