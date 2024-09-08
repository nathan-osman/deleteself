/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <iomanip>
#include <iostream>
#include <fstream>

int convert(char *inputFilename, char *outputFilename)
{
    std::ifstream inputFile(inputFilename, std::ios::binary);
    std::ofstream outputFile(outputFilename);

    if (!inputFile) {
        std::cerr << "unable to open " << inputFilename << std::endl;
        return 1;
    }

    if (!outputFile) {
        std::cerr << "unable to open " << outputFilename << std::endl;
        return 1;
    }

    // Write the preamble
    outputFile << "/** WARNING: this is an auto-generated file **/";
    outputFile << std::endl << std::endl;
    outputFile << "const char *StubImage = {" << std::endl;

    // Write the bytes
    char ch;
    for (int i = 1; inputFile.get(ch); i++) {
        outputFile << "0x"
                   << std::hex
                   << std::setw(2)
                   << std::setfill('0')
                   << (static_cast<int>(ch) & 0xff)
                   << ",";
        if (i % 20) {
            outputFile << " ";
        } else {
            outputFile << std::endl;
        }
    }

    // Complete the file
    outputFile << std::endl << "};" << std::endl;

    inputFile.close();
    outputFile.close();

    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <exe input> <cpp output>" << std::endl;
        return 1;
    }

    return convert(argv[1], argv[2]);
}
