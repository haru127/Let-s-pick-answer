#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "input.h"
#include "inputx.h"
#include "game.h"
#include "title.h"

Scene* Manager::m_Scene{};//静的メンバ変数は再宣言が必要
Scene* Manager::m_NextScene{};

void Manager::Init()
{
	Renderer::Init();
	//Input::Init();

	SetScene<Title>();
}

void Manager::Uninit()
{
	m_Scene->Uninit();
	delete m_Scene;

	//Input::Uninit();
	Renderer::Uninit();
}

void Manager::Update()
{
	//Input::Update();

	if(m_NextScene)
	{
		if (m_Scene)
		{
			m_Scene->Uninit();
			delete m_Scene;
		}

		m_Scene = m_NextScene;
		m_Scene->Init();

		m_NextScene = nullptr;
	}

	m_Scene->Update();
}

void Manager::Draw()
{
	Renderer::Begin();

	m_Scene->Draw();

	Renderer::End();
}
