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

SwapChain* GraphicsEngine::createSwapChain()
{
    return new SwapChain(this->directXDevice);
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
    ID3DBlob* errorBlob = NULL;
    HRESULT compileResult = ::D3DCompileFromFile(fileName, NULL, NULL, entryPointName, "vs_5_0", NULL, NULL, &this->customBlob, &errorBlob);

    if (FAILED(compileResult)) {
        if (errorBlob) errorBlob->Release();
        std::cout << "An error occured when compiling vertex shader. \n";
    }

    *shaderByteCode = this->customBlob->GetBufferPointer();
    *byteCodeSize = this->customBlob->GetBufferSize();
}

void GraphicsEngine::compilePixelShader(const wchar_t* fileName, const char* entryPointName, void** shaderByteCode, size_t* byteCodeSize)
{
    ID3DBlob* errorBlob = NULL;
    HRESULT compileResult = ::D3DCompileFromFile(fileName, NULL, NULL, entryPointName, "ps_5_0", NULL, NULL, &this->customBlob, &errorBlob);

    if (FAILED(compileResult)) {
        if (errorBlob) errorBlob->Release();
        std::cout << "An error occured when compiling pixel shader. \n";
    }

    *shaderByteCode = this->customBlob->GetBufferPointer();
    *byteCodeSize = this->customBlob->GetBufferSize();
}

void GraphicsEngine::releaseCompiledShader()
{
    this->customBlob->Release();
}

/*void GraphicsEngine::createShaders()
{
    ID3DBlob* errblob = nullptr;
    //D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", NULL, NULL, &m_vsblob, &errblob);
    D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", NULL, NULL, &m_psblob, &errblob);
    //this->directXDevice->CreateVertexShader(m_vsblob->GetBufferPointer(), m_vsblob->GetBufferSize(), nullptr, &m_vs);
    this->directXDevice->CreatePixelShader(m_psblob->GetBufferPointer(), m_psblob->GetBufferSize(), nullptr, &m_ps);
}*/

/*void GraphicsEngine::setShaders()
{
    //this->deviceContext->VSSetShader(m_vs, nullptr, 0);
    this->deviceContext->PSSetShader(m_ps, nullptr, 0);
}*/

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
