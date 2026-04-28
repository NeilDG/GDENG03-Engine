#include "AnitoPBRTestScenes.h"
#include "../Components/AnitoMeshRenderer.h"
#include "../Components/AnitoTransform.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace Anito {

AnitoPBRTestScenes* AnitoPBRTestScenes::s_currentInstance = nullptr;

AnitoPBRTestScenes::AnitoPBRTestScenes()
    : m_currentSceneIndex(0)
    , m_randomGen(std::random_device{}())
    , m_rotationDist(-glm::pi<float>(), glm::pi<float>())
{
    setupSceneConfigs();
    s_currentInstance = this;
}

AnitoPBRTestScenes::~AnitoPBRTestScenes() {
    clearCurrentScene();
    if (s_currentInstance == this) {
        s_currentInstance = nullptr;
    }
}

void AnitoPBRTestScenes::setupSceneConfigs() {
    m_sceneConfigs.resize(static_cast<int>(SceneType::Count));

    m_sceneConfigs[static_cast<int>(SceneType::SphereGrid)] = {
        SceneType::SphereGrid,
        "Sphere Grid - PBR Variation",
        "6x6 grid: X-axis = Roughness (0.0 to 1.0), Y-axis = Metallic (0.0 to 1.0)",
        6, 6, 3.0f, 1.0f
    };

    m_sceneConfigs[static_cast<int>(SceneType::CubeGrid)] = {
        SceneType::CubeGrid,
        "Rotating Cubes - PBR Variation",
        "6x6 grid: Randomly rotating cubes with varying roughness and metallic",
        6, 6, 3.0f, 1.0f
    };

    m_sceneConfigs[static_cast<int>(SceneType::MixedMaterials)] = {
        SceneType::MixedMaterials,
        "Mixed Materials Showcase",
        "Variety of both spheres and cubes with different material properties",
        4, 4, 3.5f, 1.2f
    };

    m_sceneConfigs[static_cast<int>(SceneType::MetallicShowcase)] = {
        SceneType::MetallicShowcase,
        "Metallic Materials (Conductors)",
        "Common metals: Gold, Silver, Copper, Aluminum, etc.",
        4, 2, 3.5f, 1.2f
    };

    m_sceneConfigs[static_cast<int>(SceneType::DielectricShowcase)] = {
        SceneType::DielectricShowcase,
        "Dielectric Materials (Non-metals)",
        "Plastics, ceramics, and other non-metallic materials",
        4, 2, 3.5f, 1.2f
    };
}

void AnitoPBRTestScenes::initialize(std::shared_ptr<AnitoShader> shader) {
    m_shader = shader;
    loadScene(m_currentSceneIndex);
}

void AnitoPBRTestScenes::switchToNextScene() {
    m_currentSceneIndex = (m_currentSceneIndex + 1) % static_cast<int>(SceneType::Count);
    loadScene(m_currentSceneIndex);
    
    std::cout << "\n[PBR Test Scenes] Switched to: " << m_sceneConfigs[m_currentSceneIndex].name << std::endl;
    std::cout << "  Description: " << m_sceneConfigs[m_currentSceneIndex].description << std::endl;
}

void AnitoPBRTestScenes::switchToPreviousScene() {
    m_currentSceneIndex = (m_currentSceneIndex - 1 + static_cast<int>(SceneType::Count)) % static_cast<int>(SceneType::Count);
    loadScene(m_currentSceneIndex);
    
    std::cout << "\n[PBR Test Scenes] Switched to: " << m_sceneConfigs[m_currentSceneIndex].name << std::endl;
    std::cout << "  Description: " << m_sceneConfigs[m_currentSceneIndex].description << std::endl;
}

