struct VS_INPUT {
	float4 pos : POSITION;
	float3 color: COLOR;
};

struct VS_OUTPUT {
	float4 pos: SV_POSITION;
	float3 color: COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.pos = input.pos;
	output.color = input.color;

	if (output.pos.y > 0 && output.pos.y < 1) {
		output.pos.x += 0.25f;
	}

	if (output.pos.y > -1 && output.pos.y < 0 && output.pos.x > 0 && output.pos.x < 1) {
		output.pos.y += 0.25f;
	}
	return output;
}