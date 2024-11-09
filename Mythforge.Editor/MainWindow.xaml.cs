using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;

namespace Mythforge.Editor
{
    /// <summary>
    /// Ventana principal que contiene el renderizador y la terminal de depuración.
    /// </summary>
    public partial class MainWindow : Window
    {
        private RendererWrapper renderer; // Instancia del renderizador
        private HwndSource hwndSource; // Fuente del hwnd para el rendererContainer

        public MainWindow()
        {
            InitializeComponent();
            Loaded += MainWindow_Loaded; // Suscribirse al evento de carga de la ventana
            Closed += MainWindow_Closed; // Suscribirse al evento de cierre de la ventana

            // Suscribirse al evento SizeChanged del rendererContainer para ajustar el hwnd
            rendererContainer.SizeChanged += RendererContainer_SizeChanged;
        }

        /// <summary>
        /// Evento que maneja la carga de la ventana principal.
        /// Inicializa el renderer y lo asocia con la interfaz.
        /// </summary>
        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            LogToDebugTerminal("Cargando la ventana principal...");

            try
            {
                // Crear el hwnd para el rendererContainer
                InitializeRendererContainerHwnd();

                LogToDebugTerminal("Renderizador inicializado correctamente.");

                // Suscribirse al evento de renderizado
                CompositionTarget.Rendering += OnRendering;
            }
            catch (Exception ex)
            {
                LogToDebugTerminal($"Error al inicializar el renderizador: {ex.Message}");
            }
        }

        /// <summary>
        /// Método para inicializar el HwndSource y RendererWrapper para el rendererContainer.
        /// </summary>
        private void InitializeRendererContainerHwnd()
        {
            if (hwndSource != null)
            {
                hwndSource.Dispose();
                hwndSource = null;
            }

            if (renderer != null)
            {
                renderer.Dispose();
                renderer = null;
            }

            // Crear los parámetros del HwndSource para asociar con el rendererContainer
            var parameters = new HwndSourceParameters("RendererContainer")
            {
                PositionX = 0,
                PositionY = 0,
                Height = (int)rendererContainer.ActualHeight,
                Width = (int)rendererContainer.ActualWidth,
                WindowStyle = (int)(WS_CHILD | WS_VISIBLE),
                ParentWindow = new WindowInteropHelper(this).Handle
            };

            hwndSource = new HwndSource(parameters);
            IntPtr hwnd = hwndSource.Handle;

            // Inicializar el renderizador con el hwnd específico del rendererContainer
            renderer = new RendererWrapper();
            renderer.Initialize(hwnd);
        }

        /// <summary>
        /// Método de renderizado que se llama continuamente para refrescar el renderizador.
        /// </summary>
        private void OnRendering(object sender, EventArgs e)
        {
            if (renderer != null)
            {
                try
                {
                    renderer.Render(); // Ejecutar el renderizado
                }
                catch (InvalidOperationException ex)
                {
                    LogToDebugTerminal($"Error de renderizado: {ex.Message}");
                }
            }
        }

        /// <summary>
        /// Evento que maneja el cambio de tamaño del rendererContainer.
        /// </summary>
        private void RendererContainer_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            // Re-inicializar el renderizador con el nuevo tamaño del rendererContainer
            InitializeRendererContainerHwnd();
        }

        /// <summary>
        /// Evento que maneja el cierre de la ventana principal.
        /// Asegura que se liberen los recursos del renderizador.
        /// </summary>
        private void MainWindow_Closed(object sender, EventArgs e)
        {
            if (renderer != null)
            {
                LogToDebugTerminal("Cerrando el renderizador y liberando recursos...");
                renderer.Dispose();
                renderer = null;
            }

            if (hwndSource != null)
            {
                hwndSource.Dispose();
                hwndSource = null;
            }
        }

        /// <summary>
        /// Método para escribir mensajes en la terminal de depuración (RichTextBox).
        /// </summary>
        /// <param name="message">Mensaje que se escribirá en la terminal.</param>
        private void LogToDebugTerminal(string message)
        {
            // Asegurarse de que la operación se ejecute en el hilo de la interfaz de usuario.
            Application.Current.Dispatcher.Invoke(() =>
            {
                if (debugTerminal != null)
                {
                    debugTerminal.AppendText($"{DateTime.Now:HH:mm:ss} - {message}{Environment.NewLine}");
                    debugTerminal.ScrollToEnd(); // Asegura que siempre se vea la última línea
                }
            });
        }

        // Constantes usadas para crear la ventana hijo
        private const int WS_CHILD = 0x40000000;
        private const int WS_VISIBLE = 0x10000000;
    }
}
