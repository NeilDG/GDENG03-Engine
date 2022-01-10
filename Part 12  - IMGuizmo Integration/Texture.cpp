#include "Texture.h"
#include <iostream>
#include <DirectXTex.h> //NOTE: Using x86 library for compatibility issues
#include "GraphicsEngine.h"
#include "LogUtils.h"

Texture::Texture(const wchar_t* fullPath): AResource(fullPath)
{
	DirectX::ScratchImage imageData;
	HRESULT res = DirectX::LoadFromWICFile(fullPath, DirectX::WIC_FLAGS_NONE, NULL, imageData);

	if (SUCCEEDED(res)) {
		ID3D11Device* dxDevice = GraphicsEngine::getInstance()->getDirectXDevice();
		res = DirectX::CreateTexture(dxDevice, imageData.GetImages(),
			imageData.GetImageCount(), imageData.GetMetadata(), &this->myTexture);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = imageData.GetMetadata().format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = (UINT)imageData.GetMetadata().mipLevels;
		desc.Texture2D.MostDetailedMip = 0;

		dxDevice->CreateShaderResourceView(this->myTexture, &desc, &this->shaderResView);
	}
	else {
		LogUtils::PrintHRResult(res);
		std::cout << "Texture not created successfully. \n";
	}
}

Texture::~Texture()
{
	AResource::~AResource();

	if(this->shaderResView != nullptr)
		this->shaderResView->Release();

	if(this->myTexture != nullptr)
		this->myTexture->Release();
}

AResource::String Texture::getPath()
{
	return this->fullPath;
}

ID3D11ShaderResourceView* Texture::getShaderResource()
{
	return this->shaderResView;
}
