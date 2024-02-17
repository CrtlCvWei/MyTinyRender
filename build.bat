@echo off
if exist "D:\D\Program files\IDE\VS2019\IDE\VC\Auxiliary\Build\vcvarsall.bat" (
    call "D:\D\Program files\IDE\VS2019\IDE\VC\Auxiliary\Build\vcvarsall.bat" x64
) else (
    if exist "D:\D\Program files\IDE\VS2019\IDE\VC\Auxiliary\Build\vcvarsall.bat" (
        call "D:\D\Program files\IDE\VS2019\IDE\VC\Auxiliary\Build\vcvarsall.bat" x64
    ) else (
        call "D:\D\Program files\IDE\VS2019\IDE\VC\Auxiliary\Build\vcvarsall.bat" x64
    )
)


@echo on

setlocal enabledelayedexpansion
set compilerflags=/Od /Zi /EHsc /std:c++17 /I include -I D:\E\CppImprot\eigen-3.4.0
@REM 
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