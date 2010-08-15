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

	return 0;
}
