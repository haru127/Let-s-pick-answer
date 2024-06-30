#include "main.h"
#include "renderer.h"
#include "hp.h"
#include "sprite.h"

void Hp::Init(int hp, float h_x,float h_y)
{
	VERTEX_3D vertex[4];
	m_x = h_x;
	m_y = h_y;

	int score;
	m_HP = hp;
	score = m_HP;

	if (0 <= score && score < 10)
	{
		m_Length = 1;
	}
	else if (10 <= score && score < 100)
	{
		m_Length = 2;
	}
	else 
	{
		m_Length = 3;
	}

	for (int i = 0; i < m_Length; i++)
	{
		//���_���W�Z�o
		float vx = m_x - i * 30.0f;
		float vy = m_y;
		float height = 50.0f;
		float width = 50.0f;

		int n;
		n = score % 10;

		score /= 10;
		//�e�N�X�`�����W�Z�o
		float x = n % 5 * (1.0f / 5);
		float y = n / 5 * (1.0f / 5);

		vertex[0].Position = D3DXVECTOR3(vx, vy, 0.0f);
		vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = D3DXVECTOR2(x, y);

		vertex[1].Position = D3DXVECTOR3(vx + width, vy, 0.0f);
		vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = D3DXVECTOR2(x + 0.2f, y);

		vertex[2].Position = D3DXVECTOR3(vx, vy + height, 0.0f);
		vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = D3DXVECTOR2(x, y + 0.2f);

		vertex[3].Position = D3DXVECTOR3(vx + width, vy + height, 0.0f);
		vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = D3DXVECTOR2(x + 0.2f, y + 0.2f);
	}

	//���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;//���_�o�b�t�@�����ł���悤��
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//���_�o�b�t�@�����ł���悤��

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	//�e�N�X�`���ǂݍ���
	D3DX11CreateShaderResourceViewFromFile(Renderer::GetDevice(), "asset\\texture\\number.png", NULL, NULL, &m_Texture, NULL);
	assert(m_Texture);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

void Hp::Uninit()
{
	m_VertexBuffer->Release();
	m_Texture->Release();

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	GameObject::Uninit();//GameObject�̂��Ăԁ@���N���X�̊֐����Ă�
}

void Hp::Update()
{
	GameObject::Update();//GameObject�̂��Ăԁ@���N���X�̊֐����Ă�
}

void Hp::Draw()
{
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	Renderer::SetWorldViewProjection2D();

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

	int score;
	score = m_HP;

	if (0 <= score && score < 10)
	{
		m_Length = 1;
	}
	else if (10 <= score && score < 100)
	{
		m_Length = 2;
	}
	else
	{
		m_Length = 3;
	}

	for (int i = 0; i < m_Length; i++)
	{
		//���_���W�Z�o
		float vx = m_x - i * 30.0f;
		float vy = m_y;
		float height = 50.0f;
		float width = 50.0f;

		int n;
		n = score % 10;

		score /= 10;
		//�e�N�X�`�����W�Z�o
		float x = n % 5 * (1.0f / 5);
		float y = n / 5 * (1.0f / 5);

		//���_�f�[�^��������
		D3D11_MAPPED_SUBRESOURCE msr;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = D3DXVECTOR3(vx, vy, 0.0f);
		vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = D3DXVECTOR2(x, y);

		vertex[1].Position = D3DXVECTOR3(vx + width, vy, 0.0f);
		vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = D3DXVECTOR2(x + 0.2f, y);

		vertex[2].Position = D3DXVECTOR3(vx, vy + height, 0.0f);
		vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = D3DXVECTOR2(x, y + 0.2f);

		vertex[3].Position = D3DXVECTOR3(vx + width, vy + height, 0.0f);
		vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = D3DXVECTOR2(x + 0.2f, y + 0.2f);

		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);

		//�|���S���`��
		Renderer::GetDeviceContext()->Draw(4, 0);
	}

}


