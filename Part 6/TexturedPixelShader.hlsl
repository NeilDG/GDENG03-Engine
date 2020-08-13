/*Texture2D Texture: register(t0);
sampler TextureSampler: register(s0);

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float2 texcoord: TEXCOORD;
};

cbuffer constant: register(b0) {
	//unsigned int time;
	row_major float4x4 world;
	row_major float4x4 view;
	row_major float4x4 proj;
	float time;
}

float4 psmain(PS_INPUT input) : SV_TARGET
{
	return Texture.Sample(TextureSampler,input.texcoord);
}*/

struct PS_INPUT {
	float4 pos: SV_POSITION;
	float3 color: COLOR;
	float3 color2: COLOR1;
};

cbuffer constant: register(b0) {
	//unsigned int time;
	row_major float4x4 world;
	row_major float4x4 view;
	row_major float4x4 proj;
	float time;
}

float4 main(PS_INPUT input) : SV_TARGET
{
	//return float4(input.color, 1.0f);
	//return float4(lerp(input.color, input.color2, (sin(time) + 1.0f) / 2.0f),1.0f);
	return float4(lerp(input.color, input.color2, (sin(time))), 1.0f);
}