#ifndef BOND_PARSER_H
#define BOND_PARSER_H

#include "bond/parsenodes.h"

#define BOND_PARSE_ERROR_LIST \
	BOND_PARSE_ERROR_ITEM(NO_ERROR)                 \
	BOND_PARSE_ERROR_ITEM(UNEXPECTED_TOKEN)         \

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
		Error(): type(NO_ERROR), expectedType(Token::INVALID), token(0) {}

		Error(ErrorType type, Token::TokenType expectedType, const Token *token):
			type(type),
			expectedType(expectedType),
			token(token)
		{}

		Error(const Error &other):
			type(other.type),
			expectedType(other.expectedType),
			token(other.token)
		{}

		ErrorType type;
		Token::TokenType expectedType;
		const Token *token;
	};

	static const int MAX_ERRORS = 16;

	Parser();
	~Parser() {}

	void Parse(TokenStream &stream);
	int GetNumErrors() const { return mNumErrors; }
	const Error *GetError(int index) const { return mErrors + index; }

private:
	enum ExpressionQualifier
	{
		EXP_NORMAL,
		EXP_CONST
	};

	TranslationUnit *ParseTranslationUnit(TokenStream &stream);
	ExternalDeclaration *ParseExternalDeclarationList(TokenStream &stream);
	ExternalDeclaration *ParseExternalDeclaration(TokenStream &stream);
	NamespaceDefinition *ParseNamespaceDefinition(TokenStream &stream);
	EnumDeclaration *ParseEnumDeclaration(TokenStream &stream);
	Enumerator *ParseEnumeratorList(TokenStream &stream);
	Enumerator *ParseEnumerator(TokenStream &stream);

	Expression *ParseConstExpression(TokenStream &stream);
	Expression *ParseConditionalExpression(TokenStream &stream, ExpressionQualifier qualifier);

	const Token *ExpectToken(TokenStream &stream, Token::TokenType expectedType);
	void PushError(ErrorType type, Token::TokenType expectedType, const Token *token);

	Error mErrors[MAX_ERRORS];
	int mNumErrors;
};

}

#endif
