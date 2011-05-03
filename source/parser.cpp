#include "bond/autostack.h"
#include "bond/parser.h"
#include "bond/tokenstream.h"

namespace Bond
{

class ParserCore
{
public:
	ParserCore(ParseErrorBuffer &errorBuffer, ParseNodeFactory &factory, TokenStream &stream):
		mErrorBuffer(errorBuffer),
		mFactory(factory),
		mStream(stream),
		mHasUnrecoveredError(false)
	{}

	~ParserCore() {}

	TranslationUnit *Parse();

private:
	// Copying disallowed.
	ParserCore(const ParserCore &other);
	ParserCore &operator=(const ParserCore &other);

	TranslationUnit *ParseTranslationUnit();
	ListParseNode *ParseExternalDeclarationList();
	ListParseNode *ParseExternalDeclaration();
	NamespaceDefinition *ParseNamespaceDefinition();
	EnumDeclaration *ParseEnumDeclaration();
	Enumerator *ParseEnumerator(const TypeDescriptor *typeDescriptor);
	StructDeclaration *ParseStructDeclaration();
	ListParseNode *ParseFunctionOrDeclarativeStatement();
	Parameter *ParseParameterList();
	Parameter *ParseParameter();
	TypeDescriptor *ParseRelaxedTypeDescriptor();
	TypeDescriptor *ParseTypeDescriptor();
	TypeSpecifier *ParseTypeSpecifier();
	TypeSpecifier *ParsePrimitiveTypeSpecifier();
	NamedInitializer *ParseNamedInitializerList(TypeDescriptor *typeDescriptor);
	NamedInitializer *ParseNamedInitializer(TypeDescriptor *typeDescriptor);
	Initializer *ParseInitializer();
	QualifiedIdentifier *ParseQualifiedIdentifier();
	ListParseNode *ParseStatement();
	CompoundStatement *ParseCompoundStatement();
	IfStatement *ParseIfStatement();
	SwitchStatement *ParseSwitchStatement();
	SwitchSection *ParseSwitchSection();
	SwitchLabel *ParseSwitchLabel();
	WhileStatement *ParseWhileStatement();
	WhileStatement *ParseDoWhileStatement();
	ForStatement *ParseForStatement();
	JumpStatement *ParseJumpStatement();
	ListParseNode *ParseExpressionOrDeclarativeStatement();
	ExpressionStatement *ParseExpressionStatement();
	Expression *ParseConstExpression();
	Expression *ParseExpression();
	Expression *ParseAssignmentExpression();
	Expression *ParseConditionalExpression();
	Expression *ParseLogicalOrExpression();
	Expression *ParseLogicalAndExpression();
	Expression *ParseInclusiveOrExpression();
	Expression *ParseExclusiveOrExpression();
	Expression *ParseAndExpression();
	Expression *ParseEqualityExpression();
	Expression *ParseRelationalExpression();
	Expression *ParseShiftExpression();
	Expression *ParseAdditiveExpression();
	Expression *ParseMultiplicativeExpression();
	Expression *ParseCastExpression();
	Expression *ParseUnaryExpression();
	Expression *ParsePostfixExpression();
	Expression *ParsePrimaryExpression();
	Expression *ParseArgumentList();

	void ExpectDeclarationTerminator();
	void SyncToDeclarationTerminator();
	void SyncToEnumeratorDelimiter();
	void SyncToStructMemberTerminator();
	void SyncToInitializerDelimiter();
	void ExpectStatementTerminator();
	void SyncToStatementTerminator();
	void ExpectLabelTerminator();

	void Recover(const TokenTypeSet &delimiters);

	const Token *ExpectToken(Token::TokenType expectedType);
	const Token *ExpectToken(const TokenTypeSet &expectedTypes);
	void AssertNode(ParseNode *node);
	void AssertNonConstExpression(ParseError::Type type, const Token *token);
	void PushError(ParseError::Type errorType, const Token *token, const char *expected = "");

	ParseErrorBuffer &mErrorBuffer;
	ParseNodeFactory &mFactory;
	TokenStream &mStream;

	BoolStack mAllowFunctionDefinitions;
	BoolStack mAllowInitializers;
	BoolStack mParseConstExpressions;
	BoolStack mParseRelaxedTypeDescriptors;

	bool mHasUnrecoveredError;
};


Parser::Parser(Allocator &allocator):
	mFactory(allocator),
	mTranslationUnitList(0)
{
}


Parser::~Parser()
{
	Dispose();
}


void Parser::Dispose()
{
	mFactory.DestroyListHierarchy(mTranslationUnitList);
	mTranslationUnitList = 0;
}


void Parser::Parse(TokenStream &stream)
{
	ParserCore core(mErrorBuffer, mFactory, stream);
	TranslationUnit *translationUnit = core.Parse();
	translationUnit->SetNextNode(mTranslationUnitList);
	mTranslationUnitList = translationUnit;
}


TranslationUnit *ParserCore::Parse()
{
	return ParseTranslationUnit();
}


// translation_unit
//  : external_declaration*
TranslationUnit *ParserCore::ParseTranslationUnit()
{
	BoolStack::Element allowFunctionDefinitionsElement(mAllowFunctionDefinitions, true);
	BoolStack::Element allowInitializersElement(mAllowInitializers, true);
	BoolStack::Element parseConstExpressionsElement(mParseConstExpressions, false);
	BoolStack::Element parseRelaxedTypeDescriptorsElement(mParseRelaxedTypeDescriptors, false);
	ListParseNode *declarations = ParseExternalDeclarationList();
	TranslationUnit *unit = mFactory.CreateTranslationUnit(declarations);
	ExpectToken(Token::END);
	return unit;
}


ListParseNode *ParserCore::ParseExternalDeclarationList()
{
	ListParseNode *declarationList = 0;
	ListParseNode *current = 0;

	while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == 0)
	{
		// Eat up superfluous semicolons.
		if (mStream.NextIf(Token::SEMICOLON) == 0)
		{
			ListParseNode *next = ParseExternalDeclaration();
			AssertNode(next);
			SyncToDeclarationTerminator();

			if (next != 0)
			{
				if (declarationList == 0)
				{
					declarationList = next;
				}
				else
				{
					current->SetNextNode(next);
				}
				current = next;
			}
		}
	}

