#pragma once
#ifndef CRAFT_ENGINE_CORE_REFLECT_H_
#define CRAFT_ENGINE_CORE_REFLECT_H_

#include "../Common.h"

#include <string_view>                 // Repository: https://github.com/Ubpa/USRefl
#include <tuple>                       // License: https://github.com/Ubpa/USRefl/blob/master/LICENSE
#include <type_traits>

namespace CraftEngine
{

	namespace core
	{

		namespace reflect
		{

			namespace detail
			{
				template<typename L, typename F> constexpr size_t FindIf(L, F&&, std::index_sequence<>) { return -1; }
				template<typename L, typename F, size_t N0, size_t... Ns>
				constexpr size_t FindIf(L l, F&& f, std::index_sequence<N0, Ns...>)
				{
					return f(l.template GetByIdx<N0>()) ? N0 : FindIf(l, std::forward<F>(f), std::index_sequence<Ns...>{});
				}
				template<typename L, typename F, typename R>
				constexpr auto Acc(L, F&&, R&& r, std::index_sequence<>) { return r; }
				template<bool m0, bool... ms, typename L, typename F, typename R, size_t N0, size_t... Ns>
				constexpr auto Acc(L l, F&& f, R&& r, std::index_sequence<N0, Ns...>) {
					if constexpr (!m0) return Acc<ms...>(l, std::forward<F>(f), std::forward<R>(r), std::index_sequence<Ns...>{});
					else return Acc<ms...>(l, std::forward<F>(f), f(std::forward<R>(r), l.template GetByIdx<N0>()), std::index_sequence<Ns...>{});
				}
				template<typename L, typename F, typename R, size_t N0, size_t... Ns>
				constexpr auto Acc(L l, F&& f, R&& r, std::index_sequence<N0, Ns...>)
				{
					return Acc(l, std::forward<F>(f), f(std::forward<R>(r), l.template GetByIdx<N0>()), std::index_sequence<Ns...>{});
				}
				template<typename MemPtrType>
				struct member_pointer_class;
				template<typename Class, typename Value>
				struct member_pointer_class<Value Class::*> { typedef Class class_type; };
				template<typename MemPtrType>
				struct member_pointer_value;
				template<typename Class, typename Value>
				struct member_pointer_value<Value Class::*> { typedef Value value_type; };

				static std::vector<std::string_view> ParseFieldChain(const std::string_view& fieldChain)
				{
					std::vector<std::string_view> parsedFieldChain;
					parsedFieldChain.reserve(10);
					int i = 0;
					while (i < fieldChain.size()) {
						auto pos = fieldChain.find('.', i);
						if (pos == fieldChain.npos) {
							parsedFieldChain.emplace_back(std::string_view(fieldChain.data() + i, fieldChain.size() - i));
							break;
						}
						parsedFieldChain.emplace_back(std::string_view(fieldChain.data() + i, pos - i));
						i = pos + 1;
					}
					return parsedFieldChain;
				}

				template<typename T>
				struct has_member_index_access
				{
				private:
					template<typename U>
					static auto Check(int) -> decltype(std::declval<U>()[0], std::true_type());
					template<typename U>
					static std::false_type Check(...);
				public:

					static constexpr bool value = std::is_same<decltype(Check<T>(0)), std::true_type>::value;
				};

