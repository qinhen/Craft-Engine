#pragma once
#include "../Common.h"

namespace CraftEngine
{
	namespace core
	{



		template<typename FuncType>
		class Callback;
		template<typename ReturnType, typename...Args>
		class Callback<ReturnType(Args...)>
		{
		public:
			typedef ReturnType(FuncType)(Args...);
		private:

			template<int N, int...I>
			struct MemberFunctionBind : MemberFunctionBind<N - 1, N - 1, I...> {};

			template<int...I>
			struct MemberFunctionBind<1, I...>
			{
				template<class CalledType, class FuncPointerType>
				static std::function<FuncType> bind(CalledType&& object, FuncPointerType&& pFunc)
				{
#ifdef _MSC_VER
					return std::bind(std::forward<FuncPointerType>(pFunc), std::forward<CalledType>(object), std::_Ph<I>{}...);
#elif defined __GNUC__
					return std::bind(std::forward<FuncPointerType>(pFunc), std::forward<CalledType>(object), std::_Placeholder<I>{}...);
#endif // _MSC_VER
				}
			};

			static ReturnType DefaultCall(Args...)
			{
				return ReturnType();
			}

			std::function<FuncType> m_callback;
		public:

			Callback() :Callback(&Callback::DefaultCall)
			{
			}

			Callback(const Callback& callback) :m_callback(callback.m_callback)
			{
			}

			Callback(const std::function<FuncType>& func) :m_callback(func)
			{
			}

			template <class CalledType>
			Callback(CalledType* pObject, ReturnType(CalledType::* pFunc)(Args...)) : m_callback(MemberFunctionBind<sizeof...(Args) + 1>::bind(pObject, pFunc))
			{
			}

			template <class CalledType>
			Callback(const CalledType* pObject, ReturnType(CalledType::* pFunc)(Args...)const) : m_callback(MemberFunctionBind<sizeof...(Args) + 1>::bind(pObject, pFunc))
			{
			}

			Callback& operator=(const Callback& callback)
			{
				m_callback = callback.m_callback;
				return *this;
			}

			void bind(const std::function<FuncType>& func)
			{
				m_callback = func;
			}

			void bind(std::function<FuncType>&& func)
			{
				m_callback = func;
			}

			template <class CalledType>
			void bind(CalledType* pObject, ReturnType(CalledType::* pFunc)(Args...))
			{
				m_callback = MemberFunctionBind<sizeof...(Args) + 1>::bind(pObject, pFunc);
			}

			template <class CalledType>
			void bind(const CalledType* pObject, ReturnType(CalledType::* pFunc)(Args...)const)
			{
				m_callback = MemberFunctionBind<sizeof...(Args) + 1>::bind(pObject, pFunc); // std::_Ph从1号开始, 因为0号参数被this指针占用
			}

			ReturnType call(Args... args)const
			{
				return m_callback(std::forward<Args>(args)...);
			}

			ReturnType operator()(Args... args)const
			{
				return m_callback(std::forward<Args>(args)...);
			}
		};




		template<typename ReturnType = void>
		class Command
		{
		private:
			std::function<ReturnType()> m_command;

			static ReturnType _DefaultCall()
			{
				return ReturnType();
			}
		public:

			Command& operator=(const Command& command)
			{
				m_command = command.m_command;
				return *this;
			}

			Command(const Command& command) :m_command(command.m_command)
			{

			}

			Command(const std::function<ReturnType()>& function) :m_command(function)
			{

			}

			Command() :m_command(_DefaultCall)
			{

			}

			template<typename Func, typename... Args, class = typename std::enable_if<!std::is_member_function_pointer<Func>::value>::type>
			void wrap(Func&& func, Args&& ...args)
			{
				m_command = [=] { return func(args...); };
			}

			template<typename Type, typename ...DArgs, typename ...Args>
			void wrap(Type* pObject, ReturnType(Type::* pFunc)(DArgs...), Args&&...args)
			{
				m_command = [=] { return (*pObject.*pFunc)(args...); };
			}

			template<typename TypeA, typename TypeB, typename ...DArgs, typename ...Args>
			void wrap(const TypeB* pObject, ReturnType(TypeA::* pFunc)(DArgs...)const, Args&&...args)
			{
				m_command = [=] { return (*pObject.*pFunc)(args...); };
			}


			template<typename Func, typename... Args, class = typename std::enable_if<!std::is_member_function_pointer<Func>::value>::type>
			void wrap_ref(Func&& func, Args&& ...args)
			{
				m_command = [&] { return func(args...); };
			}

			template<typename Type, typename ...DArgs, typename ...Args>
			void wrap_ref(Type* pObject, ReturnType(Type::* pFunc)(DArgs...), Args&&...args)
			{
				m_command = [&, pFunc] { return (*pObject.*pFunc)(args...); };
			}

			template<typename TypeA, typename TypeB, typename ...DArgs, typename ...Args>
			void wrap_ref(const TypeB* pObject, ReturnType(TypeA::* pFunc)(DArgs...)const, Args&&...args)
			{
				m_command = [&, pFunc] { return (*pObject.*pFunc)(args...); };
			}

			ReturnType execute()
			{
				return m_command();
			}
		};



	}
}