	return declarationList;
}


// external_declaration
//   : namespace_definition
//   | enum_declaration
//   | function_declaration
//   | function_definition
//   | struct_declaration
//   | const_declarative_statement
ListParseNode *ParserCore::ParseExternalDeclaration()
{
	ListParseNode *declaration = 0;

	switch (mStream.Peek()->GetTokenType())
	{
		case Token::KEY_NAMESPACE:
			declaration = ParseNamespaceDefinition();
			break;

		case Token::KEY_ENUM:
			declaration = ParseEnumDeclaration();
			break;

		case Token::KEY_STRUCT:
			declaration = ParseStructDeclaration();
			break;

		default:
			declaration = ParseFunctionOrDeclarativeStatement();
			break;
	}

	return declaration;
}


// namespace_definition
//   : NAMESPACE IDENTIFIER '{' external_declaration* '}'
NamespaceDefinition *ParserCore::ParseNamespaceDefinition()
{
	NamespaceDefinition *space = 0;

	if (mStream.NextIf(Token::KEY_NAMESPACE) != 0)
	{
		const Token *name = ExpectToken(Token::IDENTIFIER);
		ExpectToken(Token::OBRACE);
		ListParseNode *declarations = ParseExternalDeclarationList();
		ExpectToken(Token::CBRACE);
		space = mFactory.CreateNamespaceDefinition(name, declarations);
	}

	return space;
}


// enum_declaration
//  : ENUM IDENTIFIER '{' enumerator_list [',] '}' ';'
//
// enumerator_list
//   : enumerator
//   | enumerator_list ',' enumerator
EnumDeclaration *ParserCore::ParseEnumDeclaration()
{
	EnumDeclaration *enumeration = 0;

	if (mStream.NextIf(Token::KEY_ENUM) != 0)
	{
		const Token *name = ExpectToken(Token::IDENTIFIER);
		enumeration = mFactory.CreateEnumDeclaration(name);
		ExpectToken(Token::OBRACE);
		Enumerator *enumeratorList = 0;
		Enumerator *current = 0;

		while (mStream.PeekIf(ENUM_DELIMITERS_TYPESET) == 0)
		{
			Enumerator *next = ParseEnumerator(enumeration->GetTypeDescriptor());
			AssertNode(next);
			SyncToEnumeratorDelimiter();

			// Note that the comma on the last enumerator is optional.
			if (mStream.PeekIf(ENUM_DELIMITERS_TYPESET) == 0)
			{
				ExpectToken(Token::COMMA);
			}

			if (next != 0)
			{
				if (enumeratorList == 0)
				{
					enumeratorList = next;
				}
				else
				{
					current->SetNextNode(next);
				}
				current = next;
			}
		}

		enumeration->SetEnumeratorList(enumeratorList);
		ExpectToken(Token::CBRACE);
		ExpectDeclarationTerminator();
	}

	return enumeration;
}


// enumerator
//   : IDENTIFIER ['=' const_expression]
Enumerator *ParserCore::ParseEnumerator(const TypeDescriptor *typeDescriptor)
{
	Enumerator *enumerator = 0;
	const Token *name = mStream.NextIf(Token::IDENTIFIER);

	if (name != 0)
	{
		Expression *value = 0;
		if (mStream.NextIf(Token::ASSIGN) != 0)
		{
			value = ParseConstExpression();
			AssertNode(value);
		}
		enumerator = mFactory.CreateEnumerator(name, typeDescriptor, value);
	}

	return enumerator;
}


// struct_declaration
//   : STRUCT IDENTIFIER '{' struct_member_declaration+ '}' ';'
StructDeclaration *ParserCore::ParseStructDeclaration()
{
	StructDeclaration *declaration = 0;

	if (mStream.NextIf(Token::KEY_STRUCT) != 0)
	{
		BoolStack::Element allowFunctionDefinitionsElement(mAllowFunctionDefinitions, false);
		BoolStack::Element allowInitializersElement(mAllowInitializers, false);
		const Token *name = ExpectToken(Token::IDENTIFIER);
		ExpectToken(Token::OBRACE);
		ListParseNode *memberList = 0;
		ListParseNode *current = 0;

		while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == 0)
		{
			// Eat up superfluous semicolons.
			if (mStream.NextIf(Token::SEMICOLON) == 0)
			{
				ListParseNode *next = ParseFunctionOrDeclarativeStatement();
				AssertNode(next);
				SyncToStructMemberTerminator();

				if (next != 0)
				{
					if (memberList == 0)
					{
						memberList = next;
					}
					else
					{
						current->SetNextNode(next);
					}
					current = next;
				}
			}
		}

		ExpectToken(Token::CBRACE);
		ExpectDeclarationTerminator();
		declaration = mFactory.CreateStructDeclaration(name, memberList);
	}

	return declaration;
}