				template<typename T> struct NamedValue { // named value
					std::string_view name; T value; static constexpr bool has_value = true;
					template<typename U>constexpr bool operator==(U v)const { if constexpr (std::is_same_v<T, U>)return value == v; else return false; }
				};
				template<> struct NamedValue<void> {
					std::string_view name; /*T value;*/ static constexpr bool has_value = false;
					template<typename U> constexpr bool operator==(U) const { return false; }
				};
				template<typename...Es> struct ElemList { // Es is a named value
					std::tuple<Es...> elems; static constexpr size_t size = sizeof...(Es);
					constexpr ElemList(Es... elems) : elems{ elems... } {}
					template<bool... ms, typename Init, typename Func> constexpr auto Accumulate(Init&& init, Func&& func) const
					{
						return detail::Acc<ms...>(*this, std::forward<Func>(func), std::forward<Init>(init), std::make_index_sequence<size>{});
					}
					template<bool... ms, typename Func> constexpr void ForEach(Func&& func) const
					{
						Accumulate<ms...>(0, [&](auto, auto field) {std::forward<Func>(func)(field); return 0; });
					}
					template<typename Func> constexpr size_t FindIf(Func&& func) const
					{
						return detail::FindIf(*this, std::forward<Func>(func), std::make_index_sequence<sizeof...(Es)>{});
					}
					constexpr size_t FindIdx(std::string_view n) const { return FindIf([n](auto e) {return e.name == n; }); }
					template<typename T> constexpr size_t FindValue(T v) const { return FindIf([v](auto e) { return e == v; }); }
					constexpr bool Contains(std::string_view name) const { return FindIdx(name) != static_cast<size_t>(-1); }
					template<typename E>constexpr auto Push(E e)const { return std::apply([e](auto...es) {return ElemList<Es..., E>{es..., e}; }, elems); }
					template<typename E>constexpr auto Insert(E e)const { if constexpr ((std::is_same_v<Es, E> || ...))return*this; else return Push(e); }
					template<size_t N> constexpr auto GetByIdx() const { return std::get<N>(elems); }
				};
			}

			template<typename T>
			struct Field : detail::NamedValue<T>, detail::member_pointer_class<T>, detail::member_pointer_value<T>
			{
			public:
				using mem_ptr_type = T;
				constexpr Field(std::string_view n, T v) : detail::NamedValue<T>{ n,v } {}
				template<typename U> constexpr auto& VarOf(U& obj) const { return obj.*(detail::NamedValue<T>::value); }
			};

			template<typename...Fs>
			struct FieldList :detail::ElemList<Fs...>
			{
				constexpr FieldList(Fs...fs) :detail::ElemList<Fs...>{ fs... } {}
			};
			template<typename T, class = typename std::enable_if_t<std::is_same_v<std::decay_t<T>, T>, T>> struct TypeInfo; // TypeInfoBase, name, fields, attrs


