#include "pch.h"
#include "App.h"

#include <ppltasks.h>
#include "Renderer.h"
#include "DeviceUtils.h"

using namespace Mythforge;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

using Microsoft::WRL::ComPtr;

// La plantilla de la aplicación DirectX 12 está documentada en https://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x409

// La función principal solo se utiliza para inicializar nuestra clase IFrameworkView.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new App();
}

App::App() :
	m_windowClosed(false),
	m_windowVisible(true),
	renderer(nullptr)
{
}

// Se llama a este método una vez que se activa la ventana.
void App::Run()
{
	auto Destroy = [this]() -> void {
		cube->Destroy();
		renderer->Destroy();
	};

	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			renderer->ResetCommands();

			PIXBeginEvent(renderer->commandQueue.Get(), 0, L"Render");
			{
				renderer->SetRenderTargets();

				XMMATRIX view = XMMatrixLookToRH(cameraPos, cameraFw, up);
				XMMATRIX viewProjection = XMMatrixMultiply(view, renderer->perspectiveMatrix);

				cube->UpdateConstantBuffer(renderer->backBufferIndex, viewProjection);
				cube->Render(renderer->commandList, renderer->backBufferIndex);

				renderer->Present();
			}
			PIXEndEvent(renderer->commandQueue.Get());

		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
	Flush(renderer->commandQueue, renderer->fence, renderer->fenceValue, renderer->fenceEvent);
	Destroy();
}

// El primer método al que se llama cuando se crea IFrameworkView.
void App::Initialize(CoreApplicationView^ applicationView)
{
	// Registrar controladores de eventos del ciclo de vida de la aplicación. Este ejemplo incluye Activated, así que
	// podemos activar CoreWindow e iniciar la representación en la ventana.
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);
}

// Se llama cuando se crea o se vuelve a crear el objeto CoreWindow.
void App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

	renderer = std::make_shared<Renderer>();
	renderer->Initialize(CoreWindow::GetForCurrentThread());

	renderer->ResetCommands();

	cube = std::make_shared<Cube>();
	cube->Initialize(renderer->frameCount, renderer->d3dDevice, renderer->commandList);

	renderer->CloseCommandsAndFlush();
	cube->DestroyUploadResources();
}

// Controladores de eventos del ciclo de vida de la aplicación.

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	// Run() no se iniciará hasta que se active CoreWindow.
	CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Guarde el estado de la aplicación de forma asincrónica tras solicitar un aplazamiento. Cuando se efectúa un aplazamiento,
	// se indica que la aplicación está ocupada realizando operaciones de suspensión. Tenga en cuenta que
	// es posible que los aplazamientos no se realicen de forma indefinida. Transcurridos unos cinco segundos,
	// la aplicación se verá forzada a salir.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{
		deferral->Complete();
	});
}


// Controladores de eventos de Windows.

void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{
	if (renderer != nullptr)
	{
		renderer->Resize(static_cast<UINT>(sender->Bounds.Width), static_cast<UINT>(sender->Bounds.Height));
	}
}

void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}