// function_definition
//   : function_prototype compound_statement
//
// function_declaration
//   : function_prototype ';'
//
// function_prototype
//   : type_descriptor IDENTIFIER '(' [parameter_list] ')'
//
// *const_declarative_statement
//   : declarative_statement 
//   With restrictions regarding constness enforced by the semantic analyser, not the grammar of the language.
ListParseNode *ParserCore::ParseFunctionOrDeclarativeStatement()
{
	ListParseNode *node = 0;
	const int startPos = mStream.GetPosition();
	TypeDescriptor *descriptor = ParseTypeDescriptor();

	// Could be a function declaration, function definition or a constant declarative statement.
	if (descriptor != 0)
	{
		const int namePos = mStream.GetPosition();
		const Token *name = ExpectToken(Token::IDENTIFIER);

		if (name != 0)
		{
			if (mStream.NextIf(Token::OPAREN) != 0)
			{
				Parameter *parameterList = ParseParameterList();
				ExpectToken(Token::CPAREN);
				FunctionPrototype *prototype = mFactory.CreateFunctionPrototype(name, descriptor, parameterList);
				CompoundStatement *body = 0;
				const Token *obrace = mStream.PeekIf(Token::OBRACE);

				if (obrace != 0)
				{
					body = ParseCompoundStatement();
					if (!mAllowFunctionDefinitions.GetTop())
					{
						PushError(ParseError::FUNCTION_DEFINITION_NOT_ALLOWED, obrace);
						mHasUnrecoveredError = false;
					}
				}
				else
				{
					ExpectDeclarationTerminator();
				}

				node = mFactory.CreateFunctionDefinition(prototype, body);
			}
			else
			{
				// Put the name back into the stream since ParseNamedInitializerList will consume it.
				mStream.SetPosition(namePos);
				NamedInitializer *initializerList = ParseNamedInitializerList(descriptor);
				// TODO: Forgot to handle failure.

				if (initializerList != 0)
				{
					descriptor->SetLValue();
					node = mFactory.CreateDeclarativeStatement(descriptor, initializerList);
					ExpectDeclarationTerminator();
				}
			}
		}
		else
		{
			// We can't tell what this is. Undo everything and bail.
			mStream.SetPosition(startPos);
			mFactory.DestroyHierarchy(descriptor);
		}
	}

	return node;
}


// parameter_list
//   : parameter
//   | parameter_list ',' parameter
Parameter *ParserCore::ParseParameterList()
{
	Parameter *head = ParseParameter();
	Parameter *current = head;

	while ((current != 0) && (mStream.NextIf(Token::COMMA) != 0))
	{
		Parameter *next = ParseParameter();
		AssertNode(next);
		current->SetNextNode(next);
		current = next;
	}

	return head;
}


// parameter
//   : type_descriptor IDENTIFIER
Parameter *ParserCore::ParseParameter()
{
	Parameter *parameter = 0;
	TypeDescriptor *descriptor = ParseTypeDescriptor();

	if (descriptor != 0)
	{
		if (descriptor->GetVariant() == TypeDescriptor::VARIANT_ARRAY)
		{
			descriptor->SetVariant(TypeDescriptor::VARIANT_POINTER);
		}
		descriptor->SetLValue();
		const Token *name = ExpectToken(Token::IDENTIFIER);
		parameter = mFactory.CreateParameter(name, descriptor);
	}

	return parameter;
}


// type_descriptor
//   : CONST type_specifier
//   | type_specifier [CONST]
//   | type_descriptor '*' [CONST]
//   | type_descriptor '[' [const_expression] ']'
TypeDescriptor *ParserCore::ParseRelaxedTypeDescriptor()
{
	BoolStack::Element parseRelaxedTypeDescriptorsElement(mParseRelaxedTypeDescriptors, true);
	return ParseTypeDescriptor();
}
TypeDescriptor *ParserCore::ParseTypeDescriptor()
{
	TypeDescriptor *descriptor = 0;
	const int pos = mStream.GetPosition();
	const bool isConst1 = mStream.NextIf(Token::KEY_CONST) != 0;
	TypeSpecifier *specifier = ParseTypeSpecifier();

	if (specifier != 0)
	{
		const Token *const2 = mStream.NextIf(Token::KEY_CONST);
		const bool isConst2 = const2 != 0;
		if (isConst1 && isConst2)
		{
			PushError(ParseError::DUPLICATE_CONST, const2);
		}

		descriptor = mFactory.CreateTypeDescriptor(specifier, isConst1 || isConst2);
		TypeDescriptor *arrayHead = 0;
		TypeDescriptor *arrayCurrent = 0;

		const Token *token = mStream.NextIf(TYPE_DESCRIPTORS_TYPESET);
		while (token != 0)
		{
			// This loop gets tricky. Suppose we have the following type descriptor:
			//
			// int [1][2][3] * [4][5][6]
			//
			// We need to chain the nodes together as follows in order to get the array dimensions
			// to be consistent with C.
			//
			// 4 -> 5-> 6 -> * -> 1 -> 2 -> 3 -> int
			//
			// Moreover, all nodes in the chain except for the head need to be designated as l-values.
			// Whether the head should be an l-value is context sensitive and is dealt with in the
			// appropriate places in the parser.
			if (token->GetTokenType() == Token::OP_STAR)
			{
				if (arrayCurrent != 0)
				{
					descriptor->SetLValue();
					arrayCurrent->SetParent(descriptor);
					descriptor = arrayHead;
					arrayHead = 0;
					arrayCurrent = 0;
				}
				const bool isConst = mStream.NextIf(Token::KEY_CONST) != 0;
				descriptor->SetLValue();
				descriptor = mFactory.CreateTypeDescriptor(descriptor, isConst);
			}
			else
			{
				Expression *length = mParseRelaxedTypeDescriptors.GetTop() ?
					ParseExpression() : ParseConstExpression();
				ExpectToken(Token::CBRACKET);
				TypeDescriptor *parent = mFactory.CreateTypeDescriptor(0, length, descriptor->IsConst());
				if (arrayCurrent == 0)
				{
					arrayHead = parent;
					arrayCurrent = parent;
				}
				else
				{
					if (length == 0)
					{
						PushError(ParseError::MULTIDIMENTIONAL_ARRAY_BOUNDS, token);
					}
					parent->SetLValue();
					arrayCurrent->SetParent(parent);
					arrayCurrent = parent;
				}
			}
			token = mStream.NextIf(TYPE_DESCRIPTORS_TYPESET);
		}

		if (arrayCurrent != 0)
		{
			descriptor->SetLValue();
			arrayCurrent->SetParent(descriptor);
			descriptor = arrayHead;
			arrayHead = 0;
			arrayCurrent = 0;
		}
	}
	else
	{
		mStream.SetPosition(pos);
	}

	return descriptor;
}


// type_specifier
//   : primitive_type_specifier
//   | qualified_id
TypeSpecifier *ParserCore::ParseTypeSpecifier()
{
	TypeSpecifier *specifier = ParsePrimitiveTypeSpecifier();

	if (specifier == 0)
	{
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier();
		if (identifier != 0)
		{
			specifier = mFactory.CreateTypeSpecifier(identifier);
		}
	}

	return specifier;
}


