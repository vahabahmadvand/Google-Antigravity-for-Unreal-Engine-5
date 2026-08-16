# Antigravity Editor for Unreal Engine

A lightweight Unreal Engine plugin that seamlessly integrates the **Antigravity Editor** (a VSCode-based custom editor) as a Source Code Editor within Unreal Engine.

This plugin leverages Unreal Engine's `ISourceCodeAccessor` interface to ensure that clicking on C++ classes, blueprint nodes, or compilation errors inside the Unreal Editor instantly opens the correct file and line number inside your Antigravity Editor workspace.

---

## Features

* **Instant Code Navigation:** Double-click C++ classes or functions in Unreal Engine to jump directly to them in Antigravity.
* **Automatic Workspace Sync:** Bridges Unreal Engine's project structure with Antigravity’s layout.
* **Streamlined Workflow:** Tailored specifically for developers utilizing the Antigravity ecosystem instead of stock VSCode or heavy Visual Studio installations.

---

## Installation

You can install this plugin either locally to a specific project

### Project-Level Installation

1. Navigate to your Unreal Engine project's root directory (where your `.uproject` file is located).
2. Look for a folder named `Plugins`. If it does not exist, create a new folder and name it exactly **`Plugins`**.
3. Clone or extract this repository directly into that folder. Your path layout should look like this:
```text
YourProject/
├── Config/
├── Content/
├── Source/
├── Plugins/
│   └── AntigravitySourceCodeAccess/  <-- Plugin files go here
└── YourProject.uproject
```
---

## Configuration & Setup

Once the plugin is in the correct directory, you need to recompile your project, and then enable it inside your project settings.

### 1. Activating the Plugin (If it is not activated already)

1. Launch your Unreal Engine project.
2. From the top menu, go to **Edit > Plugins**.
3. In the search bar, type **Antigravity**.
4. Check the **Enabled** box next to the Antigravity Source Accessor plugin
5. Restart the Unreal Editor if prompted.

### 2. Setting Antigravity as your Default Editor

1. Go to **Edit > Editor Preferences**.
2. On the left sidebar, navigate to the **General** section and select **Source Code**.
3. In the **Source Code Editor** dropdown menu, select **Antigravity**.


![alt text](Image/EditorSettings.png)

> **Note:** If Antigravity does not appear in the dropdown menu right away, verify that your Antigravity executable path is correctly set up in your system's Environment Variables (`PATH`), or specify the absolute path inside the plugin's configuration settings if prompted.

---


## How It Works & Project File Generation

Because the Antigravity Editor is built on top of a VSCode-based core architecture, it relies on standard `.code-workspace` files to map out your Unreal project. If Unreal Engine falls back to generating Visual Studio (`.sln`) files by default, you can force it to generate a VSCode workspace using any of the three following methods:

### Method 1: The Editor Toggle Trick

1. Go to **Edit > Editor Preferences > Source Code** and temporarily set your editor to **Visual Studio Code**.
2. Close the editor, right-click your `.uproject` file, and select **Generate Project Files**.
3. Once the `.code-workspace` file is generated in your root directory, reopen the project and switch your Source Code Editor preference back to **Antigravity Editor**.

### Method 2: Local Project Configuration Override (Recommended)

You can force the Unreal Build Tool (UBT) to always output VSCode files for this specific project by placing a configuration file inside your project's `Saved` directory:

