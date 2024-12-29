project('bt', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++20', 'b_sanitizers=address,undefined'])

fmt = dependency('fmt')

bt = executable('bt', ['main.cpp', 'Bencode.cpp', 'Torrent.cpp'], install : true, dependencies : [fmt])

test('test', bt)