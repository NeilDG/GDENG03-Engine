# Copilot Instructions

## Project Guidelines
- For current framework evaluation, prioritize Windows-only development with CMake + MSVC and no Android target; user prefers delivering a complete PBR pipeline rather than incremental rollout.
- Follow the coding patterns of the user's GDENG03-Engine repository when proposing or implementing changes in this workspace.
- Keep a clear project entry point as main.cpp.

## Header Naming Conventions
- For this workspace, only keep the 'Anito' prefix on AnitoApplication.h and AnitoPlatformWindow.h; other planned headers should not use the Anito prefix.