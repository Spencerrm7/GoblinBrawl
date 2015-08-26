#include "stdafx.h"
#include "FirePlinth.h"
#include "ModelLoader.h"
#include "Vertex.h"
#include "Mesh.h"
#include "d3dUtil.h"
#include "d3dx11effect.h"
#include "Effects.h"
#include "MathUtils.h"

FirePlinth::FirePlinth() : 
mesh (nullptr),
diffuseView(nullptr)
{}

FirePlinth::~FirePlinth() {}

bool FirePlinth::Init( ModelLoader* modelLoader, ID3D11Device* device ) {
	modelLoader->Load( "firePlinth.lxo", Vertex::TERRAIN );
	mesh = modelLoader->GetMesh();
	if( !mesh ) {
		return false;
	}
	HR( D3DX11CreateShaderResourceViewFromFile( device, L"./art/textures/fire_plinth_color.tif", NULL, NULL, &diffuseView, NULL ) );
	return true;
}

void XM_CALLCONV FirePlinth::Draw( FXMMATRIX viewProj, ID3D11DeviceContext* context ) {
	context->IASetInputLayout( InputLayouts::Terrain );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	UINT stride = sizeof( Vertex::TerrainVertex );
	UINT offset = 0;
	ID3D11Buffer* buffers[1] = { mesh->VB() };
	context->IASetVertexBuffers( 0, 1, &buffers[0], &stride, &offset );
	context->IASetIndexBuffer( mesh->IB(), mesh->IndexFormat(), 0 );

	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX worldInvTranspose = MathUtils::InverseTranspose( world );
	XMMATRIX worldViewProj = world*viewProj;

	Effects::TerrainFX->SetWorld( world );
	Effects::TerrainFX->SetWorldInvTranspose( worldInvTranspose );
	Effects::TerrainFX->SetWorldViewProj( worldViewProj );
	Effects::TerrainFX->SetDiffuseMap( diffuseView );

	ID3DX11EffectTechnique* tech = Effects::TerrainFX->terrainTechnique;
	D3DX11_TECHNIQUE_DESC td;
	tech->GetDesc( &td );
	for( UINT p = 0; p<td.Passes; ++p ) {
		tech->GetPassByIndex( p )->Apply( 0, context );
		context->DrawIndexed( mesh->IndexCount(), 0, 0 );
	}
}