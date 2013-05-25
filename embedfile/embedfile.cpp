#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

void CopyTemplateHeader(FILE *templateFile, FILE *outputFile, const char *substitutionString)
{
	if ((outputFile != NULL) && (templateFile != NULL))
	{
		char buffer[LINE_MAX];
		while ((fgets(buffer, sizeof(buffer), templateFile) != NULL) &&
		       (strstr(buffer, substitutionString) == NULL))
		{
			fprintf(outputFile, "%s", buffer);
		}
	}
}

void CopyTemplateFooter(FILE *templateFile, FILE *outputFile)
{
	if ((outputFile != NULL) && (templateFile != NULL))
	{
		char buffer[LINE_MAX];
		while (fgets(buffer, sizeof(buffer), templateFile) != NULL)
		{
			fprintf(outputFile, "%s", buffer);
		}
	}
}


void EmbedFile(FILE *cppFile, FILE *hFile, const char *inputFileName)
{
	char baseName[LINE_MAX];
	const char *n = inputFileName;
	size_t j = 0;
	while ((*n != '\0') && (j < (LINE_MAX - 1)))
	{
		const char c = *n++;
		baseName[j++] = isalnum(c) ? toupper(c) : '_';
	}
	baseName[j] = '\0';

	if (hFile != NULL)
	{
		fprintf(hFile, "\nextern const Bond::FileData %s;\n", baseName);
	}

	if (cppFile != NULL)
	{
		const unsigned BUFFER_SIZE = 16;
		unsigned char buffer[BUFFER_SIZE];
		FILE *inputFile = fopen(inputFileName, "rb");
		if (inputFile != NULL)
		{
			fprintf(cppFile, "\nextern const Bond::bu8_t %s_DATA[] =\n{\n", baseName);

			while (!feof(inputFile))
			{
				const size_t numBytes = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, inputFile);

				if (numBytes > 0)
				{
					for (size_t i = 0; i < numBytes; ++i)
					{
						fprintf(cppFile, "%c0x%02X,", (i > 0) ? ' ' : '\t', buffer[i]);
					}
					fprintf(cppFile, "\n");
				}
			}

			fprintf(cppFile, "};\n\nextern const Bond::FileData %s(%s_DATA, sizeof(%s_DATA));\n", baseName, baseName, baseName);
			fclose(inputFile);
		}
	}
}


int main(int argc, const char *argv[])
{
	const size_t MAX_ENTRIES = 256;
	const char *cppFileName = NULL;
	const char *cppTemplateFileName = NULL;
	const char *hFileName = NULL;
	const char *hTemplateFileName = NULL;
	const char *inputFileNames[MAX_ENTRIES];
	const char *substitutionString = "#*# EMBED FILES #*#";
	size_t numInputFiles = 0;

	bool error = false;
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-c") == 0)
		{
			if (++i < argc)
			{
				cppFileName = argv[i];
			}
			else
			{
				fprintf(stderr, "Missing argument to -c\n");
				error = true;
			}
		}
		if (strcmp(argv[i], "-C") == 0)
		{
			if (++i < argc)
			{
				cppTemplateFileName = argv[i];
			}
			else
			{
				fprintf(stderr, "Missing argument to -C\n");
				error = true;
			}
		}
		else if (strcmp(argv[i], "-h") == 0)
		{
			if (++i < argc)
			{
				hFileName = argv[i];
			}
			else
			{
				fprintf(stderr, "Missing argument to -h\n");
				error = true;
			}
		}
		else if (strcmp(argv[i], "-H") == 0)
		{
			if (++i < argc)
			{
				hTemplateFileName = argv[i];
			}
			else
			{
				fprintf(stderr, "Missing argument to -H\n");
				error = true;
			}
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			if (++i < argc)
			{
				substitutionString = argv[i];
			}
			else
			{
				fprintf(stderr, "Missing argument to -s\n");
				error = true;
			}
		}
		else if (argv[i][0] == '-')
		{
			fprintf(stderr, "Unknown option '%s'\n", argv[i]);
			error = true;
		}
		else if (numInputFiles < MAX_ENTRIES)
		{
			inputFileNames[numInputFiles++] = argv[i];
		}
	}

	if (error)
	{
		return 1;
	}

	FILE *cppFile = NULL;
	FILE *cppTemplateFile = NULL;
	FILE *hFile = NULL;
	FILE *hTemplateFile = NULL;

	if (cppFileName != NULL)
	{
		cppFile = fopen(cppFileName, "w");
		if (cppTemplateFileName != NULL)
		{
			cppTemplateFile = fopen(cppTemplateFileName, "r");
		}
	}

	if (hFileName != NULL)
	{
		hFile = fopen(hFileName, "w");
		if (hTemplateFileName != NULL)
		{
			hTemplateFile = fopen(hTemplateFileName, "r");
		}
	}

	if (hFile != NULL)
	{
		fprintf(hFile, "#include \"bond/io/fileloader.h\"\n");
	}

	CopyTemplateHeader(hTemplateFile, hFile, substitutionString);
	CopyTemplateHeader(cppTemplateFile, cppFile, substitutionString);

	for (size_t i = 0; i < numInputFiles; ++i)
	{
		EmbedFile(cppFile, hFile, inputFileNames[i]);
	}

	CopyTemplateFooter(hTemplateFile, hFile);
	CopyTemplateFooter(cppTemplateFile, cppFile);

	fclose(cppFile);
	fclose(cppTemplateFile);
	fclose(hFile);
	fclose(hTemplateFile);

	return 0;
}
