#include "core.h"
#include "IObjectPool.h"
template<typename T>
class FreeQueue 
{
public:
    void  CreateOne();
    int Size();
    bool Dispose();
public:
    T* Take();
    bool Back(T* t);    
    bool BackList(std::list<T*> objs);
public:
    std::queue<T*> _free;
    std::mutex _lock;
};
template<typename T>
class RefreshBuffer
{
public:
    std::vector<T*>* GetAddBuffer();
    std::vector<T*>* GetRemoveBuffer();
    std::vector<T*> * GetReaderBuffer();
public:
    std::list<T*> Refresh();
    bool CanRefresh(){
        return _add.size() > 0 || _remove.size() > 0;
    }
    void Dispose();
    void inAdd(T* t);
    void inRemove(T* t);
public:
    std::vector<T*> _reader;
    std::vector<T*> _add;
    std::vector<T*> _remove;
    std::mutex _lock;
};
// template <typename T>
// class ObjectPool :public IObjectPool
// {
    
// };

template <typename T>
inline std::vector<T *> *RefreshBuffer<T>::GetAddBuffer()
{
    return &_add;
}

template <typename T>
inline std::vector<T *> *RefreshBuffer<T>::GetRemoveBuffer()
{
    return &_remove;
}

template <typename T>
inline std::vector<T *> *RefreshBuffer<T>::GetReaderBuffer()
{
    return  &_reader;
}

template <typename T>
inline std::list<T *> RefreshBuffer<T>::Refresh()
{
    std::list<T*> rs;
    for (int i = 0;i<_add.size();i++)
	{
		_reader.push_back(_add[i]);
	}
    for (auto one : _remove)
	{
		auto iterReader = std::find_if(_reader.begin(), _reader.end(), [one](auto x)
		{
			return x == one;
		});

		if (iterReader == _reader.end())
		{
            std::cout << "CacheRefresh Swap error. not find obj to remove." << std::endl;
		}
		else
		{
			rs.push_back(one);
			_reader.erase(iterReader);
		}
	}
    // for (int i = 0;i < _remove.size();i++)
	// {
	// 	if(_reader.find(_remove[i]) != _reader.end()){
    //         _reader.erase(_remove[i]);
    //         ret.push_back(_remove[i]);
    //         std::cout<<"get out " <<std::endl;
    //     }else{
    //         std::cout<<"not find obj to swap"<<std::endl;
    //         std::cout <<_remove[i]<<std::endl;
            
    //     }
        
	// }

    _remove.clear();
	_add.clear();
    return rs;
}

template <typename T>
inline void RefreshBuffer<T>::Dispose()
{
    for (auto iter = _remove.begin(); iter != _remove.end(); ++iter)
	{
         if constexpr (std::is_same<T,ObjectBlock>::value){
             (*iter)->Dispose();
           }
		
		delete (*iter);
	}
	_remove.clear();

	for (auto iter = _reader.begin(); iter != _reader.end(); ++iter)
	{
		 if constexpr (std::is_same<T,ObjectBlock>::value){
             (*iter)->Dispose();
           }
		delete (*iter);
	}
	_reader.clear();
    for (auto iter = _add.begin(); iter != _add.end(); ++iter)
	{
		 if constexpr (std::is_same<T,ObjectBlock>::value){
             (*iter)->Dispose();
           }
		delete (*iter);
	}
	_add.clear();
}

template <typename T>
inline void RefreshBuffer<T>::inAdd(T* t)
{
    std::lock_guard<std::mutex> lock(_lock);
    _add.push_back(t);
}

template <typename T>
inline void RefreshBuffer<T>::inRemove(T* t)
{
    
    std::lock_guard<std::mutex> lock(_lock);
    _remove.push_back(t);
}

template<typename T>
class ObjectPool 
{
public:
     static ObjectPool<T>* GetInstance(){
        static ObjectPool<T>* instance;
        if(instance == nullptr){
            instance = new ObjectPool<T>();
        }
        return instance;
     }
public:
     T* MallocObject();
     T* Fetch();
     bool Recycle(T* t);
     
     bool FreeObject(T* t);
public: 
    void Update();
public: 
     void Dispose();     
public:
     int Size();
     int inUse();   
public:
    RefreshBuffer<T> _refreshBuffer;
    FreeQueue<T> _freeBuffer;
};

