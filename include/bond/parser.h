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
			mParent(0)
		{}

		Status(Status &parent):
			mParseConstExpressions(parent.mParseConstExpressions),
			mParseRelaxedTypeDescriptors(parent.mParseRelaxedTypeDescriptors),
			mParent(&parent)
		{}

		void ParseConstExpressions() { mParseConstExpressions = true; }
		bool ShouldParseConstExpressions() { return mParseConstExpressions; }

		void ParseRelaxedTypeDescriptors() { mParseRelaxedTypeDescriptors = true; }
		bool ShouldParseRelaxedTypeDescriptors() { return mParseRelaxedTypeDescriptors; }

	private:
		bool mParseConstExpressions;
		bool mParseRelaxedTypeDescriptors;
		Status *mParent;
	};

	// Copying disallowed.
	Parser(const Parser &other);
	Parser &operator=(const Parser &other);

	TranslationUnit *ParseTranslationUnit(TokenStream &stream, Status &status);
	ExternalDeclaration *ParseExternalDeclarationList(TokenStream &stream, Status &status);
	ExternalDeclaration *ParseExternalDeclaration(TokenStream &stream, Status &status);
	NamespaceDefinition *ParseNamespaceDefinition(TokenStream &stream, Status &status);
	EnumDeclaration *ParseEnumDeclaration(TokenStream &stream, Status &status);
	Enumerator *ParseEnumeratorList(TokenStream &stream, Status &status);
	Enumerator *ParseEnumerator(TokenStream &stream, Status &status);
	Parameter *ParseParameterList(TokenStream &stream, Status &status);
	Parameter *ParseParameter(TokenStream &stream, Status &status);
	StructDeclaration *ParseStructDeclaration(TokenStream &stream, Status &status);
	TypeDescriptor *ParseRelaxedTypeDescriptor(TokenStream &stream, Status &status);
	TypeDescriptor *ParseTypeDescriptor(TokenStream &stream, Status &status);
	TypeSpecifier *ParseTypeSpecifier(TokenStream &stream, Status &status);
	TypeSpecifier *ParsePrimitiveTypeSpecifier(TokenStream &stream, Status &status);
	NamedInitializer *ParseNamedInitializerList(TokenStream &stream, Status &status);
	NamedInitializer *ParseNamedInitializer(TokenStream &stream, Status &status);
	QualifiedIdentifier *ParseQualifiedIdentifier(TokenStream &stream, Status &status);
	Statement *ParseStatement(TokenStream &stream, Status &status);
	CompoundStatement *ParseCompoundStatement(TokenStream &stream, Status &status);
	IfStatement *ParseIfStatement(TokenStream &stream, Status &status);
	SwitchStatement *ParseSwitchStatement(TokenStream &stream, Status &status);
	SwitchSection *ParseSwitchSection(TokenStream &stream, Status &status);
	SwitchLabel *ParseSwitchLabel(TokenStream &stream, Status &status);
	WhileStatement *ParseWhileStatement(TokenStream &stream, Status &status);
	WhileStatement *ParseDoWhileStatement(TokenStream &stream, Status &status);
	JumpStatement *ParseJumpStatement(TokenStream &stream, Status &status);
	Statement *ParseDeclarativeOrExpressionStatement(TokenStream &stream, Status &status);
	ExpressionStatement *ParseExpressionStatement(TokenStream &stream, Status &status);
	Expression *ParseConstExpression(TokenStream &stream, Status &status);
	Expression *ParseExpression(TokenStream &stream, Status &status);
	Expression *ParseAssignmentExpression(TokenStream &stream, Status &status);
	Expression *ParseConditionalExpression(TokenStream &stream, Status &status);
	Expression *ParseLogicalOrExpression(TokenStream &stream, Status &status);
	Expression *ParseLogicalAndExpression(TokenStream &stream, Status &status);
	Expression *ParseInclusiveOrExpression(TokenStream &stream, Status &status);
	Expression *ParseExclusiveOrExpression(TokenStream &stream, Status &status);
	Expression *ParseAndExpression(TokenStream &stream, Status &status);
	Expression *ParseEqualityExpression(TokenStream &stream, Status &status);
	Expression *ParseRelationalExpression(TokenStream &stream, Status &status);
	Expression *ParseShiftExpression(TokenStream &stream, Status &status);
	Expression *ParseAdditiveExpression(TokenStream &stream, Status &status);
	Expression *ParseMultiplicativeExpression(TokenStream &stream, Status &status);
	Expression *ParseCastExpression(TokenStream &stream, Status &status);
	Expression *ParseUnaryExpression(TokenStream &stream, Status &status);
	Expression *ParsePostfixExpression(TokenStream &stream, Status &status);
	Expression *ParsePrimaryExpression(TokenStream &stream, Status &status);
	Expression *ParseArgumentList(TokenStream &stream, Status &status);

	const Token *ExpectToken(TokenStream &stream, Token::TokenType expectedType);
	const Token *ExpectToken(TokenStream &stream, TokenTypeSet &typeSet);
	void AssertNode(ParseNode *node, const TokenStream &stream);
	void AssertNonConstExpression(Status &status, ErrorType type, const Token *token);
	void PushError(ErrorType errorType, const Token *token, const char *expected = "");

	Error mErrors[MAX_ERRORS];
	int mNumErrors;
	ParseNodeFactory mFactory;
	TranslationUnit *mTranslationUnit;
};

}

#endif
