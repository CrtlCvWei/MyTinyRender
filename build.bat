@echo off
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
) else (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ) else (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    )
)


@echo on

setlocal enabledelayedexpansion
set compilerflags=/Od /Zi /EHsc /std:c++17 /I include -I D:/ProgramFiles/mingw64_posix/packages/eigen-3.4.0
set linkerflags=/OUT:bin\main.exe
set source_files=

for /r src %%i in (*.cpp) do (
    set "source_files=!source_files! %%i"
) 

cl.exe %compilerflags% %source_files% /link %linkerflags%
if %errorlevel% equ 0 (
    echo Compilation successful.
) else (
    echo Compilation failed.
)
del bin\*.ilk *.obj *.pdb
del *.ilk *.obj *.pdb