// primitive_type_specifier
//   : VOID
//   | BOOL
//   | CHAR
//   | INT
//   | UINT
//   | FLOAT
TypeSpecifier *ParserCore::ParsePrimitiveTypeSpecifier()
{
	TypeSpecifier *specifier = 0;
	const Token *primitiveType = mStream.NextIf(PRIMITIVE_TYPE_SPECIFIERS_TYPESET);

	if (primitiveType != 0)
	{
		specifier = mFactory.CreateTypeSpecifier(primitiveType);
	}

	return specifier;
}


// named_initializer_list
//   : named_initializer
//   | named_initializer_list ',' named_initializer
NamedInitializer *ParserCore::ParseNamedInitializerList(TypeDescriptor *typeDescriptor)
{
	NamedInitializer *head = ParseNamedInitializer(typeDescriptor);
	NamedInitializer *current = head;

	while ((current != 0) && (mStream.NextIf(Token::COMMA) != 0))
	{
		NamedInitializer *next = ParseNamedInitializer(typeDescriptor);
		AssertNode(next);
		current->SetNextNode(next);
		current = next;
	}

	return head;
}


// named_initializer
//   : IDENTIFIER ['=' initializer]
NamedInitializer *ParserCore::ParseNamedInitializer(TypeDescriptor *typeDescriptor)
{
	NamedInitializer *namedInitializer = 0;
	const Token *name = mStream.NextIf(Token::IDENTIFIER);

	if (name != 0)
	{
		const Token *assign = mStream.NextIf(Token::ASSIGN);
		Initializer *initializer = 0;

		if (assign != 0)
		{
			initializer = ParseInitializer();
			if (!mAllowInitializers.GetTop())
			{
				PushError(ParseError::INITIALIZER_NOT_ALLOWED, assign);
			}
			AssertNode(initializer);
		}

		namedInitializer = mFactory.CreateNamedInitializer(name, initializer, typeDescriptor);
	}

	return namedInitializer;
}


// initializer_list
//   : initializer
//   | initializer_list ',' initializer
//
// initializer
//   : assignment_expression
//   | '{' initializer_list [','] '}'
Initializer *ParserCore::ParseInitializer()
{
	Initializer *initializer = 0;

	if (mStream.NextIf(Token::OBRACE))
	{
		Initializer *initializerList = 0;
		Initializer *current = 0;

		while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == 0)
		{
			Initializer *next = ParseInitializer();
			AssertNode(next);
			SyncToInitializerDelimiter();

			// Note that the comma on the last initializer is optional.
			if (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == 0)
			{
				ExpectToken(Token::COMMA);
			}

			if (next != 0)
			{
				if (initializerList == 0)
				{
					initializerList = next;
				}
				else
				{
					current->SetNextNode(next);
				}
				current = next;
			}
		}

		ExpectToken(Token::CBRACE);
		initializer = mFactory.CreateInitializer(initializerList);
	}
	else
	{
		Expression *expression = ParseAssignmentExpression();
		if (expression != 0)
		{
			initializer = mFactory.CreateInitializer(expression);
		}
	}

	return initializer;
}


// qualified_id
//   : IDENTIFIER
//   | qualified_id '::' IDENTIFIER
QualifiedIdentifier *ParserCore::ParseQualifiedIdentifier()
{
	QualifiedIdentifier *head = 0;
	const Token *name = mStream.NextIf(Token::IDENTIFIER);

	if (name != 0)
	{
		head = mFactory.CreateQualifiedIdentifier(name);
		QualifiedIdentifier *current = head;

		while ((current != 0) && mStream.NextIf(Token::SCOPE))
		{
			name = ExpectToken(Token::IDENTIFIER);
			QualifiedIdentifier *next = mFactory.CreateQualifiedIdentifier(name);
			current->SetNextNode(next);
			current = next;
		}
	}

	return head;
}


// statement
//   : compound_statement
//   | if_statement
//   | switch_statement
//   | while_statement
//   | do_statement
//   | for_statement
//   | jump_statement
//   | labeled_statement
//   | declarative_statement
//   | expression_statement
ListParseNode *ParserCore::ParseStatement()
{
	ListParseNode *statement = 0;

	switch (mStream.Peek()->GetTokenType())
	{
		case Token::OBRACE:
			statement = ParseCompoundStatement();
			break;

		case Token::KEY_IF:
			statement = ParseIfStatement();
			break;

		case Token::KEY_SWITCH:
			statement = ParseSwitchStatement();
			break;

		case Token::KEY_WHILE:
			statement = ParseWhileStatement();
			break;

		case Token::KEY_DO:
			statement = ParseDoWhileStatement();
			break;

		case Token::KEY_FOR:
			statement = ParseForStatement();
			break;

		case Token::KEY_BREAK:
		case Token::KEY_CONTINUE:
		case Token::KEY_RETURN:
			statement = ParseJumpStatement();
			break;

		default:
			statement = ParseExpressionOrDeclarativeStatement();
			break;
	}

	return statement;
}


// compound_statement
//   : '{' statement* '}'
CompoundStatement *ParserCore::ParseCompoundStatement()
{
	CompoundStatement *compoundStatement = 0;

	if (mStream.NextIf(Token::OBRACE))
	{
		ListParseNode *statementList = 0;
		ListParseNode *current = 0;

		while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == 0)
		{
			ListParseNode *next = ParseStatement();
			AssertNode(next);
			SyncToStatementTerminator();

			if (next != 0)
			{
				if (statementList == 0)
				{
					statementList = next;
				}
				else
				{
					current->SetNextNode(next);
				}
				current = next;
			}
		}

		ExpectToken(Token::CBRACE);
		compoundStatement = mFactory.CreateCompoundStatement(statementList);
	}

	return compoundStatement;
}


