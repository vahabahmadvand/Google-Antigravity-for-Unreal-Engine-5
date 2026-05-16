@echo off
:: Engine Path
SET EnginePath=C:\Program Files\Epic Games\UE_5.7

SET "filePath=%~1"

REM Check if the file exists
IF NOT EXIST "%filePath%" (
	echo Error: Project file not found
	pause
) ELSE (
	echo "%filePath%"
	"%EnginePath%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -ProjectFiles -Project="%filePath%" -Game -Engine -VSCode
	echo Workspace generation complete!
	pause
)