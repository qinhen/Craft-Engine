#pragma once
#ifndef CRAFT_ENGINE_CORE_H_
#define CRAFT_ENGINE_CORE_H_

#include "../Common.h"

#include "./core/Callable.h"
#include "./core/String.h"
#include "./core/Memory.h"
#include "./core/Containers.h"
#include "./core/Thread.h"



namespace CraftEngine
{

	namespace core
	{


		// 单例
		template<typename Type>
		class Singleton
		{
		public:
			template<typename ... Args>
			static Type* createInstance(Args&&... args)
			{
				if (m_pSingle == nullptr) 
					m_pSingle = new Type(std::forward<Args>(args)...);
				return m_pSingle;
			}

			static Type* getInstance() {
				if (m_pSingle == nullptr)
					createInstance<>();
				return m_pSingle;
			}

			static void destroyInstance()
			{
				if (m_pSingle != nullptr)
					delete m_pSingle;
				m_pSingle = nullptr;
			}
		private:
			static Type* m_pSingle;
			virtual ~Singleton() {}
		};
		template<typename Type>
		Type* Singleton<Type>::m_pSingle = nullptr;


		// 继承自该类的子类都是不可复制的
		class NonCopyable
		{
		protected:
			NonCopyable() = default;
			~NonCopyable() = default;
			NonCopyable(const NonCopyable&) = delete;
			NonCopyable& operator=(const NonCopyable&) = delete;
		};




		template<typename... Types>
		class Variant
		{
		private:
			template <size_t arg, size_t... rest>
			struct IntegerMax;
			template <size_t arg>
			struct IntegerMax<arg> : std::integral_constant<size_t, arg> {};
			template <size_t arg1, size_t arg2, size_t... rest>
			struct IntegerMax<arg1, arg2, rest...> : std::integral_constant<size_t, arg1 >= arg2 ? IntegerMax<arg1, rest...>::value
				: IntegerMax<arg2, rest...>::value> {};
			template<typename... Args>
			struct MaxAlign : std::integral_constant<int, IntegerMax<std::alignment_of<Args>::value...>::value> {};
			template <typename T, typename... List>
			struct Contains;
			template <typename T, typename Head, typename... Rest>
			struct Contains<T, Head, Rest...>
				: std::conditional<std::is_same<T, Head>::value, std::true_type, Contains<T, Rest... >> ::type {};
			template <typename T>
			struct Contains<T> : std::false_type {};
			template <typename T, typename... List>
			struct IndexOf;
			template <typename T, typename Head, typename... Rest>
			struct IndexOf<T, Head, Rest...> { enum { value = IndexOf<T, Rest...>::value + 1 }; };
			template <typename T, typename... Rest>
			struct IndexOf<T, T, Rest...> { enum { value = 0 }; };
			template <typename T>
			struct IndexOf<T> { enum { value = -1 }; };
			template<int index, typename... Types>
			struct At;
			template<int index, typename First, typename... Types>
			struct At<index, First, Types...> { using type = typename At<index - 1, Types...>::type; };
			template<typename T, typename... Types>
			struct At<0, T, Types...> { using type = T; };

			enum
			{
				data_size = IntegerMax<sizeof(Types)...>::value,
				align_size = MaxAlign<Types...>::value
			};

			using data_t = typename std::aligned_storage<data_size, align_size>::type;
			template<int index>
			using IndexType = typename At<index, Types...>::type;
		public:

			Variant(void) :m_typeIndex(typeid(void))
			{
			}

			~Variant()
			{
				_Destroy(m_typeIndex, &m_data);
			}

			Variant(Variant<Types...>&& old) : m_typeIndex(old.m_typeIndex)
			{
				_Move(old.m_typeIndex, &old.m_data, &m_data);
			}

			Variant(const Variant<Types...>& old) : m_typeIndex(old.m_typeIndex)
			{
				_Copy(old.m_typeIndex, &old.m_data, &m_data);
			}

			Variant& operator=(const Variant& old)
			{
				_Copy(old.m_typeIndex, &old.m_data, &m_data);
				m_typeIndex = old.m_typeIndex;
				return *this;
			}

			Variant& operator=(Variant&& old)
			{
				_Move(old.m_typeIndex, &old.m_data, &m_data);
				m_typeIndex = old.m_typeIndex;
				return *this;
			}

			template <class T,
				class = typename std::enable_if<Contains<typename std::decay<T>::type, Types...>::value>::type>
				Variant(T&& value) : m_typeIndex(typeid(void))
			{
				_Destroy(m_typeIndex, &m_data);
				typedef typename std::decay<T>::type U;
				new(&m_data) U(std::forward<T>(value));
				m_typeIndex = std::type_index(typeid(U));
			}

