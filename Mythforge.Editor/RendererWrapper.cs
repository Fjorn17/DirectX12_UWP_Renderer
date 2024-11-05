using System;
using System.Runtime.InteropServices;

namespace Mythforge.Editor
{
    public class RendererWrapper : IDisposable
    {
        private IntPtr rendererInstance;

        [DllImport("Mythforge.Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateRenderer();

        [DllImport("Mythforge.Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void InitializeRenderer(IntPtr renderer, IntPtr hwnd);

        [DllImport("Mythforge.Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void RenderFrame(IntPtr renderer);

        [DllImport("Mythforge.Renderer.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void DestroyRenderer(IntPtr renderer);

        public RendererWrapper()
        {
            rendererInstance = CreateRenderer();
            if (rendererInstance == IntPtr.Zero)
            {
                throw new Exception("Failed to create renderer instance.");
            }
        }

        public void Initialize(IntPtr hwnd)
        {
            InitializeRenderer(rendererInstance, hwnd);
        }

        public void Render()
        {
            try
            {
                if (rendererInstance != IntPtr.Zero)
                {
                    RenderFrame(rendererInstance);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error during RenderFrame: " + ex.Message);
                Dispose();
                throw;
            }
        }

        public void Dispose()
        {
            if (rendererInstance != IntPtr.Zero)
            {
                DestroyRenderer(rendererInstance);
                rendererInstance = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        ~RendererWrapper()
        {
            Dispose();
        }
    }
}