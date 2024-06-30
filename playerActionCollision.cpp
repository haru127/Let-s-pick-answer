#include "main.h"
#include "renderer.h"
#include "manager.h"
#include "scene.h"
#include "game.h"
#include "debug.h"
#include "camera.h"
#include "playerActionCollision.h"

ID3D11ShaderResourceView* PlayerActionCollision::m_Texture{};//static�Đ錾

void PlayerActionCollision::Load()
{
	//�e�N�X�`���ǂݍ���
	D3DX11CreateShaderResourceViewFromFile(Renderer::GetDevice(), "asset\\texture\\blue.png", NULL, NULL, &m_Texture, NULL);
	assert(m_Texture);
}

void PlayerActionCollision::Unload()
{
	m_Texture->Release();
}

void PlayerActionCollision::Init(float x, float y, float sizeX, float sizeY, int zFrame, float angle, float speed, float damage)
{
	Scene* scene = Manager::GetScene();

	VERTEX_3D vertex[4];

	if (!(isSceneGame<Game>(scene)))//�f�o�b�O�V�[���̏ꍇ
	{
		vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
		vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
		vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
		vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	}
	else if (isSceneGame<Game>(scene))//�Q�[��
	{
		vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	}

	vertex[0].Position = D3DXVECTOR3(-sizeX /2, sizeY/2, 0.0f);
	vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	//vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

	vertex[1].Position = D3DXVECTOR3(sizeX /2, sizeY/2, 0.0f);
	vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f,-1.0f);
	//vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	//vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	vertex[2].Position = D3DXVECTOR3(-sizeX /2, -sizeY/2, 0.0f);
	vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	//vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

	vertex[3].Position = D3DXVECTOR3(sizeX /2, -sizeY /2, 0.0f);
	vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	//vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	//���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;//���_�o�b�t�@�𒲐����Ȃ�
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;//���_�o�b�t�@�𒲐����Ȃ�

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_size.x = sizeX;
	m_size.y = sizeY;

	m_ZizokuFrame = zFrame;
	m_Angle = angle;
	m_Speed = speed;
	m_Damage = damage;
}

void PlayerActionCollision::Uninit()
{
	GameObject::Uninit();

	m_VertexBuffer->Release();

	//m_Texture->Release(); //Unload�Ńe�N�X�`���͉������

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void PlayerActionCollision::Update()
{
	GameObject::Update();//GameObject�̂��Ăԁ@���N���X�̊֐����Ă�

	////���W�ύX�������old���X�V
	//if (m_Position != D3DXVECTOR3(0.0f, 0.0f, 0.0f))
	//{
	//	m_OldPosition = m_Position;
	//}

}

void PlayerActionCollision::Draw()
{
	if (0 < m_ZizokuFrame)//����
	{
		Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

		Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
		Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

		////�J�����̃r���[�}�g���b�N�X�ݒ�
		//Scene* scene   = Manager::GetScene();
		//Camera* camera = scene->GetGameObject<Camera>();
		//D3DXMATRIX view = camera->GetViewMatrix();

		////�r���[�̋t�s��
		//D3DXMATRIX invView;
		//D3DXMatrixInverse(&invView, NULL, &view);//�t�s��
		//invView._41 = 0.0f;
		//invView._42 = 0.0f;
		//invView._43 = 0.0f;

		//�}�g���N�X�ݒ�
		D3DXMATRIX world, scale, rot, trans;
		D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
		//D3DXMatrixRotationYawPitchRoll(&rot, m_rotation.y, m_rotation.x, m_rotation.z);
		D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, m_Position.z);
		world = scale * trans;
		Renderer::SetWorldMatrix(&world);

		//���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
		//�}�e���A���ݒ�
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		material.TextureEnable = true;
		Renderer::SetMaterial(material);
		//�e�N�X�`���ݒ�
		Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
		//�v���~�e�B�u�g�|���W�[�ݒ�
		Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		Renderer::SetATCEnable(true);
		//�|���S���`��
		Renderer::GetDeviceContext()->Draw(4, 0);

		Renderer::SetATCEnable(false);
	}
}

void PlayerActionCollision::PullZizokuFrame()
{
	if (m_ZizokuFrame != 0)
	{
		m_ZizokuFrame--;
	}

	if (m_ZizokuFrame <= 0)
	{
		m_Destroy = true; //���X�g����폜
		return;
	}
}

void PlayerActionCollision::DeleteCollision()
{
	m_ZizokuFrame = 0;

	if (m_ZizokuFrame <= 0)
	{
		m_Destroy = true; //���X�g����폜
		return;
	}

}

D3DXVECTOR3 PlayerActionCollision::GetSize()
{
	return m_size;
}




