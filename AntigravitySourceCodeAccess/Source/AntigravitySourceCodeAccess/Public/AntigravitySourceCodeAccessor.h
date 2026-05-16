#pragma once
#include "ISourceCodeAccessor.h"

class FAntigravitySourceCodeAccessor : public ISourceCodeAccessor
{
public:
	FAntigravitySourceCodeAccessor(){}

	void Startup();
	void Shutdown();

	// ISourceCodeAccessor implementation
	virtual void RefreshAvailability() override;
	virtual bool CanAccessSourceCode() const override;
	virtual FName GetFName() const override;
	virtual FText GetNameText() const override;
	virtual FText GetDescriptionText() const override;
	virtual bool OpenSolution() override;
	virtual bool OpenSolutionAtPath(const FString& InSolutionPath) override;
	virtual bool DoesSolutionExist() const override;
	virtual bool OpenFileAtLine(const FString& FullPath, int32 LineNumber, int32 ColumnNumber = 0) override;
	virtual bool OpenSourceFiles(const TArray<FString>& AbsoluteSourcePaths) override;
	virtual bool AddSourceFiles(const TArray<FString>& AbsoluteSourcePaths, const TArray<FString>& AvailableModules) override;
	virtual bool SaveAllOpenDocuments() const override;
	virtual void Tick(const float DeltaTime) override;

private:

	// Wrapper for Antigravity execution launch information
	struct FLocation
	{
		bool IsValid() const 
		{ 
			return URL.Len() > 0; 
		}

		FString URL;
	};

	FLocation Location;

	mutable FString CachedSolutionPath;
	mutable FCriticalSection CachedSolutionPathCriticalSection;

	FString GetSolutionPath() const;

	// Helper function for launching the Antigravity instance with the given list of argumants
	bool Launch(const TArray<FString>& InArgs);
};
