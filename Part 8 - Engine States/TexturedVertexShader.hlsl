struct VS_INPUT {
	float4 pos : POSITION;
	float2 texcoord: TEXCOORD;
};

struct VS_OUTPUT {
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

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	//WORLD SPACE
	output.pos = mul(input.pos, world);

	//VIEW SPACE
	output.pos = mul(output.pos, view);

	//PROJ SPACE
	output.pos = mul(output.pos, proj);

	output.texcoord = input.texcoord;
	return output;
}