float3 Light_Position  : LightPosition = float3(0.0f,6000.0f ,0.0f);
float4 Light_Attenuation = float4(0.0f,0.0f,0.0f,0.0f);
float4 Light_Color  : LightColor = float4( 1.0f,1.0f, 1.0f,1.0f );
float4x4 matViewProjection : ViewProjection;
float4x4 matWorld;
float PlaneFactor;
float Range;
float Plane_Coef_a=0;
float Plane_Coef_b=0;
float Plane_Coef_c=-1;
float Plane_Coef_d=0;
float delta=0.5;
float brightness=1;
float brightnessISP=0.2;
float alpha;
float alpha_c;
float alpha_p;
float alpha_s;

texture heightTexture;
texture normalTexture;

sampler hTexture	=	sampler_state
{
	Texture	=	(heightTexture);
};

sampler nTexture	=	sampler_state
{
	Texture	=	(normalTexture);
};

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float3 Normal : NORMAL0;
   float4 Color : COLOR0;
};

struct VS_OUTPUT
{   
float4 Position   : POSITION;
float4 Color :COLOR0;
};

float4 Light_PointDiffuse(float3 VertPos, float3 VertNorm, float3 LightPos,
                          float4 LightColor, float4 LightAttenuation)
{
   // Determine the distance from the light to the vertex and the direction
   float3 LightDir = LightPos - VertPos;
   float  Dist = length(LightDir);
   LightDir = LightDir / Dist;

   // Compute distance based attenuation. This is defined as:
   // Attenuation = 1 / ( LA.x + LA.y*Dist + LA.z*Dist*Dist )
   float DistAttn = clamp(0,1, 1 / ( LightAttenuation.x +
                                     LightAttenuation.y * Dist + 
                                     LightAttenuation.z * Dist * Dist ));

   // Compute suface/light angle based attenuation defined as dot(N,L)
   // Note : This must be clamped as it may become negative.
   float AngleAttn = clamp(0, 1, dot(VertNorm, LightDir) );
 
   // Compute final lighting
   return LightColor * DistAttn * AngleAttn;
}

float4 WithoutLightVS( in float3 vPosition : POSITION ) :POSITION
{
	float x =vPosition.x;
	float y=vPosition.y;
	float4 z=tex2Dlod(hTexture,float4((x+150)/300,(y+150)/300,0,0)); 
	return mul( float4( x*PlaneFactor, y*PlaneFactor, z.w*PlaneFactor, 1.0f ), matViewProjection );
}  

float4 WithoutLightPS() : COLOR0
{   
   return( float4( brightness, brightness, brightness, alpha_p ) );
}


VS_OUTPUT WithLightVS( in float3 vPosition : POSITION )
{
	VS_OUTPUT Output;
	float x =vPosition.x;
	float y=vPosition.y;
	float4 z=tex2Dlod(hTexture,float4((x+150)/300,(y+150)/300,0,0));
	Output.Position = mul( float4( x*PlaneFactor, y*PlaneFactor, z.w*PlaneFactor, 1.0f ), matViewProjection );
	float3 inNormal =tex2Dlod(nTexture,float4((x+150)/300,-(y+150)/300,0,0)).xyz;
	float3 NormalWorld = mul( inNormal, (float3x3)matWorld );
	float4 Color;
	Color= Light_PointDiffuse(vPosition, NormalWorld, Light_Position, 
								   Light_Color, Light_Attenuation);
	Output.Color =Color;
	Output.Color.a=alpha_s;
	return Output;
}  


VS_OUTPUT CalNormalVS( in float3 vPosition : POSITION )
{
	VS_OUTPUT Output;
	float x=vPosition.x;
	float y=vPosition.y;
	float4 normal=float4(tex2Dlod(hTexture,float4((x-1+150)/300,(y+150)/300,0,0)).w
                     -tex2Dlod(hTexture,float4((x+1+150)/300,(y+150)/300,0,0)).w,
                     tex2Dlod(hTexture,float4((x+150)/300,(y-1+150)/300,0,0)).w
                     -tex2Dlod(hTexture,float4((x+150)/300,(y+1+150)/300,0,0)).w,1,1);
	Output.Position =  float4( vPosition.x/150, vPosition.y/150, 1.0, 1);
	Output.Color=normal;
	return Output;
} ;

VS_OUTPUT CoordinateVS( VS_INPUT Input )
{
   VS_OUTPUT Output;
   Output.Position = mul( Input.Position, matViewProjection );
   float3 NormalWorld = mul( Input.Normal, (float3x3)matWorld );
   Output.Color =Light_PointDiffuse(Input.Position, NormalWorld, Light_Position.xyz, 
                           Light_Color, Light_Attenuation);
   Output.Color.a=alpha_c;
   return Output;

}

