// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CodeConverter.h"
#include "TAAssert.h"

using namespace TA_Base_Core;

void test_CodeConverTer()
{
    std::wstring str = L"Hello, World!\nƒ„∫√£¨ ¿ΩÁ£°";
    std::string utf8 = CodeConverter::UnicodeToUTF8(str);
    std::wstring uncode = CodeConverter::UTF8ToUnicode(utf8);
    TA_Assert(str == uncode);
    std::string ansi = CodeConverter::UnicodeToANSI(uncode);
    std::string utf8_2 = CodeConverter::ANSIToUTF8(ansi);
    TA_Assert(utf8 == utf8_2);
    std::cout << ansi << std::endl;
}