			template <class T,
				class = typename std::enable_if<Contains<typename std::decay<T>::type, Types...>::value>::type>
				Variant(const T& value) : m_typeIndex(typeid(void))
			{
				_Destroy(m_typeIndex, &m_data);
				typedef typename std::decay<T>::type U;
				new(&m_data) U(value);
				m_typeIndex = std::type_index(typeid(U));
			}

			template<typename T>
			bool isSame() const
			{
				return (m_typeIndex == std::type_index(typeid(T)));
			}

			bool isEmpty() const
			{
				return m_typeIndex == std::type_index(typeid(void));
			}

			template<typename T>
			typename std::decay<T>::type& get()
			{
				using U = typename std::decay<T>::type;
				if (!isSame<U>())
				{
					std::cout << typeid(U).name() << " is not defined. "
						<< "current type is " << m_typeIndex.name() << std::endl;
					throw std::bad_cast{};
				}
				return *(U*)(&m_data);
			}

			template<typename T>
			typename const std::decay<T>::type& get() const
			{
				using U = typename std::decay<T>::type;
				if (!isSame<U>())
				{
					std::cout << typeid(U).name() << " is not defined. "
						<< "current type is " << m_typeIndex.name() << std::endl;
					throw std::bad_cast{};
				}
				return *(U*)(&m_data);
			}

			template <typename T>
			int indexOf()
			{
				return IndexOf<T, Types...>::value;
			}

			bool operator==(const Variant& rhs) const
			{
				return m_typeIndex == rhs.m_typeIndex;
			}

			bool operator<(const Variant& rhs) const
			{
				return m_typeIndex < rhs.m_typeIndex;
			}

		private:
			void _Destroy(const std::type_index& index, void* buf)
			{
				int arr[] = { (_Destroy0<Types>(index, buf), 0)... };
				//[](Types&&...) {}((_Destroy0<Types>(index, buf), 0)...);
			}

			template<typename T>
			void _Destroy0(const std::type_index& id, void* data)
			{
				if (id == std::type_index(typeid(T)))
					reinterpret_cast<T*>(data)->~T();
			}

			void _Move(const std::type_index& old_t, void* old_v, void* new_v)
			{
				[](Types&&...) {}((_Move0<Types>(old_t, old_v, new_v), 0)...);
			}

			template<typename T>
			void _Move0(const std::type_index& old_t, void* old_v, void* new_v)
			{
				if (old_t == std::type_index(typeid(T)))
					new (new_v)T(std::move(*reinterpret_cast<T*>(old_v)));
			}

			static void _Copy(const std::type_index& old_t, const void* old_v, void* new_v)
			{
				_Copy0<Types...>(old_t, old_v, new_v);
			}

			template<typename T, typename... TypeList, class = std::enable_if_t<(sizeof...(TypeList) > 0)>>
			static void _Copy0(const std::type_index& old_t, const void* old_v, void* new_v)
			{
				if (old_t == std::type_index(typeid(T)))
					new (new_v)T(*reinterpret_cast<const T*>(old_v));
				else
					_Copy0<TypeList...>(old_t, old_v, new_v);
			}

			template<typename T>
			static void _Copy0(const std::type_index& old_t, const void* old_v, void* new_v)
			{
				if (old_t == std::type_index(typeid(T)))
					new (new_v)T(*reinterpret_cast<const T*>(old_v));
				else
					0; //
			}

		private:
			data_t m_data;
			std::type_index m_typeIndex;
		};



		struct Any
		{
		private:
			struct Base;
			typedef std::unique_ptr<Base> BasePtr;
			struct Base
			{
				virtual ~Base() {}
				virtual BasePtr _Clone()const = 0;
			};
			template<typename Dervied>
			struct Derived :Base
			{
				template<typename U>
				Derived(U&& value) :m_value(std::forward<U>(value)) {}
				BasePtr _Clone()const
				{
					return BasePtr(new Derived<Dervied>(m_value));
				}
				Dervied m_value;

			};
			BasePtr _Clone()const
			{
				if (m_pointer != nullptr)
					return m_pointer->_Clone();
				return nullptr;
			}

			BasePtr m_pointer;
			std::type_index m_typeIndex;
		public:
			Any(void) :m_typeIndex(std::type_index(typeid(void))) {}
			Any(const Any& that) :m_pointer(that._Clone()), m_typeIndex(that.m_typeIndex) {}
			Any(Any&& that) :m_pointer(std::move(that.m_pointer)), m_typeIndex(that.m_typeIndex) {}

			template<typename U, class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, Any>::value, U>::type> 
			Any(U&& value) :
				m_pointer(new Derived<typename std::decay<U>::type>(std::forward<U>(value))),
				m_typeIndex(std::type_index(typeid(typename std::decay<U>::type))) {}

