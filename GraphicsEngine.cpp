#include "GraphicsEngine.h"
#include "SwapChain.h"

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
    return new SwapChain();
}

void GraphicsEngine::initializeSwapChain(HWND windowHandle, IDXGISwapChain* swapChain, UINT width, UINT height)
{
    DXGI_SWAP_CHAIN_DESC desc;
    ZeroMemory(&desc, sizeof(desc)); //fills occupied memory with zeroes
    desc.BufferCount = 1;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = windowHandle;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Windowed = true; 

    HRESULT hr = this->dxFactory->CreateSwapChain(this->directXDevice, &desc, &swapChain);

    if (FAILED(hr)) {
        std::cout << "An error occured when creating a swap chain. \n";
    }
}

bool GraphicsEngine::init()
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
            break;
            i++;
        }
    }

    if (SUCCEEDED(res)) {
        this->directXDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&this->dxgiDevice);
        this->dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&this->dxAdapter);
        this->dxAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&this->dxFactory);
        std::cout << "Initialized graphics engine \n";
        return true;
    }
    else {
        return false;
    } 
}

bool GraphicsEngine::release()
{
    this->dxgiDevice->Release();
    this->dxAdapter->Release();
    this->dxFactory->Release();

    this->deviceContext->Release();
    this->directXDevice->Release();
    std::cout << "Destroyed graphics engine \n";
    return true;
}

GraphicsEngine::GraphicsEngine()
{
}

GraphicsEngine::~GraphicsEngine()
{
}
