@echo off

set ROOT=%cd%
set NAME=%~n0
set ARGV=%*

if exist zephyr (
	cd zephyr
)

cmd /c zephyr-env.cmd

cmd /c python %NAME%.py %ARGV%

cd %ROOT%