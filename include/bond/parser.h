#ifndef BOND_PARSER_H
#define BOND_PARSER_H

#include "bond/parseerror.h"
#include "bond/parsenodefactory.h"
#include "bond/parsenodes.h"

namespace Bond
{

class Token;
class TokenStream;

class Parser
{
public:
	Parser(Allocator &allocator);
	~Parser();
	void Dispose();

	void Parse(TokenStream &stream);

	TranslationUnit *GetTranslationUnitList() { return mTranslationUnitList; }

	bool HasErrors() const { return mErrorBuffer.HasErrors(); }
	const ParseErrorBuffer &GetErrorBuffer() const { return mErrorBuffer; }

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
			mDisallowFunctionDefinitions(false),
			mDisallowInitializers(false),
			mParseConstExpressions(false),
			mParseRelaxedTypeDescriptors(false),
			mHasError(false),
			mParent(0)
		{}

		Status(Status &parent):
			mDisallowFunctionDefinitions(parent.mDisallowFunctionDefinitions),
			mDisallowInitializers(parent.mDisallowInitializers),
			mParseConstExpressions(parent.mParseConstExpressions),
			mParseRelaxedTypeDescriptors(parent.mParseRelaxedTypeDescriptors),
			mHasError(false),
			mParent(&parent)
		{}

		void DisallowFunctionDefinitions() { mDisallowFunctionDefinitions = true; }
		bool IsDisallowingFunctionDefinitions() const { return mDisallowFunctionDefinitions; }

		void DisallowInitializers() { mDisallowInitializers = true; }
		bool IsDisallowingInitializers() const { return mDisallowInitializers; }

		void ParseConstExpressions() { mParseConstExpressions = true; }
		bool IsParsingConstExpressions() const { return mParseConstExpressions; }

		void ParseRelaxedTypeDescriptors() { mParseRelaxedTypeDescriptors = true; }
		bool IsParsingRelaxedTypeDescriptors() const { return mParseRelaxedTypeDescriptors; }

		void MarkError() { mHasError = true; if (mParent != 0) mParent->MarkError(); }
		void RecoverFromError() { mHasError = false; if (mParent != 0) mParent->RecoverFromError(); }
		bool HasUnrecoveredError() const { return mHasError; }

	private:
		bool mDisallowFunctionDefinitions;
		bool mDisallowInitializers;
		bool mParseConstExpressions;
		bool mParseRelaxedTypeDescriptors;
		bool mHasError;
		Status *mParent;
	};

	// Copying disallowed.
	Parser(const Parser &other);
	Parser &operator=(const Parser &other);

	TranslationUnit *ParseTranslationUnit(Status &status, TokenStream &stream);
	ListParseNode *ParseExternalDeclarationList(Status &status, TokenStream &stream);
	ListParseNode *ParseExternalDeclaration(Status &status, TokenStream &stream);
	NamespaceDefinition *ParseNamespaceDefinition(Status &status, TokenStream &stream);
	EnumDeclaration *ParseEnumDeclaration(Status &status, TokenStream &stream);
	Enumerator *ParseEnumerator(Status &status, TokenStream &stream, EnumDeclaration *parent);
	StructDeclaration *ParseStructDeclaration(Status &status, TokenStream &stream);
	ListParseNode *ParseFunctionOrDeclarativeStatement(Status &status, TokenStream &stream);
	Parameter *ParseParameterList(Status &status, TokenStream &stream);
	Parameter *ParseParameter(Status &status, TokenStream &stream);
	TypeDescriptor *ParseRelaxedTypeDescriptor(Status &status, TokenStream &stream);
	TypeDescriptor *ParseTypeDescriptor(Status &status, TokenStream &stream);
	TypeSpecifier *ParseTypeSpecifier(Status &status, TokenStream &stream);
	TypeSpecifier *ParsePrimitiveTypeSpecifier(Status &status, TokenStream &stream);

	NamedInitializer *ParseNamedInitializerList(
		Status &status,
		TokenStream &stream,
		TypeDescriptor *typeDescriptor);

	NamedInitializer *ParseNamedInitializer(
		Status &status,
		TokenStream &stream,
		TypeDescriptor *typeDescriptor);

	Initializer *ParseInitializer(Status &status, TokenStream &stream);
	QualifiedIdentifier *ParseQualifiedIdentifier(Status &status, TokenStream &stream);
	ListParseNode *ParseStatement(Status &status, TokenStream &stream);
	CompoundStatement *ParseCompoundStatement(Status &status, TokenStream &stream);
	IfStatement *ParseIfStatement(Status &status, TokenStream &stream);
	SwitchStatement *ParseSwitchStatement(Status &status, TokenStream &stream);
	SwitchSection *ParseSwitchSection(Status &status, TokenStream &stream);
	SwitchLabel *ParseSwitchLabel(Status &status, TokenStream &stream);
	WhileStatement *ParseWhileStatement(Status &status, TokenStream &stream);
	WhileStatement *ParseDoWhileStatement(Status &status, TokenStream &stream);
	ForStatement *ParseForStatement(Status &status, TokenStream &stream);
	JumpStatement *ParseJumpStatement(Status &status, TokenStream &stream);
	ListParseNode *ParseExpressionOrDeclarativeStatement(Status &status, TokenStream &stream);
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

	void ExpectDeclarationTerminator(Status &status, TokenStream &stream);
	void SyncToDeclarationTerminator(Status &status, TokenStream &stream);
	void SyncToEnumeratorDelimiter(Status &status, TokenStream &stream);
	void SyncToStructMemberTerminator(Status &status, TokenStream &stream);
	void SyncToInitializerDelimiter(Status &status, TokenStream &stream);
	void ExpectStatementTerminator(Status &status, TokenStream &stream);
	void SyncToStatementTerminator(Status &status, TokenStream &stream);
	void ExpectLabelTerminator(Status &status, TokenStream &stream);

	void Recover(
		Status &status,
		TokenStream &stream,
		const TokenTypeSet &delimiters) const;

	const Token *ExpectToken(Status &status, TokenStream &stream, Token::TokenType expectedType);
	const Token *ExpectToken(Status &status, TokenStream &stream, const TokenTypeSet &expectedTypes);
	void AssertNode(Status &status, const TokenStream &stream, ParseNode *node);
	void AssertNonConstExpression(Status &status, ParseError::Type type, const Token *token);
	void PushError(Status &status, ParseError::Type errorType, const Token *token, const char *expected = "");

	ParseErrorBuffer mErrorBuffer;
	ParseNodeFactory mFactory;
	TranslationUnit *mTranslationUnitList;
};

}

#endif
