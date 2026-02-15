#include "common.hlsl"

cbuffer MatrixBuffer : register(b0)
{
	matrix WorldViewProj;
}

struct VS_INPUT
{
	float3 pos : POSITION;
};

struct GS_INPUT
{
	float4 pos : SV_POSITION;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
};

// ==========================
// 頂点シェーダー
// ==========================
GS_INPUT VSMain(VS_INPUT input)
{
	GS_INPUT output;
	output.pos = mul(float4(input.pos, 1.0f), WorldViewProj);
	return output;
}

// ==========================
// ジオメトリシェーダー
// ==========================
[maxvertexcount(6)]
void GSMain(triangle GS_INPUT input[3], inout LineStream<PS_INPUT> lineStream)
{
	[unroll]
	for (int i = 0; i < 3; ++i)
	{
		PS_INPUT v1;
		PS_INPUT v2;

		v1.pos = input[i].pos;
		v2.pos = input[(i + 1) % 3].pos;

		lineStream.Append(v1);
		lineStream.Append(v2);
		lineStream.RestartStrip();
	}
}

// ==========================
// ピクセルシェーダー
// ==========================
float4 PSMain(PS_INPUT input) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f); // 緑色のワイヤー
}
