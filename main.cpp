/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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
 **/

#include <iostream>

#include <windows.h>

#define STATUS_SUCCESS       0x00000000L

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG           Length;
	HANDLE          RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG           Attributes;
	PVOID           SecurityDescriptor;
	PVOID           SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef NTSTATUS(__stdcall *_NtCreateSection)(
	PHANDLE            SectionHandle,
	ULONG              DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PLARGE_INTEGER     MaximumSize,
	ULONG              PageAttributes,
	ULONG              SectionAttributes,
	HANDLE             FileHandle
);

typedef NTSTATUS(__stdcall *_NtCreateProcess)(
	PHANDLE            ProcessHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	HANDLE             ParentProcess,
	BOOLEAN            InheritObjectTable,
	HANDLE             SectionHandle,
	HANDLE             DebugPort,
	HANDLE             ExceptionPort
);

const char *MessageTitle = "Title";
const char *MessageText  = "Message body";

int main(int argc, char **argv)
{
	// Obtain the required entrypoints
	_NtCreateSection NtCreateSection = (_NtCreateSection) GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtCreateSection");
	_NtCreateProcess NtCreateProcess = (_NtCreateProcess) GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtCreateProcess");
	if (!NtCreateSection || !NtCreateProcess) {
		std::cerr << "unable to find entrypoints" << std::endl;
		return 1;
	}

	// Create a section of at least 1024 bytes (plenty for storing data)
	LARGE_INTEGER rodataSize;
	rodataSize.QuadPart = 1000;

	// Create the .rodata section
	HANDLE rodataHandle;
	NTSTATUS status = NtCreateSection(
		&rodataHandle,
		SECTION_ALL_ACCESS,
		NULL,
		&rodataSize,
		PAGE_READONLY,
		SEC_COMMIT,
		NULL
	);
	if (status != STATUS_SUCCESS) {
		std::cerr << "unable to create section: " << std::hex << status << std::endl;
		return 1;
	}

    return 0;
}
