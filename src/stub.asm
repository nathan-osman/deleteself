; The MIT License (MIT)
;
; Copyright (c) 2024 Nathan Osman
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to
; deal in the Software without restriction, including without limitation the
; rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
; sell copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
; FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
; IN THE SOFTWARE.


; Shout out to http://www.phreedom.org/research/tinype/ for tips on getting
; this file as small as it can be

; Since this application is its own process, the CPU architecture doesn't
; matter; we therefore prefer i686 for maximum compatibility
BITS 32

ImageBase equ 0x400000

filesig:

    ; The signature for EXE files
    dw "MZ"

    ; Write 29x4 empty bytes to complete the DOS header
    times 29 dw 0

    ; Point to the PE header
    dd coffhdr

coffhdr:

    ; The signature for PE headers
    dd "PE"

; IMAGE_FILE_HEADER
    dw 0x014C       ; Machine
    dw 3            ; NumberOfSections
    dd 0            ; TimeDateStamp
    dd 0            ; PointerToSymbolTable
    dd 0            ; NumberOfSymbols
    dw opthdrsize   ; SizeOfOptionalHeader
    dw 0x0103       ; Characteristics

; IMAGE_OPTIONAL_HEADER32
opthdr:
    dw 0x10b        ; Magic
    db 0            ; MajorLinkerVersion
    db 0            ; MinorLinkerVersion
    dd codesize     ; SizeOfCode
    dd idatasize    ; SizeOfInitializedData
    dd datasize     ; SizeOfUninitializedData
    dd start        ; AddressOfEntryPoint
    dd code         ; BaseOfCode
    dd data         ; BaseOfData
    dd ImageBase    ; ImageBase
    dd 1            ; SectionAlignment
    dd 1            ; FileAlignment
    dw 4            ; MajorOperatingSystemVersion
    dw 0            ; MinorOperatingSystemVersion
    dw 0            ; MajorImageVersion
    dw 0            ; MinorImageVersion
    dw 4            ; MajorSubsystemVersion
    dw 0            ; MinorSubsystemVersion
    dd 0            ; Win32VersionValue
    dd filesize     ; SizeOfImage
    dd hdrsize      ; SizeOfHeaders
    dd 0            ; CheckSum
    dw 2            ; Subsystem
    dw 0x400        ; DllCharacteristics
    dd 0x100000     ; SizeOfStackReserve
    dd 0x1000       ; SizeOfStackCommit
    dd 0x100000     ; SizeOfHeapReserve
    dd 0x1000       ; SizeOfHeapCommit
    dd 0            ; LoaderFlags
    dd 16           ; NumberOfRvaAndSizes

; Data directories

    ; Export table virtual address and size
    dd 0, 0

    ; Import table virtual address and size
    dd idata
    dd idatasize

    ; Populate the remaining directories
    times 14 dd 0, 0

opthdrsize equ $ - opthdr

; IMAGE_SECTION_HEADER[3]

; .text section
    db ".text", 0, 0, 0 ; Name
    dd codesize         ; VirtualSize
    dd code             ; VirtualAddress
    dd codesize         ; SizeOfRawData
    dd code             ; PointerToRawData
    dd 0                ; PointerToRelocations
    dd 0                ; PointerToLinenumbers
    dw 0                ; NumberOfRelocations
    dw 0                ; NumberOfLinenumbers
    dd 0x60000020       ; Characteristics

; .idata section
    db ".idata", 0, 0   ; Name
    dd idatasize        ; VirtualSize
    dd idata            ; VirtualAddress
    dd idatasize        ; SizeOfRawData
    dd idata            ; PointerToRawData
    dd 0                ; PointerToRelocations
    dd 0                ; PointerToLinenumbers
    dw 0                ; NumberOfRelocations
    dw 0                ; NumberOfLinenumbers
    dd 0xC0000040       ; Characteristics

; .data section
    db ".data", 0, 0, 0 ; Name
    dd datasize         ; VirtualSize
    dd data             ; VirtualAddress
    dd datasize         ; SizeOfRawData
    dd data             ; PointerToRawData
    dd 0                ; PointerToRelocations
    dd 0                ; PointerToLinenumbers
    dw 0                ; NumberOfRelocations
    dw 0                ; NumberOfLinenumbers
    dd 0xC0000080       ; Characteristics

hdrsize equ $ - $$

align 4

code:

