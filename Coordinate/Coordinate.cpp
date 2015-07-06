//--------------------------------------------------------------------------------------
// File: Coordinate.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsdlg.h"
#include "DXUTguiIME.h"
#include <string>

//--------------------------------------------------------------------------------------
// Global Variable
//--------------------------------------------------------------------------------------
::LPD3DXMESH								variableMesh		=		NULL;
::LPD3DXMESH								screenMesh			=		NULL;
::LPD3DXMESH								coordinateMesh	=		NULL;
::LPD3DXMESH								insertPlaneMesh	=		NULL;
::LPDIRECT3DTEXTURE9				heightTexture		=		NULL;
::LPDIRECT3DTEXTURE9				normalTexture		=		NULL;
::LPDIRECT3DTEXTURE9				screenTexture		=		NULL;
::IDirect3DSurface9*					pRTheight				=		NULL;
::IDirect3DSurface9*					pRTnormal			=		NULL;
::IDirect3DSurface9*					pScreenBuffer		=		NULL;
::IDirect3DSurface9*					pBackBuffer			=		NULL;
::LPDIRECT3DVERTEXSHADER9	calculateVS			=		NULL;
::LPDIRECT3DPIXELSHADER9		calculatePS			=		NULL;
::LPD3DXCONSTANTTABLE			constOfVS			=		NULL;
::LPD3DXCONSTANTTABLE			constOfPS			=		NULL;
::ID3DXEffect*								pEffect					=		NULL;
::D3DXHANDLE								hCenter				=		NULL;
::D3DXHANDLE								hMainRange			=		NULL;
::D3DXHANDLE								hPlaneFactor		=		NULL;
::D3DXHANDLE								hBrightness			=		NULL;
::D3DXHANDLE								hBrightnessISP	=		NULL;
::D3DXHANDLE								hWordViewProj	=		NULL;
::D3DXHANDLE								hWordProj			=		NULL;
::D3DXHANDLE								hLightPos				=		NULL;
::D3DXHANDLE								hLightColor			=		NULL;
::D3DXHANDLE								hLightAttenu		=		NULL;
::D3DXHANDLE								hPlane_a_Coef	=		NULL;
::D3DXHANDLE								hPlane_b_Coef	=		NULL;
::D3DXHANDLE								hPlane_c_Coef		=		NULL;
::D3DXHANDLE								hPlane_d_Coef	=		NULL;
::D3DXHANDLE								hPlane_alpha		=		NULL;
::D3DXHANDLE								hCoord_alpha		=		NULL;
::D3DXHANDLE								hPoint_alpha		=		NULL;
::D3DXHANDLE								hSurf_alpha			=		NULL;
::D3DXHANDLE								hTechPoint			=		NULL;
::D3DXHANDLE								hTechLight			=		NULL;
::D3DXHANDLE								hTechNormal		=		NULL;
::D3DXHANDLE								hTechCoordinate	=		NULL;
::D3DXHANDLE								hTechRPwithPlane=		NULL;
::D3DXHANDLE								hTechInsertPlane	=		NULL;
std::string									calVSpart1			=		"\
			float			Range;\
			float			MPara;\
			float			a;\
			float			b;\
			float			c;\
			float			d;\
			float2		Center;\
			struct VS_OUTPUT\
			{  \
				float4	Position		:		POSITION	;\
				float	z					:		TEXCOORD		;\
			}	;\
			VS_OUTPUT		 calculateVS		( in float3 vPosition : POSITION )\
			{\
				VS_OUTPUT		Output	;\
				float	x	=	vPosition.x	*	Range/MPara	+	Center.x	;\
				float	y	=	vPosition.y	*	Range/MPara	+	Center.y	;\
				float	z	=	(";
std::string									calVSpart2			=		"a*sin(sqrt(b*x*x+c*y*y+d*y*y))";
std::string									calVSpart3			=		")*MPara/Range;\
				Output.Position =  float4	( vPosition.x/150, vPosition.y/150, 1.0, 1);\
				Output.z	=	z;\
				return Output;\
			} ;";
std::string									calVS					=		calVSpart1 + calVSpart2 + calVSpart3;
std::string									calPS					=		"\
			float4 calculatePS	(	float z	:	TEXCOORD	)	 : COLOR0\
			{   \
				 return		( float4 ( 0.0f, 0.0f, 0.0f, z ) );\
			}";

float													angleOfEye			=		D3DX_PI/4;
float													CalVS_Range		=		2.5;
float													CalVS_MPara		=		49.5;
float													CalVS_Center[2]	=		{ 0,0 };
float													PlaneFactor			=		1;
float													brightness			=		1;
float													brightnessISP		=		0.2;
float													slider_x_base		=		200;
float													slider_y_base		=		200;
float													slider_r_base		=		200;
float													slider_a_base		=		200;
float													slider_b_base		=		200;
float													slider_c_base		=		200;
float													slider_d_base		=		200;
float													slider_Fa_base	=		200;
float													slider_Fb_base	=		200;
float													slider_Fc_base		=		200;
float													slider_Fd_base	=		200;
float													x_offset				=		0;
float													y_offset				=		0;
float													r_offset				=		0;
float													plane_a_offset		=		0;
float													plane_b_offset		=		0;
float													plane_c_offset		=		0;
float													plane_d_offset		=		0;
float													formula_a_offset	=		1;
float													formula_b_offset	=		1;
float													formula_c_offset	=		1;
float													formula_d_offset	=		1;
float													alpha					=		0;
float													pAlpha					=		0;
float													plane_alpha			=		0;
float													cAlpha					=		0;
float													coord_alpha			=		1;
float													sAlpha					=		0;
float													surf_alpha			=		0;
float													poAlpha				=		0;
float													point_alpha			=		1;
float													beta						=		0;
float													processBright		=		0;
float													coef_planeHUD	=		0;
float													coef_formulaHUD	=		0;
bool													NorOption			=		false;
bool													CoordOption		=		true;
bool													InsertOption		=		false;
bool													ModifyOption		=		false;
bool													AutoRotate			=		true;
bool													FullScreen			=		false;
bool													HelpOn					=		false;
::CModelViewerCamera					camera;
::CDXUTDialogResourceManager		dlgManager;
::CDXUTDialog									UI	;
::CDXUTDialog									mainHUD;
::CDXUTDialog									planeHUD;
::CDXUTDialog									formulaHUD;
::CD3DSettingsDlg							settingsDlg;
::D3DXMATRIX									mRotate;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN		1
#define IDC_AUTOROTATE					2
#define IDC_CHANGEDEVICE				3
#define IDC_EDITBOX							4
#define IDC_STATIC_X						5
#define IDC_SLIDER_X						6
#define IDC_STATIC_Y							7
#define IDC_SLIDER_Y						8
#define IDC_STATIC_RANGE				9
#define IDC_SLIDER_RANGE				10
#define IDC_CHECKBOX_SURF			11
#define IDC_CHECKBOX_COORD			12
#define IDC_CHECKBOX_INSERT			13
#define IDC_CHECKBOX_MODIFY		14
#define IDC_RADIOA							15
#define IDC_RADIOB							16
#define IDC_RADIOC							17
#define IDC_COMBOBOX						18
#define IDC_STATIC_HELP					19
#define IDC_STATIC_A_COEF				20
#define IDC_EDITBOX_A						21
#define IDC_SLIDER_A_COEF				22
#define IDC_STATIC_B_COEF				23
#define IDC_EDITBOX_B						24
#define IDC_SLIDER_B_COEF				25
#define IDC_STATIC_C_COEF				26
#define IDC_EDITBOX_C 					27
#define IDC_SLIDER_C_COEF				28
#define IDC_STATIC_BRIGHT				29
#define IDC_SLIDER_BRIGHT				30
#define IDC_STATIC_PLANE				31
#define IDC_STATIC_FA_COEF			32
#define IDC_EDITBOX_FA_COEF			33
#define IDC_SLIDER_FA_COEF			34
#define IDC_STATIC_FB_COEF			35
#define IDC_EDITBOX_FB_COEF			36
#define IDC_SLIDER_FB_COEF			37
#define IDC_STATIC_FC_COEF			38
#define IDC_EDITBOX_FC_COEF 		39
#define IDC_SLIDER_FC_COEF			40
#define IDC_STATIC_FD_COEF			41
#define IDC_EDITBOX_FD_COEF 		42
#define IDC_SLIDER_FD_COEF			43


