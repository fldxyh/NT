class ObjectBlock
{
    ~ObjectBlock();
    virtual void BackFromPool();
    virtual void Dispose();
};