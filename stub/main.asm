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

    ; i386
    dw 0x014C

    ; Number of sections
    dw 2

    ; TimeDateStamp
    dd 0

    ; PointerToSymbolTable and NumberOfSymbolTable (deprecated)
    dd 0
    dd 0

    ; Size of the optional header
    dw opthdrsize

    ; Attributes of the file
    ; - IMAGE_FILE_RELOCS_STRIPPED
    ; - IMAGE_FILE_EXECUTABLE_IMAGE
    ; - IMAGE_FILE_32BIT_MACHINE
    dw 0x0103

opthdr:

    ; Identify this as a PE32 executable
    dw 0x10b

    ; Major / minor linker version
    db 0
    db 0

    ; Size of the code section
    dd codesize

    ; Size of initialized and uninitialized data
    dd 0
    dd 0

    ; Address of entrypoint
    dd start

    ; Base of code
    dd code

    ; Base of data
    dd idata

    ; Image base
    dd 0x400000

    ; Section and file alignment
    dd 1
    dd 1

    ; Major and minor operating system version
    dw 4
    dw 0

    ; Major and minor image version
    dw 0
    dw 0

    ; Major and minor subsystem version
    dw 4
    dw 0

    ; Reserved - must be zero
    dd 0

    ; Size of image
    dd filesize

    ; Size of headers
    dd hdrsize

    ; Checksum (ignored)
    dd 0

    ; Subsystem (IMAGE_SUBSYSTEM_WINDOWS_GUI)
    dw 2

    ; DLL characteristics (IMAGE_DLLCHARACTERISTICS_NO_SEH)
    dw 0x400

    ; Size of stack reserve & commit
    dd 0x100000
    dd 0x1000

    ; Size of heap reserve & commit
    dd 0x100000
    dd 0x1000

    ; Loader flags
    dd 0

    ; Number of RVA and size
    dd 16

; Data directories

    ; Export table virtual address and size
    dd 0
    dd 0

    ; Import table virtual address and size
    dd idata
    dd idatasize

    ; Populate the remaining directories
    times 14 dd 0, 0

opthdrsize equ $ - opthdr

; .text section

    ; Name of section
    db ".text", 0, 0, 0

    ; Virtual size
    dd codesize

    ; Virtual address
    dd code

    ; Size of raw data
    dd codesize

    ; Pointer to raw data
    dd code

    ; Pointer to relocations & line numbers
    dd 0
    dd 0

    ; Number of relocations & line numbers
    dw 0
    dw 0

    ; Characteristics
    ; - IMAGE_SCN_CNT_CODE
    ; - IMAGE_SCN_MEM_EXECUTE
    ; - IMAGE_SCN_MEM_READ
    dd 0x60000020

; .idata section

    ; Name of section
    db ".idata", 0, 0

    ; Virtual size
    dd idatasize

    ; Virtual address
    dd idata

    ; Size of raw data
    dd idatasize

    ; Pointer to raw data
    dd idata

    ; Pointer to relocations & line numbers
    dd 0
    dd 0

    ; Number of relocations & line numbers
    dw 0
    dw 0

    ; Characteristics
    ; - IMAGE_SCN_CNT_INITIALIZED_DATA
    ; - IMAGE_SCN_MEM_READ
    ; - IMAGE_SCN_MEM_WRITE
    dd 0xC0000040

hdrsize equ $ - $$

align 4

code:

start:

    push 0
    push Title
    push Message
    push 0
    call [user32_MessageBoxA]

    xor eax, eax
    ret

codesize equ $ - code

align 4

idata:

; Import descriptor for User32.dll

    ; Original first thunk
    dd user32_ilt

    ; TimeDataStamp and ForwarderChain (not used)
    dd 0
    dd 0

    ; RVA to DLL name
    dd user32

    ; First thunk
    dd user32_iat

; Blank import descriptor

    times 5 dd 0

; Import lookup and import address tables

user32:
    db "USER32.dll", 0

user32_ilt:
    dd MessageBoxA
    dd 0

user32_iat:
user32_MessageBoxA:
    dd MessageBoxA
    dd 0

MessageBoxA:
    dw 0
    db "MessageBoxA", 0

Title:
    db "Test Title"
Message:
    db "Test Message"

idatasize equ $ - idata

filesize equ $ - $$