start:

    ; Load command line arguments
    call    [ImageBase + GetCommandLineW]
    mov     [ImageBase + pCmdLine], eax

    ; Parse command line arguments
    push    ImageBase + nargs
    push    dword [ImageBase + pCmdLine]
    call    [ImageBase + CommandLineToArgvW]
    mov     [ImageBase + szArgList], eax

    ; Make sure there were three
    mov     eax, [ImageBase + nargs]
    cmp     eax, 3
    je      valid_args
    mov     eax, 0xa0
    jmp     exit
valid_args:

    ; Convert the first argument to long
    push    10
    push    0
    mov     eax, [ImageBase + szArgList]
    add     eax, 4
    push    eax
    call    [ImageBase + wcstol]

    ; Attempt to open the process
    push    eax
    push    0
    push    0x100000
    call    [ImageBase + OpenProcess]
    mov     [ImageBase + hProcess], eax
    cmp     eax, 0
    jne     process_opened
    mov     eax, 0x6
    jmp     exit
process_opened:

    ; Wait for the process to terminate
    push    0xffffffff
    push    dword [ImageBase + hProcess]
    call    [ImageBase + WaitForSingleObject]

    ; Close the handle
    push    dword [ImageBase + hProcess]
    call    [ImageBase + CloseHandle]

    ; Delete the image file
    mov     eax, [ImageBase + szArgList]
    add     eax, 8
    push    eax
    call    [ImageBase + DeleteFileW]
    cmp     eax, 0
    jne     file_deleted
    mov     eax, 0x2
    jmp     exit
file_deleted:

    ; Free szArgList
    push    dword [ImageBase + szArgList]
    call    [ImageBase + LocalFree]

    ; Set the return value
    xor     eax, eax

exit:
    ret

codesize equ $ - code

align 4

idata:

; IMAGE_IMPORT_DESCRIPTOR[3]

; kernel32.dll
    dd 0            ; OriginalFirstThunk
    dd 0            ; TimeDateStamp
    dd 0            ; ForwarderChain
    dd kernel32     ; Name
    dd kernel32_iat ; FirstThunk

; msvcrt.dll
    dd 0            ; OriginalFirstThunk
    dd 0            ; TimeDateStamp
    dd 0            ; ForwarderChain
    dd msvcrt       ; Name
    dd msvcrt_iat   ; FirstThunk

; shell32.dll
    dd 0            ; OriginalFirstThunk
    dd 0            ; TimeDateStamp
    dd 0            ; ForwarderChain
    dd shell32      ; Name
    dd shell32_iat  ; FirstThunk

; Blank import descriptor

    times 5 dd 0

; Import lookup and import address tables

kernel32:
    db "KERNEL32.dll", 0

kernel32_iat:
    CloseHandle             dd kernel32_iat_CloseHandle
    DeleteFileW             dd kernel32_iat_DeleteFileW
    GetCommandLineW         dd kernel32_iat_GetCommandLineW
    LocalFree               dd kernel32_iat_LocalFree
    OpenProcess             dd kernel32_iat_OpenProcess
    WaitForSingleObject     dd kernel32_iat_WaitForSingleObject
    dd 0

kernel32_iat_CloseHandle:
    dw 0
    db "CloseHandle", 0

kernel32_iat_DeleteFileW:
    dw 0
    db "DeleteFileW", 0

kernel32_iat_GetCommandLineW:
    dw 0
    db "GetCommandLineW", 0

kernel32_iat_LocalFree:
    dw 0
    db "LocalFree", 0

kernel32_iat_OpenProcess:
    dw 0
    db "OpenProcess", 0

kernel32_iat_WaitForSingleObject:
    dw 0
    db "WaitForSingleObject", 0

msvcrt:
    db "MSVCRT.dll", 0

msvcrt_iat:
    wcstol                  dd msvcrt_iat_wcstol
    dd 0

msvcrt_iat_wcstol:
    dw 0
    db "wcstol", 0

shell32:
    db "SHELL32.dll", 0

shell32_iat:
    CommandLineToArgvW      dd shell32_iat_CommandLineToArgvW
    dd 0

shell32_iat_CommandLineToArgvW:
    dw 0
    db "CommandLineToArgvW", 0

idatasize equ $ - idata

align 4

data:
    pCmdLine                dd 0
    nargs                   dd 0
    szArgList               dd 0
    pid                     dd 0
    hProcess                dd 0

datasize equ $ - data

filesize equ $ - $$