			bool isNull()const { return bool(m_pointer); }
			
			template<typename U>
			bool isType()const { return m_typeIndex == std::type_index(typeid(U)); }

			template<typename U>
			U& cast()const
			{
				if (!this->isType<U>())
					throw std::runtime_error("error type cast!");
				auto derived = dynamic_cast<Derived<U>*>(m_pointer.get());
				return derived->m_value;
			}
			
			Any& operator=(const Any& a)
			{
				if (m_pointer == a.m_pointer)
					return *this;
				m_pointer = a._Clone();
				m_typeIndex = a.m_typeIndex;
				return *this; 
			}

			void swap(Any& other)
			{
				BasePtr ptr = std::move(other.m_pointer);
				auto typeIndex = other.m_typeIndex;
				other.m_pointer.swap(m_pointer);
				other.m_typeIndex = m_typeIndex;
				m_pointer.swap(ptr);
				m_typeIndex = typeIndex;
			}

			const char* typeName()const
			{
				return m_typeIndex.name();
			}
		};



		// 观察者
		template<typename FuncType>
		class Observer
		{
		public:
			typedef uint32_t KeyType;
			typedef FuncType CallType;
		private:
			template<typename CallableEntity>
			KeyType _Register_One(CallableEntity&& func)
			{
				auto key = m_nextKey++;
				m_connections.emplace(key, std::forward<CallableEntity>(func));
				return key;
			}
		public:

			Observer() :m_nextKey(0) {};
			~Observer() = default;
			/*
			  connect(func：可调用实体)->编号
			  添加一个可调用实体,支持常函数/lambda表达式/被调用类静态函数
			*/
			KeyType connect(std::function<FuncType>&& func)
			{
				return _Register_One(Callback<FuncType>(std::forward<std::function<FuncType>>(func)));
			}

			/*
			  connect(func：可调用实体)->编号
			  添加一个可调用实体,支持常函数/lambda表达式/被调用类静态函数
			*/
			KeyType connect(FuncType func)
			{
				return _Register_One(Callback<FuncType>(func));
			}

			/*
			  connect(func：可调用实体)->编号
			  添加一个可调用实体,支持常函数/lambda表达式/被调用类静态函数
			*/
			KeyType connect(Callback<FuncType>&& func)
			{
				return _Register_One(func);
			}

			/*
			  connect(func：可调用实体)->编号
			  添加一个可调用实体,支持常函数/lambda表达式/被调用类静态函数
			*/
			KeyType connect(const Callback<FuncType>& func)
			{
				return _Register_One(func);
			}

			/*
			  connect(pObject：被调用对象指针, pFunc：被调用对象函数指针)->编号
			  添加一个可调用实体,支持非常量类成员函数
			*/
			template <class CalledType, class ReturnType, typename...Args>
			KeyType connect(CalledType* pObject, ReturnType(CalledType::* pFunc)(Args...))
			{
				return _Register_One(Callback<FuncType>(pObject, pFunc)); // std::_Ph从1号开始, 因为0号参数被this指针占用
			}
			/*
			  connect(pObject：被调用对象指针, pFunc：被调用对象函数指针)->编号
			  添加一个可调用实体,支持常量类成员函数
			*/
			template <class CalledType, class ReturnType, typename...Args>
			KeyType connect(const CalledType* pObject, ReturnType(CalledType::* pFunc)(Args...)const)
			{
				return _Register_One(Callback<FuncType>(pObject, pFunc)); // std::_Ph从1号开始, 因为0号参数被this指针占用
			}

			// 删除一个可调用实体
			void disconnect(KeyType key)
			{
				m_connections.erase(key);
			}
			// 触发调用所有可调用对象
			template<typename... Args>
			void notify(Args&&... args)
			{
				for (auto& it : m_connections)
					it.second.call(std::forward<Args>(args)...);
			}
			// 可调用对象的总数
			uint32_t size()const { return m_connections.size(); }
		private:

			KeyType m_nextKey = 0;
			std::unordered_map<KeyType, Callback<FuncType>> m_connections;
		};



		class IocContainer :NonCopyable
		{
		private:
			struct ConstructorData
			{
				Any func;
				std::type_index index = typeid(void);
			};

		public:
			//template<typename BaseType, typename DerviedType, typename... ConstructorArgs>
			//typename std::enable_if<!std::is_base_of<BaseType, DerviedType>::value>::type
			//registerType(const std::string key)
			//{
			//	std::function<BaseType * (ConstructorArgs...)> func = [](ConstructorArgs... args) {
			//		return (BaseType*)(new DerviedType(args...));
			//	};
			//	_Register_Type(key, Any(func));
			//}

