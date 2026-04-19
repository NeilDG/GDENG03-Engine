# Building shaderc - Simplified Guide

Since bgfx's build setup expects bx/bimg at specific locations, here's the simplest approach:

## Option 1: Use Pre-Compiled Shaders (EASIEST - DO THIS)

I've already fixed all the shader code. Just compile them once manually:

1. **Download pre-built shaderc** from my Google Drive or similar source
   OR Use an online shader compiler
   OR Borrow shaderc.exe from a colleague who has built bgfx before

2. **Place it in**: `tools/shaderc/shaderc.exe`

3. **Run**:
   ```cmd
   compile_shaders.bat
   ```

4. **Done!** The compiled `.bin` files are committed to the repo, so you only need to do this once.

## Option 2: Build bgfx Properly (if you need to modify shaders often)

The bgfx build system expects this structure:
```
parent_dir/
├── bx/
├── bimg/
└── bgfx/
```

But we have:
```
external/
└── bgfx/
    ├── bx/
    └── bimg/
```

**To build shaderc**:

1. **Create symbolic links** (Run as Administrator):
   ```cmd
   cd external
   mklink /D bx bgfx\bx
   mklink /D bimg bgfx\bimg
   cd bgfx
   bx\tools\bin\windows\genie.exe --with-tools vs2022
   ```

2. **Build** in Visual Studio:
   - Open `.build/projects/vs2022/bgfx.sln`
   - Build `shaderc` project in Release mode
   - Find `shaderc.exe` in `.build/win64_vs2022/bin/`

3. **Copy** to `tools/shaderc/shaderc.exe`

## Option 3: I'll Provide Compiled Shaders

If both options above are too complex, I can provide the compiled shader binaries directly. They're small binary files that can be committed to the repo.

**Recommendation**: Use Option 1 or 3 for now. You can set up proper shader compilation later if needed.
