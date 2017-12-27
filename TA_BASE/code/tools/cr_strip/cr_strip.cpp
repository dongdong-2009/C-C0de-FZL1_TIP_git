// cr_strip.cpp : Defines the entry point for the console application.
//
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: $ 
  * @author:  
  * @version: $Revision: $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  */
#include "stdafx.h"
#include <iostream>
#include <fstream.h>
#include <io.h>


bool cr_strip(const char *fileName)
{
	cout << "Processing: '" << fileName << "'...";
	// open for reading, binary
	ifstream in(fileName, ios::in | ios::binary);

	if (in)
	{
		// create temp file for writing
		std::string fName(fileName);
		std::string ext(".cr_temp");

		std::string outName = fName;
		outName += ext;
		
		ofstream out(outName.c_str(), ios::out | ios::binary);
		if (out)
		{
			// we parse the input file, one char at a time.
			bool bCR = false;
			while (!in.eof())
			{
				char x;
				in.read(&x,1);
				if (0x0D == x) // if carriage return
				{
					if (! bCR) // if first one
					{
						bCR = true;
						out << x;
					}
				}
				else
				{
					out << x;
					bCR = false;
				}
			}

			out.close();
			in.close();
			// replace the original file with the new one
			if ( 0 == remove(fileName))
			{
				rename(outName.c_str(), fileName);
				cout << " Done.\n";

			}
			else
			{
				cout << "Could not overwrite '" << fileName << "'. Is it write protected?\n";
				remove(outName.c_str());
			}
		}

	}

	return true;
}

void show_usage(void)
{
	cout << "Usage: cr_strip [file] [file] ... [file]";
	cout << "\n\nStrips any extra carriage returns from text files.\n";
	cout << "Note that [file] can contain wildcards. For example...\n\n";
	cout << "cr_strip *.cpp *.h\n\n";
	cout << "... would process all .cpp and .h files in the current directory\n";
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		show_usage();
		return 0;
	}

	for (int i = 1; i < argc; i++)
	{
	
		struct _finddata_t c_file;
	    long hFile;

		/* Find first file in current directory that matches the argument*/
		if( (hFile = _findfirst( argv[i], &c_file )) == -1L )
			cout << "Could not find any file that matches '" << argv[i] << "'.\n";
		else
		{
			// process all files found
			cr_strip(c_file.name);
			while (_findnext( hFile, &c_file ) == 0)
			{
				cr_strip(c_file.name);
			}

			_findclose(hFile);
		}
	}

	return 0;
}
