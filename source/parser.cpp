#include "bond/autostack.h"
#include "bond/compilererror.h"
#include "bond/parser.h"
#include "bond/parsenodeutil.h"
#include "bond/tokenstream.h"

namespace Bond
{

class ParserCore
{
public:
	ParserCore(CompilerErrorBuffer &errorBuffer, ParseNodeFactory &factory, TokenStream &stream):
		mErrorBuffer(errorBuffer),
		mFactory(factory),
		mStream(stream),
		mHasUnrecoveredError(false)
	{}

	~ParserCore() {}

	TranslationUnit *Parse();

private:
	typedef AutoStack<Scope> ScopeStack;

	// Copying disallowed.
	ParserCore(const ParserCore &other);
	ParserCore &operator=(const ParserCore &other);

	TranslationUnit *ParseTranslationUnit();
	ListParseNode *ParseExternalDeclarationList();
	ListParseNode *ParseExternalDeclaration();
	NamespaceDefinition *ParseNamespaceDefinition();
	EnumDeclaration *ParseEnumDeclaration();
	Enumerator *ParseEnumerator(TypeDescriptor *typeDescriptor);
	StructDeclaration *ParseStructDeclaration();

	ListParseNode *ParseFunctionOrDeclarativeStatement(StructDeclaration *structDeclaration = NULL);
	void ParseFunctionOrDeclarativeStatement(
		StructDeclaration *structDeclaration,
		FunctionDefinition **functionDefinition,
		DeclarativeStatement **declarativeStatement);

	Parameter *ParseParameterList();
	Parameter *ParseParameter();
	TypeDescriptor *ParseRelaxedTypeDescriptor();
	TypeDescriptor *ParseTypeDescriptor(bool isRelaxedTypeDescriptor = false);
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
	void AssertNonConstExpression(CompilerError::Type type, const Token *token);
	void AssertNonVoidType(const TypeDescriptor *typeDescriptor);
	void PushUnrecoverableError(CompilerError::Type errorType, const Token *token, const void *arg = NULL);
	void PushError(CompilerError::Type errorType, const Token *token, const void *arg = NULL);

	CompilerErrorBuffer &mErrorBuffer;
	ParseNodeFactory &mFactory;
	TokenStream &mStream;

	ScopeStack mScope;
	BoolStack mParseConstExpressions;

	bool mHasUnrecoveredError;
};


Parser::Parser(Allocator &allocator, CompilerErrorBuffer &errorBuffer):
	mFactory(allocator),
	mErrorBuffer(errorBuffer),
	mTranslationUnitList(NULL)
{
}


Parser::~Parser()
{
	Dispose();
}


void Parser::Dispose()
{
	mFactory.DestroyListHierarchy(mTranslationUnitList);
	mTranslationUnitList = NULL;
}


TranslationUnit *Parser::Parse(TokenStream &stream)
{
	ParserCore parser(mErrorBuffer, mFactory, stream);
	TranslationUnit *translationUnit = parser.Parse();
	translationUnit->SetNextNode(mTranslationUnitList);
	mTranslationUnitList = translationUnit;
	return translationUnit;
}


TranslationUnit *ParserCore::Parse()
{
	return ParseTranslationUnit();
}


// translation_unit
//  : external_declaration*
TranslationUnit *ParserCore::ParseTranslationUnit()
{
	ScopeStack::Element scopeElement(mScope, SCOPE_GLOBAL);
	BoolStack::Element parseConstExpressionsElement(mParseConstExpressions, false);
	ListParseNode *declarations = ParseExternalDeclarationList();
	TranslationUnit *unit = mFactory.CreateTranslationUnit(declarations);
	ExpectToken(Token::END);
	return unit;
}


ListParseNode *ParserCore::ParseExternalDeclarationList()
{
	ParseNodeList<ListParseNode> declarationList;

	while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == NULL)
	{
		// Eat up superfluous semicolons.
		if (mStream.NextIf(Token::SEMICOLON) == NULL)
		{
			ListParseNode *next = ParseExternalDeclaration();
			AssertNode(next);
			SyncToDeclarationTerminator();
			declarationList.Append(next);
		}
	}

	return declarationList.GetHead();
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
	ListParseNode *declaration = NULL;

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
	NamespaceDefinition *space = NULL;

