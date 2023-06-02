class Renderer
{
public:
    Renderer();
    ~Renderer();

    virtual void initialize() = 0;
    virtual void renderLoop();
    virtual void terminate() = 0;
    
    virtual void exec();
};