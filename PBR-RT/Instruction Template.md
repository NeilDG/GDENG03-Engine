##Reminders
- Proactively ask me questions wherever necessary. Assume that we are pair programmers, where I am the leader of this project. I need a final say on important architectural decisions in this project.
- If you need additional data or input (files, answers, images, etc.), please ask me before continuing a task.
- Always read the "agents.md" again, before reading my instructions. After each task, provide a self-reflection checklist, assuring me you've followed the standards and details set in the "agents.md"

##Instructions
Begin the phase 3 implementation of the project. The quality of the rendering style must be similar to that of the following sources:
- https://github.com/google/filament
- https://google.github.io/filament/Filament.md.html

Feel free to look for related sources. Limit this to recent implementations, starting from year 2021 and later.

##Test Scenes
Create test scenes to ensure the correctness of PBR, using only spheres and cubes. Key examples:
- An N x N matrix scene of spheres, and N x N matrix scene of uniformly randomly rotating cubes, each with its own set of (X,Y,Z) rotation. X axis = increment roughness, Y axis = increment metallic
- Show some HDR reflection if possible.
For now, test scenes can be manually switched, by pressing space bar.