//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool			CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed,void* pUserContext );
bool			CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT		CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,void* pUserContext );
HRESULT		CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,void* pUserContext );
void				CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void				CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT		CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,void* pUserContext );
void				CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void				CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void				CALLBACK OnLostDevice( void* pUserContext );
void				CALLBACK OnDestroyDevice( void* pUserContext );

void				InitApp();
void				DrawheightTexture( bool NorOption );
void				DrawnormalTexture( );
std::string	WChar2Ansi(LPCWSTR pwszSrc);



//--------------------------------------------------------------------------------------
// Initial the Application
//--------------------------------------------------------------------------------------
void InitApp()
{
	settingsDlg.Init( &dlgManager );
	
	mainHUD.Init( &dlgManager );int iY=58;
	mainHUD.SetCallback( OnGUIEvent );
	mainHUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Full Screen", 35, 10, 125, 22 );
    mainHUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, 34, 125, 22, VK_F2 );
	mainHUD.AddButton( IDC_AUTOROTATE, L"AutoRotate", 35, 58, 125, 22 );
	mainHUD.AddCheckBox( IDC_CHECKBOX_SURF, L"Render Surface",35, iY+=50,125, 22, false, L'C', false );
	mainHUD.AddCheckBox( IDC_CHECKBOX_COORD, L"Render Coordinate",35, iY+=25,125, 22,true, L'C', false);
	mainHUD.AddCheckBox( IDC_CHECKBOX_INSERT, L"Insert Plane",35, iY+=25,125, 22,false, L'C', false);
	mainHUD.AddCheckBox( IDC_CHECKBOX_MODIFY, L"Modify Formula",35, iY+=25,125, 22,false, L'C', false);
	mainHUD.AddStatic( IDC_STATIC_X,L"X Offset : 0",35, iY+=28,125,22);
	mainHUD.AddSlider(IDC_SLIDER_X,35,iY+=24,125,22,-1000,1000,0);
	mainHUD.AddStatic( IDC_STATIC_Y,L"Y Offset : 0",35,iY+=24,125,22);
	mainHUD.AddSlider(IDC_SLIDER_Y,35,iY+=24,125,22,-1000,1000,0);
	mainHUD.AddStatic( IDC_STATIC_RANGE,L"Range : 2.5",35,iY+=24,125,22);
	mainHUD.AddSlider(IDC_SLIDER_RANGE,35,iY+=24,125,22,-1000,1000,500);
	mainHUD.AddStatic( IDC_STATIC_BRIGHT,L"Brightness : 1",35,iY+=24,125,22);
	mainHUD.AddSlider(IDC_SLIDER_BRIGHT,35,iY+=24,125,22,-1000,1000,1000);
	mainHUD.GetControl( IDC_STATIC_X )->GetElement( 0 )->dwTextFormat = DT_CENTER;
	mainHUD.GetControl( IDC_STATIC_Y )->GetElement( 0 )->dwTextFormat = DT_CENTER;
	mainHUD.GetControl( IDC_STATIC_RANGE )->GetElement( 0 )->dwTextFormat = DT_CENTER;
	mainHUD.AddRadioButton( IDC_RADIOA, 1, L"S  Sample : 4900", 35, iY+=24, 125, 24, false, L'1' );
	mainHUD.AddRadioButton( IDC_RADIOB, 1, L"M Sample : 10000", 35, iY+=24, 125, 24,true, L'2' );
	mainHUD.AddRadioButton( IDC_RADIOC, 1, L"L   Sample : 40000", 35, iY+=24, 125, 24, false, L'3' );
	mainHUD.SetFont( 1,L"Calibri",17,FW_NORMAL );
	for(int i=5;i<=17;i++)
		mainHUD.GetControl(i)->GetElement( 0 )->iFont = 1;
	mainHUD.GetControl(IDC_STATIC_BRIGHT)->GetElement( 0 )->iFont = 1;
	mainHUD.GetControl(IDC_SLIDER_BRIGHT)->GetElement( 0 )->iFont = 1;
	mainHUD.GetButton(IDC_TOGGLEFULLSCREEN)->GetElement( 0 )->iFont = 1;
	mainHUD.GetButton(IDC_TOGGLEFULLSCREEN)->GetElement( 1 )->iFont = 1;
	mainHUD.GetButton(IDC_CHANGEDEVICE)->GetElement( 0 )->iFont = 1;
	mainHUD.GetButton(IDC_CHANGEDEVICE)->GetElement( 1 )->iFont = 1;
	mainHUD.GetButton(IDC_AUTOROTATE)->GetElement( 0 )->iFont = 1;
	mainHUD.GetButton(IDC_AUTOROTATE)->GetElement( 1 )->iFont = 1;

	planeHUD.Init( &dlgManager );
	planeHUD.SetCallback( OnGUIEvent );int iX=10;
	planeHUD.AddStatic( IDC_STATIC_A_COEF,L"Parameter A",iX, 3,70,30);
	planeHUD.AddEditBox(IDC_EDITBOX_A,L"0.0",iX+80,5,70,22);
	planeHUD.AddSlider(IDC_SLIDER_A_COEF,iX,30,150,22,-1000,1000,0);
	planeHUD.AddStatic( IDC_STATIC_B_COEF,L"Parameter B",iX+=160,3,70,30);
	planeHUD.AddEditBox(IDC_EDITBOX_B,L"0.0",iX+80,5,70,22);
	planeHUD.AddSlider(IDC_SLIDER_B_COEF,iX,30,150,22,-1000,1000,0);
	planeHUD.AddStatic( IDC_STATIC_C_COEF,L"Parameter C",iX+=160,3,70,30);
	planeHUD.AddEditBox(IDC_EDITBOX_C,L"0.0",iX+80,5,70,22);
	planeHUD.AddSlider(IDC_SLIDER_C_COEF,iX,30,150,22,-1000,1000,0);
	planeHUD.AddStatic( IDC_STATIC_PLANE,L"The Plane's Formula is: z = A*x + B*y + C",100,60,320,30);
	planeHUD.SetFont( 2,L"Comic Sans MS",20,FW_BOLD );
	planeHUD.GetControl(IDC_STATIC_PLANE)->GetElement( 0 )->iFont = 2;
	planeHUD.GetEditBox(IDC_EDITBOX_A)->SetBorderWidth(0);
	planeHUD.GetEditBox(IDC_EDITBOX_B)->SetBorderWidth(0);
	planeHUD.GetEditBox(IDC_EDITBOX_C)->SetBorderWidth(0);
	planeHUD.SetFont( 1,L"Calibri",17,FW_NORMAL );
	for(int i=20;i<=28;i++)
		planeHUD.GetControl(i)->GetElement( 0 )->iFont = 1;

	formulaHUD.Init( &dlgManager );
	formulaHUD.SetCallback( OnGUIEvent );iY=10;
	formulaHUD.AddStatic( IDC_STATIC_FA_COEF,L"Parameter A",5,iY+2,80,20);
	formulaHUD.AddEditBox(IDC_EDITBOX_FA_COEF,L"1.0",100,iY,70,22);
	formulaHUD.AddSlider(IDC_SLIDER_FA_COEF,5,iY+=22,170,22,-1000,1000,0);
	formulaHUD.AddStatic( IDC_STATIC_FB_COEF,L"Parameter B",5,iY+=38,80,20);
	formulaHUD.AddEditBox(IDC_EDITBOX_FB_COEF,L"1.0",100,iY-=3,70,22);
	formulaHUD.AddSlider(IDC_SLIDER_FB_COEF,5,iY+=22,170,22,-1000,1000,0);
	formulaHUD.AddStatic( IDC_STATIC_FC_COEF,L"Parameter C",5,iY+=38,80,20);
	formulaHUD.AddEditBox(IDC_EDITBOX_FC_COEF,L"1.0",100,iY-=3,70,22);
	formulaHUD.AddSlider(IDC_SLIDER_FC_COEF,5,iY+=22,170,22,-1000,1000,0);
	formulaHUD.AddStatic( IDC_STATIC_FD_COEF,L"Parameter D",5,iY+=38,80,20);
	formulaHUD.AddEditBox(IDC_EDITBOX_FD_COEF,L"1.0",100,iY-=3,70,22);
	formulaHUD.AddSlider(IDC_SLIDER_FD_COEF,5,iY+=22,170,22,-1000,1000,0);
	formulaHUD.GetEditBox(IDC_EDITBOX_FA_COEF)->SetBorderWidth(0);
	formulaHUD.GetEditBox(IDC_EDITBOX_FB_COEF)->SetBorderWidth(0);
	formulaHUD.GetEditBox(IDC_EDITBOX_FC_COEF)->SetBorderWidth(0);
	formulaHUD.GetEditBox(IDC_EDITBOX_FD_COEF)->SetBorderWidth(0);
	formulaHUD.SetFont( 1,L"Calibri",17,FW_NORMAL );
	for(int i=32;i<=43;i++)
		formulaHUD.GetControl(i)->GetElement( 0 )->iFont = 1;

	UI.Init( &dlgManager );
	UI.SetCallback( OnGUIEvent );
	UI.SetFont( 1,L"Tekton Pro",28,FW_NORMAL );
	UI.SetFont( 2,L"Tekton Pro",30,FW_BOLD );
	UI.AddStatic(IDC_STATIC_HELP,L"",0,0,600,800);
	UI.GetStatic( IDC_STATIC_HELP )->SetTextColor( D3DCOLOR_ARGB( 255, 255, 0, 0 ) );
	UI.GetControl(IDC_STATIC_HELP)->GetElement( 0 )->iFont = 2;
	CDXUTIMEEditBox* pIMEEdit;
    CDXUTIMEEditBox::InitDefaultElements( &UI );
    if( SUCCEEDED( CDXUTIMEEditBox::CreateIMEEditBox( &UI, IDC_EDITBOX,
                                                      L"a*sin(sqrt(b*x*x+c*y*y+d*y*y))", 0, 0, 600, 40, false, &pIMEEdit ) ) )
    {
        UI.AddControl( pIMEEdit );
        pIMEEdit->GetElement( 0 )->iFont = 1;
        pIMEEdit->GetElement( 1 )->iFont = 1;
        pIMEEdit->GetElement( 9 )->iFont = 1;
        pIMEEdit->GetElement( 0 )->TextureColor.Init( D3DCOLOR_ARGB( 100, 255, 255, 255 ) );  // Transparent center
        pIMEEdit->SetBorderWidth( 2 );
        pIMEEdit->SetTextColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
        pIMEEdit->SetCaretColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) );
        pIMEEdit->SetSelectedTextColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
        pIMEEdit->SetSelectedBackColor( D3DCOLOR_ARGB( 255, 40, 72, 72 ) );
    }
	CDXUTIMEEditBox::SetImeEnableFlag( true );
}
//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                      bool bWindowed, void* pUserContext )
{
    // Typically want to skip back buffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // No fallback, so need ps3.0
    if( pCaps->PixelShaderVersion < D3DPS_VERSION( 3, 0 ) )
        return false;

	 // No fallback, so need vs3.0
    if( pCaps->VertexShaderVersion < D3DVS_VERSION( 3, 0 ) )
        return false;

    // No fallback, so need to support render target
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_RENDERTARGET,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
    {
        return false;
    }

    // No fallback, so need to support D3DFMT_G16R16F or D3DFMT_A16B16G16R16F render target
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_RENDERTARGET,
                                         D3DRTYPE_TEXTURE, D3DFMT_G16R16F ) ) )
    {
        if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                             AdapterFormat, D3DUSAGE_RENDERTARGET,
                                             D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F ) ) )
        {
            return false;
        }
    }

	
    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	//pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	//pDeviceSettings->d3d9.pp.SwapEffect = D3DSWAPEFFECT_COPY;
	//pDeviceSettings->d3d9.pp.MultiSampleType=D3DMULTISAMPLE_4_SAMPLES;
	//pDeviceSettings->d3d9.pp.MultiSampleQuality=0;
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
	HRESULT hr;

	 
	/*Load the default xy_plane mesh*/
	if( FAILED( D3DXLoadMeshFromX( L"Data\\Plane.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &variableMesh ) ) )
    {
        MessageBox(NULL, L"Could not find Plane.x", L"File error", MB_OK);
        return E_FAIL;
    }
	/*Load the screen mesh*/
	if( FAILED( D3DXLoadMeshFromX( L"Data\\Screen.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &screenMesh ) ) )
    {
        MessageBox(NULL, L"Could not find Screen.x", L"File error", MB_OK);
        return E_FAIL;
    }
	/*Load the coordinate mesh*/
	if( FAILED( D3DXLoadMeshFromX( L"Data\\Coordinate.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &coordinateMesh ) ) )
    {
        MessageBox(NULL, L"Could not find Coordinate.x", L"File error", MB_OK);
        return E_FAIL;
    }
	/*Load the insertPlane mesh*/
	if( FAILED( D3DXLoadMeshFromX( L"Data\\InsertPlane.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &insertPlaneMesh ) ) )
    {
        MessageBox(NULL, L"Could not find InsertPlane.x", L"File error", MB_OK);
        return E_FAIL;
    }
	/*Load the effect file*/
	if( FAILED( D3DXCreateEffectFromFile(pd3dDevice,L"Data\\mainEffect.fx",NULL,NULL,
		D3DXSHADER_DEBUG,NULL,&pEffect,NULL) ) )
    {
        MessageBox(NULL, L"Could not find mainEffect.fx", L"File error", MB_OK);
        return E_FAIL;
    }

	CDXUTIMEEditBox::Initialize( DXUTGetHWND() );

    V_RETURN( dlgManager.OnD3D9CreateDevice( pd3dDevice ) );
    V_RETURN( settingsDlg.OnD3D9CreateDevice( pd3dDevice ) );

	/*Compile the calculate VS*/
	::LPD3DXBUFFER pCode_VS;
	DWORD dwShaderFlags = D3DXSHADER_SKIPOPTIMIZATION;

	V_RETURN( D3DXCompileShader(calVS.c_str(), calVS.size(),NULL, NULL, "calculateVS",
		                                 "vs_3_0", dwShaderFlags, &pCode_VS,
										 NULL, &constOfVS ) );
	V_RETURN(pd3dDevice->CreateVertexShader( (DWORD*)pCode_VS->GetBufferPointer(), &calculateVS));
	pCode_VS->Release();
	
	/*Compile the calculate PS*/
	::LPD3DXBUFFER pCode_PS;
	V_RETURN( D3DXCompileShader(calPS.c_str(), calPS.size(),NULL, NULL, "calculatePS",
		                                 "ps_3_0", dwShaderFlags, &pCode_PS,
										 NULL, &constOfPS ) );
	V_RETURN(pd3dDevice->CreatePixelShader((DWORD*)pCode_PS->GetBufferPointer(), &calculatePS));
	pCode_PS->Release();
	
	/*Bound the variable to the handles*/
	hTechNormal		=		pEffect->GetTechniqueByName("CalNormal");
	hTechPoint			=		pEffect->GetTechniqueByName("RenderPoint");
	hTechLight			=		pEffect->GetTechniqueByName("RenderWithLight");
	hTechCoordinate	=		pEffect->GetTechniqueByName("Coordinate");
	hTechRPwithPlane=		pEffect->GetTechniqueByName("RPwithPlane");
	hTechInsertPlane	=		pEffect->GetTechniqueByName("InsertPlane");
	hWordViewProj	=		pEffect->GetParameterByName( NULL,"matViewProjection" );
	hWordProj			=		pEffect->GetParameterByName( NULL,"matWorld" );
	hPlaneFactor		=		pEffect->GetParameterByName( NULL,"PlaneFactor" );
	hMainRange			=		pEffect->GetParameterByName( NULL,"Range" );
	hBrightness			=		pEffect->GetParameterByName( NULL,"brightness" );
	hBrightnessISP	=		pEffect->GetParameterByName( NULL,"brightnessISP" );
	hLightPos				=		pEffect->GetParameterByName( NULL,"Light_Position" );
	hLightColor			=		pEffect->GetParameterByName( NULL,"Light_Color" );
	hLightAttenu		=		pEffect->GetParameterByName( NULL,"Light_Attenuation" );
	hPlane_a_Coef	=		pEffect->GetParameterByName( NULL,"Plane_Coef_a" );
	hPlane_b_Coef	=		pEffect->GetParameterByName( NULL,"Plane_Coef_b" );
	hPlane_c_Coef		=		pEffect->GetParameterByName( NULL,"Plane_Coef_c" );
	hPlane_d_Coef	=		pEffect->GetParameterByName( NULL,"Plane_Coef_d" );
	hPlane_alpha		=		pEffect->GetParameterByName( NULL,"alpha" );
	hCoord_alpha		=		pEffect->GetParameterByName( NULL,"alpha_c" );
	hPoint_alpha		=		pEffect->GetParameterByName( NULL,"alpha_p" );
	hSurf_alpha			=		pEffect->GetParameterByName( NULL,"alpha_s" );


	/*Initial the camera*/
	::D3DXVECTOR3		vEyePosition(120.0f,-120.0f,90.0f);
	::D3DXVECTOR3		vLookAt(0.0,0.0,0.0);
	camera.SetViewParams(&vEyePosition,&vLookAt);
	D3DXMatrixIdentity(&mRotate);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext )
{
	HRESULT hr;

	/*Reset the dialogManagers*/
	V_RETURN( dlgManager.OnD3D9ResetDevice() );
    V_RETURN( settingsDlg.OnD3D9ResetDevice() );

	/*Set and reset the dialogs*/
	mainHUD.SetLocation(pBackBufferSurfaceDesc->Width-170,0);
	mainHUD.SetSize(170,600);
	planeHUD.SetLocation(pBackBufferSurfaceDesc->Width/2-245,0);
	planeHUD.SetSize(490,60);
	formulaHUD.SetLocation(0,pBackBufferSurfaceDesc->Height/2-120);
	formulaHUD.SetSize(170,100);
	UI.SetLocation(0,0);
	UI.SetSize(pBackBufferSurfaceDesc->Width,pBackBufferSurfaceDesc->Height);
	UI.GetControl(IDC_EDITBOX)->SetLocation(pBackBufferSurfaceDesc->Width/2-300,pBackBufferSurfaceDesc->Height-60);
	UI.GetControl(IDC_STATIC_HELP)->SetLocation(pBackBufferSurfaceDesc->Width/2-300,pBackBufferSurfaceDesc->Height/2-600);
	/*Initial the heightTexture*/
	V_RETURN(D3DXCreateTexture(pd3dDevice,300,300,
								1,D3DUSAGE_RENDERTARGET,::D3DFMT_A16B16G16R16F,
								D3DPOOL_DEFAULT,&heightTexture));

	/*Initial the normalTexture*/
	V_RETURN(D3DXCreateTexture(pd3dDevice,300,300,
								1,D3DUSAGE_RENDERTARGET,::D3DFMT_A16B16G16R16F,
								D3DPOOL_DEFAULT,&normalTexture));

	/*Initial the screenTexture*/
	V_RETURN(D3DXCreateTexture(pd3dDevice,pBackBufferSurfaceDesc->Width,pBackBufferSurfaceDesc->Height,
		1,D3DUSAGE_RENDERTARGET,::D3DFMT_A8B8G8R8,
								D3DPOOL_DEFAULT,&screenTexture));

	/*Get the BackBuffer*/
	V_RETURN(pd3dDevice->GetRenderTarget(0,&pBackBuffer));
	
	/*Bound the texture to the surface*/
	V_RETURN(normalTexture->GetSurfaceLevel(0,&pRTnormal));
	V_RETURN(heightTexture->GetSurfaceLevel(0,&pRTheight));
	V_RETURN(screenTexture->GetSurfaceLevel(0,&pScreenBuffer));

	
	/*Calculate the heightTexture*/
	constOfVS->SetFloat( pd3dDevice,"Range",CalVS_Range );
	constOfVS->SetFloat( pd3dDevice,"MPara",CalVS_MPara );
	constOfVS->SetFloatArray( pd3dDevice,"Center",CalVS_Center,2 );
	constOfVS->SetFloat( pd3dDevice,"a",formula_a_offset );
	constOfVS->SetFloat( pd3dDevice,"b",formula_b_offset );
	constOfVS->SetFloat( pd3dDevice,"c",formula_c_offset );
	constOfVS->SetFloat( pd3dDevice,"d",formula_d_offset );
	DrawheightTexture( false);
	
	/*Bound the heightTexture to the mainEffect*/
	V_RETURN(pEffect->SetTexture("heightTexture",heightTexture));
	DrawnormalTexture();
	V_RETURN(pEffect->SetTexture("normalTexture",normalTexture));
	pEffect->SetFloat(hPlaneFactor,PlaneFactor);
	pEffect->SetFloat(hMainRange,CalVS_Range);
	pEffect->SetFloat(hPlane_alpha,plane_alpha);
	pEffect->SetFloat(hPoint_alpha,point_alpha);

	float	fAspectRatio	=	pBackBufferSurfaceDesc->Width	/	(float)pBackBufferSurfaceDesc->Height;
	camera.SetProjParams(angleOfEye,fAspectRatio,0.1f,5000.0f);
	camera.SetWindow(pBackBufferSurfaceDesc->Width,pBackBufferSurfaceDesc->Height);
	camera.SetButtonMasks( MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON );
	if(pEffect)
		V_RETURN(pEffect->OnResetDevice());
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	camera.FrameMove(fElapsedTime);
	if(AutoRotate){
		alpha+=fElapsedTime*0.5;
		if(alpha>=D3DX_PI*2)
			alpha=0;
		D3DXMatrixRotationZ(&mRotate,alpha);
	}
	if(InsertOption){
		pAlpha=(pAlpha>=D3DX_PI/2)?D3DX_PI/2:pAlpha+3*fElapsedTime;
		plane_alpha=0.8*sin(pAlpha);
		pEffect->SetFloat(hPlane_alpha,plane_alpha);
		coef_planeHUD=(coef_planeHUD>=D3DX_PI/2)?D3DX_PI/2:coef_planeHUD+3*fElapsedTime;
		planeHUD.m_y=90*sin(coef_planeHUD)-90;
		if(beta<=100){
			beta+=fElapsedTime*200;
			processBright=brightness+beta*(brightnessISP-brightness)/100.0f;
			pEffect->SetFloat(hBrightnessISP,processBright);
		}
	}else{
		pAlpha=(pAlpha<=0)?0:pAlpha-3*fElapsedTime;
		plane_alpha=0.8*sin(pAlpha);
		pEffect->SetFloat(hPlane_alpha,plane_alpha);
		coef_planeHUD=(coef_planeHUD<=0)?0:coef_planeHUD-3*fElapsedTime;
		planeHUD.m_y=90*sin(coef_planeHUD)-90;
		if(beta>=0){
			beta-=fElapsedTime*200;
			processBright=brightness+beta*(brightnessISP-brightness)/100.0f;
			pEffect->SetFloat(hBrightnessISP,processBright);
		}
	}

	if(CoordOption){
		cAlpha=(cAlpha>=D3DX_PI/2)?D3DX_PI/2:cAlpha+3*fElapsedTime;
		coord_alpha=0.8*sin(cAlpha);
		pEffect->SetFloat(hCoord_alpha,coord_alpha);
	}else{
		cAlpha=(cAlpha<=0)?0:cAlpha-3*fElapsedTime;
		coord_alpha=0.8*sin(cAlpha);
		pEffect->SetFloat(hCoord_alpha,coord_alpha);
	}

	if(NorOption){
		sAlpha=(sAlpha>=D3DX_PI/2)?D3DX_PI/2:sAlpha+3*fElapsedTime;
		surf_alpha=0.8*sin(sAlpha);
		poAlpha=(poAlpha>=D3DX_PI/2)?D3DX_PI/2:poAlpha+3*fElapsedTime;
		point_alpha=0.8*cos(poAlpha);
		pEffect->SetFloat(hSurf_alpha,surf_alpha);
		pEffect->SetFloat(hPoint_alpha,point_alpha);
	}else{
		sAlpha=(sAlpha<=0)?0:sAlpha-3*fElapsedTime;
		surf_alpha=0.8*sin(sAlpha);
		poAlpha=(poAlpha<=0)?0:poAlpha-3*fElapsedTime;
		point_alpha=0.8*cos(poAlpha);
		pEffect->SetFloat(hSurf_alpha,surf_alpha);
		pEffect->SetFloat(hPoint_alpha,point_alpha);
	}

	if(ModifyOption){
		coef_formulaHUD=(coef_formulaHUD>=D3DX_PI/2)?D3DX_PI/2:coef_formulaHUD+3*fElapsedTime;
		formulaHUD.m_x=200*sin(coef_formulaHUD)-200;
	}else{
		coef_formulaHUD=(coef_formulaHUD<=0)?0:coef_formulaHUD-3*fElapsedTime;
		formulaHUD.m_x=200*sin(coef_formulaHUD)-200;
	}
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	 if( settingsDlg.IsActive() )
    {
        settingsDlg.OnRender( fElapsedTime );
        return;
    }

    HRESULT hr;
	::D3DXMATRIX		mWorld;
	::D3DXMATRIX		mView;
	::D3DXMATRIX		mProj;
	::D3DXMATRIX		mWorldViewProj;
	::D3DXVECTOR3	mViewPos;
	UINT					iPass,cPasses;
	
	mWorld					=	*camera.GetWorldMatrix();
	mView						=	*camera.GetViewMatrix();
	mProj						=	*camera.GetProjMatrix();
	mViewPos				=	*camera.GetEyePt();
	FLOAT					fViewPos[]={mViewPos.x,mViewPos.y,mViewPos.z};
	mWorldViewProj	=	mRotate*mWorld* mView  * mProj;

	V(pEffect->SetMatrix(hWordViewProj,&mWorldViewProj));
	V(pEffect->SetMatrix(hWordProj,&mWorld));
	V(pEffect->SetFloatArray(hLightPos,fViewPos,3));
	V(pd3dDevice->SetRenderTarget(0,pBackBuffer));

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		
		if(plane_alpha!=0){
			V(pEffect->SetTechnique(hTechRPwithPlane));
			V(pEffect->Begin(&cPasses,0));
			for( iPass = 0 ; iPass < cPasses ; iPass++)
			{
				V(pEffect->BeginPass(iPass));
				V(variableMesh->DrawSubset(0));
				V(pEffect->EndPass());
			}
			V(pEffect->End());

			V(pEffect->SetTechnique(hTechInsertPlane));
			V(pEffect->Begin(&cPasses,0));
			for( iPass = 0 ; iPass < cPasses ; iPass++)
			{
				V(pEffect->BeginPass(iPass));
				V(insertPlaneMesh->DrawSubset(0));
				V(pEffect->EndPass());
			}
			V(pEffect->End());
		}else{
			if(surf_alpha!=0){
				V(pEffect->SetTechnique(hTechLight));
				V(pEffect->Begin(&cPasses,0));
				for( iPass = 0 ; iPass < cPasses ; iPass++)
				{
					V(pEffect->BeginPass(iPass));
					V(variableMesh->DrawSubset(0));
					V(pEffect->EndPass());
				}
				V(pEffect->End());
			}
			if(point_alpha!=0){
				V(pEffect->SetTechnique(hTechPoint));
				V(pEffect->Begin(&cPasses,0));
				for( iPass = 0 ; iPass < cPasses ; iPass++)
				{
					V(pEffect->BeginPass(iPass));
					V(variableMesh->DrawSubset(0));
					V(pEffect->EndPass());
				}
				V(pEffect->End());
			}
		}
		if(coord_alpha!=0){
			V(pEffect->SetTechnique(hTechCoordinate));
			V(pEffect->Begin(&cPasses,0));
			for( iPass = 0 ; iPass < cPasses ; iPass++)
			{
				V(pEffect->BeginPass(iPass));
				V(coordinateMesh->DrawSubset(0));
				V(pEffect->EndPass());
			}
			V(pEffect->End());
		}	
		

		V( mainHUD.OnRender( fElapsedTime ) );
        V( planeHUD.OnRender( fElapsedTime ) );
		V( formulaHUD.OnRender( fElapsedTime ) );
		V( UI.OnRender( fElapsedTime ) );

        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	*pbNoFurtherProcessing = CDXUTIMEEditBox::StaticMsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = dlgManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    if( settingsDlg.IsActive() )
    {
        settingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = mainHUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

	*pbNoFurtherProcessing = planeHUD.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	*pbNoFurtherProcessing = formulaHUD.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	*pbNoFurtherProcessing = UI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;


	 camera.HandleMessages( hWnd, uMsg, wParam, lParam );
    return 0;
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
	if(pEffect)
		pEffect->OnLostDevice();
	dlgManager.OnD3D9LostDevice();
    settingsDlg.OnD3D9LostDevice();
	SAFE_RELEASE(pRTnormal);
	SAFE_RELEASE(pRTheight);
	SAFE_RELEASE(pScreenBuffer);
	SAFE_RELEASE(pBackBuffer);
	SAFE_RELEASE(heightTexture);
	SAFE_RELEASE(normalTexture);
	SAFE_RELEASE(screenTexture);
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
	SAFE_RELEASE(pEffect);
	SAFE_RELEASE(pRTnormal);
	SAFE_RELEASE(pRTheight);
	SAFE_RELEASE(pScreenBuffer);
	SAFE_RELEASE(pBackBuffer);
	SAFE_RELEASE(heightTexture);
	SAFE_RELEASE(normalTexture);
	SAFE_RELEASE(screenTexture);
	SAFE_RELEASE(variableMesh);
	SAFE_RELEASE(screenMesh);
	SAFE_RELEASE(coordinateMesh);
	SAFE_RELEASE(insertPlaneMesh);
	SAFE_RELEASE(calculateVS);
	SAFE_RELEASE(calculatePS);
	SAFE_RELEASE(constOfVS);
	SAFE_RELEASE(constOfPS);
	CDXUTIMEEditBox::Uninitialize( );

    dlgManager.OnD3D9DestroyDevice();
    settingsDlg.OnD3D9DestroyDevice();
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	WCHAR wszOutput[1024];
	int sliderValue=0;
	float newValue = 0;
	float newOffset = 0;
	IDirect3DDevice9* pd3dDevice = DXUTGetD3D9Device();
	 switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); 
			if(FullScreen){
				mainHUD.GetButton(IDC_TOGGLEFULLSCREEN)->SetText(L"FullScreen");
			}else{
				mainHUD.GetButton(IDC_TOGGLEFULLSCREEN)->SetText(L"Window");
			}
			FullScreen=!FullScreen;
			break;


        case IDC_AUTOROTATE:
			AutoRotate=!AutoRotate; break;


        case IDC_CHANGEDEVICE:
            settingsDlg.SetActive( !settingsDlg.IsActive() ); break;


		case IDC_SLIDER_X:
			sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_x_base;
			newOffset = newValue+ x_offset;
			if(!mainHUD.GetSlider(IDC_SLIDER_X)->m_bPressed)
			{
				mainHUD.GetSlider(IDC_SLIDER_X)->SetValue(0);
				x_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_x_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_x_base*=2;
					if(slider_x_base>=102400)
						slider_x_base=102400;
				}
			}
			swprintf_s( wszOutput, 1024, L"X Offset :  %6.3f",newOffset );		
			mainHUD.GetStatic( IDC_STATIC_X )->SetText( wszOutput );	
			
			/*Calculate the heightTexture*/
			CalVS_Center[0]=newOffset;
			constOfVS->SetFloatArray( pd3dDevice,"Center",CalVS_Center,2 );
			DrawheightTexture( NorOption);
            break;


		case IDC_SLIDER_Y:
           sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_y_base;
			newOffset = newValue+ y_offset;
			if(!mainHUD.GetSlider(IDC_SLIDER_Y)->m_bPressed)
			{
				mainHUD.GetSlider(IDC_SLIDER_Y)->SetValue(0);
				y_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_y_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_y_base*=2;
					if(slider_y_base>=102400)
						slider_y_base=102400;
				}
			}
			swprintf_s( wszOutput, 1024, L"Y Offset :  %6.3f",newOffset );		
			mainHUD.GetStatic( IDC_STATIC_Y )->SetText( wszOutput );	

            /*Calculate the heightTexture*/
			CalVS_Center[1]=newOffset;
			constOfVS->SetFloatArray( pd3dDevice,"Center",CalVS_Center,2 );
			DrawheightTexture( NorOption);
            break;


		case IDC_SLIDER_RANGE:
           sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_r_base;
			newOffset = newValue+ r_offset;
			if(newOffset<0.001)
				newOffset=0.001;
			if(!mainHUD.GetSlider(IDC_SLIDER_RANGE)->m_bPressed)
			{
				mainHUD.GetSlider(IDC_SLIDER_RANGE)->SetValue(0);
				r_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_r_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_r_base*=2;
					if(slider_r_base>=102400)
						slider_r_base=102400;
				}
			}
			swprintf_s( wszOutput, 1024, L"Range :  %6.3f",newOffset );		
			mainHUD.GetStatic( IDC_STATIC_RANGE )->SetText( wszOutput );	

            /*Calculate the heightTexture*/
			CalVS_Range=newOffset;
			constOfVS->SetFloat( pd3dDevice,"Range",CalVS_Range );
			pEffect->SetFloat(hMainRange,CalVS_Range);
			DrawheightTexture( NorOption);
            break;



		case IDC_SLIDER_BRIGHT:
            sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) (sliderValue +1000))/ 2000;
			newOffset = newValue;
			swprintf_s( wszOutput, 1024, L"Brightness :  %6.3f",newOffset );		
			mainHUD.GetStatic( IDC_STATIC_BRIGHT )->SetText( wszOutput );	
			if(!InsertOption){
				brightness=newOffset;
				pEffect->SetFloat(hBrightness,brightness);
			}else{
				brightnessISP=newOffset;
				pEffect->SetFloat(hBrightnessISP,brightnessISP);
			}

            break;


		case IDC_CHECKBOX_SURF:
            NorOption=( ( CDXUTCheckBox* )pControl )->GetChecked();
			if( NorOption )
				DrawnormalTexture();
            break;


		case IDC_CHECKBOX_COORD:
            CoordOption=( ( CDXUTCheckBox* )pControl )->GetChecked();
            break;


		case IDC_CHECKBOX_MODIFY:
            ModifyOption=( ( CDXUTCheckBox* )pControl )->GetChecked();
            break;


		case IDC_CHECKBOX_INSERT:
            InsertOption=( ( CDXUTCheckBox* )pControl )->GetChecked();
			if(InsertOption){
				mainHUD.GetCheckBox(IDC_CHECKBOX_SURF)->SetChecked(false);
				mainHUD.GetCheckBox(IDC_CHECKBOX_SURF)->SetEnabled(false);
				mainHUD.GetSlider(IDC_SLIDER_BRIGHT)->SetValue((int)(brightnessISP*2000-1000));
				swprintf_s( wszOutput, 1024, L"Brightness :  %6.3f",brightnessISP);
				mainHUD.GetStatic(IDC_STATIC_BRIGHT)->SetText(wszOutput);
			}else{
				mainHUD.GetCheckBox(IDC_CHECKBOX_SURF)->SetChecked(NorOption);
				mainHUD.GetCheckBox(IDC_CHECKBOX_SURF)->SetEnabled(true);
				mainHUD.GetSlider(IDC_SLIDER_BRIGHT)->SetValue((int)(brightness*2000-1000));
				swprintf_s( wszOutput, 1024, L"Brightness :  %6.3f",brightness);
				mainHUD.GetStatic(IDC_STATIC_BRIGHT)->SetText(wszOutput);
			}	
			break;


		case IDC_RADIOA:
        case IDC_RADIOB:
        case IDC_RADIOC:
			if( WChar2Ansi( ( ( CDXUTRadioButton* )pControl )->GetText())== "S  Sample : 4900" ){
				/*Load the default xy_plane mesh*/
				SAFE_RELEASE(variableMesh);
				if( FAILED( D3DXLoadMeshFromX( L"Data\\PlaneS.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &variableMesh ) ) )
				{
					MessageBox(NULL, L"Could not find PlaneS.x", L"File error", MB_OK);
					break;
				}
				CalVS_MPara=34.5;
				PlaneFactor=1.4348;
				pEffect->SetFloat(hPlaneFactor,PlaneFactor);
				constOfVS->SetFloat( pd3dDevice,"MPara",CalVS_MPara );
				DrawheightTexture(NorOption);
			}
			
			if( WChar2Ansi( ( ( CDXUTRadioButton* )pControl )->GetText())=="L   Sample : 40000"  ){
				/*Load the default xy_plane mesh*/
				SAFE_RELEASE(variableMesh);
				if( FAILED( D3DXLoadMeshFromX( L"Data\\PlaneL.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &variableMesh ) ) )
				{
					MessageBox(NULL, L"Could not find PlaneL.x", L"File error", MB_OK);
					break;
				}
				CalVS_MPara=99.5;
				PlaneFactor=0.4975;
				pEffect->SetFloat(hPlaneFactor,PlaneFactor);
				constOfVS->SetFloat( pd3dDevice,"MPara",CalVS_MPara );
				DrawheightTexture(NorOption);
			}
			if( WChar2Ansi( ( ( CDXUTRadioButton* )pControl )->GetText())== "M Sample : 10000" ){
				/*Load the default xy_plane mesh*/
				SAFE_RELEASE(variableMesh);
				if( FAILED( D3DXLoadMeshFromX( L"Data\\Plane.x", D3DXMESH_MANAGED, 
                                   pd3dDevice, NULL, 
                                   NULL, NULL,NULL, 
                                   &variableMesh ) ) )
				{
					MessageBox(NULL, L"Could not find Plane.x", L"File error", MB_OK);
					break;
				}
				CalVS_MPara=49.5;
				PlaneFactor=1;
				pEffect->SetFloat(hPlaneFactor,PlaneFactor);
				constOfVS->SetFloat( pd3dDevice,"MPara",CalVS_MPara );
				DrawheightTexture(NorOption);
			}
            break;


		case IDC_SLIDER_A_COEF:
			sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_a_base;
			newOffset = newValue+ plane_a_offset;
			if(!planeHUD.GetSlider(IDC_SLIDER_A_COEF)->m_bPressed)
			{
				planeHUD.GetSlider(IDC_SLIDER_A_COEF)->SetValue(0);
				plane_a_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_a_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_a_base*=2;
					if(slider_a_base>=102400)
						slider_a_base=102400;
				}
			}
			planeHUD.GetStatic( IDC_STATIC_A_COEF )->SetText( L"Parameter A" );
			swprintf_s( wszOutput, 1024, L"%6.3f",newOffset );	
			planeHUD.GetEditBox(IDC_EDITBOX_A)->SetText(wszOutput);
			pEffect->SetFloat( hPlane_a_Coef,newOffset );
            break;


		case IDC_SLIDER_B_COEF:
			sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_b_base;
			newOffset = newValue+ plane_b_offset;
			if(!planeHUD.GetSlider(IDC_SLIDER_B_COEF)->m_bPressed)
			{
				planeHUD.GetSlider(IDC_SLIDER_B_COEF)->SetValue(0);
				plane_b_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_b_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_b_base*=2;
					if(slider_b_base>=102400)
						slider_b_base=102400;
				}
			}	
			planeHUD.GetStatic( IDC_STATIC_B_COEF )->SetText( L"Parameter B"  );	
			swprintf_s( wszOutput, 1024, L"%6.3f",newOffset );	
			planeHUD.GetEditBox(IDC_EDITBOX_B)->SetText(wszOutput);
			pEffect->SetFloat( hPlane_b_Coef,newOffset );
            break;


		case IDC_SLIDER_C_COEF:
			sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_d_base;
			newOffset = newValue+ plane_d_offset;
			if(!planeHUD.GetSlider(IDC_SLIDER_C_COEF)->m_bPressed)
			{
				planeHUD.GetSlider(IDC_SLIDER_C_COEF)->SetValue(0);
				plane_d_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_d_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_d_base*=2;
					if(slider_d_base>=102400)
						slider_d_base=102400;
				}
			}	
			planeHUD.GetStatic( IDC_STATIC_C_COEF )->SetText( L"Parameter C"  );	
			swprintf_s( wszOutput, 1024, L"%6.3f",newOffset );	
			planeHUD.GetEditBox(IDC_EDITBOX_C)->SetText(wszOutput);
			pEffect->SetFloat( hPlane_d_Coef,newOffset );
            break;


		case IDC_SLIDER_FA_COEF:
			sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_Fa_base;
			newOffset = newValue+ formula_a_offset;
			if(!formulaHUD.GetSlider(IDC_SLIDER_FA_COEF)->m_bPressed)
			{
				formulaHUD.GetSlider(IDC_SLIDER_FA_COEF)->SetValue(0);
				formula_a_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_Fa_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_Fa_base*=2;
					if(slider_Fa_base>=102400)
						slider_Fa_base=102400;
				}
			}
			formulaHUD.GetStatic( IDC_STATIC_FA_COEF )->SetText( L"Parameter A" );
			swprintf_s( wszOutput, 1024, L"%6.3f",newOffset );	
			formulaHUD.GetEditBox(IDC_EDITBOX_FA_COEF)->SetText(wszOutput);
			constOfVS->SetFloat( pd3dDevice,"a",newOffset );
			DrawheightTexture( NorOption);
            break;


		case IDC_SLIDER_FB_COEF:
			sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_Fb_base;
			newOffset = newValue+ formula_b_offset;
			if(!formulaHUD.GetSlider(IDC_SLIDER_FB_COEF)->m_bPressed)
			{
				formulaHUD.GetSlider(IDC_SLIDER_FB_COEF)->SetValue(0);
				formula_b_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_Fb_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_Fb_base*=2;
					if(slider_Fb_base>=102400)
						slider_Fb_base=102400;
				}
			}
			formulaHUD.GetStatic( IDC_STATIC_FB_COEF )->SetText( L"Parameter B" );
			swprintf_s( wszOutput, 1024, L"%6.3f",newOffset );	
			formulaHUD.GetEditBox(IDC_EDITBOX_FB_COEF)->SetText(wszOutput);
			constOfVS->SetFloat( pd3dDevice,"b",newOffset );
			DrawheightTexture( NorOption);
            break;



		case IDC_SLIDER_FC_COEF:
			sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_Fc_base;
			newOffset = newValue+ formula_c_offset;
			if(!formulaHUD.GetSlider(IDC_SLIDER_FC_COEF)->m_bPressed)
			{
				formulaHUD.GetSlider(IDC_SLIDER_FC_COEF)->SetValue(0);
				formula_c_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_Fc_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_Fc_base*=2;
					if(slider_Fc_base>=102400)
						slider_Fc_base=102400;
				}
			}
			formulaHUD.GetStatic( IDC_STATIC_FC_COEF )->SetText( L"Parameter C" );
			swprintf_s( wszOutput, 1024, L"%6.3f",newOffset );	
			formulaHUD.GetEditBox(IDC_EDITBOX_FC_COEF)->SetText(wszOutput);
			constOfVS->SetFloat( pd3dDevice,"c",newOffset );
			DrawheightTexture( NorOption);
            break;


		case IDC_SLIDER_FD_COEF:
			sliderValue = ( ( CDXUTSlider* )pControl )->GetValue();
            newValue = ( ( float ) sliderValue )/ slider_Fd_base;
			newOffset = newValue+ formula_d_offset;
			if(!formulaHUD.GetSlider(IDC_SLIDER_FD_COEF)->m_bPressed)
			{
				formulaHUD.GetSlider(IDC_SLIDER_FD_COEF)->SetValue(0);
				formula_d_offset=newOffset;
				if(abs(sliderValue)>700)
					slider_Fd_base/=2;
				if(abs(sliderValue)<500)
				{
					slider_Fd_base*=2;
					if(slider_Fd_base>=102400)
						slider_Fd_base=102400;
				}
			}
			formulaHUD.GetStatic( IDC_STATIC_FD_COEF )->SetText( L"Parameter D" );
			swprintf_s( wszOutput, 1024, L"%6.3f",newOffset );	
			formulaHUD.GetEditBox(IDC_EDITBOX_FD_COEF)->SetText(wszOutput);
			constOfVS->SetFloat( pd3dDevice,"d",newOffset );
			DrawheightTexture( NorOption);
            break;



		case IDC_EDITBOX:
		case IDC_EDITBOX_A:
		case IDC_EDITBOX_B:
		case IDC_EDITBOX_C:
		case IDC_EDITBOX_FA_COEF:
		case IDC_EDITBOX_FB_COEF:
		case IDC_EDITBOX_FC_COEF:
		case IDC_EDITBOX_FD_COEF:
			switch( nEvent )
            {
                case EVENT_EDITBOX_STRING:
                {
					int ID=((CDXUTEditBox*)pControl)->GetID();
					if(ID==4)
					{
						HRESULT hr;
						calVSpart2	= WChar2Ansi(((CDXUTEditBox*)pControl)->GetText() );
						calVS			=	calVSpart1 + calVSpart2  + calVSpart3;
						
						
						IDirect3DDevice9* pd3dDevice=DXUTGetD3D9Device();
						
						/*Compile the calculate VS*/
						::LPD3DXBUFFER pCode_VS;
						DWORD dwShaderFlags = D3DXSHADER_SKIPOPTIMIZATION;

						hr = D3DXCompileShader(calVS.c_str(), calVS.size(),NULL, NULL, "calculateVS",
															 "vs_3_0", dwShaderFlags, &pCode_VS,
															NULL, &constOfVS ) ;
						if(FAILED(hr)){
							UI.GetStatic(IDC_STATIC_HELP)->SetText(L"Your input is illegal.\nPlease consult the Help.txt and try again.");
							return;
						}
				
						constOfVS->SetFloat( pd3dDevice,"Range",CalVS_Range );
						constOfVS->SetFloat( pd3dDevice,"MPara",CalVS_MPara );
						constOfVS->SetFloatArray( pd3dDevice,"Center",CalVS_Center,2 );
						constOfVS->SetFloat( pd3dDevice,"a",formula_a_offset );
						constOfVS->SetFloat( pd3dDevice,"b",formula_b_offset );
						constOfVS->SetFloat( pd3dDevice,"c",formula_c_offset );
						constOfVS->SetFloat( pd3dDevice,"d",formula_d_offset);
						UI.GetStatic(IDC_STATIC_HELP)->SetText(L"");
						SAFE_RELEASE( calculateVS );
						V(pd3dDevice->CreateVertexShader( (DWORD*)pCode_VS->GetBufferPointer(), &calculateVS));
						pCode_VS->Release();
						DrawheightTexture( NorOption);
		  
						break;
					}
                }

				case EVENT_EDITBOX_CHANGE:
                {
					std::string 	dataget= WChar2Ansi(((CDXUTEditBox*)pControl)->GetText() );
					float data=atof(dataget.c_str());
					int ID=((CDXUTEditBox*)pControl)->GetID();
					if(ID==IDC_EDITBOX_A)
					{
						plane_a_offset=data;
						pEffect->SetFloat( hPlane_a_Coef,plane_a_offset );
					}
                    if(ID==IDC_EDITBOX_B)
					{
						plane_b_offset=data;
						pEffect->SetFloat( hPlane_b_Coef,plane_b_offset );
					}
					if(ID==IDC_EDITBOX_C)
					{
						plane_d_offset=data;
						pEffect->SetFloat( hPlane_d_Coef,plane_d_offset );
					}
					if(ID==IDC_EDITBOX_FA_COEF)
					{
						formula_a_offset=data;
						constOfVS->SetFloat( pd3dDevice,"a",formula_a_offset );
						DrawheightTexture( NorOption);
					}
                    if(ID==IDC_EDITBOX_FB_COEF)
					{
						formula_b_offset=data;
						constOfVS->SetFloat( pd3dDevice,"b",formula_b_offset );
						DrawheightTexture( NorOption);
					}
					if(ID==IDC_EDITBOX_FC_COEF)
					{
						formula_c_offset=data;
						constOfVS->SetFloat( pd3dDevice,"c",formula_c_offset );
						DrawheightTexture( NorOption);
					}
					if(ID==IDC_EDITBOX_FD_COEF)
					{
						formula_d_offset=data;
						constOfVS->SetFloat( pd3dDevice,"d",formula_d_offset );
						DrawheightTexture( NorOption);
					}

                    break;
                }

                
            }
	 }
}
//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // Initial the application
	InitApp();
	
	// Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the default hotkeys
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"Coordinate" );
    DXUTCreateDevice( true, 900, 600 );

	

    // Start the render loop
    DXUTMainLoop();

    return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Draw the heightTexture 
//--------------------------------------------------------------------------------------
void DrawheightTexture( bool NorOption )
{
	/*Calculate the heightTexture*/
	HRESULT hr;
	IDirect3DDevice9* pd3dDevice = DXUTGetD3D9Device();
	V( pd3dDevice->SetRenderTarget( 0,pRTheight ) );
	V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 ) );
	if(SUCCEEDED( pd3dDevice->BeginScene()))
	{
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_POINT);
		pd3dDevice->SetVertexShader(calculateVS);
		pd3dDevice->SetPixelShader(calculatePS);
		variableMesh->DrawSubset(0);
		pd3dDevice->EndScene();
	}
	if(NorOption)
		DrawnormalTexture();
}


//--------------------------------------------------------------------------------------
// Draw the normalTexture 
//--------------------------------------------------------------------------------------
void DrawnormalTexture( )
{
	/*Calculate the heightTexture*/
	HRESULT hr;
	UINT					iPass,cPasses;
	IDirect3DDevice9* pd3dDevice = DXUTGetD3D9Device();
	V( pd3dDevice->SetRenderTarget( 0,pRTnormal ) );
	V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 ) );
	// Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		V(pEffect->SetTechnique(hTechNormal));
		V(pEffect->Begin(&cPasses,0));
		for( iPass = 0 ; iPass < cPasses ; iPass++)
		{
			V(pEffect->BeginPass(iPass));
			V(variableMesh->DrawSubset(0));
			V(pEffect->EndPass());
		}
		V(pEffect->End());
        V( pd3dDevice->EndScene() );
    }
}

//--------------------------------------------------------------------------------------
// Convert WChar to string for shaders
//--------------------------------------------------------------------------------------
std::string WChar2Ansi(LPCWSTR pwszSrc)
{
         int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
 
         if (nLen<= 0) return std::string("");
 
         char* pszDst = new char[nLen];
         if (NULL == pszDst) return std::string("");
 
         WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
         pszDst[nLen -1] = 0;
 
         std::string strTemp(pszDst);
         delete [] pszDst;
 
         return strTemp;
}
