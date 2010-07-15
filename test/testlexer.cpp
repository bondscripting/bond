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

	//for (int i = 0; i < script.length; ++i)
	//{
	//	printf("%c", script.text[i]);
	//}

	int numTokens = lexer.GetNumTokens();
	for (int i = 0; i < numTokens; ++i)
	{
		const Bond::Token &token = lexer.GetToken(i);
		const Bond::StreamPos &start = token.GetStartPos();
		const Bond::StreamPos &end = token.GetEndPos();
		printf("%-14s i:%-3d %-3d l:%-3d %-3d c:%-3d %-3d '%s'\n",
			token.GetTokenName(), start.index, end.index, start.line, end.line,
			start.column, end.column, token.GetText());

		if (token.GetTokenType() == Bond::Token::END)
		{
			break;
		}
	}

	return 0;
}
