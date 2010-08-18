#include <stdio.h>
#include "bond/lexer.h"
#include "bond/parser.h"

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
	const char *fileName = "../scripts/parse.bond";
	Script script = ReadScript(fileName);
	Bond::Lexer lexer;
	lexer.Lex(script.text, script.length);
	Bond::TokenStream stream = lexer.GetTokenStream();
	Bond::Parser parser;
	parser.Parse(stream);

	const int numErrors = parser.GetNumErrors();
	for (int i = 0; i < numErrors; ++i)
	{
		const Bond::Parser::Error *error = parser.GetError(i);
		const Bond::Token *token = error->token;
		const Bond::StreamPos &pos = token->GetStartPos();
		printf("Error %d (%d, %d): expected %s before '%s'\n",
			error->type,
			pos.line,
			pos.column,
			Bond::Token::GetTokenName(error->expectedType),
			token->GetText());
	}

	return 0;
}
