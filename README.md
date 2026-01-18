# OpenGL 3D Interactive Scene

![OpenGL](https://img.shields.io/badge/OpenGL-4.1-green.svg) ![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)

A photorealistic 3D interactive scene developed in **C++** using **OpenGL 4.1**. This project demonstrates advanced computer graphics concepts including complex lighting, shadow mapping, procedural generation, and cinematic camera animation.

Developed for the *Computer Graphics Systems (SPG)* course at the **Technical University of Cluj-Napoca**.

## ⚠️ Important Note on Assets
**This repository contains the source code only.**
Due to copyright and file size limitations, the 3D models (`.obj`) and textures are **not** included in this repository. To run the application successfully, you must provide your own assets or download the original models listed in the [Credits](#-credits--resources) section below and place them in the corresponding folders.

## ✨ Key Features

* **Advanced Lighting (Phong Model):**
    * **Directional Light:** Sun-like light source with dynamic positioning.
    * **Point Lights:** Localized light sources.
    * **Spotlight:** Player-controlled flashlight with soft edges (penumbra).
* **Shadows:** Dynamic **Shadow Mapping** implementation (2-pass rendering).
* **Optimization:** **Billboarding** technique used for rendering dense vegetation (200+ trees) efficiently.
* **Camera System:**
    * First-person free-roam camera.
    * Cinematic camera path using **Catmull-Rom Spline** interpolation.
    * **AABB Collision Detection** to prevent walking through objects.
* **Atmosphere:** Custom Skybox and distance-based Fog integration.

## 🎮 Controls

| Key | Action |
| :--- | :--- |
| **W, A, S, D** | Move Camera |
| **Mouse** | Look around |
| **F** | Toggle Flashlight |
| **V** | Start/Stop Cinematic Animation (Spline) |
| **3, 4, 5** | Change Render Mode (Solid, Wireframe, Point) |
| **7, 8** | Increase/Decrease Fog Density |
| **Q, E** | Rotate the Nanosuit model |
| **J, L** | Move the Light Source (Sun) |

## 🛠️ Dependencies

The project is built using the following libraries:
* **GLFW** (Window & Input)
* **GLEW** (Extension Wrangler)
* **GLM** (Mathematics)
* **stb_image** (Texture Loading)

## 🏆 Credits & Resources

Since assets are not provided, here are the sources used in the original project:

* **Nanosuit:** [Crytek / Free3D](https://free3d.com/3d-model/crysis-2-nanosuit-2-97837.html)
* **Mario:** [Nintendo / Sketchfab](https://sketchfab.com/3d-models/mario-ds-fbx-552baccb1ef74a799fb7440fdc5fd69d)
* **Minecraft Mobs (Creeper/Zombie):** [Mojang / Sketchfab](https://sketchfab.com/)
* **Textures:** [Minecraft Default Pack](https://texture-packs.com/resourcepack/default-pack/)
* **Vegetation:** [Tech Monkey Business](https://www.techmonkeybusiness.com/galleries/Texture_Galleries/Billboard_Trees/)

## 👤 Author
**Stolniceanu Iustin-Pavel**
*January 2026*
