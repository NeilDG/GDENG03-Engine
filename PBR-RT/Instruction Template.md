##Reminders
- Proactively ask me questions wherever necessary. Assume that we are pair programmers, where I am the leader of this project. I need a final say on important architectural decisions in this project.
- If you need additional data or input (files, answers, images, etc.), please ask me before continuing a task.
- Always read the "agents.md" again, before reading my instructions. After each task, provide a self-reflection checklist, assuring me you've followed the standards and details set in the "agents.md"
- You must consider modifying shader code, wherever necessary. Do not focus simply on the C++ aspects since we're mostly tasked with the rendering system. Be proactive in shader programming.
- Before completing a task, ensure that the build and compilation is successful. Do not hastily change the build process as much as possible. Ask permission from me if this is necessary.
- For each task, always perform a web search, of relevant resources, as specified in the agents.md. Ensure that the code is inspired by these professional and educational resources about game engine development, and real-time rendering.


###########
- Check the following files: BUILD_INSTRUCTIONS.md, SHADER_COMPILATION_INSTRUCTIONS.md. Ensure that such instructions are also followed strictly.

##Instructions
My answers to your previous questions are as follows:
1.	Do you want the environment map to be visible as a skybox, or just used for lighting/reflection?
- Yes
2.	Should I add a toggle (e.g., key press) to enable/disable IBL for debugging?
- Yes. Use "Z" key for this.
3.	Is the HDR file in equirectangular (lat-long) format, or already a cubemap?
- I downloaded it from PolyHaven as HDR. I'm not sure.

Perform the following steps you've proposed:
-----------------
Key Integration Steps
1. HDR Loading & Cubemap Conversion
•	Load hdr as an equirectangular texture.
•	Convert to cubemap (using a compute shader or offscreen rendering).
•	Store the cubemap as a GPU texture for both skybox and IBL.
2. Skybox Rendering
•	Add a skybox rendering pass using the cubemap.
3. PBR Shader Update
•	Add a cubemap sampler uniform (e.g., samplerCube u_envMap;).
•	Sample the cubemap for specular IBL in the PBR shader.
•	Add a uniform/flag to enable/disable IBL.
4. Input Handling
•	Use the Z key to toggle IBL on/off (using AnitoInputManager).
5. C++ Integration
•	Load the HDR and create the cubemap in the renderer.
•	Pass the cubemap to both the skybox and PBR shaders.
•	Pass the IBL toggle state as a uniform.
---
I will now:
•	Add a new skybox shader and rendering logic.
•	Update the PBR shader to use the cubemap for IBL.
•	Add the Z key toggle for IBL.
•	Integrate the HDR loading and cubemap conversion in the renderer.
•	Ensure all changes are modular and follow your project’s standards.
---
-----------------



