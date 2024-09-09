stubconv = executable('stubconv', 'stubconv.cpp')

stub_exe = custom_target('stub_exe',
    input : 'stub.asm',
    output : 'stub.exe',
    command : [nasm, '-f', 'bin', '@INPUT@', '-o', '@OUTPUT@']
)

stub_cpp = custom_target('stub_cpp',
    input : stub_exe,
    output : 'stub.cpp',
    command : [stubconv, '@INPUT@', '@OUTPUT@']
)

deleteself_inc = include_directories('.')

deleteself = static_library('deleteself',
    sources : ['deleteself.cpp', stub_cpp],
)