			template<typename T> struct TypeInfoBase {
				using type = T;

				template<size_t N, typename U> static constexpr auto& getVar(U& obj) {
					constexpr auto field = TypeInfo<type>::fields.GetByIdx<N>();
					return obj.*(field.value);
				}

				template<size_t N, typename U> static constexpr const auto& getVar(const U& obj) {
					constexpr auto field = TypeInfo<type>::fields.GetByIdx<N>();
					return obj.*(field.value);
				}

				template<size_t N> static constexpr auto getField() {
					return TypeInfo<type>::fields.GetByIdx<N>();
				}

				static constexpr auto getField(std::string_view name) {
					return TypeInfo<type>::fields.FindIdx(name);
				}

				template <typename VarType, typename U>
				static VarType& getVarDirectly(U& obj, const std::string name)
				{
					auto offset = _Get_Offset_By_Name<VarType>(name, std::make_index_sequence<TypeInfo<type>::fields.size>{});
					if (offset == -1)
						throw 0;
					return *((VarType*)((char*)(&obj) + offset));
				}

				template <typename VarType, typename U>
				static VarType& getVar(U& obj, const std::string_view& fieldChain)
				{
					auto&& parsedFieldChain = detail::ParseFieldChain(fieldChain);
					if (parsedFieldChain.size() == 0)
						throw 0;
					return TypeInfo<std::decay<type>::type>::template _DFS_Get_Var_By_Name<VarType>(obj, parsedFieldChain, 0, std::make_index_sequence<TypeInfo<std::decay<type>::type>::fields.size>{});
				}
			public:

				template <typename FieldType, typename U, size_t N, size_t... Ns>
				static FieldType& _DFS_Get_Var_By_Name(U& obj, const std::vector<std::string_view>& fieldChain, int idx, std::index_sequence<N, Ns...>)
				{
					constexpr auto field = TypeInfo<std::decay<U>::type>::fields.GetByIdx<N>();
					if constexpr (detail::has_member_index_access<std::decay<U>::type>::value) {
						if (fieldChain[idx].front() == '[' && fieldChain[idx].back() == ']' & idx <= fieldChain.size())
						{
							auto& subobj = obj[std::atoi(std::string(fieldChain[idx].substr(1, fieldChain[idx].size() - 2)).c_str())];
							return TypeInfo<std::decay<decltype(subobj)>::type>::template _DFS_Get_Var_By_Name<FieldType>(subobj, fieldChain, idx + 1, std::make_index_sequence<TypeInfo<std::decay<decltype(subobj)>::type>::fields.size>{});
						}
					}
					if (TypeInfo<std::decay<U>::type>::template getField<N>().name.data() == fieldChain[idx]) {
						if (idx < fieldChain.size() - 1) {
							auto& subobj = field.VarOf(obj);
							return TypeInfo<decltype(field)::value_type>::template _DFS_Get_Var_By_Name<FieldType>(subobj, fieldChain, idx + 1, std::make_index_sequence<TypeInfo<decltype(field)::value_type>::fields.size>{});
						}
						else if (std::is_same<decltype(field)::value_type, FieldType>::value) {
							return *((FieldType*)((char*)(&obj) + TypeInfo<std::decay<U>::type>::template _Get_Offset_Of(field)));   // Expect type is same as field type
						}
						else {
							return *((FieldType*)nullptr);  // Expect type is different with field type
						}
					}
					return TypeInfo<std::decay<U>::type>::template _DFS_Get_Var_By_Name<FieldType>(obj, fieldChain, idx, std::index_sequence<Ns...>{});
				}
				template <typename FieldType, typename U, size_t N>
				static FieldType& _DFS_Get_Var_By_Name(U& obj, const std::vector<std::string_view>& fieldChain, int idx, std::index_sequence<N>)
				{
					constexpr auto field = TypeInfo<std::decay<U>::type>::fields.GetByIdx<N>();
					if constexpr (detail::has_member_index_access<std::decay<U>::type>::value) {
						if (fieldChain[idx].front() == '[' && fieldChain[idx].back() == ']' & idx <= fieldChain.size())
						{
							auto& subobj = obj[std::atoi(std::string(fieldChain[idx].substr(1, fieldChain[idx].size() - 2)).c_str())];
							return TypeInfo<std::decay<decltype(subobj)>::type>::template _DFS_Get_Var_By_Name<FieldType>(subobj, fieldChain, idx + 1, std::make_index_sequence<TypeInfo<std::decay<decltype(subobj)>::type>::fields.size>{});
						}
					}
					if (TypeInfo<std::decay<U>::type>::template getField<N>().name.data() == fieldChain[idx]) {
						if (idx < fieldChain.size() - 1) {
							auto& subobj = field.VarOf(obj);
							return TypeInfo<decltype(field)::value_type>::template _DFS_Get_Var_By_Name<FieldType>(subobj, fieldChain, idx + 1, std::make_index_sequence<TypeInfo<decltype(field)::value_type>::fields.size>{});
						}
						else if (std::is_same<decltype(field)::value_type, FieldType>::value) {
							return *((FieldType*)((char*)(&obj) + TypeInfo<std::decay<U>::type>::template _Get_Offset_Of(field)));   // Expect type is same as field type
						}
						else {
							return *((FieldType*)nullptr);  // Expect type is different with field type
						}
					}
					return *((FieldType*)nullptr); // No such field named nameChain[idx] in type std::type_index(typeid(type)).name()
				}
				template <typename FieldType, typename U>
				static FieldType& _DFS_Get_Var_By_Name(U& obj, const std::vector<std::string_view>& fieldChain, int idx, std::index_sequence<>)
				{
					if constexpr (detail::has_member_index_access<U>::value) {
						if (fieldChain[idx].front() == '[' && fieldChain[idx].back() == ']' && idx <= fieldChain.size())
						{
							auto& subobj = obj[std::atoi(std::string(fieldChain[idx].substr(1, fieldChain[idx].size() - 2)).c_str())];
							return TypeInfo<std::decay<decltype(subobj)>::type>::template _DFS_Get_Var_By_Name<FieldType>(subobj, fieldChain, idx + 1, std::make_index_sequence<TypeInfo<std::decay<decltype(subobj)>::type>::fields.size>{});
						}
					}
					if constexpr (std::is_same_v<U, FieldType>)
					{
						if (idx == fieldChain.size()) {
							return obj; // Last Field
						}
					}
					return *((FieldType*)nullptr); // Could not find implementation TypeInfo<type> 
				}

				template <typename FieldType, size_t N, size_t... Ns>
				static int32_t _DFS_Get_Offset_By_Name(const std::vector<std::string>& fieldChain, int idx, std::index_sequence<N, Ns...>)
				{
					constexpr auto field = TypeInfo<type>::fields.GetByIdx<N>();
					if (TypeInfo<type>::template getField<N>().name.data() == fieldChain[idx])
						if (idx < fieldChain.size() - 1) {
							int32_t accOffset = TypeInfo<decltype(field)::value_type>::template _DFS_Get_Offset_By_Name<FieldType>(fieldChain, idx + 1, std::make_index_sequence<TypeInfo<decltype(field)::value_type>::fields.size>{});
							if (accOffset == -1)
								return -1; // Error
							return TypeInfo<type>::template _Get_Offset_Of(field) + accOffset;
						}
						else if (std::is_same<decltype(field)::value_type, FieldType>::value) {
							return TypeInfo<type>::template _Get_Offset_Of(field);  // Expect type is same as field type
						}
						else {
							return -1;  // Expect type is different with field type
						}
					return TypeInfo<type>::template _DFS_Get_Offset_By_Name<FieldType>(fieldChain, idx, std::index_sequence<Ns...>{});
				}
				template <typename FieldType, size_t N>
				static int32_t _DFS_Get_Offset_By_Name(const std::vector<std::string>& fieldChain, int idx, std::index_sequence<N>)
				{
					constexpr auto field = TypeInfo<type>::fields.GetByIdx<N>();
					if (TypeInfo<type>::template getField<N>().name.data() == fieldChain[idx])
						if (idx < fieldChain.size() - 1) {
							int32_t accOffset = TypeInfo<decltype(field)::value_type>::template _DFS_Get_Offset_By_Name<FieldType>(fieldChain, idx + 1, std::make_index_sequence<TypeInfo<decltype(field)::value_type>::fields.size>{});
							if (accOffset == -1)
								return -1; // Error
							return TypeInfo<type>::template _Get_Offset_Of(field) + accOffset;
						}
						else if (std::is_same<decltype(field)::value_type, FieldType>::value) {
							return TypeInfo<type>::template _Get_Offset_Of(field);  // Expect type is same as field type
						}
						else {
							return -1;  // Expect type is different with field type
						}
					return -1; // No such field named nameChain[idx] in type std::type_index(typeid(type)).name()
				}
				template <typename FieldType>
				static int32_t _DFS_Get_Offset_By_Name(const std::vector<std::string>& fieldChain, int idx, std::index_sequence<>)
				{
					if (idx == fieldChain.size()) {
						return 0; // Last Field
					}
					else {
						return -1; // Could not find implementation TypeInfo<type> 
					}
				}
			private:
				template <typename FieldType, size_t N, size_t... Ns>
				static int32_t _Get_Offset_By_Name(std::string name, std::index_sequence<N, Ns...>)
				{
					constexpr auto field = TypeInfo<type>::fields.GetByIdx<N>();
					if (std::is_same<decltype(field)::value_type, FieldType>::value)
						if (getField<N>().name.data() == name)
							return _Get_Offset_Of(field);
					return _Get_Offset_By_Name<FieldType, Ns...>(name, std::index_sequence<Ns...>{});
				}
				template <typename FieldType, size_t N>
				static int32_t _Get_Offset_By_Name(std::string name, std::index_sequence<N>)
				{
					constexpr auto field = TypeInfo<type>::fields.GetByIdx<N>();
					if (std::is_same<decltype(field)::value_type, FieldType>::value)
						if (getField<N>().name.data() == name)
							return _Get_Offset_Of(field);
					return -1;
				}
				template<typename Field>
				static int32_t _Get_Offset_Of(const Field& field)
				{
					return (int32_t) & ((*((type*)0)).*(field.value));
				}

			};

