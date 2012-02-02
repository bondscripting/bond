#include "bond/codegenerator.h"
#include "bond/defaultallocator.h"
#include "bond/defaultfileloader.h"
#include "bond/filebinarywriter.h"
#include "bond/lexer.h"
#include "bond/parseerror.h"
#include "bond/parser.h"
#include "bond/semanticanalyzer.h"
#include "bond/stdouttextwriter.h"
#include "bond/opcodes.h"
#include <stdio.h>

void PrintErrors(Bond::TextWriter &writer, const Bond::ParseErrorBuffer &errorBuffer)
{
	if (errorBuffer.HasErrors())
	{
		for (int i = 0; i < errorBuffer.GetNumErrors(); ++i)
		{
			const Bond::ParseError *error = errorBuffer.GetError(i);
			error->Print(writer);
			writer.Write("\n");
		}
	}
}


void Compile(const char *scriptName)
{
	Bond::DefaultAllocator allocator;
	Bond::DefaultFileLoader fileLoader(allocator);
	Bond::FileData script = fileLoader.LoadFile(scriptName);
	if (script.mData != NULL)
	{
		Bond::Lexer lexer(allocator);
		lexer.Lex(script.mData, script.mLength);
		fileLoader.DisposeFile(script);

		Bond::TokenStream stream = lexer.GetTokenStream();
		Bond::ParseErrorBuffer errorBuffer;
		Bond::Parser parser(allocator, errorBuffer);
		parser.Parse(stream);

		if (!errorBuffer.HasErrors())
		{
			Bond::SemanticAnalyzer analyzer(errorBuffer);
			analyzer.Analyze(parser.GetTranslationUnitList());

			if (!errorBuffer.HasErrors())
			{
				FILE *outputFile = fopen("bond.cbo", "wb");
				if (outputFile != NULL)
				{
					Bond::FileBinaryWriter cboWriter(outputFile);
					Bond::CodeGenerator generator(allocator);
					generator.Generate(parser.GetTranslationUnitList(), cboWriter);
					fclose(outputFile);
				}
			}
		}

		Bond::StdOutTextWriter errorWriter;
		PrintErrors(errorWriter, errorBuffer);
	}
}


int main(int argc, const char *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		Compile(argv[i]);
	}
	printf("%d\n", Bond::OPCODE_MAX);

	return 0;
}
