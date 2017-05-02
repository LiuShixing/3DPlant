
cbuffer cbPerObject
{
   float4x4 gWorldViewProj;
};
// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
   float3 PosL     :POSITION;
   float2 Tex      :TEXCOORD;
};
struct VertexOut
{
   float4 PosH      :SV_POSITION;
   float2 Tex       :TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
   VertexOut vout;
   vout.PosH=mul(float4(vin.PosL,1.0f),gWorldViewProj);
   vout.Tex=vin.Tex;
   return vout;
}
float4 PS(VertexOut pin) :SV_Target
{
   float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
   texColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex );
   clip(texColor.a - 0.1f);
   return texColor;
}
technique11 PosTexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }

}