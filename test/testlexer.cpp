#include <stdio.h>
#include "bond/lexer.h"

struct Script
{
	const char *text;
	int length;
};

Script ReadScript(const char *fileName)
{
	Script script;
	FILE *scriptFile = fopen(fileName, "r");
	fseek(scriptFile, 0, SEEK_END);
	script.length = (int) ftell(scriptFile);
	fseek(scriptFile, 0, SEEK_SET);
	char *buffer = new char[script.length];
	fread(buffer, sizeof(char), script.length, scriptFile);
	script.text = buffer;
	fclose(scriptFile);
	return script;
}

int main()
{
	const char *fileName = "../scripts/lex.bond";
	Script script = ReadScript(fileName);
	Bond::Lexer lexer;
	lexer.Lex(script.text, script.length);

	int numTokens = lexer.GetNumTokens();
	for (int i = 0; i < numTokens; ++i)
	{
		const Bond::Token &token = lexer.GetToken(i);
		const Bond::StreamPos &start = token.GetStartPos();
		const Bond::StreamPos &end = token.GetEndPos();
		printf("%-12s i:%-3d %-3d l:%-3d %-3d c:%-3d %-3d '%s'",
			token.GetTokenName(), start.index, end.index, start.line, end.line,
			start.column, end.column, token.GetText());

		if (token.HasAnnotation(Bond::Token::OCTAL))
		{
			printf(" O");
		}
		if (token.HasAnnotation(Bond::Token::HEX))
		{
			printf(" X");
		}

		switch (token.GetTokenType())
		{
			case Bond::Token::INVALID:
				printf(" %s c:%d", token.GetErrorName(), token.GetErrorPos().column);
				break;

			case Bond::Token::CONST_BOOL:
				printf(" %s", token.GetBoolValue() ? "true" : "false");
				break;

			case Bond::Token::CONST_CHAR:
				printf(" %c", token.GetCharValue());
				break;

			case Bond::Token::CONST_FLOAT:
				printf(" %g", token.GetFloatValue());
				break;

			case Bond::Token::CONST_INT:
				printf(" %d", token.GetIntValue());
				break;

			case Bond::Token::CONST_UINT:
				printf(" %u", token.GetUIntValue());
				break;

			case Bond::Token::CONST_STRING:
				printf(" %s", token.GetStringValue());
				break;

			default:
				break;
		}

		printf("\n");

		if (token.GetTokenType() == Bond::Token::END)
		{
			break;
		}
	}

	return 0;
}