void AnitoPBRTestScenes::switchToScene(int index) {
    if (index >= 0 && index < static_cast<int>(SceneType::Count)) {
        m_currentSceneIndex = index;
        loadScene(m_currentSceneIndex);
        
        std::cout << "\n[PBR Test Scenes] Switched to: " << m_sceneConfigs[m_currentSceneIndex].name << std::endl;
        std::cout << "  Description: " << m_sceneConfigs[m_currentSceneIndex].description << std::endl;
    }
}

void AnitoPBRTestScenes::update(float deltaTime) {
    // Update rotating cubes if in CubeGrid scene
    if (m_currentSceneIndex == static_cast<int>(SceneType::CubeGrid) ||
        m_currentSceneIndex == static_cast<int>(SceneType::MixedMaterials)) {

        for (auto* obj : m_currentSceneObjects) {
            if (obj->getObjectType() == AnitoGameObject::PrimitiveType::Cube) {
                AnitoVector3D currentRot = obj->getRotation();

                // Each cube has unique rotation speed based on its position
                float speedX = 0.3f + static_cast<float>(std::hash<void*>{}(obj) % 100) / 200.0f;
                float speedY = 0.5f + static_cast<float>(std::hash<void*>{}(obj) % 100) / 150.0f;
                float speedZ = 0.2f + static_cast<float>(std::hash<void*>{}(obj) % 100) / 250.0f;

                obj->setRotation(
                    currentRot.x() + deltaTime * speedX,
                    currentRot.y() + deltaTime * speedY,
                    currentRot.z() + deltaTime * speedZ
                );
            }
        }
    }
}

glm::vec3 AnitoPBRTestScenes::getCameraPositionForScene() const {
    const SceneConfig& config = m_sceneConfigs[m_currentSceneIndex];

    // Calculate camera distance based on grid extent (grid is centered at origin)
    float gridExtentX = (config.gridSizeX - 1) * config.spacing;
    float gridExtentY = (config.gridSizeY - 1) * config.spacing;
    float maxExtent = glm::max(gridExtentX, gridExtentY);

    // Position camera to view entire grid with some margin
    float distance = maxExtent * 0.75f + 8.0f;
    float height = maxExtent * 0.25f;

    return glm::vec3(0.0f, height, distance);
}

