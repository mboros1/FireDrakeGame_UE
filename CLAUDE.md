# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

FireDrake is an Unreal Engine 5.6 C++ game project configured for macOS development with cross-platform support for Windows and Linux.

## Build Commands

### macOS (Primary Development Platform)
- Open `FireDrake (Mac).xcworkspace` in Xcode
- Build using Xcode's build system (Cmd+B)
- Run from Xcode or launch the editor directly

### Generating Project Files
```bash
# If Unreal Engine is in your PATH
UnrealBuildTool -projectfiles -project="/Users/martinboros/Documents/Unreal Projects/FireDrake/FireDrake.uproject" -game
```

### Common Development Tasks
- **Hot Reload**: Use the Compile button in Unreal Editor for C++ changes
- **Full Rebuild**: Clean and rebuild from Xcode or regenerate project files
- **Package Game**: Use Project Settings > Packaging in the editor

## Code Architecture

### Module Structure
- **Primary Game Module**: `FireDrake` (Source/FireDrake/)
  - Module definition: FireDrake.h/cpp
  - Build configuration: FireDrake.Build.cs
  - Dependencies: Core, CoreUObject, Engine, InputCore, EnhancedInput

### Build Targets
- **Game Target**: Source/FireDrake.Target.cs - Builds the game executable
- **Editor Target**: Source/FireDrakeEditor.Target.cs - Builds the editor version

### Key Configuration Files
- **FireDrake.uproject**: Main project file defining engine version and modules
- **Config/DefaultEngine.ini**: Engine settings including rendering (Lumen, Nanite) and physics
- **Config/DefaultGame.ini**: Game-specific settings and project metadata
- **Config/DefaultInput.ini**: Input mappings using Enhanced Input system

### Engine Features Enabled
- Lumen Global Illumination
- Nanite Virtualized Geometry
- Virtual Texturing
- Path Tracing Support
- Enhanced Input System (default)

## Development Guidelines

### Adding New C++ Classes
1. Use Unreal Editor's "New C++ Class" wizard when possible
2. Place gameplay classes in appropriate folders (e.g., Characters/, GameModes/)
3. Follow Unreal's naming conventions (A prefix for Actors, U for UObjects)
4. Include proper UCLASS macros and reflection markup

### Common Class Types to Implement
- `AFireDrakeGameMode`: Main game rules and flow
- `AFireDrakeCharacter`: Player character class
- `AFireDrakePlayerController`: Player input handling
- `UFireDrakeGameInstance`: Persistent game state

### Build Configuration Notes
- PCH (Precompiled Headers) are enabled for faster compilation
- Slate UI and Online subsystems are available but commented out in Build.cs
- Add them to PublicDependencyModuleNames when needed