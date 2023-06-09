#pragma once

class Renderer
{
public:
    Renderer();
    virtual ~Renderer() = default;

    virtual void initialize() = 0;
    virtual void renderLoop();
    virtual void terminate() = 0;
    
    virtual void exec();
};