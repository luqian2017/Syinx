
template <typename T>
class SingletonClass
{
public:
	static T* Instance()
	{
		static T obj;
		obj_creator.DoNothing();
		return &obj;
	}
protected:
	SingletonClass() {}
private:
	struct ObjCreator
	{
		ObjCreator()
		{
			SingletonClass<T>::Instance();
			
		}
		inline void DoNothing()const {}
	};
	static ObjCreator obj_creator;
};
template<typename T>
typename SingletonClass<T>::ObjCreator SingletonClass<T>::obj_creator;


template<typename T>
class CreateObject
{
public:
	static T*  Instance()
	{
		T* obj = new T;
		return obj;
	}
	template<typename V>
	static T*  Instance(V sz)
	{
		T* obj = new T(sz);
		return obj;
	}
protected:
	CreateObject() {}
};