// if_statement
//   : IF '(' expression ')' statement [ELSE statement]
IfStatement *ParserCore::ParseIfStatement()
{
	IfStatement *ifStatement = 0;

	if (mStream.NextIf(Token::KEY_IF) != 0)
	{
		ExpectToken(Token::OPAREN);
		Expression *condition = ParseExpression();
		AssertNode(condition);
		ExpectToken(Token::CPAREN);
		ParseNode *thenStatement = ParseStatement();
		AssertNode(thenStatement);
		ParseNode *elseStatement = 0;

		if (mStream.NextIf(Token::KEY_ELSE))
		{
			elseStatement = ParseStatement();
			AssertNode(elseStatement);
		}

		ifStatement = mFactory.CreateIfStatement(condition, thenStatement, elseStatement);
	}

	return ifStatement;
}


// switch_statement
//   : SWITCH '(' expression ')' '{' switch_section* '}'
SwitchStatement *ParserCore::ParseSwitchStatement()
{
	SwitchStatement *switchStatement = 0;

	if (mStream.NextIf(Token::KEY_SWITCH) != 0)
	{
		ExpectToken(Token::OPAREN);
		Expression *control = ParseExpression();
		AssertNode(control);
		ExpectToken(Token::CPAREN);
		ExpectToken(Token::OBRACE);

		SwitchSection *sectionList = ParseSwitchSection();
		SwitchSection *current = sectionList;
		while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == 0)
		{
			SwitchSection *next = ParseSwitchSection();
			current->SetNextNode(next);
			current = next;
		}
		// TODO: Semantic analyser must ensure the list is not empty.

		ExpectToken(Token::CBRACE);
		switchStatement = mFactory.CreateSwitchStatement(control, sectionList);
	}

	return switchStatement;
}


// switch_section
//   : switch_label+ statement+
SwitchSection *ParserCore::ParseSwitchSection()
{
	SwitchSection *section = 0;
	SwitchLabel *labelList = ParseSwitchLabel();
	SwitchLabel *currentLabel = labelList;
	while (currentLabel != 0)
	{
		SwitchLabel *next = ParseSwitchLabel();
		currentLabel->SetNextNode(next);
		currentLabel = next;
	}
	// TODO: Semantic analyser must ensure the list is not empty.

	ListParseNode *statementList = 0;
	ListParseNode *currentStatement = 0;
	while (mStream.PeekIf(SWITCH_SECTION_DELIMITERS_TYPESET) == 0)
	{
		ListParseNode *next = ParseStatement();
		AssertNode(next);
		SyncToStatementTerminator();

		if (next != 0)
		{
			if (statementList == 0)
			{
				statementList = next;
			}
			else
			{
				currentStatement->SetNextNode(next);
			}
			currentStatement = next;
		}
	}
	// TODO: Semantic analyser must ensure the list is not empty.

	section = mFactory.CreateSwitchSection(labelList, statementList);;

	return section;
}


// switch_label
//   : CASE const_expression ':'
//   | DEFAULT ':'
SwitchLabel *ParserCore::ParseSwitchLabel()
{
	SwitchLabel *label = 0;
	const Token *labelToken = mStream.NextIf(SWITCH_LABELS_TYPESET);

	if (labelToken != 0)
	{
		if (labelToken->GetTokenType() == Token::KEY_CASE)
		{
			Expression *expression = ParseConstExpression();
			AssertNode(expression);
			label = mFactory.CreateSwitchLabel(labelToken, expression);
		}
		else
		{
			label = mFactory.CreateDefaultLabel(labelToken);
		}

		ExpectLabelTerminator();
	}

	return label;
}


// while_statement
//   : WHILE '(' expression ')' statement
WhileStatement *ParserCore::ParseWhileStatement()
{
	WhileStatement *whileStatement = 0;

	if (mStream.NextIf(Token::KEY_WHILE) != 0)
	{
		ExpectToken(Token::OPAREN);
		Expression *condition = ParseExpression();
		AssertNode(condition);
		ExpectToken(Token::CPAREN);
		ParseNode *body = ParseStatement();
		AssertNode(body);
		whileStatement = mFactory.CreateWhileStatement(condition, body);
	}

	return whileStatement;
}


// do_statement
//   : DO statement WHILE '(' expression ')' ';'
WhileStatement *ParserCore::ParseDoWhileStatement()
{
	WhileStatement *whileStatement = 0;

	if (mStream.NextIf(Token::KEY_DO) != 0)
	{
		ParseNode *body = ParseStatement();
		AssertNode(body);
		ExpectToken(Token::KEY_WHILE);
		ExpectToken(Token::OPAREN);
		Expression *condition = ParseExpression();
		AssertNode(condition);
		ExpectToken(Token::CPAREN);
		ExpectToken(Token::SEMICOLON);
		whileStatement = mFactory.CreateDoWhileStatement(condition, body);
	}

	return whileStatement;
}


// for_statement
//   : FOR '(' for_init [expression] ';' [expression] ')' statement
//
// for_init
//   : declarative_statement
//   | expression_statement
ForStatement *ParserCore::ParseForStatement()
{
	ForStatement *forStatement = 0;

	if (mStream.NextIf(Token::KEY_FOR) != 0)
	{
		ExpectToken(Token::OPAREN);
		ParseNode *initializer = ParseExpressionOrDeclarativeStatement();
		AssertNode(initializer);
		Expression *condition = ParseExpression();
		ExpectToken(Token::SEMICOLON);
		Expression *countingExpression = ParseExpression();
		ExpectToken(Token::CPAREN);
		ParseNode *body = ParseStatement();
		AssertNode(body);
		forStatement = mFactory.CreateForStatement(initializer, condition, countingExpression, body);
	}

	return forStatement;
}


// jump_statement
//   : CONTINUE ';'
//   | BREAK ';'
//   | RETURN [expression] ';'
JumpStatement *ParserCore::ParseJumpStatement()
{
	JumpStatement *jumpStatement = 0;
	const Token *op = mStream.NextIf(JUMP_OPERATORS_TYPESET);

	if (op != 0)
	{
		Expression *rhs = 0;
		if (op->GetTokenType() == Token::KEY_RETURN)
		{
			rhs = ParseExpression();
		}
		ExpectStatementTerminator();
		jumpStatement = mFactory.CreateJumpStatement(op, rhs);
	}

	return jumpStatement;
}


