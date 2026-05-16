// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class AntigravitySourceCodeAccess : ModuleRules
    {
	    public AntigravitySourceCodeAccess(ReadOnlyTargetRules Target) : base(Target)
        {
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                        "Core",
                        "SourceCodeAccess",
                        "DesktopPlatform"
                }
            );

            if (Target.bBuildEditor)
            {
                PrivateDependencyModuleNames.Add("HotReload");
            }

            bBuildLocallyWithSNDBS = true;
            ShortName = "AGSCA";
        }
    }
}

