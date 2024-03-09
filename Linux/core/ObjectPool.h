#include "core.h"
template<typename T>
class FreeQueue 
{
public:
    void  CreateOne();
private:
    std::queue<T> _free;
    std::mutex _lock;
};

template <typename T>
class ObjectPool :public IObjectPool, public: Singleton<T>
{

    
};