1. Navigate to your project's `Saved` folder.
2. Create a folder named `UnrealBuildTool` (if it doesn't exist).
3. Copy the file named `BuildConfiguration.xml` from repo inside it 


<details>
<summary>Manually create BuildConfiguration.xml</summary>
You can create the file manually and paste the following configuration:

```xml
<?xml version="1.0" encoding="utf-8" ?>
<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration">
  <ProjectFileGenerator>
    <Format>VisualStudioCode</Format>
  </ProjectFileGenerator>
</Configuration>

```
</details>

### Method 3: Automated Generation via Batch File

Alternatively, you can bypass the editor completely and force-generate the files via a command-line script. Copy `GenerateVSCodeProjectfiles.bat` in your project's root folder and drag&drop your `.uproject` file on it.

# Running Unreal Engine with Google Antigravity (Modern Workspace Setup)

Latest versions of Unreal Engine package all build tasks and run settings directly inside a single `YourProjectName.code-workspace` file. However, the engine defaults to Microsoft's closed-source debugger (`"cppvsdbg"`), which triggers a licensing error inside open-source AI IDEs like Google Antigravity.

Follow this setup to clean the workspace file, fix variable scoping bugs, and bypass Microsoft licensing restrictions completely.

---

## Prerequisites
1. **IDE:** Google Antigravity (or any open-source VSCodium fork).
2. **Extension:** Install **CodeLLDB** from the extensions marketplace (`Ctrl+Shift+X`). This replaces Microsoft's restricted debugging engines.
3. **Compilers:** Ensure your native build tools (Visual Studio 2022 on Windows or Xcode on Mac) are installed.

---

## Unified Workspace Configuration

Open your project's main folder on your computer. Find the **`YourProjectName.code-workspace`** file, open it in a text editor, and structure it like the template below. 

Note the use of `"type": "lldb"` (bypasses the license error) and `${fileDirname}` (bypasses multi-folder variable scope bugs):

```json
{
    "folders": [
        {
            "name": "ProjectRoot",
            "path": "."
        }
    ],
    "settings": {},
    "launch": {
        "version": "0.2.0",
        "configurations": [
            {
                "name": "Launch Game Editor (Antigravity)",
                "type": "lldb",
                "request": "launch",
                "program": "C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\Win64\\UnrealEditor.exe",
                "args": [
                    "\${fileDirname}\\YourProjectName.uproject",
                    "-skipcompile"
                ],
                "cwd": "C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\Win64",
                "preLaunchTask": "UnrealEngine-Compile-Project"
            }
        ]
    },
    "tasks": {
        "version": "2.0.0",
        "tasks": [
            {
                "label": "UnrealEngine-Compile-Project",
                "type": "shell",
                "command": "C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\DotNET\\UnrealBuildTool\\UnrealBuildTool.exe",
                "args": [
                    "Development",
                    "Win64",
                    "-Project=\\${fileDirname}\\YourProjectName.uproject",
                    "-TargetType=Editor",
                    "-Progress"
                ],
                "options": {
                    "cwd": "C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\DotNET\\UnrealBuildTool"
                },
                "group": {
                    "kind": "build",
                    "isDefault": true
                },
                "problemMatcher": "\$msCompile"
            }
        ]
    }
}
```
*(⚠️ Remember to change `UE_5.8` and `YourProjectName` inside the text paths to match your exact engine version and project name!)*

<details>
<summary> Set up Launch.json and Tasks.json separately for older version</summary>
Replace the files inside your hidden `.vscode/` folder with the open-source configurations below.

### 1. `.vscode/launch.json`
This file launches the Unreal Editor using the open-source **CodeLLDB** debugger instead of `cppvsdbg`. It uses `${fileDirname}` to avoid multi-root workspace variable bugs.

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Launch Game Editor (Antigravity)",
            "type": "lldb",
            "request": "launch",
            "program": "C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\Win64\\UnrealEditor.exe",
            "args": [
                "\${fileDirname}\\YourProjectName.uproject",
                "-skipcompile"
            ],
            "cwd": "C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\Win64",
            "preLaunchTask": "UnrealEngine-Compile-Project"
        }
    ]
}
```
*(Note: Change `UE_5.8` and `YourProjectName` to match your engine version and project file name.)*

### 2. `.vscode/tasks.json`
This file configures the **Unreal Build Tool (UBT)** path so Antigravity can compile your source code natively.

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "UnrealEngine-Compile-Project",
            "type": "shell",
            "command": "C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\DotNET\\UnrealBuildTool\\UnrealBuildTool.exe",
            "args": [
                "Development",
                "Win64",
                "-Project=\${fileDirname}\\YourProjectName.uproject",
                "-TargetType=Editor",
                "-Progress"
            ],
            "options": {
                "cwd": "C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\DotNET\\UnrealBuildTool"
            },
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": "\$msCompile"
        }
    ]
}
```
</details>
---

## 🏃‍♂️ How to Compile and Run
1. Launch **Google Antigravity**.
2. Click **File > Open Workspace from File...** and select your `YourProjectName.code-workspace` file.
3. Open **any C++ source file** (`.cpp` or `.h`) in your main editor pane.
   * *Why? This initializes `${fileDirname}` so Antigravity can calculate your project file's exact location path.*
4. Press **`F5`** on your keyboard.
5. Antigravity will cleanly compile your code through the embedded task and immediately launch the Unreal Editor!


## Platform Support
 - **Windows**
---
 Enjoy!
