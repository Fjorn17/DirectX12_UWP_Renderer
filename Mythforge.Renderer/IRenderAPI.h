#pragma once

class IRenderAPI {
public:
    virtual ~IRenderAPI() = default;

    virtual void Initialize(void* hwnd) = 0;
    virtual void RenderFrame() = 0;
    virtual void Resize(int width, int height) = 0;
    virtual void WaitForPreviousFrame() = 0;
    virtual void CleanUp() = 0;
};