// declarative_statement
//   : type_descriptor named_initializer_list ';'
ListParseNode *ParserCore::ParseExpressionOrDeclarativeStatement()
{
	ListParseNode *statement = 0;
	const int startPos = mStream.GetPosition();

	// The grammar is somewhat ambiguous. Since a qualified identifier followed by '*' tokens and array
	// index operators can appear like a type descriptor as well as an expression, we'll treat anything
	// that fits the profile of a declaration as such and everything else like an expression statement.
	TypeDescriptor *descriptor = ParseRelaxedTypeDescriptor();
	if (descriptor != 0)
	{
		NamedInitializer *initializerList = ParseNamedInitializerList(descriptor);

		if (initializerList != 0)
		{
			descriptor->SetLValue();
			statement = mFactory.CreateDeclarativeStatement(descriptor, initializerList);
			ExpectStatementTerminator();
		}
		else
		{
			// The tokens that looked like a type descriptor, might actually be part of an expression.
			const int descriptorPos = mStream.GetPosition();
			mFactory.DestroyHierarchy(descriptor);
			descriptor = 0;
			mStream.SetPosition(startPos);

			statement = ParseExpressionStatement();

			if (statement == 0)
			{
				// Uh, oh. Looks like we're even worse off.
				mStream.SetPosition(descriptorPos);
				AssertNode(statement);
				ExpectStatementTerminator();
			}
		}
	}

	else
	{
		statement = ParseExpressionStatement();
	}

	return statement;
}


// expression_statement
//   : [expression] ';'
ExpressionStatement *ParserCore::ParseExpressionStatement()
{
	ExpressionStatement *expressionStatement = 0;
	Expression *expression = ParseExpression();

	if (expression != 0)
	{
		ExpectStatementTerminator();
		expressionStatement = mFactory.CreateExpressionStatement(expression);
	}
	else if (mStream.NextIf(Token::SEMICOLON))
	{
		expressionStatement = mFactory.CreateExpressionStatement(0);
	}

	return expressionStatement;
}


// const_expression
//   : conditional_expression
//
// expression
//   : assignment_expression
//   | expression ',' assignment_expression
Expression *ParserCore::ParseConstExpression()
{
	BoolStack::Element parseConstExpressionsElement(mParseConstExpressions, true);
	return ParseExpression();
}
Expression *ParserCore::ParseExpression()
{
	Expression *expression = 0;

	if (mParseConstExpressions.GetTop())
	{
		expression = ParseConditionalExpression();
	}
	else
	{
		expression = ParseAssignmentExpression();
 
		if (expression != 0)
 		{
			const Token *token = mStream.NextIf(Token::COMMA);
			while (token != 0)
			{
				Expression *rhs = ParseAssignmentExpression();
				AssertNode(rhs);
				expression = mFactory.CreateBinaryExpression(token, expression, rhs);
				token = mStream.NextIf(Token::COMMA);
			}
 		}
 	}
 
 	return expression;
}


// assignment_expression
//   : conditional_expression
//   | unary_expression '=' assignment_expression
//   | unary_expression '<<=' assignment_expression
//   | unary_expression '>>=' assignment_expression
//   | unary_expression '+=' assignment_expression
//   | unary_expression '-=' assignment_expression
//   | unary_expression '*=' assignment_expression
//   | unary_expression '/=' assignment_expression
//   | unary_expression '%=' assignment_expression
//   | unary_expression '&=' assignment_expression
//   | unary_expression '^=' assignment_expression
//   | unary_expression '|=' assignment_expression
Expression *ParserCore::ParseAssignmentExpression()
{
	Expression *expression = ParseConditionalExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(ASSIGNMENT_OPERATORS_TYPESET);
		if (token != 0)
		{
			AssertNonConstExpression(ParseError::INVALID_OPERATOR_IN_CONST_EXPRESSION, token);
			Expression *rhs = ParseAssignmentExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
		}
	}

	return expression;
}


// conditional_expression
//   : logical_or_expression
//   | logical_or_expression '?' expression ':' conditional_expression
Expression *ParserCore::ParseConditionalExpression()
{
	Expression *expression = ParseLogicalOrExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(Token::OP_TERNARY);
		if (token != 0)
		{
			Expression *trueExpression = ParseExpression();
			AssertNode(trueExpression);
			ExpectToken(Token::COLON);
			Expression *falseExpression = ParseConditionalExpression();
			AssertNode(falseExpression);
			expression = mFactory.CreateConditionalExpression(token, expression, trueExpression, falseExpression);
		}
	}

	return expression;
}


// logical_or_expression
//   : logical_and_expression
//   | logical_or_expression '||' logical_and_expression
Expression *ParserCore::ParseLogicalOrExpression()
{
	Expression *expression = ParseLogicalAndExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(Token::OP_OR);
		while (token != 0)
		{
			Expression *rhs = ParseLogicalAndExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(Token::OP_OR);
		}
	}

	return expression;
}


// logical_and_expression
//   : inclusive_or_expression
//   | logical_and_expression '&&' inclusive_or_expression
Expression *ParserCore::ParseLogicalAndExpression()
{
	Expression *expression = ParseInclusiveOrExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(Token::OP_AND);
		while (token != 0)
		{
			Expression *rhs = ParseInclusiveOrExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(Token::OP_AND);
		}
	}

	return expression;
}


// inclusive_or_expression
//   : exclusive_or_expression
//   | inclusive_or_expression '|' exclusive_or_expression
Expression *ParserCore::ParseInclusiveOrExpression()
{
	Expression *expression = ParseExclusiveOrExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(Token::OP_BIT_OR);
		while (token != 0)
		{
			Expression *rhs = ParseExclusiveOrExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(Token::OP_BIT_OR);
		}
	}

	return expression;
}