	if (mStream.NextIf(Token::KEY_NAMESPACE) != NULL)
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
	EnumDeclaration *enumeration = NULL;

	if (mStream.NextIf(Token::KEY_ENUM) != NULL)
	{
		const Token *name = ExpectToken(Token::IDENTIFIER);
		enumeration = mFactory.CreateEnumDeclaration(name);
		ExpectToken(Token::OBRACE);
		ParseNodeList<Enumerator> enumeratorList;

		while (mStream.PeekIf(ENUM_DELIMITERS_TYPESET) == NULL)
		{
			Enumerator *next = ParseEnumerator(enumeration->GetTypeDescriptor());
			AssertNode(next);
			SyncToEnumeratorDelimiter();

			// Note that the comma on the last enumerator is optional.
			if (mStream.PeekIf(ENUM_DELIMITERS_TYPESET) == NULL)
			{
				ExpectToken(Token::COMMA);
			}

			enumeratorList.Append(next);
		}

		enumeration->SetEnumeratorList(enumeratorList.GetHead());
		ExpectToken(Token::CBRACE);
		ExpectDeclarationTerminator();
	}

	return enumeration;
}


// enumerator
//   : IDENTIFIER ['=' const_expression]
Enumerator *ParserCore::ParseEnumerator(TypeDescriptor *typeDescriptor)
{
	Enumerator *enumerator = NULL;
	const Token *name = mStream.NextIf(Token::IDENTIFIER);

	if (name != NULL)
	{
		Expression *value = NULL;
		if (mStream.NextIf(Token::ASSIGN) != NULL)
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
//   | STRUCT REF IDENTIFIER '{' native_struct_member_declaration+ '}' ';'
//   | STRUCT NATIVE '<' CONST_UINT [ ',' CONST_UINT ] '>' IDENTIFIER '{' native_struct_member_declaration+ '}' ';'
StructDeclaration *ParserCore::ParseStructDeclaration()
{
	StructDeclaration *declaration = NULL;

	if (mStream.NextIf(Token::KEY_STRUCT) != NULL)
	{
		ScopeStack::Element scopeElement(mScope, SCOPE_STRUCT_MEMBER);
		const Token *native = mStream.NextIf(STRUCT_VARIANT_TYPESET);
		const Token *size = NULL;
		const Token *alignment = NULL;
		StructDeclaration::Variant variant = StructDeclaration::VARIANT_BOND;

		if (native == NULL)
		{
			variant = StructDeclaration::VARIANT_BOND;
		}
		else if (native->GetTokenType() == Token::KEY_NATIVE)
		{
			variant = StructDeclaration::VARIANT_NATIVE;
			ExpectToken(Token::OP_LT);
			size = ExpectToken(INTEGER_CONSTANTS_TYPESET);

			if (mStream.NextIf(Token::COMMA))
			{
				alignment = ExpectToken(INTEGER_CONSTANTS_TYPESET);
			}

			ExpectToken(Token::OP_GT);
		}
		else
		{
			variant = StructDeclaration::VARIANT_REFERENCE;
		}

		const Token *name = ExpectToken(Token::IDENTIFIER);
		ExpectToken(Token::OBRACE);

		declaration = mFactory.CreateStructDeclaration(
			name,
			size,
			alignment,
			NULL,
			NULL,
			variant);

		ParseNodeList<FunctionDefinition> memberFunctionList;
		ParseNodeList<DeclarativeStatement> memberVariableList;

		while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == NULL)
		{
			// Eat up superfluous semicolons.
			if (mStream.NextIf(Token::SEMICOLON) == NULL)
			{
				FunctionDefinition *functionDefinition;
				DeclarativeStatement *declarativeStatement;
				ParseFunctionOrDeclarativeStatement(declaration, &functionDefinition, &declarativeStatement);
				if (functionDefinition != NULL)
				{
					memberFunctionList.Append(functionDefinition);
				}
				else
				{
					AssertNode(declarativeStatement);
					memberVariableList.Append(declarativeStatement);
				}
				SyncToStructMemberTerminator();
			}
		}

		declaration->SetMemberFunctionList(memberFunctionList.GetHead());
		declaration->SetMemberVariableList(memberVariableList.GetHead());
		ExpectToken(Token::CBRACE);
		ExpectDeclarationTerminator();
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
ListParseNode *ParserCore::ParseFunctionOrDeclarativeStatement(StructDeclaration *structDeclaration)
{
	FunctionDefinition *functionDefinition;
	DeclarativeStatement *declarativeStatement;
	ParseFunctionOrDeclarativeStatement(structDeclaration, &functionDefinition, &declarativeStatement);
	if (functionDefinition != NULL)
	{
		return functionDefinition;
	}
	return declarativeStatement;
}


void ParserCore::ParseFunctionOrDeclarativeStatement(
	StructDeclaration *structDeclaration,
	FunctionDefinition **functionDefinition,
	DeclarativeStatement **declarativeStatement)
{
	*functionDefinition = NULL;
	*declarativeStatement = NULL;
	const int startPos = mStream.GetPosition();
	TypeDescriptor *descriptor = ParseTypeDescriptor();

	// Could be a function declaration, function definition or a constant declarative statement.
	if (descriptor != NULL)
	{
		const int namePos = mStream.GetPosition();
		const Token *name = ExpectToken(Token::IDENTIFIER);

		if (name != NULL)
		{
			if (mStream.NextIf(Token::OPAREN) != NULL)
			{
				Parameter *parameterList = ParseParameterList();
				ExpectToken(Token::CPAREN);
				const Token *keywordConst = mStream.NextIf(Token::KEY_CONST);
				bool isConst = keywordConst != NULL;
				TypeDescriptor *thisTypeDescriptor = NULL;

				if (structDeclaration != NULL)
				{
					thisTypeDescriptor = isConst ?
						structDeclaration->GetConstThisTypeDescriptor() :
						structDeclaration->GetThisTypeDescriptor();
				}
				else if (isConst)
				{
					PushError(CompilerError::CONST_NON_MEMBER_FUNCTION, keywordConst, name);
					isConst = false;
				}

				FunctionPrototype *prototype = mFactory.CreateFunctionPrototype(name, descriptor, parameterList, isConst);
				CompoundStatement *body = NULL;
				const Token *obrace = mStream.PeekIf(Token::OBRACE);

				if (obrace != NULL)
				{
					ScopeStack::Element scopeElement(mScope, SCOPE_LOCAL);
					body = ParseCompoundStatement();
					if ((structDeclaration != NULL) && (structDeclaration->IsNative()))
					{
						PushError(CompilerError::NATIVE_MEMBER_FUNCTION_DEFINITION, name);
					}
				}
				else
				{
					ExpectDeclarationTerminator();
					if ((structDeclaration != NULL) && (!structDeclaration->IsNative()))
					{
						PushError(CompilerError::NON_NATIVE_MEMBER_FUNCTION_DECLARATION, name);
					}
				}

				*functionDefinition = mFactory.CreateFunctionDefinition(prototype, body, thisTypeDescriptor, mScope.GetTop());
			}
			else
			{
				// Put the name back into the stream since ParseNamedInitializerList will consume it.
				mStream.SetPosition(namePos);
				NamedInitializer *initializerList = ParseNamedInitializerList(descriptor);
				// TODO: Forgot to handle failure.

				if (initializerList != NULL)
				{
					AssertNonVoidType(descriptor);
					descriptor->SetLValue();
					*declarativeStatement = mFactory.CreateDeclarativeStatement(descriptor, initializerList);
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
}


// parameter_list
//   : parameter
//   | parameter_list ',' parameter
Parameter *ParserCore::ParseParameterList()
{
	Parameter *head = ParseParameter();
	Parameter *current = head;

	while ((current != NULL) && (mStream.NextIf(Token::COMMA) != NULL))
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
	Parameter *parameter = NULL;
	TypeDescriptor *descriptor = ParseTypeDescriptor();

	if (descriptor != NULL)
	{
		if (descriptor->IsArrayType())
		{
			descriptor->ConvertToPointerIntrinsic();
		}
		AssertNonVoidType(descriptor);
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
	return ParseTypeDescriptor(true);
}
TypeDescriptor *ParserCore::ParseTypeDescriptor(bool isRelaxedTypeDescriptor)
{
	TypeDescriptor *descriptor = NULL;
	const int pos = mStream.GetPosition();
	const bool isConst1 = mStream.NextIf(Token::KEY_CONST) != NULL;
	TypeSpecifier *specifier = ParseTypeSpecifier();

	if (specifier != NULL)
	{
		const Token *const2 = mStream.NextIf(Token::KEY_CONST);
		const bool isConst2 = const2 != NULL;
		if (isConst1 && isConst2)
		{
			PushError(CompilerError::DUPLICATE_CONST, const2);
		}

		descriptor = mFactory.CreateTypeDescriptor(specifier, isConst1 || isConst2);
		Expression *lengthTail = NULL;

		const Token *token = mStream.NextIf(TYPE_DESCRIPTORS_TYPESET);
		while (token != NULL)
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
				const bool isConst = mStream.NextIf(Token::KEY_CONST) != NULL;
				descriptor->SetLValue();
				descriptor = mFactory.CreateTypeDescriptor(descriptor, isConst);
				lengthTail = NULL;
			}
			else
			{
				Expression *length = isRelaxedTypeDescriptor ? ParseExpression() : ParseConstExpression();

				const bool lengthAbsent = length == NULL;
				if (lengthAbsent)
				{
					length = mFactory.CreateEmptyExpression();
				}

				ExpectToken(Token::CBRACKET);
				if (lengthTail == NULL)
				{
					if (descriptor->IsVoidType())
					{
						PushError(CompilerError::ARRAY_OF_VOID, token);
					}
					descriptor->ConvertToArray(length);
				}
				else
				{
					if (lengthAbsent)
					{
						PushError(CompilerError::MULTIDIMENTIONAL_ARRAY_BOUNDS, token);
					}
					lengthTail->SetNextNode(length);
				}
				lengthTail = length;
			}
			token = mStream.NextIf(TYPE_DESCRIPTORS_TYPESET);
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

	if (specifier == NULL)
	{
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier();
		if (identifier != NULL)
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
//   | UCHAR
//   | SHORT
//   | USHORT
//   | INT
//   | UINT
//   | LONG
//   | ULONG
//   | FLOAT
//   | DOUBLE
TypeSpecifier *ParserCore::ParsePrimitiveTypeSpecifier()
{
	TypeSpecifier *specifier = NULL;
	const Token *primitiveType = mStream.NextIf(PRIMITIVE_TYPE_SPECIFIERS_TYPESET);

	if (primitiveType != NULL)
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

	while ((current != NULL) && (mStream.NextIf(Token::COMMA) != NULL))
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
	NamedInitializer *namedInitializer = NULL;
	const Token *name = mStream.NextIf(Token::IDENTIFIER);

	if (name != NULL)
	{
		const Token *assign = mStream.NextIf(Token::ASSIGN);
		Initializer *initializer = NULL;

		if (assign != NULL)
		{
			initializer = ParseInitializer();
			if (mScope.GetTop() == SCOPE_STRUCT_MEMBER)
			{
				PushError(CompilerError::INITIALIZER_NOT_ALLOWED, assign);
			}
			AssertNode(initializer);
		}

		namedInitializer = mFactory.CreateNamedInitializer(name, initializer, typeDescriptor, mScope.GetTop());
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
	Initializer *initializer = NULL;

	if (mStream.NextIf(Token::OBRACE))
	{
		ParseNodeList<Initializer> initializerList;

		while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == NULL)
		{
			Initializer *next = ParseInitializer();
			AssertNode(next);
			SyncToInitializerDelimiter();

			// Note that the comma on the last initializer is optional.
			if (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == NULL)
			{
				ExpectToken(Token::COMMA);
			}

			initializerList.Append(next);
		}

		ExpectToken(Token::CBRACE);
		initializer = mFactory.CreateInitializer(initializerList.GetHead());
	}
	else
	{
		Expression *expression = ParseAssignmentExpression();
		if (expression != NULL)
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
	QualifiedIdentifier *head = NULL;
	const Token *name = mStream.NextIf(Token::IDENTIFIER);

	if (name != NULL)
	{
		head = mFactory.CreateQualifiedIdentifier(name);
		QualifiedIdentifier *current = head;

		while ((current != NULL) && mStream.NextIf(Token::SCOPE))
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
	ListParseNode *statement = NULL;

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
	CompoundStatement *compoundStatement = NULL;

	if (mStream.NextIf(Token::OBRACE))
	{
		ParseNodeList<ListParseNode> statementList;

		while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == NULL)
		{
			ListParseNode *next = ParseStatement();
			AssertNode(next);
			SyncToStatementTerminator();
			statementList.Append(next);
		}

		ExpectToken(Token::CBRACE);
		compoundStatement = mFactory.CreateCompoundStatement(statementList.GetHead());
	}

	return compoundStatement;
}


// if_statement
//   : IF '(' expression ')' statement [ELSE statement]
IfStatement *ParserCore::ParseIfStatement()
{
	IfStatement *ifStatement = NULL;
	const Token *keyword = mStream.NextIf(Token::KEY_IF);

	if (keyword)
	{
		ExpectToken(Token::OPAREN);
		Expression *condition = ParseExpression();
		AssertNode(condition);
		ExpectToken(Token::CPAREN);
		ParseNode *thenStatement = ParseStatement();
		AssertNode(thenStatement);
		ParseNode *elseStatement = NULL;

		if (mStream.NextIf(Token::KEY_ELSE))
		{
			elseStatement = ParseStatement();
			AssertNode(elseStatement);
		}

		ifStatement = mFactory.CreateIfStatement(keyword, condition, thenStatement, elseStatement);
	}

	return ifStatement;
}


// switch_statement
//   : SWITCH '(' expression ')' '{' switch_section* '}'
SwitchStatement *ParserCore::ParseSwitchStatement()
{
	SwitchStatement *switchStatement = NULL;
	const Token *keyword = mStream.NextIf(Token::KEY_SWITCH);

	if (keyword != NULL)
	{
		ExpectToken(Token::OPAREN);
		Expression *control = ParseExpression();
		AssertNode(control);
		ExpectToken(Token::CPAREN);
		ExpectToken(Token::OBRACE);

		ParseNodeList<SwitchSection> sectionList;
		while (mStream.PeekIf(BLOCK_DELIMITERS_TYPESET) == NULL)
		{
			SwitchSection *next = ParseSwitchSection();
			sectionList.Append(next);
		}

		ExpectToken(Token::CBRACE);

		if (sectionList.GetHead() == NULL)
		{
			PushError(CompilerError::EMPTY_SWITCH_STATEMENT, keyword);
		}

		switchStatement = mFactory.CreateSwitchStatement(keyword, control, sectionList.GetHead());
	}

	return switchStatement;
}


// switch_section
//   : switch_label+ statement+
SwitchSection *ParserCore::ParseSwitchSection()
{
	SwitchSection *section = NULL;
	SwitchLabel *labelList = ParseSwitchLabel();
	SwitchLabel *currentLabel = labelList;
	while (currentLabel != NULL)
	{
		SwitchLabel *next = ParseSwitchLabel();
		currentLabel->SetNextNode(next);
		currentLabel = next;
	}

	if (labelList == NULL)
	{
		PushError(CompilerError::EMPTY_SWITCH_LABEL_LIST, mStream.Peek());
	}

	ParseNodeList<ListParseNode> statementList;
	while (mStream.PeekIf(SWITCH_SECTION_DELIMITERS_TYPESET) == NULL)
	{
		ListParseNode *next = ParseStatement();
		AssertNode(next);
		SyncToStatementTerminator();
		statementList.Append(next);
	}

	section = mFactory.CreateSwitchSection(labelList, statementList.GetHead());

	return section;
}


// switch_label
//   : CASE const_expression ':'
//   | DEFAULT ':'
SwitchLabel *ParserCore::ParseSwitchLabel()
{
	SwitchLabel *label = NULL;
	const Token *labelToken = mStream.NextIf(SWITCH_LABELS_TYPESET);

	if (labelToken != NULL)
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
	WhileStatement *whileStatement = NULL;
	const Token *keyword = mStream.NextIf(Token::KEY_WHILE);

	if (keyword)
	{
		ExpectToken(Token::OPAREN);
		Expression *condition = ParseExpression();
		AssertNode(condition);
		ExpectToken(Token::CPAREN);
		ParseNode *body = ParseStatement();
		AssertNode(body);
		whileStatement = mFactory.CreateWhileStatement(keyword, condition, body);
	}

	return whileStatement;
}


// do_statement
//   : DO statement WHILE '(' expression ')' ';'
WhileStatement *ParserCore::ParseDoWhileStatement()
{
	WhileStatement *whileStatement = NULL;
	const Token *keyword = mStream.NextIf(Token::KEY_DO);

	if (keyword)
	{
		ParseNode *body = ParseStatement();
		AssertNode(body);
		ExpectToken(Token::KEY_WHILE);
		ExpectToken(Token::OPAREN);
		Expression *condition = ParseExpression();
		AssertNode(condition);
		ExpectToken(Token::CPAREN);
		ExpectToken(Token::SEMICOLON);
		whileStatement = mFactory.CreateWhileStatement(keyword, condition, body);
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
	ForStatement *forStatement = NULL;
	const Token *keyword = mStream.NextIf(Token::KEY_FOR);

	if (keyword)
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
		forStatement = mFactory.CreateForStatement(keyword, initializer, condition, countingExpression, body);
	}

	return forStatement;
}


// jump_statement
//   : CONTINUE ';'
//   | BREAK ';'
//   | RETURN [expression] ';'
JumpStatement *ParserCore::ParseJumpStatement()
{
	JumpStatement *jumpStatement = NULL;
	const Token *keyword = mStream.NextIf(JUMP_KEYWORDS_TYPESET);

	if (keyword != NULL)
	{
		Expression *rhs = NULL;
		if (keyword->GetTokenType() == Token::KEY_RETURN)
		{
			rhs = ParseExpression();
		}
		ExpectStatementTerminator();
		jumpStatement = mFactory.CreateJumpStatement(keyword, rhs);
	}

	return jumpStatement;
}


// declarative_statement
//   : type_descriptor named_initializer_list ';'
ListParseNode *ParserCore::ParseExpressionOrDeclarativeStatement()
{
	ListParseNode *statement = NULL;
	const int startPos = mStream.GetPosition();

	// The grammar is somewhat ambiguous. Since a qualified identifier followed by '*' tokens and array
	// index operators can appear like a type descriptor as well as an expression, we'll treat anything
	// that fits the profile of a declaration as such and everything else like an expression statement.
	TypeDescriptor *descriptor = ParseRelaxedTypeDescriptor();
	if (descriptor != NULL)
	{
		NamedInitializer *initializerList = ParseNamedInitializerList(descriptor);

		if (initializerList != NULL)
		{
			AssertNonVoidType(descriptor);
			descriptor->SetLValue();
			statement = mFactory.CreateDeclarativeStatement(descriptor, initializerList);
			ExpectStatementTerminator();
		}
		else
		{
			// The tokens that looked like a type descriptor, might actually be part of an expression.
			const int descriptorPos = mStream.GetPosition();
			mFactory.DestroyHierarchy(descriptor);
			descriptor = NULL;
			mStream.SetPosition(startPos);

			statement = ParseExpressionStatement();

			if (statement == NULL)
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
	ExpressionStatement *expressionStatement = NULL;
	Expression *expression = ParseExpression();

	if (expression != NULL)
	{
		ExpectStatementTerminator();
		expressionStatement = mFactory.CreateExpressionStatement(expression);
	}
	else if (mStream.NextIf(Token::SEMICOLON))
	{
		expressionStatement = mFactory.CreateExpressionStatement(NULL);
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
	Expression *expression = NULL;

	if (mParseConstExpressions.GetTop())
	{
		expression = ParseConditionalExpression();
	}
	else
	{
		expression = ParseAssignmentExpression();
 
		if (expression != NULL)
 		{
			const Token *token = mStream.NextIf(Token::COMMA);
			while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(ASSIGNMENT_OPERATORS_TYPESET);
		if (token != NULL)
		{
			AssertNonConstExpression(CompilerError::INVALID_OPERATOR_IN_CONST_EXPRESSION, token);
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(Token::OP_TERNARY);
		if (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(Token::OP_OR);
		while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(Token::OP_AND);
		while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(Token::OP_BIT_OR);
		while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(Token::OP_BIT_XOR);
		while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(Token::OP_AMP);
		while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(EQUALITY_OPERATORS_TYPESET);
		while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(RELATIONAL_OPERATORS_TYPESET);
		while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(SHIFT_OPERATORS_TYPESET);
		while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(ADDITIVE_OPERATORS_TYPESET);
		while (token != NULL)
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(MULTIPLICATIVE_OPERATORS_TYPESET);
		while (token != NULL)
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
	Expression *expression = NULL;
	const Token *token = mStream.NextIf(Token::KEY_CAST);

	if (token != NULL)
	{
		ExpectToken(Token::OP_LT);
		TypeDescriptor *descriptor = ParseTypeDescriptor();
		AssertNode(descriptor);
		AssertNonVoidType(descriptor);
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
	Expression *expression = NULL;
	const Token *op = NULL;

	if ((op = mStream.NextIf(UNARY_OPERATORS_TYPESET)) != NULL)
	{
		Expression *rhs = NULL;

		switch (op->GetTokenType())
		{
			case Token::OP_INC:
			case Token::OP_DEC:
				rhs = ParseUnaryExpression();
				AssertNonConstExpression(CompilerError::INVALID_OPERATOR_IN_CONST_EXPRESSION, op);
				break;

			default:
				rhs = ParseCastExpression();
				break;
		}

		AssertNode(rhs);
		expression = mFactory.CreateUnaryExpression(op, rhs);
	}

	else if ((op = mStream.NextIf(Token::KEY_SIZEOF)) != NULL)
	{
		if (mStream.NextIf(Token::OP_LT) != NULL)
		{
			TypeDescriptor *descriptor = ParseTypeDescriptor();
			AssertNode(descriptor);
			AssertNonVoidType(descriptor);
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

	if (expression != NULL)
	{
		const Token *token = mStream.NextIf(POSTFIX_OPERATORS_TYPESET);
		while (token != NULL)
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
					AssertNonConstExpression(CompilerError::FUNCTION_CALL_IN_CONST_EXPRESSION, token);
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
					AssertNonConstExpression(CompilerError::INVALID_OPERATOR_IN_CONST_EXPRESSION, token);
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
//   | THIS
//   | '(' expression ')'
Expression *ParserCore::ParsePrimaryExpression()
{
	Expression *expression = NULL;
	const Token *token = mStream.NextIf(CONSTANT_VALUES_TYPESET);

	if (token != NULL)
	{
		expression = mFactory.CreateConstantExpression(token);
	}
	else if ((token = mStream.NextIf(Token::KEY_THIS)) != NULL)
	{
		expression = mFactory.CreateThisExpression(token);
	}
	else if (mStream.NextIf(Token::OPAREN) != NULL)
	{
		expression = ParseExpression();
		AssertNode(expression);
		ExpectToken(Token::CPAREN);
	}
	else
	{
		QualifiedIdentifier *identifier = ParseQualifiedIdentifier();
		if (identifier != NULL)
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

	while ((current != NULL) && (mStream.NextIf(Token::COMMA) != NULL))
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
	if (token == NULL)
	{
		PushUnrecoverableError(CompilerError::UNEXPECTED_TOKEN, mStream.Peek(), Token::GetTokenName(expectedType));
	}
	return token;
}


const Token *ParserCore::ExpectToken(const TokenTypeSet &expectedTypes)
{
	const Token *token = mStream.NextIf(expectedTypes);
	if (token == NULL)
	{
		PushUnrecoverableError(CompilerError::UNEXPECTED_TOKEN, mStream.Peek(), expectedTypes.GetTypeName());
	}
	return token;
}


void ParserCore::AssertNode(ParseNode *node)
{
	if (node == NULL)
	{
		PushUnrecoverableError(CompilerError::PARSE_ERROR, mStream.Peek());
	}
}


void ParserCore::AssertNonConstExpression(CompilerError::Type errorType, const Token *context)
{
	if (mParseConstExpressions.GetTop())
	{
		PushError(errorType, context);
	}
}


void ParserCore::AssertNonVoidType(const TypeDescriptor *typeDescriptor)
{
	if ((typeDescriptor != NULL) && typeDescriptor->IsVoidType())
	{
		PushError(CompilerError::VOID_NOT_ALLOWED, typeDescriptor->GetContextToken());
	}
}


void ParserCore::PushUnrecoverableError(CompilerError::Type type, const Token *context, const void *arg)
{
	if (!mHasUnrecoveredError)
	{
		mErrorBuffer.PushError(type, context, arg);
		mHasUnrecoveredError = true;
	}
}


void ParserCore::PushError(CompilerError::Type type, const Token *context, const void *arg)
{
	if (!mHasUnrecoveredError)
	{
		mErrorBuffer.PushError(type, context, arg);
	}
}

}
