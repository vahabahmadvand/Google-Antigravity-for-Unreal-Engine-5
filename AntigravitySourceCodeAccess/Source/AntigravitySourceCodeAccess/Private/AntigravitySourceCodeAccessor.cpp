#include "AntigravitySourceCodeAccessor.h"
#include "AntigravitySourceCodeAccessModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "Misc/UProjectInfo.h"
#include "Misc/App.h"
#include "HAL/PlatformProcess.h"

#if PLATFORM_WINDOWS
#include "Internationalization/Regex.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogAntigravity, Log, All);

#define LOCTEXT_NAMESPACE "AntigravitySourceCodeAccessor"

namespace
{
	static const TCHAR* GAntigravityWorkspaceExtension = TEXT(".code-workspace");
}


static FString MakePath(const FString& InPath)
{
	return TEXT("\"") + InPath + TEXT("\"");
}



FString FAntigravitySourceCodeAccessor::GetSolutionPath() const
{
	FScopeLock Lock(&CachedSolutionPathCriticalSection);
	if (IsInGameThread())
	{
		CachedSolutionPath = FPaths::ProjectDir();

		if (!FUProjectDictionary::GetDefault().IsForeignProject(CachedSolutionPath))
		{
			CachedSolutionPath = FPaths::Combine(FPaths::RootDir(), FString("UE5") + GAntigravityWorkspaceExtension);
		}
		else
		{
			FString BaseName = FApp::HasProjectName() ? FApp::GetProjectName() : FPaths::GetBaseFilename(CachedSolutionPath);
			CachedSolutionPath = FPaths::Combine(CachedSolutionPath, BaseName + GAntigravityWorkspaceExtension);
		}
	}
	return CachedSolutionPath;
}

/** save all open documents in visual studio, when recompiling */
static void OnModuleCompileStarted(bool bIsAsyncCompile)
{
	FAntigravitySourceCodeAccessModule& AntigravitySourceCodeAccessModule = FModuleManager::LoadModuleChecked<FAntigravitySourceCodeAccessModule>(TEXT("AntigravitySourceCodeAccess"));
	AntigravitySourceCodeAccessModule.GetAccessor().SaveAllOpenDocuments();
}

void FAntigravitySourceCodeAccessor::Startup()
{
	GetSolutionPath();
	RefreshAvailability();
}

void FAntigravitySourceCodeAccessor::RefreshAvailability()
{
#if PLATFORM_WINDOWS
	FString IDEPath;

	if (!FWindowsPlatformMisc::QueryRegKey(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Classes\\Applications\\Antigravity IDE.exe\\shell\\open\\command\\"), TEXT(""), IDEPath))
	{
		FWindowsPlatformMisc::QueryRegKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Classes\\Applications\\Antigravity IDE.exe\\shell\\open\\command\\"), TEXT(""), IDEPath);
	}

	FString PatternString(TEXT("\"(.*)\" \".*\""));
	FRegexPattern Pattern(PatternString);
	FRegexMatcher Matcher(Pattern, IDEPath);
	if (Matcher.FindNext())
	{
		FString URL = Matcher.GetCaptureGroup(1);
		if (FPaths::FileExists(URL))
		{
			Location.URL = URL;
		}
	}
#endif
}

void FAntigravitySourceCodeAccessor::Shutdown()
{
}

bool FAntigravitySourceCodeAccessor::OpenSourceFiles(const TArray<FString>& AbsoluteSourcePaths)
{
	if (Location.IsValid())
	{
		FString SolutionDir = GetSolutionPath();
		TArray<FString> Args;
		Args.Add(MakePath(SolutionDir));

		for (const FString& SourcePath : AbsoluteSourcePaths)
		{
			Args.Add(MakePath(SourcePath));
		}

		return Launch(Args);
	}
	return false;
}

bool FAntigravitySourceCodeAccessor::AddSourceFiles(const TArray<FString>& AbsoluteSourcePaths, const TArray<FString>& AvailableModules)
{
	// Antigravity doesn't need to do anything when new files are added
	return true;
}

bool FAntigravitySourceCodeAccessor::OpenFileAtLine(const FString& FullPath, int32 LineNumber, int32 ColumnNumber)
{
	if (Location.IsValid())
	{
		// Column & line numbers are 1-based, so dont allow zero
		LineNumber = LineNumber > 0 ? LineNumber : 1;
		ColumnNumber = ColumnNumber > 0 ? ColumnNumber : 1;

		FString SolutionDir = GetSolutionPath();
		TArray<FString> Args;
		Args.Add(MakePath(SolutionDir));
		Args.Add(TEXT("-g ") + MakePath(FullPath) + FString::Printf(TEXT(":%d:%d"), LineNumber, ColumnNumber));
		return Launch(Args);
	}

	return false;
}

bool FAntigravitySourceCodeAccessor::CanAccessSourceCode() const
{
	// True if we have any versions of Antigravity installed
	return Location.IsValid();
}

FName FAntigravitySourceCodeAccessor::GetFName() const
{
	return FName("Antigravity");
}

FText FAntigravitySourceCodeAccessor::GetNameText() const
{
	return LOCTEXT("AntigravityDisplayName", "Antigravity");
}

FText FAntigravitySourceCodeAccessor::GetDescriptionText() const
{
	return LOCTEXT("AntigravityDisplayDesc", "Open source code files in Antigravity");
}

void FAntigravitySourceCodeAccessor::Tick(const float DeltaTime)
{
}

bool FAntigravitySourceCodeAccessor::OpenSolution()
{
	if (Location.IsValid())
	{
		return OpenSolutionAtPath(GetSolutionPath());
	}

	return false;
}

bool FAntigravitySourceCodeAccessor::OpenSolutionAtPath(const FString& InSolutionPath)
{
	if (Location.IsValid())
	{
		FString SolutionPath = InSolutionPath;

		if (!SolutionPath.EndsWith(GAntigravityWorkspaceExtension))
		{
			SolutionPath = SolutionPath + GAntigravityWorkspaceExtension;
		}

		TArray<FString> Args;
		Args.Add(MakePath(SolutionPath));
		return Launch(Args);
	}

	return false;
}

bool FAntigravitySourceCodeAccessor::DoesSolutionExist() const
{
	return FPaths::FileExists(GetSolutionPath());
}

bool FAntigravitySourceCodeAccessor::SaveAllOpenDocuments() const
{
	return true;
}

bool FAntigravitySourceCodeAccessor::Launch(const TArray<FString>& InArgs)
{
	if (Location.IsValid())
	{
		FString ArgsString;
		for (const FString& Arg : InArgs)
		{
			ArgsString.Append(Arg);
			ArgsString.Append(TEXT(" "));
		}

		uint32 ProcessID;
		FProcHandle hProcess = FPlatformProcess::CreateProc(*Location.URL, *ArgsString, true, false, false, &ProcessID, 0, nullptr, nullptr, nullptr);
		return hProcess.IsValid();
	}

	return false;
}

#undef LOCTEXT_NAMESPACE
