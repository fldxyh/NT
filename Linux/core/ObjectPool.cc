#include "ObjectPool.h"
template <typename T> 
void  FreeQueue<T>::CreateOne()
{
    T* t = new T();
    _free.push(t);
}

