#ifndef BOND_PARSER_H
#define BOND_PARSER_H

#include "bond/parsenodefactory.h"
#include "bond/parsenodes.h"

#define BOND_PARSE_ERROR_LIST \
	BOND_PARSE_ERROR_ITEM(NO_ERROR)                          \
	BOND_PARSE_ERROR_ITEM(PARSE_ERROR)                       \
	BOND_PARSE_ERROR_ITEM(UNEXPECTED_TOKEN)                  \
	BOND_PARSE_ERROR_ITEM(DUPLICATE_CONST)                   \
	BOND_PARSE_ERROR_ITEM(COMMA_IN_CONST_EXPRESSION)         \
	BOND_PARSE_ERROR_ITEM(ASSIGNMENT_IN_CONST_EXPRESSION)    \
	BOND_PARSE_ERROR_ITEM(INCREMENT_IN_CONST_EXPRESSION)     \
	BOND_PARSE_ERROR_ITEM(FUNCTION_CALL_IN_CONST_EXPRESSION) \


namespace Bond
{

class Token;
class TokenStream;

class Parser
{
public:
	enum ErrorType
	{
#define BOND_PARSE_ERROR_ITEM(item) item,
		BOND_PARSE_ERROR_LIST
#undef BOND_PARSE_ERROR_ITEM
	};

	struct Error
	{
		Error(): type(NO_ERROR), token(0), expected("") {}

		Error(ErrorType type, const Token *token, const char *expected):
			type(type),
			token(token),
			expected(expected)
		{}

		Error(const Error &other):
			type(other.type),
			token(other.token),
			expected(other.expected)
		{}

		ErrorType type;
		const Token *token;
		const char *expected;
	};

	static const int MAX_ERRORS = 16;

	Parser(Allocator &allocator);
	~Parser();
	void Dispose();

	void Parse(TokenStream &stream);

	TranslationUnit *GetTranslationUnit() { return mTranslationUnit; }

	bool HasErrors() const { return mNumErrors > 0; }
	int GetNumErrors() const { return mNumErrors; }
	const Error *GetError(int index) const { return mErrors + index; }

private:
	enum ExpressionQualifier
	{
		EXP_NORMAL,
		EXP_CONST
	};

	// Copying disallowed.
	Parser(const Parser &other);
	Parser &operator=(const Parser &other);

	TranslationUnit *ParseTranslationUnit(TokenStream &stream);
	ExternalDeclaration *ParseExternalDeclarationList(TokenStream &stream);
	ExternalDeclaration *ParseExternalDeclaration(TokenStream &stream);
	NamespaceDefinition *ParseNamespaceDefinition(TokenStream &stream);
	EnumDeclaration *ParseEnumDeclaration(TokenStream &stream);
	Enumerator *ParseEnumeratorList(TokenStream &stream);
	Enumerator *ParseEnumerator(TokenStream &stream);
	TypeDescriptor *ParseTypeDescriptor(TokenStream &stream);
	TypeSpecifier *ParseTypeSpecifier(TokenStream &stream);
	TypeSpecifier *ParsePrimitiveTypeSpecifier(TokenStream &stream);
	QualifiedIdentifier *ParseQualifiedIdentifier(TokenStream &stream);
	Expression *ParseConstExpression(TokenStream &stream);
	Expression *ParseExpression(TokenStream &stream, ExpressionQualifier qualifier = EXP_NORMAL);
	Expression *ParseAssignmentExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseConditionalExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseLogicalOrExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseLogicalAndExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseInclusiveOrExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseExclusiveOrExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseAndExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseEqualityExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseRelationalExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseShiftExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseAdditiveExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseMultiplicativeExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseCastExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseUnaryExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParsePostfixExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParsePrimaryExpression(TokenStream &stream, ExpressionQualifier qualifier);
	Expression *ParseArgumentList(TokenStream &stream);

	const Token *ExpectToken(TokenStream &stream, Token::TokenType expectedType);
	const Token *ExpectToken(TokenStream &stream, TokenTypeSet &typeSet);
	void AssertNode(ParseNode *node, const TokenStream &stream);
	void AssertConstExpression(ExpressionQualifier qualifier, ErrorType type, const Token *token);
	void PushError(ErrorType errorType, const Token *token, const char *expected = "");

	Error mErrors[MAX_ERRORS];
	int mNumErrors;
	ParseNodeFactory mFactory;
	TranslationUnit *mTranslationUnit;
};

}

#endif
