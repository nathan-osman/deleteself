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

#define STATUS_SUCCESS 0x00000000L

#define NtCurrentProcess() ((HANDLE) -1)

enum SECTION_INHERIT {
	ViewUnmap = 2
};

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

typedef NTSTATUS(__stdcall *_NtMapViewOfSection)(
	HANDLE          SectionHandle,
	HANDLE          ProcessHandle,
	PVOID           *BaseAddress,
	ULONG_PTR       ZeroBits,
	SIZE_T          CommitSize,
	PLARGE_INTEGER  SectionOffset,
	PSIZE_T         ViewSize,
	SECTION_INHERIT InheritDisposition,
	ULONG           AllocationType,
	ULONG           Win32Protect
);

typedef NTSTATUS(__stdcall *_NtUnmapViewOfSection)(
	HANDLE ProcessHandle,
	PVOID  BaseAddress
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

_NtCreateSection      NtCreateSection;
_NtMapViewOfSection   NtMapViewOfSection;
_NtUnmapViewOfSection NtUnmapViewOfSection;
_NtCreateProcess      NtCreateProcess;

/**
 * Load the entrypoints for required functions
 * @return true if the entrypoints were loaded
 */
bool loadEntrypoints()
{
	NtCreateSection      = (_NtCreateSection)      GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtCreateSection");
	NtMapViewOfSection   = (_NtMapViewOfSection)   GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtMapViewOfSection");
	NtUnmapViewOfSection = (_NtUnmapViewOfSection) GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtUnmapViewOfSection");
	NtCreateProcess      = (_NtCreateProcess)      GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtCreateProcess");

	// Return true only if all of the entrypoints were loaded
	return NtCreateSection && NtMapViewOfSection && NtCreateProcess;
}

/**
 * Create a new section with the specified content
 * @param sectionHandle  pointer to the handle for the new section
 * @param sectionContent content for the new section
 * @param sectionSize    size of the content in bytes
 * @return STATUS_SUCCESS if the section was created
 */
NTSTATUS createSection(
	PHANDLE    sectionHandle,
	const VOID *sectionContent,
	SIZE_T     sectionSize
)
{
	NTSTATUS status;

	// Allocate enough bytes for the section (the size will be increased to the
	// nearest multiple of PAGE_SIZE automatically)
	LARGE_INTEGER maximumSize;
	maximumSize.QuadPart = sectionSize;

	// Attempt to create the section
	status = NtCreateSection(
		sectionHandle,
		SECTION_ALL_ACCESS,
		NULL,
		&maximumSize,
		PAGE_READWRITE,
		SEC_COMMIT,
		NULL
	);
	if (status != STATUS_SUCCESS) {
		return status;
	}

	// Map the section in order to copy the content into it
	PVOID baseAddress = NULL;
	SIZE_T viewSize = 0;
	status = NtMapViewOfSection(
		*sectionHandle,
		NtCurrentProcess(),
		&baseAddress,
		0,
		sectionSize,
		NULL,
		&viewSize,
		ViewUnmap,
		0,
		PAGE_READWRITE
	);
	if (status != STATUS_SUCCESS) {
		return status;
	}

	// Perform the copy operation
	CopyMemory(baseAddress, sectionContent, sectionSize);

	// Unmap the section
	return NtUnmapViewOfSection(
		NtCurrentProcess(),
		baseAddress
	);
}

int main(int argc, char **argv)
{
	NTSTATUS status;

	// Load the entrypoints
	if (!loadEntrypoints()) {
		std::cerr << "unable to load entrypoints" << std::endl;
		return 1;
	}

	// Create the .rodata section
	HANDLE rodataSection;
	status = createSection(
		&rodataSection,
		"1234567890",
		10
	);
	if (status != STATUS_SUCCESS) {
		std::cerr << "unable to create .rodata section: " << std::hex << status << std::endl;
		return 1;
	}

    return 0;
}