// exclusive_or_expression
//   : and_expression
//   | exclusive_or_expression '^' and_expression
Expression *ParserCore::ParseExclusiveOrExpression()
{
	Expression *expression = ParseAndExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(Token::OP_BIT_XOR);
		while (token != 0)
		{
			Expression *rhs = ParseAndExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(Token::OP_BIT_XOR);
		}
	}

	return expression;
}


// and_expression
//   : equality_expression
//   | and_expression '&' equality_expression
Expression *ParserCore::ParseAndExpression()
{
	Expression *expression = ParseEqualityExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(Token::OP_AMP);
		while (token != 0)
		{
			Expression *rhs = ParseEqualityExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(Token::OP_AMP);
		}
	}

	return expression;
}


// equality_expression
//   : relational_expression
//   | equality_expression '==' relational_expression
//   | equality_expression '!=' relational_expression
Expression *ParserCore::ParseEqualityExpression()
{
	Expression *expression = ParseRelationalExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(EQUALITY_OPERATORS_TYPESET);
		while (token != 0)
		{
			Expression *rhs = ParseRelationalExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(EQUALITY_OPERATORS_TYPESET);
		}
	}

	return expression;
}


// relational_expression
//   : shift_expression
//   | relational_expression '<' shift_expression
//   | relational_expression '>' shift_expression
//   | relational_expression '<=' shift_expression
//   | relational_expression '>=' shift_expression
Expression *ParserCore::ParseRelationalExpression()
{
	Expression *expression = ParseShiftExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(RELATIONAL_OPERATORS_TYPESET);
		while (token != 0)
		{
			Expression *rhs = ParseShiftExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(RELATIONAL_OPERATORS_TYPESET);
		}
	}

	return expression;
}


// shift_expression
//   : additive_expression
//   | shift_expression '<<' additive_expression
//   | shift_expression '>>' additive_expression
Expression *ParserCore::ParseShiftExpression()
{
	Expression *expression = ParseAdditiveExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(SHIFT_OPERATORS_TYPESET);
		while (token != 0)
		{
			Expression *rhs = ParseAdditiveExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(SHIFT_OPERATORS_TYPESET);
		}
	}

	return expression;
}


// additive_expression
//   : multiplicative_expression
//   | additive_expression '+' multiplicative_expression
//   | additive_expression '-' multiplicative_expression
Expression *ParserCore::ParseAdditiveExpression()
{
	Expression *expression = ParseMultiplicativeExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(ADDITIVE_OPERATORS_TYPESET);
		while (token != 0)
		{
			Expression *rhs = ParseMultiplicativeExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(ADDITIVE_OPERATORS_TYPESET);
		}
	}

	return expression;
}


// multiplicative_expression
//   : cast_expression
//   | multiplicative_expression '*' cast_expression
//   | multiplicative_expression '/' cast_expression
//   | multiplicative_expression '%' cast_expression
Expression *ParserCore::ParseMultiplicativeExpression()
{
	Expression *expression = ParseCastExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(MULTIPLICATIVE_OPERATORS_TYPESET);
		while (token != 0)
		{
			Expression *rhs = ParseCastExpression();
			AssertNode(rhs);
			expression = mFactory.CreateBinaryExpression(token, expression, rhs);
			token = mStream.NextIf(MULTIPLICATIVE_OPERATORS_TYPESET);
		}
	}

	return expression;
}


// cast_expression
//   : unary_expression
//   | CAST '<' type_descriptor '>' '(' cast_expression ')'
Expression *ParserCore::ParseCastExpression()
{
	Expression *expression = 0;
	const Token *token = mStream.NextIf(Token::KEY_CAST);

	if (token != 0)
	{
		ExpectToken(Token::OP_LT);
		TypeDescriptor *descriptor = ParseTypeDescriptor();
		AssertNode(descriptor);
		ExpectToken(Token::OP_GT);
		ExpectToken(Token::OPAREN);
		Expression *rhs = ParseCastExpression();
		AssertNode(rhs);
		ExpectToken(Token::CPAREN);
		expression = mFactory.CreateCastExpression(token, descriptor, rhs);
	}
	else
	{
		expression = ParseUnaryExpression();
	}

	return expression;
}


// unary_expression
//   : postfix_expression
//   | '++' unary_expression
//   | '--' unary_expression
//   | '&' cast_expression
//   | '*' cast_expression
//   | '+' cast_expression
//   | '-' cast_expression
//   | '~' cast_expression
//   | '!' cast_expression
//   | SIZEOF unary_expression
//   | SIZEOF '<' type_descriptor '>'
Expression *ParserCore::ParseUnaryExpression()
{
	Expression *expression = 0;
	const Token *op = 0;

	if ((op = mStream.NextIf(UNARY_OPERATORS_TYPESET)) != 0)
	{
		Expression *rhs = 0;

		switch (op->GetTokenType())
		{
			case Token::OP_INC:
			case Token::OP_DEC:
				rhs = ParseUnaryExpression();
				AssertNonConstExpression(ParseError::INVALID_OPERATOR_IN_CONST_EXPRESSION, op);
				break;

			default:
				rhs = ParseCastExpression();
				break;
		}

		AssertNode(rhs);
		expression = mFactory.CreateUnaryExpression(op, rhs);
	}

	else if ((op = mStream.NextIf(Token::KEY_SIZEOF)) != 0)
	{
		if (mStream.NextIf(Token::OP_LT) != 0)
		{
			TypeDescriptor *descriptor = ParseTypeDescriptor();
			AssertNode(descriptor);
			ExpectToken(Token::OP_GT);
			expression = mFactory.CreateSizeofExpression(op, descriptor);
		}
		else
		{
			Expression *unary = ParseUnaryExpression();
			AssertNode(unary);
			expression = mFactory.CreateSizeofExpression(op, unary);
		}
	}
	else
	{
		expression = ParsePostfixExpression();
	}

	return expression;
}


