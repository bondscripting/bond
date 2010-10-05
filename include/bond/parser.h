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

	class Status
	{
	public:
		Status():
			mParseConstExpressions(false),
			mParseRelaxedTypeDescriptors(false),
			mHasError(false),
			mParent(0)
		{}

		Status(Status &parent):
			mParseConstExpressions(parent.mParseConstExpressions),
			mParseRelaxedTypeDescriptors(parent.mParseRelaxedTypeDescriptors),
			mHasError(false),
			mParent(&parent)
		{}

		void ParseConstExpressions() { mParseConstExpressions = true; }
		bool IsParsingConstExpressions() const { return mParseConstExpressions; }

		void ParseRelaxedTypeDescriptors() { mParseRelaxedTypeDescriptors = true; }
		bool IsParsingRelaxedTypeDescriptors() const { return mParseRelaxedTypeDescriptors; }

		void MarkError() { mHasError = true; if (mParent != 0) mParent->MarkError(); }
		void RecoverFromError() { mHasError = false; if (mParent != 0) mParent->RecoverFromError(); }
		bool HasUnrecoveredError() const { return mHasError; }

	private:
		bool mParseConstExpressions;
		bool mParseRelaxedTypeDescriptors;
		bool mHasError;
		Status *mParent;
	};

	// Copying disallowed.
	Parser(const Parser &other);
	Parser &operator=(const Parser &other);

	TranslationUnit *ParseTranslationUnit(Status &status, TokenStream &stream);
	ExternalDeclaration *ParseExternalDeclarationList(Status &status, TokenStream &stream);
	ExternalDeclaration *ParseExternalDeclaration(Status &status, TokenStream &stream);
	NamespaceDefinition *ParseNamespaceDefinition(Status &status, TokenStream &stream);
	EnumDeclaration *ParseEnumDeclaration(Status &status, TokenStream &stream);
	Enumerator *ParseEnumeratorList(Status &status, TokenStream &stream);
	Enumerator *ParseEnumerator(Status &status, TokenStream &stream);
	Parameter *ParseParameterList(Status &status, TokenStream &stream);
	Parameter *ParseParameter(Status &status, TokenStream &stream);
	StructDeclaration *ParseStructDeclaration(Status &status, TokenStream &stream);
	TypeDescriptor *ParseRelaxedTypeDescriptor(Status &status, TokenStream &stream);
	TypeDescriptor *ParseTypeDescriptor(Status &status, TokenStream &stream);
	TypeSpecifier *ParseTypeSpecifier(Status &status, TokenStream &stream);
	TypeSpecifier *ParsePrimitiveTypeSpecifier(Status &status, TokenStream &stream);
	NamedInitializer *ParseNamedInitializerList(Status &status, TokenStream &stream);
	NamedInitializer *ParseNamedInitializer(Status &status, TokenStream &stream);
	QualifiedIdentifier *ParseQualifiedIdentifier(Status &status, TokenStream &stream);
	Statement *ParseStatement(Status &status, TokenStream &stream);
	CompoundStatement *ParseCompoundStatement(Status &status, TokenStream &stream);
	IfStatement *ParseIfStatement(Status &status, TokenStream &stream);
	SwitchStatement *ParseSwitchStatement(Status &status, TokenStream &stream);
	SwitchSection *ParseSwitchSection(Status &status, TokenStream &stream);
	SwitchLabel *ParseSwitchLabel(Status &status, TokenStream &stream);
	WhileStatement *ParseWhileStatement(Status &status, TokenStream &stream);
	WhileStatement *ParseDoWhileStatement(Status &status, TokenStream &stream);
	JumpStatement *ParseJumpStatement(Status &status, TokenStream &stream);
	Statement *ParseDeclarativeOrExpressionStatement(Status &status, TokenStream &stream);
	ExpressionStatement *ParseExpressionStatement(Status &status, TokenStream &stream);
	Expression *ParseConstExpression(Status &status, TokenStream &stream);
	Expression *ParseExpression(Status &status, TokenStream &stream);
	Expression *ParseAssignmentExpression(Status &status, TokenStream &stream);
	Expression *ParseConditionalExpression(Status &status, TokenStream &stream);
	Expression *ParseLogicalOrExpression(Status &status, TokenStream &stream);
	Expression *ParseLogicalAndExpression(Status &status, TokenStream &stream);
	Expression *ParseInclusiveOrExpression(Status &status, TokenStream &stream);
	Expression *ParseExclusiveOrExpression(Status &status, TokenStream &stream);
	Expression *ParseAndExpression(Status &status, TokenStream &stream);
	Expression *ParseEqualityExpression(Status &status, TokenStream &stream);
	Expression *ParseRelationalExpression(Status &status, TokenStream &stream);
	Expression *ParseShiftExpression(Status &status, TokenStream &stream);
	Expression *ParseAdditiveExpression(Status &status, TokenStream &stream);
	Expression *ParseMultiplicativeExpression(Status &status, TokenStream &stream);
	Expression *ParseCastExpression(Status &status, TokenStream &stream);
	Expression *ParseUnaryExpression(Status &status, TokenStream &stream);
	Expression *ParsePostfixExpression(Status &status, TokenStream &stream);
	Expression *ParsePrimaryExpression(Status &status, TokenStream &stream);
	Expression *ParseArgumentList(Status &status, TokenStream &stream);

	void SyncToStatementTerminator(Status &status, TokenStream &stream);
	void SyncToStatementDelimiter(Status &status, TokenStream &stream);
	void SyncToLabelTerminator(Status &status, TokenStream &stream);
	void SyncToLabelDelimiter(Status &status, TokenStream &stream);

	void Recover(
		Status &status,
		TokenStream &stream,
		const TokenTypeSet &delimiters) const;

	const Token *ExpectToken(Status &status, TokenStream &stream, Token::TokenType expectedType);
	const Token *ExpectToken(Status &status, TokenStream &stream, const TokenTypeSet &expectedTypes);
	void AssertNode(Status &status, const TokenStream &stream, ParseNode *node);
	void AssertNonConstExpression(Status &status, ErrorType type, const Token *token);
	void PushError(Status &status, ErrorType errorType, const Token *token, const char *expected = "");

	Error mErrors[MAX_ERRORS];
	int mNumErrors;
	ParseNodeFactory mFactory;
	TranslationUnit *mTranslationUnit;
};

}

#endif
