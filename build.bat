@echo off
rem MSVC でビルドする場合は先に "Developer Command Prompt" を開くこと
cl /nologo /std:c++03 /EHsc /W3 /subsystem:windows /Fe:WavPlayer.exe src\main.cpp winmm.lib comdlg32.lib user32.lib gdi32.lib