VS_OUTPUT RPwithPlaneVS( in float3 vPosition : POSITION ) 
{
	VS_OUTPUT Output;
	float x =vPosition.x;
	float y=vPosition.y;
	float4 z=tex2Dlod(hTexture,float4((x+150)/300,(y+150)/300,0,0)); 
	float p=(Plane_Coef_a*x-Plane_Coef_b*y+Plane_Coef_c*z.w+Plane_Coef_d*49.5/PlaneFactor/Range);
	Output.Position= mul( float4( x*PlaneFactor, y*PlaneFactor, z.w*PlaneFactor, 1.0f ), matViewProjection );
	float4 Color;
	if(p<delta && p>-delta){
		Color=float4(1,1,1,1);
	}else{
		Color=float4(brightnessISP,brightnessISP,brightnessISP,0.5);
	}
	Output.Color=Color;
	return Output;
}  

VS_OUTPUT InsertPlaneVS( in float3 vPosition : POSITION )
{
   VS_OUTPUT Output;
	float x =vPosition.x/PlaneFactor;
	float y=vPosition.y/PlaneFactor;
	float f=1/sqrt(Plane_Coef_c*Plane_Coef_c+Plane_Coef_a*Plane_Coef_a);
	float3 pos;
	float3 rx={1,0,0};
	float3x3 R={Plane_Coef_c*f,0,-Plane_Coef_a*f,
						0,1,0,
						Plane_Coef_a*f,0,Plane_Coef_c*f};
	pos=mul(float3(x,y,0),R);
	rx=mul(rx,R);	
	float cosa=sqrt((Plane_Coef_a*Plane_Coef_a+Plane_Coef_c*Plane_Coef_c)/(Plane_Coef_a*Plane_Coef_a+Plane_Coef_b*Plane_Coef_b+Plane_Coef_c*Plane_Coef_c));
	float sina=-Plane_Coef_b/sqrt(Plane_Coef_a*Plane_Coef_a+Plane_Coef_b*Plane_Coef_b+Plane_Coef_c*Plane_Coef_c);
	float3x3 R1={	rx.x*rx.x*(1-cosa)+cosa,rx.x*rx.y*(1-cosa)+rx.z*sina,rx.x*rx.z*(1-cosa)-rx.y*sina,
							rx.x*rx.y*(1-cosa)-rx.z*sina,rx.y*rx.y*(1-cosa)+cosa,rx.y*rx.z*(1-cosa)+rx.x*sina,
				    		rx.x*rx.z*(1-cosa)+rx.y*sina,rx.y*rx.z*(1-cosa)-rx.x*sina,rx.z*rx.z*(1-cosa)+cosa};
	pos=mul(pos,R1);			 				
	
	Output.Position = mul( float4(pos.x,-pos.y,pos.z-Plane_Coef_d/Plane_Coef_c*49.5/PlaneFactor/Range,1.0f/PlaneFactor)*PlaneFactor, matViewProjection );
    Output.Color = float4(0.3,0.4,0.5,alpha);
    return( Output ); 
}


float4 ps_main(float4 col:COLOR0) : COLOR0
{   
   return col;
}


//--------------------------------------------------------------//
// Technique Section for InsertPlane
//--------------------------------------------------------------//
technique InsertPlane
{
   pass Pass_0
   {
      FILLMODE = SOLID;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 InsertPlaneVS();
      PixelShader = compile ps_3_0 ps_main();
   }

}

//--------------------------------------------------------------//
// Technique Section for RPwithPlane
//--------------------------------------------------------------//
technique RPwithPlane
{
   pass Pass_0
   {
      CULLMODE = NONE;
      FILLMODE = POINT;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 RPwithPlaneVS();
      PixelShader = compile ps_3_0 ps_main();
   }

}


//--------------------------------------------------------------//
// Technique Section for Coordinate
//--------------------------------------------------------------//
technique Coordinate
{
   pass Pass_0
   {
      CULLMODE = NONE;
      FILLMODE = SOLID;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 CoordinateVS();
      PixelShader = compile ps_3_0 ps_main();
   }

}


//--------------------------------------------------------------//
// Technique Section for RenderPoint
//--------------------------------------------------------------//
technique RenderPoint
{
   pass Pass_0
   {
      CULLMODE = NONE;
      FILLMODE = POINT;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 WithoutLightVS();
      PixelShader = compile ps_3_0 WithoutLightPS();
   }

}


//--------------------------------------------------------------//
// Technique Section for CalculateNormal
//--------------------------------------------------------------//
technique CalNormal
{
   pass Pass_0
   {
      CULLMODE = NONE;
      FILLMODE = POINT;

      VertexShader = compile vs_3_0 CalNormalVS();
      PixelShader = compile ps_3_0 ps_main();
   }

}

//--------------------------------------------------------------//
// Technique Section for RenderWithLight
//--------------------------------------------------------------//
technique RenderWithLight
{
   pass Pass_0
   {
      CULLMODE = NONE;
      FILLMODE = SOLID;
      ALPHABLENDENABLE = TRUE;
      SRCBLEND = SRCALPHA;
      DESTBLEND = INVSRCALPHA;

      VertexShader = compile vs_3_0 WithLightVS();
      PixelShader = compile ps_3_0 ps_main();
   }

}