glm::vec3 AnitoPBRTestScenes::getLookAtPositionForScene() const {
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

void AnitoPBRTestScenes::clearCurrentScene() {
    auto* manager = AnitoGameObjectManager::getInstance();
    if (manager) {
        for (auto* obj : m_currentSceneObjects) {
            manager->removeObject(obj);
            // Note: removeObject() already deletes the object, so we don't delete it here
        }
    }
    m_currentSceneObjects.clear();
}

void AnitoPBRTestScenes::loadScene(int index) {
    clearCurrentScene();

    std::cout << "\n[PBR Test Scenes] Loading scene: " << m_sceneConfigs[index].name << std::endl;

    switch (static_cast<SceneType>(index)) {
        case SceneType::SphereGrid:
            createSphereGridScene();
            break;
        case SceneType::CubeGrid:
            createCubeGridScene();
            break;
        case SceneType::MixedMaterials:
            createMixedMaterialsScene();
            break;
        case SceneType::MetallicShowcase:
            createMetallicShowcaseScene();
            break;
        case SceneType::DielectricShowcase:
            createDielectricShowcaseScene();
            break;
        default:
            break;
    }

    std::cout << "[PBR Test Scenes] Scene loaded with " << m_currentSceneObjects.size() << " objects" << std::endl;
}

void AnitoPBRTestScenes::createSphereGridScene() {
    const SceneConfig& config = m_sceneConfigs[m_currentSceneIndex];
    auto* manager = AnitoGameObjectManager::getInstance();
    
    // Pre-generate sphere mesh (reuse for all spheres)
    auto sphereMesh = AnitoMeshGenerator::createSphere(config.objectScale, 32, 16);

    for (int y = 0; y < config.gridSizeY; ++y) {
        for (int x = 0; x < config.gridSizeX; ++x) {
            // Calculate PBR parameters based on grid position
            float roughness = static_cast<float>(x) / static_cast<float>(config.gridSizeX - 1);
            float metallic = static_cast<float>(y) / static_cast<float>(config.gridSizeY - 1);

            // Get color for this grid position
            glm::vec3 color = getColorForGridPosition(x, y, config.gridSizeX, config.gridSizeY);

            // Create material
            std::string matName = "SphereGrid_" + std::to_string(x) + "_" + std::to_string(y);
            auto material = createMaterialWithParams(matName, color, metallic, roughness);

            // Create game object
            std::string objName = "Sphere_" + std::to_string(x) + "_" + std::to_string(y);
            auto* sphere = new AnitoGameObject(objName, AnitoGameObject::PrimitiveType::Sphere);

            // Position in grid (centered around origin)
            float posX = (x - (config.gridSizeX - 1) / 2.0f) * config.spacing;
            float posY = (y - (config.gridSizeY - 1) / 2.0f) * config.spacing;
            sphere->setPosition(posX, posY, 0.0f);

            // Add mesh renderer
            auto* renderer = new AnitoMeshRenderer(objName + "_Renderer");
            renderer->setVertexBuffer(sphereMesh.vertexBuffer);
            renderer->setIndexBuffer(sphereMesh.indexBuffer);
            renderer->setMaterial(material);
            sphere->attachComponent(renderer);

            // Register
            manager->addObject(sphere);
            m_currentSceneObjects.push_back(sphere);
        }
    }
}

void AnitoPBRTestScenes::createCubeGridScene() {
    const SceneConfig& config = m_sceneConfigs[m_currentSceneIndex];
    auto* manager = AnitoGameObjectManager::getInstance();
    
    // Pre-generate cube mesh (reuse for all cubes)
    auto cubeMesh = AnitoMeshGenerator::createCube(config.objectScale * 1.5f);

    for (int y = 0; y < config.gridSizeY; ++y) {
        for (int x = 0; x < config.gridSizeX; ++x) {
            // Calculate PBR parameters
            float roughness = static_cast<float>(x) / static_cast<float>(config.gridSizeX - 1);
            float metallic = static_cast<float>(y) / static_cast<float>(config.gridSizeY - 1);

            // Get color
            glm::vec3 color = getColorForGridPosition(x, y, config.gridSizeX, config.gridSizeY);

            // Create material
            std::string matName = "CubeGrid_" + std::to_string(x) + "_" + std::to_string(y);
            auto material = createMaterialWithParams(matName, color, metallic, roughness);

            // Create game object
            std::string objName = "Cube_" + std::to_string(x) + "_" + std::to_string(y);
            auto* cube = new AnitoGameObject(objName, AnitoGameObject::PrimitiveType::Cube);

            // Position in grid (centered around origin)
            float posX = (x - (config.gridSizeX - 1) / 2.0f) * config.spacing;
            float posY = (y - (config.gridSizeY - 1) / 2.0f) * config.spacing;
            cube->setPosition(posX, posY, 0.0f);

            // Set random initial rotation
            cube->setRotation(
                m_rotationDist(m_randomGen),
                m_rotationDist(m_randomGen),
                m_rotationDist(m_randomGen)
            );

            // Add mesh renderer
            auto* renderer = new AnitoMeshRenderer(objName + "_Renderer");
            renderer->setVertexBuffer(cubeMesh.vertexBuffer);
            renderer->setIndexBuffer(cubeMesh.indexBuffer);
            renderer->setMaterial(material);
            cube->attachComponent(renderer);

            // Register
            manager->addObject(cube);
            m_currentSceneObjects.push_back(cube);
        }
    }
}

void AnitoPBRTestScenes::createMixedMaterialsScene() {
    const SceneConfig& config = m_sceneConfigs[m_currentSceneIndex];
    auto* manager = AnitoGameObjectManager::getInstance();
    
    // Pre-generate meshes
    auto sphereMesh = AnitoMeshGenerator::createSphere(config.objectScale, 32, 16);
    auto cubeMesh = AnitoMeshGenerator::createCube(config.objectScale * 1.5f);

    for (int y = 0; y < config.gridSizeY; ++y) {
        for (int x = 0; x < config.gridSizeX; ++x) {
            // Alternate between sphere and cube
            bool isSphere = (x + y) % 2 == 0;
            
            // Calculate PBR parameters
            float roughness = static_cast<float>(x) / static_cast<float>(config.gridSizeX - 1);
            float metallic = static_cast<float>(y) / static_cast<float>(config.gridSizeY - 1);

            // Get color
            glm::vec3 color = getColorForGridPosition(x, y, config.gridSizeX, config.gridSizeY);

            // Create material
            std::string matName = "Mixed_" + std::to_string(x) + "_" + std::to_string(y);
            auto material = createMaterialWithParams(matName, color, metallic, roughness);

            // Create game object
            std::string objName = (isSphere ? "Sphere_" : "Cube_") + std::to_string(x) + "_" + std::to_string(y);
            auto* obj = new AnitoGameObject(
                objName,
                isSphere ? AnitoGameObject::PrimitiveType::Sphere : AnitoGameObject::PrimitiveType::Cube
            );

            // Position (centered around origin)
            float posX = (x - (config.gridSizeX - 1) / 2.0f) * config.spacing;
            float posY = (y - (config.gridSizeY - 1) / 2.0f) * config.spacing;
            obj->setPosition(posX, posY, 0.0f);

            // Random rotation for cubes
            if (!isSphere) {
                obj->setRotation(
                    m_rotationDist(m_randomGen),
                    m_rotationDist(m_randomGen),
                    m_rotationDist(m_randomGen)
                );
            }

            // Add mesh renderer
            auto* renderer = new AnitoMeshRenderer(objName + "_Renderer");
            renderer->setVertexBuffer(isSphere ? sphereMesh.vertexBuffer : cubeMesh.vertexBuffer);
            renderer->setIndexBuffer(isSphere ? sphereMesh.indexBuffer : cubeMesh.indexBuffer);
            renderer->setMaterial(material);
            obj->attachComponent(renderer);

            // Register
            manager->addObject(obj);
            m_currentSceneObjects.push_back(obj);
        }
    }
}

void AnitoPBRTestScenes::createMetallicShowcaseScene() {
    const SceneConfig& config = m_sceneConfigs[m_currentSceneIndex];
    auto* manager = AnitoGameObjectManager::getInstance();
    
    auto sphereMesh = AnitoMeshGenerator::createSphere(config.objectScale * 1.2f, 32, 16);

    // Create showcase of common metals with varying roughness
    const std::vector<std::pair<std::string, glm::vec3>> metals = {
        {"Gold", glm::vec3(1.0f, 0.85f, 0.57f)},
        {"Silver", glm::vec3(0.97f, 0.96f, 0.91f)},
        {"Copper", glm::vec3(0.97f, 0.74f, 0.62f)},
        {"Aluminum", glm::vec3(0.91f, 0.92f, 0.92f)},
        {"Iron", glm::vec3(0.77f, 0.78f, 0.78f)},
        {"Titanium", glm::vec3(0.76f, 0.73f, 0.69f)},
        {"Brass", glm::vec3(0.98f, 0.90f, 0.59f)},
        {"Platinum", glm::vec3(0.83f, 0.81f, 0.78f)}
    };

    int index = 0;
    for (int y = 0; y < config.gridSizeY; ++y) {
        for (int x = 0; x < config.gridSizeX; ++x) {
            // Calculate which metal to use - cycle through available metals
            int metalIndex = index % metals.size();

            // Roughness varies across X
            float roughness = static_cast<float>(x) / static_cast<float>(config.gridSizeX - 1);
            roughness = 0.0f + roughness * 0.6f; // Range from 0.0 to 0.6 for metals

            // Always metallic
            float metallic = 1.0f;

            // Get metal color
            glm::vec3 color = metals[metalIndex].second;

            // Create material
            std::string matName = metals[metalIndex].first + "_" + std::to_string(y) + "_" + std::to_string(x);
            auto material = createMaterialWithParams(matName, color, metallic, roughness);

            // Create sphere
            std::string objName = "Metal_" + metals[metalIndex].first + "_" + std::to_string(y) + "_" + std::to_string(x);
            auto* sphere = new AnitoGameObject(objName, AnitoGameObject::PrimitiveType::Sphere);

            // Position (centered around origin)
            float posX = (x - (config.gridSizeX - 1) / 2.0f) * config.spacing;
            float posY = (y - (config.gridSizeY - 1) / 2.0f) * config.spacing;
            sphere->setPosition(posX, posY, 0.0f);

            // Add renderer
            auto* renderer = new AnitoMeshRenderer(objName + "_Renderer");
            renderer->setVertexBuffer(sphereMesh.vertexBuffer);
            renderer->setIndexBuffer(sphereMesh.indexBuffer);
            renderer->setMaterial(material);
            sphere->attachComponent(renderer);

            manager->addObject(sphere);
            m_currentSceneObjects.push_back(sphere);

            index++;
        }
    }
}

void AnitoPBRTestScenes::createDielectricShowcaseScene() {
    const SceneConfig& config = m_sceneConfigs[m_currentSceneIndex];
    auto* manager = AnitoGameObjectManager::getInstance();
    
    auto sphereMesh = AnitoMeshGenerator::createSphere(config.objectScale * 1.2f, 32, 16);

    // Common dielectric materials with their typical colors
    const std::vector<std::pair<std::string, glm::vec3>> dielectrics = {
        {"Red Plastic", glm::vec3(0.81f, 0.0f, 0.0f)},
        {"Blue Plastic", glm::vec3(0.0f, 0.0f, 0.81f)},
        {"Green Plastic", glm::vec3(0.0f, 0.81f, 0.0f)},
        {"White Ceramic", glm::vec3(0.9f, 0.9f, 0.9f)},
        {"Wood", glm::vec3(0.55f, 0.35f, 0.15f)},
        {"Rubber", glm::vec3(0.1f, 0.1f, 0.1f)},
        {"Glass", glm::vec3(0.9f, 0.95f, 1.0f)},
        {"Stone", glm::vec3(0.6f, 0.6f, 0.55f)}
    };

    int index = 0;
    for (int y = 0; y < config.gridSizeY; ++y) {
        for (int x = 0; x < config.gridSizeX; ++x) {
            // Calculate which dielectric to use - cycle through available dielectrics
            int dielectricIndex = index % dielectrics.size();

            // Roughness varies across X
            float roughness = static_cast<float>(x) / static_cast<float>(config.gridSizeX - 1);
            roughness = 0.2f + roughness * 0.8f; // Range from 0.2 to 1.0 for dielectrics

            // Never metallic
            float metallic = 0.0f;

            // Get dielectric color
            glm::vec3 color = dielectrics[dielectricIndex].second;

            // Create material
            std::string matName = dielectrics[dielectricIndex].first + "_" + std::to_string(y) + "_" + std::to_string(x);
            auto material = createMaterialWithParams(matName, color, metallic, roughness);

            // Create sphere
            std::string objName = "Dielec_" + dielectrics[dielectricIndex].first + "_" + std::to_string(y) + "_" + std::to_string(x);
            auto* sphere = new AnitoGameObject(objName, AnitoGameObject::PrimitiveType::Sphere);

            // Position (centered around origin)
            float posX = (x - (config.gridSizeX - 1) / 2.0f) * config.spacing;
            float posY = (y - (config.gridSizeY - 1) / 2.0f) * config.spacing;
            sphere->setPosition(posX, posY, 0.0f);

            // Add renderer
            auto* renderer = new AnitoMeshRenderer(objName + "_Renderer");
            renderer->setVertexBuffer(sphereMesh.vertexBuffer);
            renderer->setIndexBuffer(sphereMesh.indexBuffer);
            renderer->setMaterial(material);
            sphere->attachComponent(renderer);

            manager->addObject(sphere);
            m_currentSceneObjects.push_back(sphere);

            index++;
        }
    }
}

std::shared_ptr<AnitoMaterial> AnitoPBRTestScenes::createMaterialWithParams(
    const std::string& name,
    const glm::vec3& baseColor,
    float metallic,
    float roughness)
{
    auto material = std::make_shared<AnitoMaterial>(name);
    material->setShader(m_shader);
    material->setBaseColor(baseColor.r, baseColor.g, baseColor.b, 1.0f);
    material->setMetallic(metallic);
    material->setRoughness(roughness);
    material->setEmissive(0.0f, 0.0f, 0.0f);
    
    return material;
}

glm::vec3 AnitoPBRTestScenes::getColorForGridPosition(int x, int y, int gridX, int gridY) {
    // Create a color gradient across the grid
    float hue = (static_cast<float>(x) / static_cast<float>(gridX) + 
                 static_cast<float>(y) / static_cast<float>(gridY)) * 0.5f;

    // Convert HSV to RGB (simple approximation)
    hue *= 6.0f;
    int hi = static_cast<int>(hue);
    float f = hue - hi;

    glm::vec3 color;
    switch (hi % 6) {
        case 0: color = glm::vec3(1.0f, f, 0.0f); break;
        case 1: color = glm::vec3(1.0f - f, 1.0f, 0.0f); break;
        case 2: color = glm::vec3(0.0f, 1.0f, f); break;
        case 3: color = glm::vec3(0.0f, 1.0f - f, 1.0f); break;
        case 4: color = glm::vec3(f, 0.0f, 1.0f); break;
        case 5: color = glm::vec3(1.0f, 0.0f, 1.0f - f); break;
    }

    return color * 0.8f + glm::vec3(0.2f); // Avoid too dark colors
}

glm::vec3 AnitoPBRTestScenes::getMetallicColor(int index) {
    const std::vector<glm::vec3> metallicColors = {
        glm::vec3(1.0f, 0.85f, 0.57f),   // Gold
        glm::vec3(0.97f, 0.96f, 0.91f),  // Silver
        glm::vec3(0.97f, 0.74f, 0.62f),  // Copper
        glm::vec3(0.91f, 0.92f, 0.92f),  // Aluminum
        glm::vec3(0.77f, 0.78f, 0.78f),  // Iron
        glm::vec3(0.76f, 0.73f, 0.69f),  // Titanium
        glm::vec3(0.98f, 0.90f, 0.59f),  // Brass
        glm::vec3(0.83f, 0.81f, 0.78f)   // Platinum
    };
    
    return metallicColors[index % metallicColors.size()];
}

glm::vec3 AnitoPBRTestScenes::getDielectricColor(int index) {
    const std::vector<glm::vec3> dielectricColors = {
        glm::vec3(0.81f, 0.0f, 0.0f),    // Red plastic
        glm::vec3(0.0f, 0.0f, 0.81f),    // Blue plastic
        glm::vec3(0.0f, 0.81f, 0.0f),    // Green plastic
        glm::vec3(0.9f, 0.9f, 0.9f),     // White ceramic
        glm::vec3(0.55f, 0.35f, 0.15f),  // Wood
        glm::vec3(0.1f, 0.1f, 0.1f),     // Rubber
        glm::vec3(0.9f, 0.95f, 1.0f),    // Glass
        glm::vec3(0.6f, 0.6f, 0.55f)     // Stone
    };
    
    return dielectricColors[index % dielectricColors.size()];
}

} // namespace Anito
