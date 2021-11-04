#
# Copyright (c) 2020 Pavlo Lavrenenko
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.19)

project(DX11)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT DX11)

set(SOURCE_ROOT ${CMAKE_SOURCE_DIR}/src)

file(GLOB SourceFiles ${SOURCE_ROOT}/*.cpp)
file(GLOB HeaderFiles ${SOURCE_ROOT}/*.h)
file(GLOB ShaderFiles ${SOURCE_ROOT}/*.fx)

source_group(TREE ${SOURCE_ROOT} PREFIX "Source Files" FILES ${SourceFiles})
source_group(TREE ${SOURCE_ROOT} PREFIX "Header Files" FILES ${HeaderFiles})
source_group(TREE ${SOURCE_ROOT} PREFIX "Shader Files" FILES ${ShaderFiles})

add_executable(DX11 WIN32 ${SourceFiles} ${HeaderFiles} ${ShaderFiles})

target_compile_options(DX11 PRIVATE /W4 /WX /wd4100)
target_compile_features(DX11 PRIVATE cxx_std_17)
target_link_libraries(DX11 PRIVATE d3d11 d3dcompiler)

add_custom_command(TARGET DX11 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ShaderFiles} $<TARGET_FILE_DIR:DX11>)
