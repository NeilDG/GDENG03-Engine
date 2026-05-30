#pragma once

#include "../GameObjects/AnitoGameObject.h"
#include "../GameObjects/AnitoGameObjectManager.h"
#include "AnitoMaterial.h"
#include "AnitoShader.h"
#include "AnitoMeshGenerator.h"
#include <vector>
#include <memory>
#include <random>

namespace Anito {

/**
 * AnitoPBRTestScenes - Manages test scenes for PBR validation
 * Creates various configurations of spheres and cubes to demonstrate PBR properties:
 * - Roughness variation (X-axis)
 * - Metallic variation (Y-axis)
 * - Random rotations for cubes
 */
class AnitoPBRTestScenes {
public:
    enum class SceneType {
        BasicLayout,         // Flat plane + 3 cubes at specified transforms (reference mockup)
        SphereGrid,          // N x N grid of spheres with varying roughness/metallic
        CubeGrid,            // N x N grid of rotating cubes with varying properties
        MixedMaterials,      // Mix of different material types
        MetallicShowcase,    // Focus on metallic materials (simulating different metals)
        DielectricShowcase,  // Focus on non-metallic materials (plastics, ceramics, etc.)
        Count
    };

    struct SceneConfig {
        SceneType type;
        std::string name;
        std::string description;
        int gridSizeX;
        int gridSizeY;
        float spacing;
        float objectScale;
    };

public:
    AnitoPBRTestScenes();
    ~AnitoPBRTestScenes();

    // Singleton access (optional - for easy access from input manager)
    static AnitoPBRTestScenes* s_currentInstance;
    static AnitoPBRTestScenes* getCurrentInstance() { return s_currentInstance; }

    // Scene management
    void initialize(std::shared_ptr<AnitoShader> shader);
    void switchToNextScene();
    void switchToPreviousScene();
    void switchToScene(int index);
    
    // Update (for rotating cubes)
    void update(float deltaTime);

    // Getters
    int getCurrentSceneIndex() const { return m_currentSceneIndex; }
    int getSceneCount() const { return static_cast<int>(SceneType::Count); }
    const SceneConfig& getCurrentSceneConfig() const { return m_sceneConfigs[m_currentSceneIndex]; }

    // Camera positioning helper
    glm::vec3 getCameraPositionForScene() const;
    glm::vec3 getLookAtPositionForScene() const;

private:
    void setupSceneConfigs();
    void createBasicLayoutScene();
    void createSphereGridScene();
    void createCubeGridScene();
    void createMixedMaterialsScene();
    void createMetallicShowcaseScene();
    void createDielectricShowcaseScene();
    
    void clearCurrentScene();
    void loadScene(int index);

    // Material creation helpers
    std::shared_ptr<AnitoMaterial> createMaterialWithParams(
        const std::string& name,
        const glm::vec3& baseColor,
        float metallic,
        float roughness
    );

    // Color helpers for visual variety
    glm::vec3 getColorForGridPosition(int x, int y, int gridX, int gridY);
    glm::vec3 getMetallicColor(int index); // Returns colors for common metals
    glm::vec3 getDielectricColor(int index); // Returns colors for common dielectrics

private:
    std::shared_ptr<AnitoShader> m_shader;
    std::vector<SceneConfig> m_sceneConfigs;
    std::vector<AnitoGameObject*> m_currentSceneObjects;
    
    int m_currentSceneIndex;
    
    // Random number generator for cube rotations
    std::mt19937 m_randomGen;
    std::uniform_real_distribution<float> m_rotationDist;
};

} // namespace Anito