			// default implementation
			template<typename T, class = typename std::enable_if_t<std::is_same_v<std::decay_t<T>, T>, T>> struct TypeInfo : public TypeInfoBase<T>
			{
				static constexpr std::string_view name = "undefined";
				static constexpr FieldList fields =
				{
				};
			};

			template<typename T> Field(std::string_view, T)->Field<T>;

			template<typename type, typename U>
			type& dynamicRefl(U& obj, const std::string& fieldChain)
			{
				return TypeInfo<std::decay<U>>:: template getVar<type>(obj, fieldChain);
			}

#define craft_engine_reflect_begin(type_name) \
			template<> struct CraftEngine::core::reflect::TypeInfo<type_name> : CraftEngine::core::reflect::TypeInfoBase<type_name> { \
			static constexpr std::string_view name = #type_name; \
			static constexpr FieldList fields = {

#define craft_engine_reflect_field(field_name) Field{ #field_name, &type::field_name },
#define craft_engine_reflect_map_field(field_name, map_name) Field{ map_name, &type::field_name },

#define craft_engine_reflect_end() }; };

#define _CRAFT_ENGINE_REFLECT_FIELD_0(fn0) 
#define _CRAFT_ENGINE_REFLECT_FIELD_1(fn0) craft_engine_reflect_field(fn0)
#define _CRAFT_ENGINE_REFLECT_FIELD_2(fn0, fn1) _CRAFT_ENGINE_REFLECT_FIELD_1(fn0) _CRAFT_ENGINE_REFLECT_FIELD_1(fn1)
#define _CRAFT_ENGINE_REFLECT_FIELD_3(fn0, fn1, fn2) _CRAFT_ENGINE_REFLECT_FIELD_1(fn0) _CRAFT_ENGINE_REFLECT_FIELD_2(fn1, fn2)
#define _CRAFT_ENGINE_REFLECT_FIELD_4(fn0, fn1, fn2, fn3) _CRAFT_ENGINE_REFLECT_FIELD_1(fn0) _CRAFT_ENGINE_REFLECT_FIELD_3(fn1, fn2, fn3)
#define _CRAFT_ENGINE_REFLECT_FIELD_5(fn0, fn1, fn2, fn3, fn4) _CRAFT_ENGINE_REFLECT_FIELD_1(fn0) _CRAFT_ENGINE_REFLECT_FIELD_4(fn1, fn2, fn3, fn4)
#define _CRAFT_ENGINE_REFLECT_FIELD_6(fn0, fn1, fn2, fn3, fn4, fn5) _CRAFT_ENGINE_REFLECT_FIELD_1(fn0) _CRAFT_ENGINE_REFLECT_FIELD_5(fn1, fn2, fn3, fn4, fn5)
#define _CRAFT_ENGINE_REFLECT_FIELD_7(fn0, fn1, fn2, fn3, fn4, fn5, fn6) _CRAFT_ENGINE_REFLECT_FIELD_1(fn0) _CRAFT_ENGINE_REFLECT_FIELD_6(fn1, fn2, fn3, fn4, fn5, fn6)
#define _CRAFT_ENGINE_REFLECT_FIELD_8(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7) _CRAFT_ENGINE_REFLECT_FIELD_1(fn0) _CRAFT_ENGINE_REFLECT_FIELD_7(fn1, fn2, fn3, fn4, fn5, fn6, fn7)

#define _CRAFT_ENGINE_REFLECT_FIELD_9(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8) _CRAFT_ENGINE_REFLECT_FIELD_8(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7) _CRAFT_ENGINE_REFLECT_FIELD_1(fn8)
#define _CRAFT_ENGINE_REFLECT_FIELD_10(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9) _CRAFT_ENGINE_REFLECT_FIELD_8(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7) _CRAFT_ENGINE_REFLECT_FIELD_2(fn8, fn9)
#define _CRAFT_ENGINE_REFLECT_FIELD_11(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA) _CRAFT_ENGINE_REFLECT_FIELD_8(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7) _CRAFT_ENGINE_REFLECT_FIELD_3(fn8, fn9, fnA)
#define _CRAFT_ENGINE_REFLECT_FIELD_12(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB) _CRAFT_ENGINE_REFLECT_FIELD_8(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7) _CRAFT_ENGINE_REFLECT_FIELD_4(fn8, fn9, fnA, fnB)
#define _CRAFT_ENGINE_REFLECT_FIELD_13(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC) _CRAFT_ENGINE_REFLECT_FIELD_8(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7) _CRAFT_ENGINE_REFLECT_FIELD_5(fn8, fn9, fnA, fnB, fnC)
#define _CRAFT_ENGINE_REFLECT_FIELD_14(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD) _CRAFT_ENGINE_REFLECT_FIELD_8(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7) _CRAFT_ENGINE_REFLECT_FIELD_6(fn8, fn9, fnA, fnB, fnC, fnD)
#define _CRAFT_ENGINE_REFLECT_FIELD_15(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE) _CRAFT_ENGINE_REFLECT_FIELD_8(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7) _CRAFT_ENGINE_REFLECT_FIELD_7(fn8, fn9, fnA, fnB, fnC, fnD, fnE)
#define _CRAFT_ENGINE_REFLECT_FIELD_16(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF) _CRAFT_ENGINE_REFLECT_FIELD_8(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7) _CRAFT_ENGINE_REFLECT_FIELD_8(fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF)

#define _CRAFT_ENGINE_REFLECT_FIELD_17(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10) \
		_CRAFT_ENGINE_REFLECT_FIELD_16(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF) _CRAFT_ENGINE_REFLECT_FIELD_1(fn10)
#define _CRAFT_ENGINE_REFLECT_FIELD_18(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11) \
		_CRAFT_ENGINE_REFLECT_FIELD_16(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF) _CRAFT_ENGINE_REFLECT_FIELD_2(fn10, fn11)
#define _CRAFT_ENGINE_REFLECT_FIELD_19(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12) \
		_CRAFT_ENGINE_REFLECT_FIELD_16(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF) _CRAFT_ENGINE_REFLECT_FIELD_3(fn10, fn11, fn12)
#define _CRAFT_ENGINE_REFLECT_FIELD_20(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12, fn13) \
		_CRAFT_ENGINE_REFLECT_FIELD_16(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF) _CRAFT_ENGINE_REFLECT_FIELD_4(fn10, fn11, fn12, fn13)
#define _CRAFT_ENGINE_REFLECT_FIELD_21(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12, fn13, fn14) \
		_CRAFT_ENGINE_REFLECT_FIELD_16(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF) _CRAFT_ENGINE_REFLECT_FIELD_5(fn10, fn11, fn12, fn13, fn14)
#define _CRAFT_ENGINE_REFLECT_FIELD_22(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12, fn13, fn14, fn15) \
		_CRAFT_ENGINE_REFLECT_FIELD_16(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF) _CRAFT_ENGINE_REFLECT_FIELD_6(fn10, fn11, fn12, fn13, fn14, fn15)
#define _CRAFT_ENGINE_REFLECT_FIELD_23(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12, fn13, fn14, fn15, fn16) \
		_CRAFT_ENGINE_REFLECT_FIELD_16(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF) _CRAFT_ENGINE_REFLECT_FIELD_7(fn10, fn11, fn12, fn13, fn14, fn15, fn16)
#define _CRAFT_ENGINE_REFLECT_FIELD_24(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12, fn13, fn14, fn15, fn16, fn17) \
		_CRAFT_ENGINE_REFLECT_FIELD_16(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF) _CRAFT_ENGINE_REFLECT_FIELD_8(fn10, fn11, fn12, fn13, fn14, fn15, fn16, fn17)

#define _CRAFT_ENGINE_REFLECT_FIELD_N(c, fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12, fn13, fn14, fn15, fn16, fn17) \
				_CRAFT_ENGINE_REFLECT_FIELD_##c(fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12, fn13, fn14, fn15, fn16, fn17) 

#define craft_engine_reflect_field_n _CRAFT_ENGINE_REFLECT_FIELD_N

#define craft_engine_reflect(c, tn, fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12, fn13, fn14, fn15, fn16, fn17) \
			craft_engine_reflect_begin(tn) craft_engine_reflect_field_n(c, fn0, fn1, fn2, fn3, fn4, fn5, fn6, fn7, fn8, fn9, fnA, fnB, fnC, fnD, fnE, fnF, fn10, fn11, fn12, fn13, fn14, fn15, fn16, fn17) craft_engine_reflect_end()
		}



	}



}



#endif // !CRAFT_ENGINE_CORE_REFLECT_H_