// postfix_expression
//   : primary_expression
//   | postfix_expression '[' expression ']'
//   | postfix_expression '(' [argument_expression_list] ')'
//   | postfix_expression '.' IDENTIFIER
//   | postfix_expression '->' IDENTIFIER
//   | postfix_expression '++'
//   | postfix_expression '--'
Expression *ParserCore::ParsePostfixExpression()
{
	Expression *expression = ParsePrimaryExpression();

	if (expression != 0)
	{
		const Token *token = mStream.NextIf(POSTFIX_OPERATORS_TYPESET);
		while (token != 0)
		{
			switch (token->GetTokenType())
			{
				case Token::OBRACKET:
				{
					Expression *index = ParseExpression();
					expression = mFactory.CreateArraySubscriptExpression(token, expression, index);
					ExpectToken(Token::CBRACKET);
				}
				break;

				case Token::OPAREN:
				{
					Expression *argumentList = ParseArgumentList();
					expression = mFactory.CreateFunctionCallExpression(token, expression, argumentList);
					AssertNonConstExpression(ParseError::FUNCTION_CALL_IN_CONST_EXPRESSION, token);
					ExpectToken(Token::CPAREN);
				}
				break;

				case Token::OP_ARROW:
				case Token::PERIOD:
				{
					const Token *memberName = ExpectToken(Token::IDENTIFIER);
					expression = mFactory.CreateMemberExpression(token, memberName, expression);
				}
				break;

				default:
				{
					expression = mFactory.CreatePostfixExpression(token, expression);
					AssertNonConstExpression(ParseError::INVALID_OPERATOR_IN_CONST_EXPRESSION, token);
				}
				break;
			}
			token = mStream.NextIf(POSTFIX_OPERATORS_TYPESET);
		}
	}

	return expression;
}


// primary_expression
//   : qualified_id
//   | CONSTANT
//   | '(' expression ')'
Expression *ParserCore::ParsePrimaryExpression()
{
	Expression *expression = 0;
	const Token *value = mStream.NextIf(CONSTANT_VALUES_TYPESET);

	if (value != 0)
	{
		expression = mFactory.CreateConstantExpression(value);
	}
	else if (mStream.NextIf(Token::OPAREN) != 0)
	{
		expression = ParseExpression();
		AssertNode(expression);
		ExpectToken(Token::CPAREN);
	}
	else
	{
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier();
		if (identifier != 0)
		{
			expression = mFactory.CreateIdentifierExpression(identifier);
		}
	}

	return expression;
}


// argument_expression_list
//   : assignment_expression
//   | argument_expression_list ',' assignment_expression
Expression *ParserCore::ParseArgumentList()
{
	Expression *head = ParseAssignmentExpression();
	Expression *current = head;

	while ((current != 0) && (mStream.NextIf(Token::COMMA) != 0))
	{
		Expression *next = ParseAssignmentExpression();
		AssertNode(next);
		current->SetNextNode(next);
		current = next;
	}

	return head;
}


void ParserCore::ExpectDeclarationTerminator()
{
	Recover(DECLARATION_DELIMITERS_TYPESET);
	ExpectToken(Token::SEMICOLON);
	Recover(DECLARATION_DELIMITERS_TYPESET);
}


void ParserCore::SyncToDeclarationTerminator()
{
	if (mHasUnrecoveredError)
	{
		Recover(DECLARATION_DELIMITERS_TYPESET);
		mStream.NextIf(Token::SEMICOLON);
	}
}


void ParserCore::SyncToEnumeratorDelimiter()
{
	Recover(ENUMERATOR_DELIMITERS_TYPESET);
}


void ParserCore::SyncToStructMemberTerminator()
{
	if (mHasUnrecoveredError)
	{
		Recover(STRUCT_MEMBER_DELIMITERS_TYPESET);
		mStream.NextIf(Token::SEMICOLON);
	}
}


void ParserCore::SyncToInitializerDelimiter()
{
	Recover(INITIALIZER_DELIMITERS_TYPESET);
}


void ParserCore::ExpectStatementTerminator()
{
	Recover(STATEMENT_DELIMITERS_TYPESET);
	ExpectToken(Token::SEMICOLON);
	Recover(STATEMENT_DELIMITERS_TYPESET);
}


void ParserCore::SyncToStatementTerminator()
{
	if (mHasUnrecoveredError)
	{
		Recover(STATEMENT_DELIMITERS_TYPESET);
		mStream.NextIf(Token::SEMICOLON);
	}
}


void ParserCore::ExpectLabelTerminator()
{
	Recover(LABEL_DELIMITERS_TYPESET);
	ExpectToken(Token::COLON);
	Recover(LABEL_DELIMITERS_TYPESET);
}


void ParserCore::Recover(const TokenTypeSet &delimiters)
{
	if (mHasUnrecoveredError)
	{
		mStream.SkipTo(delimiters);
		mHasUnrecoveredError = false;
	}
}


const Token *ParserCore::ExpectToken(Token::TokenType expectedType)
{
	const Token *token = mStream.NextIf(expectedType);
	if (token == 0)
	{
		PushError(ParseError::UNEXPECTED_TOKEN, mStream.Peek(), Token::GetTokenName(expectedType));
	}
	return token;
}


const Token *ParserCore::ExpectToken(const TokenTypeSet &expectedTypes)
{
	const Token *token = mStream.NextIf(expectedTypes);
	if (token == 0)
	{
		PushError(ParseError::UNEXPECTED_TOKEN, mStream.Peek(), expectedTypes.GetTypeName());
	}
	return token;
}


void ParserCore::AssertNode(ParseNode *node)
{
	if (node == 0)
	{
		PushError(ParseError::PARSE_ERROR, mStream.Peek());
	}
}


void ParserCore::AssertNonConstExpression(ParseError::Type errorType, const Token *context)
{
	if (mParseConstExpressions.GetTop())
	{
		PushError(errorType, context);
	}
}


void ParserCore::PushError(ParseError::Type type, const Token *context, const char *expected)
{
	if (!mHasUnrecoveredError)
	{
		mErrorBuffer.PushError(type, context, expected);
		mHasUnrecoveredError = true;
	}
}

}
