
class ObjectBlock;
class IObjectPool
{
    virtual void FreeObject(ObjectBlock* pObj) = 0;
    virtual ~IObjectPool() = default;
};
