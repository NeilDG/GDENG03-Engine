#include "GraphicsEngine.h"
#include "SwapChain.h"
#include <dxgi.h>
#include <system_error>
#include "VertexShader.h"
#include "PixelShader.h"


GraphicsEngine* GraphicsEngine::sharedInstance = NULL;

GraphicsEngine* GraphicsEngine::getInstance()
{
    return sharedInstance;
}

void GraphicsEngine::initialize()
{
    sharedInstance = new GraphicsEngine();
    sharedInstance->init();
}

void GraphicsEngine::destroy()
{
    if (sharedInstance != NULL) {
        sharedInstance->release();
    }
}

SwapChain* GraphicsEngine::getSwapChain()
{
    if (this->activeSwapChain == NULL) {
        this->activeSwapChain = new SwapChain(this->directXDevice);
    }
    
    return this->activeSwapChain;
}

DeviceContext* GraphicsEngine::getImmediateContext()
{
    return this->customContext;
}

IDXGIFactory* GraphicsEngine::getDirectXFactory()
{
    return this->dxFactory;
}

ID3D11Device* GraphicsEngine::getDirectXDevice()
{
    return this->directXDevice;
}

VertexBuffer* GraphicsEngine::createVertexBuffer()
{
    return new VertexBuffer();
}

TexturedVertexBuffer* GraphicsEngine::createTexturedBuffer()
{
    return new TexturedVertexBuffer();
}

ConstantBuffer* GraphicsEngine::createConstantBuffer()
{
    return new ConstantBuffer();
}

IndexBuffer* GraphicsEngine::createIndexBuffer()
{
    return new IndexBuffer();
}

VertexShader* GraphicsEngine::createVertexShader(void* shaderByteCode, size_t byteCodeSize)
{
    VertexShader* vertexShader = new VertexShader();
    vertexShader->init(shaderByteCode, byteCodeSize);
    return vertexShader;
}

PixelShader* GraphicsEngine::createPixelShader(void* shaderByteCode, size_t byteCodeSize)
{
    PixelShader* pixelShader = new PixelShader();
    pixelShader->init(shaderByteCode, byteCodeSize);
    return pixelShader;
}


void GraphicsEngine::compileVertexShader(const wchar_t* fileName, const char* entryPointName, void** shaderByteCode, size_t* byteCodeSize)
{
    ID3DBlob* resultBlob = NULL;
    ID3DBlob* errorBlob = NULL;
    HRESULT compileResult = ::D3DCompileFromFile(fileName, NULL, NULL, entryPointName, "vs_5_0", NULL, NULL, &resultBlob, &errorBlob);

    if (FAILED(compileResult)) {
        if (errorBlob) errorBlob->Release();
        std::cout << "An error occured when compiling vertex shader. \n";
    }

    *shaderByteCode = resultBlob->GetBufferPointer();
    *byteCodeSize = resultBlob->GetBufferSize();
}

void GraphicsEngine::compilePixelShader(const wchar_t* fileName, const char* entryPointName, void** shaderByteCode, size_t* byteCodeSize)
{
    ID3DBlob* resultBlob = NULL;
    ID3DBlob* errorBlob = NULL;
    HRESULT compileResult = ::D3DCompileFromFile(fileName, NULL, NULL, entryPointName, "ps_5_0", NULL, NULL, &resultBlob, &errorBlob);

    if (FAILED(compileResult)) {
        if (errorBlob) errorBlob->Release();
        std::cout << "An error occured when compiling pixel shader. \n";
    }

    *shaderByteCode = resultBlob->GetBufferPointer();
    *byteCodeSize = resultBlob->GetBufferSize();
}

void GraphicsEngine::init()
{
    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE, //uses GPU
        D3D_DRIVER_TYPE_WARP, //uses CPU
        D3D_DRIVER_TYPE_REFERENCE, //CPU, but slowest.
    };

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    HRESULT res = 0;
    for (UINT i = 0; i < numDriverTypes; i++) {
        res = D3D11CreateDevice(NULL, driverTypes[i], NULL, NULL, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &this->directXDevice, &this->featureLevel, &this->deviceContext);

        if (SUCCEEDED(res)) {
            std::cout << "Selected driver type: " << driverTypes[i] << "\n";
            break;
        }
    }

    if (SUCCEEDED(res)) {
        this->directXDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&this->dxgiDevice);
        this->dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&this->dxAdapter);
        res = this->dxAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&this->dxFactory);
        this->customContext = new DeviceContext(this->deviceContext);
        if (SUCCEEDED(res)) {
            std::cout << "DX factory for graphics engine created. " << this->dxFactory << "\n";
            std::string message = std::system_category().message(res);
            std::cout << message << "\n";
        }
        else {
            std::cout << "An error occured when creating DX factory \n";
        }
        
        
    }
}

void GraphicsEngine::release()
{
    this->dxgiDevice->Release();
    this->dxAdapter->Release();
    this->dxFactory->Release();

    this->deviceContext->Release();
    this->customContext->release();
    this->directXDevice->Release();
    std::cout << "Destroyed graphics engine \n";
}

GraphicsEngine::GraphicsEngine()
{
}

GraphicsEngine::~GraphicsEngine()
{
}
