﻿#pragma once

#include "pch.h"
#include "Renderer.h"
#include "Cube.h"

using namespace DirectX;

namespace Mythforge
{
	// Punto de entrada principal de nuestra aplicación. Conecta la aplicación con el shell de Windows y controla los eventos de ciclo de vida de la aplicación.
	ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
	{
	public:
		App();

		// Métodos IFrameworkView.
		virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
		virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
		virtual void Load(Platform::String^ entryPoint) {}
		virtual void Run();
		virtual void Uninitialize() {}

	protected:
		// Controladores de eventos del ciclo de vida de la aplicación.
		void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);

		// Controladores de eventos de Windows.
		void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
		void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

	private:
		bool m_windowClosed;
		bool m_windowVisible;
		std::shared_ptr<Renderer> renderer;

		std::shared_ptr<Cube> cube;

		XMVECTOR cameraPos = {0.0f, 0.0f, -5.0f};
		XMVECTOR cameraFw = { 0.0f, 0.0f, 1.0f, 0.0f };
		XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
	};
}

ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};
