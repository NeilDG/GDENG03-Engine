#include "AnitoEngine.h"
#include "Window/AnitoWindow.h"
#include "Renderer/AnitoRenderer.h"
#include "Renderer/AnitoVertexBuffer.h"
#include "Renderer/AnitoShader.h"
#include "Renderer/AnitoMaterial.h"
#include "Renderer/AnitoMeshGenerator.h"
#include "GameObjects/AnitoGameObjectManager.h"
#include "GameObjects/AnitoGameObject.h"
#include "Components/AnitoMeshRenderer.h"
#include "Components/AnitoTransform.h"
#include "Components/AnitoCamera.h"
#include "Input/AnitoInputManager.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace Anito {

AnitoEngine* AnitoEngine::s_instance = nullptr;

AnitoEngine::AnitoEngine()
    : m_window(nullptr)
    , m_running(false)
    , m_lastFrameTime(0.0f)
{
    s_instance = this;
}

AnitoEngine::~AnitoEngine() {
    shutdown();
    s_instance = nullptr;
}

bool AnitoEngine::initialize(const std::string& title, uint32_t width, uint32_t height) {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Anito Engine - Project Anito (DOST-PCIEERD)" << std::endl;
    std::cout << "  DLSU GAME Lab - AAA Game Engine Development" << std::endl;
    std::cout << "  Principal Investigator: Neil Patrick Del Gallego, Ph.D." << std::endl;
    std::cout << "==================================================" << std::endl;

    // Create window
    m_window = AnitoWindow::create(title, width, height);
    if (!m_window) {
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    // Initialize renderer
    void* nativeHandle = m_window->getNativeWindowHandle();
    AnitoRenderer::initialize(nativeHandle, width, height);

    // Set up window resize callback
    m_window->setResizeCallback([](uint32_t w, uint32_t h) {
        if (AnitoRenderer::getInstance()) {
            AnitoRenderer::getInstance()->resize(w, h);
        }
    });

    // Initialize vertex layouts
    PosColorVertex::init();
    PosNormalTexcoordVertex::init();

    // Initialize GameObjectManager
    AnitoGameObjectManager::initialize();

    // Create test scene
    createTestScene();

    m_running = true;
    m_lastFrameTime = static_cast<float>(glfwGetTime());

    std::cout << "Anito Engine initialized successfully!" << std::endl;
    std::cout << "==================================================" << std::endl;
    return true;
}

void AnitoEngine::createTestScene() {
    std::cout << "\n[Scene Setup] Creating test scene..." << std::endl;

    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer) return;

    // Load shader
    std::cout << "[Scene Setup] Loading shader..." << std::endl;
    auto shader = AnitoShader::createOrGet("SimpleShader", 
        "assets/shaders/compiled/vs_simple.bin",
        "assets/shaders/compiled/fs_simple.bin");

    if (!shader || !shader->isValid()) {
        std::cerr << "[Scene Setup] WARNING: Failed to load shader!" << std::endl;
        std::cerr << "[Scene Setup] Shaders need to be compiled first." << std::endl;
        std::cerr << "[Scene Setup] TO COMPILE SHADERS:" << std::endl;
        std::cerr << "[Scene Setup]   1. Download bgfx prebuilt tools from:" << std::endl;
        std::cerr << "[Scene Setup]      https://github.com/bkaradzic/bgfx/releases" << std::endl;
        std::cerr << "[Scene Setup]   2. Extract shaderc.exe to: external/bgfx/.build/win64_vs2022/bin/" << std::endl;
        std::cerr << "[Scene Setup]   3. Run: powershell -ExecutionPolicy Bypass -File tools/compile_shaders.ps1" << std::endl;
        std::cerr << "[Scene Setup] " << std::endl;
        std::cerr << "[Scene Setup] For now, the engine will run but not render anything." << std::endl;
        std::cout << "==================================================" << std::endl;
        return;
    }

    // Create uniforms for lighting
    shader->createUniform("u_lightDir", bgfx::UniformType::Vec4);
    shader->createUniform("u_baseColor", bgfx::UniformType::Vec4);
    shader->createUniform("u_pbrParams", bgfx::UniformType::Vec4);

    // Create material
    std::cout << "[Scene Setup] Creating material..." << std::endl;
    auto material = std::make_shared<AnitoMaterial>("TestMaterial");
    material->setShader(shader);
    material->setBaseColor(1.0f, 1.0f, 1.0f, 1.0f);  // Pure white for maximum visibility
    material->setEmissive(0.2f, 0.2f, 0.2f);  // Add emissive to make it visible even without lighting
    material->setMetallic(0.0f);  // Non-metallic for better visibility
    material->setRoughness(0.8f);

    // Generate cube mesh
    std::cout << "[Scene Setup] Generating cube mesh..." << std::endl;
    auto cubeMesh = AnitoMeshGenerator::createCube(2.0f);

    // Create cube GameObject
    std::cout << "[Scene Setup] Creating cube GameObject..." << std::endl;
    AnitoGameObject* cubeObject = new AnitoGameObject("TestCube", AnitoGameObject::PrimitiveType::Cube);
    cubeObject->setPosition(0.0f, 0.0f, 0.0f);

    // Add mesh renderer component
    AnitoMeshRenderer* meshRenderer = new AnitoMeshRenderer("CubeMeshRenderer");
    meshRenderer->setVertexBuffer(cubeMesh.vertexBuffer);
    meshRenderer->setIndexBuffer(cubeMesh.indexBuffer);
    meshRenderer->setMaterial(material);
    cubeObject->attachComponent(meshRenderer);

    // Register with manager
    AnitoGameObjectManager::getInstance()->addObject(cubeObject);

    // Setup camera view
    std::cout << "[Scene Setup] Setting up camera..." << std::endl;
    AnitoMatrix4x4 view = AnitoMatrix4x4::lookAt(
        glm::vec3(0.0f, 2.0f, 5.0f),   // Eye position
        glm::vec3(0.0f, 0.0f, -5.0f),  // Look at cube
        glm::vec3(0.0f, 1.0f, 0.0f)    // Up vector
    );

    float aspect = static_cast<float>(renderer->getWidth()) / static_cast<float>(renderer->getHeight());
    AnitoMatrix4x4 proj = AnitoMatrix4x4::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

    bgfx::setViewTransform(renderer->getMainViewId(), view.data(), proj.data());

    // Set light direction (pointing FROM the surface TO the light source)
    // Negate to point downward-left-back so surfaces facing up-right-front are lit
    // w component = intensity (try higher value for visibility)
    float lightDir[4] = { -0.5f, -1.0f, -0.3f, 2.0f };
    shader->setUniform("u_lightDir", lightDir);

    std::cout << "[Scene Setup] Test scene created successfully!" << std::endl;
    std::cout << "  - Cube at (0, 0, -5)" << std::endl;
    std::cout << "  - Camera at (0, 2, 5)" << std::endl;
    std::cout << "  - Material base color: (1.0, 1.0, 1.0)" << std::endl;
    std::cout << "  - Light direction: (-0.5, -1.0, -0.3), intensity: 2.0" << std::endl;
    std::cout << "==================================================" << std::endl;
}

