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
	const char *fileName = "scripts/lex.bond";
	Script script = ReadScript(fileName);
	Bond::Lexer lexer;
	lexer.SetText(fileName, script.text, script.length);

	//for (int i = 0; i < script.length; ++i)
	//{
	//	printf("%c", script.text[i]);
	//}

	while (true)
	{
		Bond::Token token = lexer.NextToken();
		printf("%s '%s'\n", token.GetTokenName(), token.GetText());
		if (token.GetTokenType() == Bond::Token::END)
		{
			break;
		}
	}

	return 0;
}
