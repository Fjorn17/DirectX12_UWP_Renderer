using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;

namespace Mythforge.Editor
{
    /// <summary>
    /// Ventana principal que contiene y controla el ciclo de vida de la instancia de RendererWrapper.
    /// </summary>
    public partial class MainWindow : Window, IDisposable
    {
        private bool disposed = false; // Indica si los recursos ya han sido liberados
        private RendererWrapper renderer; // Instancia de RendererWrapper para administrar el renderizado DirectX

        /// <summary>
        /// Constructor que inicializa la ventana principal y sus eventos.
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
            Loaded += MainWindow_Loaded;
            Closed += MainWindow_Closed;
        }

        /// <summary>
        /// Evento que se ejecuta al cargar la ventana. Configura el renderizador y el contexto de rendering.
        /// </summary>
        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            // Obtener el identificador de la ventana para pasarlo al renderer
            IntPtr hwnd = new WindowInteropHelper(this).Handle;

            // Inicializar el renderer y asociarlo con el hwnd de esta ventana
            renderer = new RendererWrapper();
            renderer.Initialize(hwnd);

            // Suscribirse al evento de renderizado continuo
            CompositionTarget.Rendering += OnRendering;
        }

        /// <summary>
        /// Método de renderizado que se llama continuamente por el evento CompositionTarget.Rendering.
        /// </summary>
        private void OnRendering(object sender, EventArgs e)
        {
            try
            {
                if (renderer != null)
                {
                    renderer.Render(); // Ejecuta el renderizado en cada fotograma
                }
            }
            catch (InvalidOperationException ex)
            {
                // Manejo de errores en el renderizado y salida del mensaje
                Console.WriteLine(Mythforge.Editor.Properties.Resources.RenderErrorMessage + ex.Message);

                // Desuscribirse del evento si ocurre una excepción
                CompositionTarget.Rendering -= OnRendering;
            }
        }

        /// <summary>
        /// Evento que se ejecuta al cerrar la ventana para asegurar que se liberen los recursos.
        /// </summary>
        private void MainWindow_Closed(object sender, EventArgs e)
        {
            Dispose(); // Llama a Dispose cuando se cierra la ventana
        }

        /// <summary>
        /// Implementación de Dispose para liberar recursos no administrados.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this); // Evita la finalización por el recolector de basura
        }

        /// <summary>
        /// Libera los recursos gestionados y no gestionados.
        /// </summary>
        /// <param name="disposing">True si se llama desde Dispose, false si se llama desde el finalizador.</param>
        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    // Libera el renderer si se ha inicializado
                    renderer?.Dispose();
                    renderer = null;
                }
                disposed = true;
            }
        }

        /// <summary>
        /// Destructor de la ventana principal que asegura la liberación de recursos.
        /// </summary>
        ~MainWindow()
        {
            Dispose(false);
        }
    }
}
