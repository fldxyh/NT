#include "ObjectPool.h"
#include "ObjectBlock.h"
template <typename T> 
void  FreeQueue<T>::CreateOne()
{
    T* t = new T();
    _free.push(t);
}
template <typename T>
int FreeQueue<T>::Size(){
    return _free.size();
}
template <typename T>
bool FreeQueue<T>::Dispose(){
       while(_free.size() >0){
          T* t = _free.front();
           _free.pop();
           if constexpr (std::is_same<T,ObjectBlock>::value){
              t->Dispose();
           }
            delete t;
       }
       return true;
}

template <typename T>
T* FreeQueue<T>::Take()
{
    std::lock_guard<std::mutex> lock(_lock);
    if(_free.size() <= 0){
        CreateOne();
    }
    T* ret =  _free.front();
    _free.pop();
    return ret;
}

template <typename T>
bool FreeQueue<T>::Back(T* t)
{
    if(t== nullptr){
        return false;
    }
     std::lock_guard<std::mutex> lock(_lock);
     _free.push(t);
     return true;
}
template<typename T>
bool FreeQueue<T>::BackList(std::list<T*> objs){
     std::lock_guard<std::mutex> lock(_lock);
     for(auto it : objs){
        _free.push(it);
     }
     return true;
}
template <typename T>
T *ObjectPool<T>::Fetch()
{
    T* t =  _freeBuffer.Take();
    _refreshBuffer.inAdd(t);
}
template <typename T>
T *ObjectPool<T>::MallocObject()
{
    return Fetch();
}



template <typename T>
bool ObjectPool<T>::Recycle(T* t)
{
    
     _refreshBuffer.inRemove(t);
     return true;
}

template <typename T>
bool ObjectPool<T>::FreeObject(T* t)
{
    return Recycle(t);
}

template <typename T>
void ObjectPool<T>::Update()
{       
    
    if(_refreshBuffer.CanRefresh()){
      std::list<T*> objs = _refreshBuffer.Refresh();
      _freeBuffer.BackList(objs);
    }
    
}

template <typename T>
void ObjectPool<T>::Dispose()
{
    _freeBuffer.Dispose();
    _refreshBuffer.Dispose();
}

template <typename T>
inline int ObjectPool<T>::Size()
{
    return  _freeBuffer.Size();
}

template <typename T>
int ObjectPool<T>::inUse()
{
    return _refreshBuffer.GetReaderBuffer.size();
}

class A{ 

public:
~A(){
    std::cout<<" ~A "<<std::endl;
};
void test(){
    std::cout<<"A test"<<std::endl;
}

};
void test1(){
     FreeQueue<int>  bufferfree;
    std::cout<< bufferfree._free.size()<<std::endl;
    bufferfree.CreateOne();
    std::cout<< bufferfree._free.size()<<std::endl;
    bufferfree.CreateOne();
    // std::cout<<bufferfree.Size()<<std::endl;
    bufferfree.Dispose();
    // std::cout<<bufferfree.Size()<<std::endl;
    int* a = bufferfree.Take();
    //a->test();
    // std::cout<<bufferfree.Size()<<std::endl;
    bufferfree.Back(a);
    // std::cout<<bufferfree.Size()<<std::endl;
    bufferfree.Take();
    bufferfree.Take();
    bufferfree.Take();
    bufferfree.Back(a);
    bufferfree.Back(a);
    bufferfree.Back(a);
    bufferfree.Back(a);
    bufferfree.Back(a);
    bufferfree.Back(a);
    bufferfree.Back(a);
    std::cout<<bufferfree.Size()<<std::endl;
  //  RefreshBuffer<A> refreshBuffer;
    //refreshBuffer.GetRemoveBuffer()->push_back(a);
   // refreshBuffer.Refresh();
  // std::cout<<refreshBuffer.GetRemoveBuffer()->size()<<std::endl;
  
    //refreshBuffer.Dispose();      
}
int find( A *b,A *a){
    if(b == a){
        std::cout<<"1"<<std::endl;
        return true;
    }else{
        return false;
    }
}
std::multiset<A*> sets;
void add(A* b){
sets.insert(b);
}
int main(){
      ObjectPool<int>* pObjectPool =  ObjectPool<int>::GetInstance();
      int* a =  pObjectPool->Fetch();
      int* a1 = pObjectPool->Fetch();
      int* a2 = pObjectPool->Fetch();
      int* a3 = pObjectPool->Fetch();
  
      pObjectPool->Recycle(a);
         pObjectPool->Recycle(a1);
         pObjectPool->Recycle(a);
         pObjectPool->Recycle(a1);
      pObjectPool->Update();
 
        int *  pint= new int();
       std::multiset<int*> sets;
       sets.insert(pint);
       if(sets.find(pint) != sets.end()){
            std::cout<<"ddd"<<std::endl;
       }else{
        std::cout<<"no obj"<<std::endl;
       }
};