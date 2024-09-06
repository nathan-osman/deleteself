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

#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Make sure exactly two parameters were specified; convert the first to a long
    int nargs;
    LPWSTR *szArgList = CommandLineToArgvW(pCmdLine, &nargs);
    if (nargs != 3) {
        return ERROR_BAD_ARGUMENTS;
    }
    long pid = wcstol(szArgList[1], nullptr, 10);

    // Attempt to open the process using its PID
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (hProcess == NULL) {
        return ERROR_INVALID_HANDLE;
    }

    // Wait for the application to terminate
    WaitForSingleObject(hProcess, INFINITE);
    CloseHandle(hProcess);

    // Now delete the image on disk
    if (DeleteFileW(szArgList[2]) == FALSE) {
        return ERROR_FILE_NOT_FOUND;
    }

    return 0;
}
