#include "helper.hlsl"

//the simplest vertex shader imageinable - just pass the position and colour straight out
VSOut main( AppData IN )
{
    VSOut OUT;

	OUT.position = float4(IN.position, 1.f);
	OUT.color = IN.color;

    return OUT;
}