void AnitoEngine::run() {
    std::cout << "\n[Engine] Starting main loop..." << std::endl;

    while (m_running && !m_window->shouldClose()) {
        // Calculate delta time
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        // Process events
        m_window->pollEvents();

        // Update
        update(deltaTime);

        // Render
        render();
    }

    std::cout << "[Engine] Main loop ended." << std::endl;
}

void AnitoEngine::shutdown() {
    std::cout << "\n[Engine] Shutting down Anito Engine..." << std::endl;

    // Destroy game objects
    if (AnitoGameObjectManager::getInstance()) {
        AnitoGameObjectManager::destroy();
    }

    // Clear shader and texture caches
    AnitoShader::clearCache();
    AnitoTexture::clearCache();

    // Destroy subsystems in reverse order
    AnitoRenderer::destroy();

    if (m_window) {
        delete m_window;
        m_window = nullptr;
    }

    m_running = false;
    std::cout << "[Engine] Shutdown complete." << std::endl;
}

void AnitoEngine::update(float deltaTime) {
    // Update input
    if (AnitoInputManager::getInstance()) {
        AnitoInputManager::getInstance()->update();
    }

    // Update all game objects (including rotation)
    if (AnitoGameObjectManager::getInstance()) {
        AnitoGameObjectManager::getInstance()->updateAll(deltaTime);

        // Rotate the test cube
        AnitoGameObject* cube = AnitoGameObjectManager::getInstance()->findObjectByName("TestCube");
        if (cube) {
            AnitoVector3D currentRot = cube->getRotation();
            cube->setRotation(
                currentRot.x() + deltaTime * 0.5f,
                currentRot.y() + deltaTime * 4.0f,
                currentRot.z() + deltaTime * 0.3f
            );
        }
    }
}

void AnitoEngine::render() {
    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer) return;

    renderer->beginFrame();

    // Render all game objects
    if (AnitoGameObjectManager::getInstance()) {
        AnitoGameObjectManager::getInstance()->renderAll();
    }

    renderer->endFrame();
}

} // namespace Anito