			template<typename BaseType, typename DependType, typename... ConstructorArgs>
			typename std::enable_if<std::is_base_of<BaseType, DependType>::value>::type
				regtype(const std::string key)
			{
				std::function<BaseType* (ConstructorArgs...)> func = [](ConstructorArgs... args) {
					return new DependType(args...);
				};
				_Register_Type(key, Any(func), std::type_index(typeid(DependType)));
			}

			template<typename Type, typename... ConstructorArgs>
			void regtype_s(const std::string key)
			{
				std::function<Type* (ConstructorArgs...)> func = [](ConstructorArgs... args) {
					return new Type(args...);
				};
				_Register_Type(key, Any(func), std::type_index(typeid(Type)));
			}

			template<class BaseType, typename... ConstructorArgs>
			BaseType* create(const std::string key, ConstructorArgs... args, std::type_index* idx = nullptr)
			{
				auto it = m_constructorMap.find(key);			
				if (it == m_constructorMap.end())
					return nullptr;
				auto func = it->second.func.cast<std::function<BaseType * (ConstructorArgs...)>>();
				if (idx != nullptr)
					*idx = it->second.index;
				return func(args...);
			}

		private:
			void _Register_Type(const std::string key, const Any& constructor, const std::type_index& idx)
			{
				ConstructorData data = { constructor, idx };
				m_constructorMap.emplace(key, data);
			}
			std::unordered_map<std::string, ConstructorData> m_constructorMap;
		};







		CRAFT_ENGINE_CORE_API std::vector<uint8_t> readFile(const std::string& filename)
		{
			std::ifstream file(filename, std::ios::ate | std::ios::binary);
			if (!file.is_open())
				throw std::runtime_error("failed to open file!");
			size_t fileSize = (size_t)file.tellg();
			std::vector<uint8_t> buffer(fileSize);
			file.seekg(0);
			file.read((char*)buffer.data(), fileSize);
			file.close();
			return buffer;
		}

		CRAFT_ENGINE_CORE_API void writeFile(void* data, int size,const std::string& filename)
		{
			std::ofstream file(filename, std::ios::binary);
			if (!file.is_open())
				throw std::runtime_error("failed to open file!");
			file.write((char*)data, size);
			file.close();
		}

		CRAFT_ENGINE_CORE_API std::string readString(const std::string& filename)
		{
			std::ifstream t(filename);
			std::stringstream buffer;
			buffer << t.rdbuf();
			std::string contents(buffer.str());
			return contents;
		}

		CRAFT_ENGINE_CORE_API void writeString(char* str, int size, const std::string& filename)
		{
			std::ofstream file(filename, std::ios::binary);
			if (!file.is_open())
				throw std::runtime_error("failed to open file!");
			file.write((char*)str, size);
			file.close();
		}

		CRAFT_ENGINE_CORE_API void writeString(const std::string& str, const std::string& filename)
		{
			std::ofstream file(filename, std::ios::binary);
			if (!file.is_open())
				throw std::runtime_error("failed to open file!");
			file.write((char*)str.data(), str.size());
			file.close();
		}

		CRAFT_ENGINE_CORE_API std::vector<std::string> readLines(const std::string& filename)
		{
			std::ifstream t(filename);
			std::string l;
			std::vector<std::string> lines;
			while (std::getline(t, l))
				lines.emplace_back(l);
			return lines;
		}

		CRAFT_ENGINE_CORE_API std::vector<std::string> splitLines(const std::string& string)
		{
			std::stringstream t(string);
			std::string l;
			std::vector<std::string> lines;
			while (std::getline(t, l))
				lines.emplace_back(l);
			return lines;
		}

		CRAFT_ENGINE_CORE_API std::vector<std::string> splitLines(const char* string)
		{
			std::stringstream t(string);
			std::string l;
			std::vector<std::string> lines;
			while (std::getline(t, l))
				lines.emplace_back(l);
			return lines;
		}

		CRAFT_ENGINE_CORE_API std::vector<std::wstring> splitLines(const std::wstring& string)
		{
			std::wstringstream t(string);
			std::wstring l;
			std::vector<std::wstring> lines;
			while (std::getline(t, l))
				lines.emplace_back(l);
			return lines;
		}
		CRAFT_ENGINE_CORE_API std::vector<std::wstring> splitLines(const Char* string)
		{
			std::wstringstream t(string);
			std::wstring l;
			std::vector<std::wstring> lines;
			while (std::getline(t, l))
				lines.emplace_back(l);
			return lines;
		}

		CRAFT_ENGINE_CORE_API void setUtf8()
		{
			std::locale oldlocale = std::locale::global(std::locale("LC_CTYPE=.utf8"));
		}



	}



}



#endif // !CRAFT_ENGINE_CORE_H_


