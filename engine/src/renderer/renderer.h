#pragma once

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    virtual void initialize() = 0;
    virtual void renderLoop();
    virtual void terminate() = 0;
    
    virtual void exec();
};