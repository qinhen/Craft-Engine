#pragma once
#ifndef CRAFT_ENGINE_MATH_H_
#define CRAFT_ENGINE_MATH_H_

#include "../Common.h"
#include <assert.h>
#include <cmath>
#include <limits>



#ifdef max
#undef max
#endif 
#ifdef min
#undef min
#endif 


//#define CRAFT_ENGINE_USING_WORLD_SPACE_RIGHT_HAND


#define CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_LH (1 << 0) // vulkan/direct3d
#define CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_RH (1 << 1) // opengl
#define CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_LH (1 << 2) // opengl
#define CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_RH (1 << 3) // vulkan/direct3d

#ifdef CRAFT_ENGINE_USING_WORLD_SPACE_RIGHT_HAND
#	ifdef CRAFT_ENGINE_USING_DEVICE_SPACE_LEFT_HAND
#		define CRAFT_ENGINE_MATH_CLIP_CONTROL (CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_RH | CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_LH) // openGL
#	else 
#		define CRAFT_ENGINE_MATH_CLIP_CONTROL (CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_RH | CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_RH) // 
#	endif
#else
#	ifdef CRAFT_ENGINE_USING_DEVICE_SPACE_LEFT_HAND
#		define CRAFT_ENGINE_MATH_CLIP_CONTROL (CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_LH | CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_LH) //
#	else 
#		define CRAFT_ENGINE_MATH_CLIP_CONTROL (CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_LH | CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_RH) // vulkan
#	endif
#endif


#define CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_INLINE

namespace CraftEngine
{
	/*
	 math
	*/
	namespace math
	{

		// Math Constants
		// * pi
		template<typename genType = float>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR genType pi() { return static_cast<genType>(3.1415926535897932384626433832795); }
		// * 2*pi
		template<typename genType = float>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR genType two_pi() { return static_cast<genType>(6.28318530717958647692528676655900576); }
		// * pi/2
		template<typename genType = float>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR genType half_pi() { return static_cast<genType>(1.57079632679489661923132169163975144); }
		// * pi/4
		template<typename genType = float>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR genType quarter_pi() { return static_cast<genType>(0.785398163397448309615660845819875721); }
		// * pi的倒数
		template<typename genType = float>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR genType one_over_pi() { return static_cast<genType>(0.318309886183790671537767526745028724); }
		// * 自然常数
		template<typename genType = float>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR genType e() { return static_cast<genType>(2.71828182845904523536); }
		// * 黄金分割率
		template<typename genType = float>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR genType golden_ratio() { return static_cast<genType>(1.61803398874989484820458683436563811); }

		template<typename genType = float>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR genType epsilon() { return std::numeric_limits<genType>::epsilon(); }
		//// Physics Constants
		//template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType AvogadroConstant() { return static_cast<genType>(6.0221367E-23); }
		//template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType GasConstant() { return static_cast<genType>(8.314472); }
		//template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType PlanckConstant() { return static_cast<genType>(6.63e-34); }
		//template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType BoltzmannConstant() { return static_cast<genType>(1.3806485279E-23); }
		//template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType Permittivity() { return static_cast<genType>(8.854187817E-12); }
		//template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType ElectronMass() { return static_cast<genType>(9.10956E-31); }
		//template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType ElectronCharge() { return static_cast<genType>(1.602176634E-19); }


		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType min(genType x, genType y) CRAFT_ENGINE_NOEXCEPT { return x < y ? x : y; }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType max(genType x, genType y) CRAFT_ENGINE_NOEXCEPT { return x > y ? x : y; }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType sum(genType x, genType y) CRAFT_ENGINE_NOEXCEPT { return x + y; }
		template<typename genType, typename...Args, class = typename std::enable_if_t<(sizeof...(Args) > 1)>> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType min(genType arg, Args...args) CRAFT_ENGINE_NOEXCEPT { return min<genType>(arg, min<genType>(args...)); }
		template<typename genType, typename...Args, class = typename std::enable_if_t<(sizeof...(Args) > 1)>> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType max(genType arg, Args...args) CRAFT_ENGINE_NOEXCEPT { return max<genType>(arg, max<genType>(args...)); }
		template<typename genType, typename...Args, class = typename std::enable_if_t<(sizeof...(Args) > 1)>> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType sum(genType arg, Args...args) CRAFT_ENGINE_NOEXCEPT { return sum<genType>(arg, sum<genType>(args...)); }
		template<typename genType, typename...Args> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType average(genType arg, Args...args) CRAFT_ENGINE_NOEXCEPT { return sum(arg, args...) / (sizeof...(args) + 1); }


		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType sign(genType x) CRAFT_ENGINE_NOEXCEPT 
		{ return x > 0 ? static_cast<genType>(1) : x < 0 ? static_cast<genType>(-1) : static_cast<genType>(0); }


		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType tan(genType x) CRAFT_ENGINE_NOEXCEPT { return std::tan(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType cos(genType x) CRAFT_ENGINE_NOEXCEPT { return std::cos(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType sin(genType x) CRAFT_ENGINE_NOEXCEPT { return std::sin(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType atan(genType x) CRAFT_ENGINE_NOEXCEPT { return std::atan(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType acos(genType x) CRAFT_ENGINE_NOEXCEPT { return std::acos(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType asin(genType x) CRAFT_ENGINE_NOEXCEPT { return std::asin(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType exp(genType x) CRAFT_ENGINE_NOEXCEPT { return std::exp(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType log(genType x) CRAFT_ENGINE_NOEXCEPT { return std::log(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType log2(genType x) CRAFT_ENGINE_NOEXCEPT { return std::log2(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType pow(genType x, genType y) CRAFT_ENGINE_NOEXCEPT { return std::pow(x, y); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType atan2(genType x, genType y) CRAFT_ENGINE_NOEXCEPT { return std::atan2(x, y); }

		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType sqrt(genType x) CRAFT_ENGINE_NOEXCEPT { return std::sqrt(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType clamp(genType x, genType minVal, genType maxVal) CRAFT_ENGINE_NOEXCEPT { return min(max(x, minVal), maxVal); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType floor(genType x) CRAFT_ENGINE_NOEXCEPT { return std::floor(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType trunc(genType x) CRAFT_ENGINE_NOEXCEPT { return std::trunc(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType ceil(genType x) CRAFT_ENGINE_NOEXCEPT { return std::ceil(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType fract(genType x) CRAFT_ENGINE_NOEXCEPT { return x - math::trunc(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType round(genType x) CRAFT_ENGINE_NOEXCEPT { return std::round(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType mod(genType a, genType b) CRAFT_ENGINE_NOEXCEPT { return a - b * math::floor(a / b); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType abs(genType x) CRAFT_ENGINE_NOEXCEPT { return std::abs(x); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType inverse(genType x) CRAFT_ENGINE_NOEXCEPT { /*assert(x != static_cast<genType>(0));*/ return static_cast<genType>(1) / x; }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType radians(genType degree) CRAFT_ENGINE_NOEXCEPT { return degree * (math::pi() / static_cast<genType>(180)); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType degrees(genType radian) CRAFT_ENGINE_NOEXCEPT { return radian * (static_cast<genType>(180) / math::pi()); }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType lerp(genType x, genType y, genType t) CRAFT_ENGINE_NOEXCEPT { return x * (static_cast<genType>(1) - t) + y * t; }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType slerp(genType x, genType y, genType t) CRAFT_ENGINE_NOEXCEPT { auto tt = 3 * t * t - 2 * t * t * t; return x * (static_cast<genType>(1) - tt) + y * tt; }
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType mix(genType x, genType y, genType t) CRAFT_ENGINE_NOEXCEPT { return x * (static_cast<genType>(1) - t) + y * t; }

		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType step(genType x, genType y, genType t) CRAFT_ENGINE_NOEXCEPT
		{ 
			if (t < x) return static_cast<genType>(0);
			else if (t > y)return static_cast<genType>(1);
			else {
				genType f = (t - x) / (y - x);
				return clamp(f, genType(0), genType(1));
			}
		}
		template<typename genType> CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR  genType smoothstep(genType x, genType y, genType t) CRAFT_ENGINE_NOEXCEPT
		{ 
			if (t < x) return static_cast<genType>(0);
			else if (t > y)return static_cast<genType>(1);
			else {
				genType f = (t - x) / (y - x);
				return clamp(3 * f * f - 2 * f * f * f, genType(0), genType(1));
			}
		}


		typedef int32_t length_t;
		template<length_t Len, typename Type> struct Vector;
		template<length_t Col, length_t Row, typename Type> struct Matrix;

		namespace _Math_Detail
		{
			template<template<length_t Len, typename Type> class Vector, length_t Len, typename Type>
			struct functor1 {};
			template<template<length_t Len, typename Type> class Vector, typename Type>
			struct functor1<Vector, 2, Type> {
				CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> static call(Type(*f)(Type), Vector<2, Type> const& v) {
					return Vector<2, Type>(f(v[0]), f(v[1]));
				}
			};
			template<template<length_t Len, typename Type> class Vector, typename Type>
			struct functor1<Vector, 3, Type> {
				CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> static call(Type(*f)(Type), Vector<3, Type> const& v) {
					return Vector<3, Type>(f(v[0]), f(v[1]), f(v[2]));
				}
			};
			template<template<length_t Len, typename Type> class Vector, typename Type>
			struct functor1<Vector, 4, Type> {
				CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<4, Type> static call(Type(*f)(Type), Vector<4, Type> const& v) {
					return Vector<4, Type>(f(v[0]), f(v[1]), f(v[2]), f(v[3]));
				}
			};
			template<template<length_t Len, typename Type> class Vector, length_t Len, typename Type>
			struct functor2 {};
			template<template<length_t Len, typename Type> class Vector, typename Type>
			struct functor2<Vector, 2, Type> {
				CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR static Vector<2, Type> call(Type(*f)(Type, Type), Vector<2, Type> const& a, Vector<2, Type> const& b) {
					return Vector<2, Type>(f(a[0], b[0]), f(a[1], b[1]));
				}
			};
			template<template<length_t Len, typename Type> class Vector, typename Type>
			struct functor2<Vector, 3, Type> {
				CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR static Vector<3, Type> call(Type(*f)(Type, Type), Vector<3, Type> const& a, Vector<3, Type> const& b) {
					return Vector<3, Type>(f(a[0], b[0]), f(a[1], b[1]), f(a[2], b[2]));
				}
			};
			template<template<length_t Len, typename Type> class Vector, typename Type>
			struct functor2<Vector, 4, Type> {
				CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR static Vector<4, Type> call(Type(*f)(Type, Type), Vector<4, Type> const& a, Vector<4, Type> const& b) {
					return Vector<4, Type>(f(a[0], b[0]), f(a[1], b[1]), f(a[2], b[2]), f(a[3], b[3]));
				}
			};
			template<template<length_t Len, typename Type> class Vector, length_t Len, typename Type>
			struct functor3 {};
			template<template<length_t Len, typename Type> class Vector, typename Type>
			struct functor3<Vector, 2, Type> {
				CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR static Vector<2, Type> call(Type(*f)(Type, Type, Type), Vector<2, Type> const& a, Vector<2, Type> const& b, Vector<2, Type> const& c) {
					return Vector<2, Type>(f(a[0], b[0], c[0]), f(a[1], b[1], c[1]));
				}
			};
			template<template<length_t Len, typename Type> class Vector, typename Type>
			struct functor3<Vector, 3, Type> {
				CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR static Vector<3, Type> call(Type(*f)(Type, Type, Type), Vector<3, Type> const& a, Vector<3, Type> const& b, Vector<3, Type> const& c) {
					return Vector<3, Type>(f(a[0], b[0], c[0]), f(a[1], b[1], c[1]), f(a[2], b[2], c[2]));
				}
			};
			template<template<length_t Len, typename Type> class Vector, typename Type>
			struct functor3<Vector, 4, Type> {
				CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR static Vector<4, Type> call(Type(*f)(Type, Type, Type), Vector<4, Type> const& a, Vector<4, Type> const& b, Vector<4, Type> const& c) {
					return Vector<4, Type>(f(a[0], b[0], c[0]), f(a[1], b[1], c[1]), f(a[2], b[2], c[2]), f(a[3], b[3], c[3]));
				}
			};
		}


		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> min(const Vector<Len, Type>& a, const Vector<Len, Type>& b) CRAFT_ENGINE_NOEXCEPT { return _Math_Detail::functor2<Vector, Len, Type>::call(math::min, a, b); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> max(const Vector<Len, Type>& a, const Vector<Len, Type>& b) CRAFT_ENGINE_NOEXCEPT { return _Math_Detail::functor2<Vector, Len, Type>::call(math::max, a, b); }
		template<length_t Len, typename Type, typename...Args> 
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> min(const Vector<Len, Type>& arg, Args...args) CRAFT_ENGINE_NOEXCEPT { return min<Len, Type>(arg, min<Len, Type>(args...)); }
		template<length_t Len, typename Type, typename...Args>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> max(const Vector<Len, Type>& arg, Args...args) CRAFT_ENGINE_NOEXCEPT { return max<Len, Type>(arg, max<Len, Type>(args...)); }

		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> atan(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::atan, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> asin(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::asin, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> acos(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::acos, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> tan(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::tan, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> sin(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::sin, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> cos(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::cos, v); }

		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> exp(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::exp, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> log(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::log, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> log2(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::log2, v); }

		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> pow(const Vector<Len, Type>& a, const Vector<Len, Type>& b) { return _Math_Detail::functor2<Vector, Len, Type>::call(math::pow, a, b); }

		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> clamp(const Vector<Len, Type>& x, const Vector<Len, Type>& minVal, const Vector<Len, Type>& maxVal) 
		{ return _Math_Detail::functor3<Vector, Len, Type>::call(math::clamp, x, minVal, maxVal); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> floor(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::floor, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> trunc(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::trunc, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> ceil(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::ceil, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> fract(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::fract, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> round(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::round, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> abs(const Vector<Len, Type>& v) { return _Math_Detail::functor1<Vector, Len, Type>::call(math::abs, v); }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> mod(const Vector<Len, Type>& a, const Vector<Len, Type>& b) { return a - b * math::floor(a / b); }

		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> lerp(const Vector<Len, Type>& a, const Vector<Len, Type>& b, Type t) { return a * (static_cast<Type>(1) - t) + b * t; }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> slerp(const Vector<Len, Type>& a, const Vector<Len, Type>& b, Type t) { auto tt = 3 * t * t - 2 * t * t * t; return a * (static_cast<Type>(1) - tt) + b * tt; }
		template<length_t Len, typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<Len, Type> mix(const Vector<Len, Type>& a, const Vector<Len, Type>& b, Type t) { return a * (static_cast<Type>(1) - t) + b * t; }


		template<typename Type>
		struct Vector<2, Type>
		{
			typedef Type                value_t;
			typedef Vector<2, Type>     vec2_t;

			union {
				struct { Type x, y; };
				struct { Type r, g; };
				struct { Type s, t; };
				Type value[2];
			};
			//Vector() {}
#ifdef CRAFT_ENGINE_MATH_HAVING_INITIALIZE_VALUE
			CRAFT_ENGINE_CONSTEXPR Vector() :x(static_cast<Type>(0)), y(static_cast<Type>(0)) {}
#else
			CRAFT_ENGINE_CONSTEXPR Vector() {};
#endif
			CRAFT_ENGINE_CONSTEXPR Vector(Type Xx, Type Yy) :x(Xx), y(Yy) {}
			template<typename A, typename B> CRAFT_ENGINE_CONSTEXPR Vector(A Xx, B Yy)
				: x(static_cast<Type>(Xx)), y(static_cast<Type>(Yy)) {}
			CRAFT_ENGINE_CONSTEXPR Vector(Vector<2, Type> const& v) :x(v.x), y(v.y) {}
			template<typename U> CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Vector(Vector<2, U> const& v) : x(v.x), y(v.y) {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Vector(Type v) : x(v), y(v) {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Vector(Vector<3, Type> const& v) : x(v.x), y(v.y) {}
			CRAFT_ENGINE_CONSTEXPR Vector<2, Type>& operator=(Vector<2, Type> const& v) { x = v.x; y = v.y; return *this; }

			Type& operator[](uint32_t index) { return value[index]; }
			Type const& operator[](uint32_t index)const { return value[index]; }

			Vector<2, Type>& operator+=(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x += v.x; y += v.y; return *this; }
			Vector<2, Type>& operator-=(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x -= v.x; y -= v.y; return *this; }
			Vector<2, Type>& operator*=(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x *= v.x; y *= v.y; return *this; }
			Vector<2, Type>& operator/=(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x /= v.x; y /= v.y; return *this; }
			Vector<2, Type>& operator%=(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x %= v.x; y %= v.y; return *this; }
			Vector<2, Type>& operator+=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x += scalar; y += scalar; return *this; }
			Vector<2, Type>& operator-=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x -= scalar; y -= scalar; return *this; }
			Vector<2, Type>& operator*=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x *= scalar; y *= scalar; return *this; }
			Vector<2, Type>& operator/=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { auto it = inverse(scalar);  return (*this) *= it; }
			Vector<2, Type>  operator-()const { return Vector<2, Type>(-x, -y); }
			bool             operator==(const Vector<2, Type>& v)const { return x == v.x && y == v.y; }
			bool             operator==(Type val)const { return x == val && y == val; }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator+(Vector<2, Type> const& v1, Vector<2, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v1.x + v2.x, v1.y + v2.y); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator-(Vector<2, Type> const& v1, Vector<2, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v1.x - v2.x, v1.y - v2.y); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator*(Vector<2, Type> const& v1, Vector<2, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v1.x * v2.x, v1.y * v2.y); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator/(Vector<2, Type> const& v1, Vector<2, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v1.x / v2.x, v1.y / v2.y); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator%(Vector<2, Type> const& v1, Vector<2, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v1.x % v2.x, v1.y % v2.y); }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator+(Type scalar, Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v.x + scalar, v.y + scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator-(Type scalar, Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(scalar - v.x, scalar - v.y); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator*(Type scalar, Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v.x * scalar, v.y * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator/(Type scalar, Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(scalar / v.x, scalar / v.y); }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator+(Vector<2, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v.x + scalar, v.y + scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator-(Vector<2, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v.x - scalar, v.y - scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator*(Vector<2, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v.x * scalar, v.y * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<2, Type> operator/(Vector<2, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(v.x / scalar, v.y / scalar); }

			Type length()const CRAFT_ENGINE_NOEXCEPT { return std::sqrt(length2()); }
			Type length2()const CRAFT_ENGINE_NOEXCEPT { return x * x + y * y; }
			Vector<2, Type> normalize()const CRAFT_ENGINE_NOEXCEPT { auto il = inverse(length()); return *this * il; }
			Vector<2, Type> mul(Vector<2, Type> const& v)const CRAFT_ENGINE_NOEXCEPT { return *this * v; }

			Vector<2, Type> pow(Vector<2, Type> const& v)const CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(std::pow(x, v.x), std::pow(y, v.y)); }
			Vector<2, Type> exp()const CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(std::exp(x), std::exp(y)); }
			Vector<2, Type> log()const CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(std::log(x), std::log(y)); }
			Vector<2, Type> log2()const CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(std::log2(x), std::log2(y)); }
			Vector<2, Type> tan()const CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(std::tan(x), std::tan(y)); }
			Vector<2, Type> cos()const CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(std::cos(x), std::cos(y)); }
			Vector<2, Type> sin()const CRAFT_ENGINE_NOEXCEPT { return Vector<2, Type>(std::sin(x), std::sin(y)); }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type dot(Vector<2, Type> const& v)const
			{
				return x * v.x + y * v.y;
			}

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type cross(Vector<2, Type> const& v)const CRAFT_ENGINE_NOEXCEPT
			{
				return x * v.y - y * v.x;
			}
		};

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type distance(Vector<2, Type> const& v1, Vector<2, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return (v1 - v2).length(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type length(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return v.length(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type length2(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return v.length2(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> normalize(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return v.normalize(); }

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type dot(Vector<2, Type> const& v1, Vector<2, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return v1.dot(v2); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type cross(Vector<2, Type> const& v1, Vector<2, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return v1.cross(v2); }



		template<typename Type>
		struct Vector<3, Type>
		{
			typedef Type                value_t;
			typedef Vector<3, Type>     vec3_t;

			union {
				struct { Type x, y, z; };
				struct { Type r, g, b; };
				struct { Type s, t, p; };
				Type value[3];
				Vector<2, Type> xy;
				Vector<2, Type> rg;
			};
			//Vector() {}
#ifdef CRAFT_ENGINE_MATH_HAVING_INITIALIZE_VALUE
			CRAFT_ENGINE_CONSTEXPR Vector() :x(static_cast<Type>(0)), y(static_cast<Type>(0)), z(static_cast<Type>(0)) {}
#else
			CRAFT_ENGINE_CONSTEXPR Vector() {};
#endif
			CRAFT_ENGINE_CONSTEXPR Vector(Type Xx, Type Yy, Type Zz) :x(Xx), y(Yy), z(Zz) {}
			CRAFT_ENGINE_CONSTEXPR Vector(Vector<3, Type> const& v) :x(v.x), y(v.y), z(v.z) {}
			template<typename A, typename B, typename C> CRAFT_ENGINE_CONSTEXPR Vector(A Xx, B Yy, C Zz)
				: x(static_cast<Type>(Xx)), y(static_cast<Type>(Yy)), z(static_cast<Type>(Zz)) {}
			template<typename U> CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Vector(Vector<3, U> const& v) : x(v.x), y(v.y), z(v.z) {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Vector(Type v) : x(v), y(v), z(v) {}
			CRAFT_ENGINE_CONSTEXPR Vector(Vector<2, Type> const& v, Type z) : x(v.x), y(v.y), z(z) {}
			CRAFT_ENGINE_CONSTEXPR Vector<3, Type>& operator=(Vector<3, Type> const& v) { x = v.x; y = v.y; z = v.z; return *this; }
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Vector(Vector<4, Type> const& v) : x(v.x), y(v.y), z(v.z) {}

			Type& operator[](uint32_t index) { return value[index]; }
			Type const& operator[](uint32_t index)const { return value[index]; }

			Vector<3, Type>& operator+=(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x += v.x; y += v.y; z += v.z; return *this; }
			Vector<3, Type>& operator-=(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x -= v.x; y -= v.y; z -= v.z; return *this; }
			Vector<3, Type>& operator*=(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x *= v.x; y *= v.y; z *= v.z; return *this; }
			Vector<3, Type>& operator/=(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x /= v.x; y /= v.y; z /= v.z; return *this; }
			Vector<3, Type>& operator%=(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x %= v.x; y %= v.y; z %= v.z; return *this; }
			Vector<3, Type>& operator+=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x += scalar; y += scalar; z += scalar; return *this; }
			Vector<3, Type>& operator-=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x -= scalar; y -= scalar; z -= scalar; return *this; }
			Vector<3, Type>& operator*=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x *= scalar; y *= scalar; z *= scalar; return *this; }
			Vector<3, Type>& operator/=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { auto it = inverse(scalar);  return (*this) *= it; }
			Vector<3, Type>  operator-()const CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(-x, -y, -z); }
			bool             operator==(const Vector<3, Type>& v)const { return x == v.x && y == v.y && z == v.z; }
			bool             operator==(Type val)const { return x == val && y == val && z == val; }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator+(Vector<3, Type> const& v1, Vector<3, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator-(Vector<3, Type> const& v1, Vector<3, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator*(Vector<3, Type> const& v1, Vector<3, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator/(Vector<3, Type> const& v1, Vector<3, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator%(Vector<3, Type> const& v1, Vector<3, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z); }


			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator+(Type scalar, Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v.x + scalar, v.y + scalar, v.z + scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator-(Type scalar, Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(scalar - v.x, scalar - v.y, scalar - v.z); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator*(Type scalar, Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v.x * scalar, v.y * scalar, v.z * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator/(Type scalar, Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(scalar / v.x, scalar / v.y, scalar / v.z); }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator+(Vector<3, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v.x + scalar, v.y + scalar, v.z + scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator-(Vector<3, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v.x - scalar, v.y - scalar, v.z - scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator*(Vector<3, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v.x * scalar, v.y * scalar, v.z * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<3, Type> operator/(Vector<3, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(v.x / scalar, v.y / scalar, v.z / scalar); }

			Type length()const CRAFT_ENGINE_NOEXCEPT { return std::sqrt(length2()); }
			Type length2()const CRAFT_ENGINE_NOEXCEPT { return x * x + y * y + z * z; }
			Vector<3, Type> normalize()const CRAFT_ENGINE_NOEXCEPT { auto il = inverse(length()); return *this * il; }
			Vector<3, Type> mul(Vector<3, Type> const& v)const CRAFT_ENGINE_NOEXCEPT { return *this * v; }

			Vector<3, Type> pow(Vector<3, Type> const& v)const CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(std::pow(x, v.x), std::pow(y, v.y), std::pow(z, v.z)); }
			Vector<3, Type> exp()const CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(std::exp(x), std::exp(y), std::exp(z)); }
			Vector<3, Type> log()const CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(std::log(x), std::log(y), std::log(z)); }
			Vector<3, Type> log2()const CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(std::log2(x), std::log2(y), std::log2(z)); }
			Vector<3, Type> tan()const CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(std::tan(x), std::tan(y), std::tan(z)); }
			Vector<3, Type> cos()const CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(std::cos(x), std::cos(y), std::cos(z)); }
			Vector<3, Type> sin()const CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(std::sin(x), std::sin(y), std::sin(z)); }


			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type dot(Vector<3, Type> const& v)const CRAFT_ENGINE_NOEXCEPT
			{
				return x * v.x + y * v.y + z * v.z;
			}

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> cross(Vector<3, Type> const& v)const CRAFT_ENGINE_NOEXCEPT
			{
				return Vector<3, Type>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
			}
		};

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type distance(Vector<3, Type> const& v1, Vector<3, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return (v1 - v2).length(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type length(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return v.length(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type length2(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return v.length2(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> normalize(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return v.normalize(); }

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type dot(Vector<3, Type> const& v1, Vector<3, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return v1.dot(v2); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> cross(Vector<3, Type> const& v1, Vector<3, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return v1.cross(v2); }
		template<typename Type>
		// dot(cross(v1, v2), v3)
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type triple(Vector<3, Type> const& v1, Vector<3, Type> const& v2, Vector<3, Type> const& v3) CRAFT_ENGINE_NOEXCEPT { return dot(cross(v1, v2), v3); }








		template<typename Type>
		struct Vector<4, Type>
		{
			typedef Type                value_t;
			typedef Vector<4, Type>     vec4_t;

			union {
				struct { Type x, y, z, w; };
				struct { Type r, g, b, a; };
				struct { Type s, t, p, q; };
				Type value[4];
				Vector<3, Type> xyz;
				Vector<3, Type> rgb;
				struct { Vector<2, Type> xy, zw; };	
				struct { Vector<2, Type> rg, ba; };
			};
			//Vector() {}
#ifdef CRAFT_ENGINE_MATH_HAVING_INITIALIZE_VALUE
			CRAFT_ENGINE_CONSTEXPR Vector() :x(static_cast<Type>(0)), y(static_cast<Type>(0)), z(static_cast<Type>(0)), w(static_cast<Type>(0)) {}
#else
			CRAFT_ENGINE_CONSTEXPR Vector() {};
#endif
			CRAFT_ENGINE_CONSTEXPR Vector(Type Xx, Type Yy, Type Zz, Type Ww) :x(Xx), y(Yy), z(Zz), w(Ww) {}
			CRAFT_ENGINE_CONSTEXPR Vector(Vector<4, Type> const& v) :x(v.x), y(v.y), z(v.z), w(v.w) {}
			template<typename A, typename B, typename C, typename D> CRAFT_ENGINE_CONSTEXPR Vector(A Xx, B Yy, C Zz, D Ww)
				: x(static_cast<Type>(Xx)), y(static_cast<Type>(Yy)), z(static_cast<Type>(Zz)), w(static_cast<Type>(Ww)) {}
			template<typename U> CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Vector(Vector<4, U> const& v) : Vector(v.x, v.y, v.z, v.w) {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Vector(Type v) : x(v), y(v), z(v), w(v) {}
			CRAFT_ENGINE_CONSTEXPR Vector<4, Type>& operator=(Vector<4, Type> const& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
			CRAFT_ENGINE_CONSTEXPR Vector<4, Type>(Vector<3, Type> const& v, Type w) : x(v.x), y(v.y), z(v.z), w(w) {}
			CRAFT_ENGINE_CONSTEXPR Vector<4, Type>(Vector<2, Type> const& v, Type z, Type w) : x(v.x), y(v.y), z(z), w(w) {}

			Type& operator[](uint32_t index) { return value[index]; }
			Type const& operator[](uint32_t index)const { return value[index]; }

			Vector<4, Type>& operator+=(Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
			Vector<4, Type>& operator-=(Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
			Vector<4, Type>& operator*=(Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
			Vector<4, Type>& operator/=(Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
			Vector<4, Type>& operator%=(Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { x %= v.x; y %= v.y; z %= v.z; w %= v.w; return *this; }
			Vector<4, Type>& operator+=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x += scalar; y += scalar; z += scalar; w += scalar; return *this; }
			Vector<4, Type>& operator-=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x -= scalar; y -= scalar; z -= scalar; w += scalar; return *this; }
			Vector<4, Type>& operator*=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
			Vector<4, Type>& operator/=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { auto it = inverse(scalar);  return (*this) *= it; }
			Vector<4, Type>  operator-()const { return Vector<4, Type>(-x, -y, -z, -w); }
			bool             operator==(const Vector<4, Type>& v)const { return x == v.x && y == v.y && z == v.z && w == v.w; }
			bool             operator==(Type val)const { return x == val && y == val && z == val && w == val; }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator+(Vector<4, Type> const& v1, Vector<4, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator-(Vector<4, Type> const& v1, Vector<4, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator*(Vector<4, Type> const& v1, Vector<4, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator/(Vector<4, Type> const& v1, Vector<4, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator%(Vector<4, Type> const& v1, Vector<4, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z, v1.w % v2.w); }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator+(Type scalar, Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v.x + scalar, v.y + scalar, v.z + scalar, v.w + scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator-(Type scalar, Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(scalar - v.x, scalar - v.y, scalar - v.z, scalar - v.w); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator*(Type scalar, Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator/(Type scalar, Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w); }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator+(Vector<4, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v.x + scalar, v.y + scalar, v.z + scalar, v.w + scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator-(Vector<4, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v.x - scalar, v.y - scalar, v.z - scalar, v.w - scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator*(Vector<4, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Vector<4, Type> operator/(Vector<4, Type> const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar); }

			Type length()const CRAFT_ENGINE_NOEXCEPT { return std::sqrt(length2()); }
			Type length2()const CRAFT_ENGINE_NOEXCEPT { return x * x + y * y + z * z + w * w; }
			Vector<4, Type> normalize()const CRAFT_ENGINE_NOEXCEPT { auto il = inverse(length()); return *this * il; }
			Vector<4, Type> mul(Vector<4, Type> const& v)const CRAFT_ENGINE_NOEXCEPT { return *this * v; }

			Vector<4, Type> pow(Vector<4, Type> const& v)const CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(std::pow(x, v.x), std::pow(y, v.y), std::pow(z, v.z), std::pow(w, v.w)); }
			Vector<4, Type> exp()const CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(std::exp(x), std::exp(y), std::exp(z), std::exp(w)); }
			Vector<4, Type> log()const CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(std::log(x), std::log(y), std::log(z), std::log(w)); }
			Vector<4, Type> log2()const CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(std::log2(x), std::log2(y), std::log2(z), std::log2(w)); }
			Vector<4, Type> tan()const CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(std::tan(x), std::tan(y), std::tan(z), std::tan(w)); }
			Vector<4, Type> cos()const CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(std::cos(x), std::cos(y), std::cos(z), std::cos(w)); }
			Vector<4, Type> sin()const CRAFT_ENGINE_NOEXCEPT { return Vector<4, Type>(std::sin(x), std::sin(y), std::sin(z), std::sin(w)); }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type dot(Vector<4, Type> const& v)const CRAFT_ENGINE_NOEXCEPT
			{
				return x * v.x + y * v.y + z * v.z + w * v.w;
			}

		};

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type distance(Vector<4, Type> const& v1, Vector<4, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return (v1 - v2).length(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type length(Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return v.length(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type length2(Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return v.length2(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<4, Type> normalize(Vector<4, Type> const& v) CRAFT_ENGINE_NOEXCEPT { return v.normalize(); }

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type dot(Vector<4, Type> const& v1, Vector<4, Type> const& v2) CRAFT_ENGINE_NOEXCEPT { return v1.dot(v2); }





		template<typename Type>
		struct Quaternion
		{
		public:
			union {
				struct { Type i, j, k, r; };
				struct { Type x, y, z, w; };
				Type value[4];
			};

#ifdef CRAFT_ENGINE_MATH_HAVING_INITIALIZE_VALUE
			CRAFT_ENGINE_CONSTEXPR Quaternion() :i(0), j(0), k(0), r(1) {};
#else
			CRAFT_ENGINE_CONSTEXPR Quaternion() {};
#endif
			// 数值构造
			CRAFT_ENGINE_CONSTEXPR Quaternion(Type Ii, Type Jj, Type Kk, Type realpart) :i(Ii), j(Jj), k(Kk), r(realpart) {};
			// 复制构造
			Quaternion(Quaternion const& quat) :i(quat.i), j(quat.j), k(quat.k), r(quat.r) {};

			// 构造轴-角旋转四元数
			CRAFT_ENGINE_CONSTEXPR Quaternion(Type const& angle, Vector<3, Type> const& axis) {
				auto norm_axis = math::normalize(axis);
				Type h = 0.5f * angle;
				Type s = std::sin(h);
				i = s * norm_axis.x;
				j = s * norm_axis.y;
				k = s * norm_axis.z;
				r = std::cos(h);
			}

			// 构造欧拉角旋转四元数
			// *(x->pitch, y->yaw, z->roll)
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Quaternion(Vector<3, Type> const& eularAngle) {
				Vector<3, Type> halfAngle = 0.5f * eularAngle;
				Type
					cx = cos(halfAngle.x),
					sx = sin(halfAngle.x),
					cy = cos(halfAngle.y),
					sy = sin(halfAngle.y),
					cz = cos(halfAngle.z),
					sz = sin(halfAngle.z);

				i = sx * cy * cz - cx * sy * sz;
				j = cx * sy * cz + sx * cy * sz;
				k = cx * cy * sz - sx * sy * cz;
				r = cx * cy * cz + sx * sy * sz;
			}

			CRAFT_ENGINE_CONSTEXPR Quaternion(Vector<3, Type> const& u, Vector<3, Type> const& v)
			{
				Type norm_u_norm_v = std::sqrt(u.length2() * v.length2());
				Type real_part = norm_u_norm_v + u.dot(v);
				Vector<3, Type> t;
				if (real_part < static_cast<Type>(1.e-6f) * norm_u_norm_v)
					t = abs(u.x) > abs(u.z) ? Vector<3, Type>(-u.y, u.x, real_part = static_cast<Type>(0)) : Vector<3, Type>(real_part = static_cast<Type>(0), -u.z, u.y);
				else
					t = cross(u, v);
				*this = Quaternion(t.x, t.y, t.z, real_part);
			}

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Quaternion operator*(Type scalar, Quaternion const& v) CRAFT_ENGINE_NOEXCEPT { return Quaternion(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Quaternion operator/(Type scalar, Quaternion const& v) CRAFT_ENGINE_NOEXCEPT { return Quaternion(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Quaternion operator*(Quaternion const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Quaternion(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR friend Quaternion operator/(Quaternion const& v, Type scalar) CRAFT_ENGINE_NOEXCEPT { return Quaternion(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar); }
			Quaternion& operator*=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
			Quaternion& operator/=(Type const& scalar) CRAFT_ENGINE_NOEXCEPT { auto it = inverse(scalar);  return (*this) *= it; }
			Quaternion  operator-()const { return Quaternion(-x, -y, -z, -w); }


			CRAFT_ENGINE_CONSTEXPR Quaternion& operator =(Quaternion const& q) CRAFT_ENGINE_NOEXCEPT { i = q.i; j = q.j; k = q.k, r = q.r; return *this; }
			CRAFT_ENGINE_CONSTEXPR Quaternion& operator*=(Quaternion const& q) CRAFT_ENGINE_NOEXCEPT { (*this) = ((*this) * q); return *this; }

			CRAFT_ENGINE_MATH_FUNC_DECL friend Quaternion operator+(Quaternion const& q1, Quaternion const& q2) CRAFT_ENGINE_NOEXCEPT { return Quaternion(q1.i + q2.i, q1.j + q2.j, q1.k + q2.k, q1.r + q2.r); }
			CRAFT_ENGINE_MATH_FUNC_DECL friend Quaternion operator-(Quaternion const& q1, Quaternion const& q2) CRAFT_ENGINE_NOEXCEPT { return Quaternion(q1.i - q2.i, q1.j - q2.j, q1.k - q2.k, q1.r - q2.r); }
			CRAFT_ENGINE_MATH_FUNC_DECL friend Quaternion operator*(Quaternion const& p, Quaternion const& q)CRAFT_ENGINE_NOEXCEPT
			{
				return Quaternion(
					p.r * q.i + p.i * q.r + p.j * q.k - p.k * q.j,
					p.r * q.j + p.j * q.r + p.k * q.i - p.i * q.k,
					p.r * q.k + p.k * q.r + p.i * q.j - p.j * q.i,
					p.r * q.r - p.i * q.i - p.j * q.j - p.k * q.k
				);
			}

			CRAFT_ENGINE_MATH_FUNC_DECL friend Vector<3, Type> operator*(Vector<3, Type> const& v, Quaternion const& q) CRAFT_ENGINE_NOEXCEPT
			{
				Vector<3, Type> QuatVector(q.i, q.j, q.k);
				Vector<3, Type> uv(cross(QuatVector, v));
				Vector<3, Type> uuv(cross(QuatVector, uv));
				return v + ((uv * q.r) + uuv) * static_cast<Type>(2);
			}

			CRAFT_ENGINE_MATH_FUNC_DECL friend Vector<4, Type> operator*(Vector<4, Type> const& v, Quaternion const& q) CRAFT_ENGINE_NOEXCEPT
			{
				Vector<3, Type> tec = Vector<3, Type>(v.x, v.y, v.z);
				Vector<3, Type> QuatVector(q.i, q.j, q.k);
				Vector<3, Type> uv(cross(QuatVector, tec));
				Vector<3, Type> uuv(cross(QuatVector, uv));
				return Vector<4, Type>(tec + ((uv * q.r) + uuv) * static_cast<Type>(2), v.w);
			}

			Type length()const CRAFT_ENGINE_NOEXCEPT { return std::sqrt(length2()); }
			Type length2()const CRAFT_ENGINE_NOEXCEPT { return i * i + j * j + k * k + r * r; }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Quaternion conjugate()const CRAFT_ENGINE_NOEXCEPT { return Quaternion(-i, -j, -k, r); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Quaternion normalize()const CRAFT_ENGINE_NOEXCEPT { Type inv = static_cast<Type>(1) / length(); return *this * inv; }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Quaternion inverse()  const CRAFT_ENGINE_NOEXCEPT { return (static_cast<Type>(1) / length()) * conjugate(); } // ?

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type dot(Quaternion const& q)const CRAFT_ENGINE_NOEXCEPT { return i * q.i + j * q.j + k * q.k + r * q.r; }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type angle()const CRAFT_ENGINE_NOEXCEPT { return Type(2) * std::acos(r); }
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> axis()const CRAFT_ENGINE_NOEXCEPT {
				Type s2 = static_cast<Type>(1) - r * r;
				if (s2 < math::epsilon<Type>())
					return Vector<3, Type>(1.0, 0.0, 0.0);
				Type s = static_cast<Type>(1) / sqrt(s2);
				return Vector<3, Type>(i * s, j * s, k * s);
			}

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type pitch() const CRAFT_ENGINE_NOEXCEPT { 

				Type const y = static_cast<Type>(2) * (j * k + r * i);
				Type const x = r * r - i * i - j * j + k * k;
				if (abs(x) < math::epsilon<Type>() && abs(y) < math::epsilon<Type>())
					return static_cast<Type>(static_cast<Type>(2) * atan2(i, r));//avoid atan2(0,0) - handle singularity - Matiis
				return static_cast<Type>(atan2(y, x));
			}
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type roll() const CRAFT_ENGINE_NOEXCEPT
			{ 
				return static_cast<Type>(atan2(static_cast<Type>(2) * (i * j + r * k), r * r + i * i - j * j - k * k));
			}
			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type yaw() const CRAFT_ENGINE_NOEXCEPT
			{ 
				return asin(clamp(static_cast<Type>(-2) * (i * k - r * j), static_cast<Type>(-1), static_cast<Type>(1)));
			}


			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> eulerAngles() const CRAFT_ENGINE_NOEXCEPT { return Vector<3, Type>(pitch(), yaw(), roll()); }

			CRAFT_ENGINE_MATH_FUNC_DECL Quaternion addEuler(Vector<3, Type> const& eularAngle)const CRAFT_ENGINE_NOEXCEPT { Quaternion q(eularAngle); return (*this) * q; }
			CRAFT_ENGINE_MATH_FUNC_DECL Quaternion addAngleAxis(Vector<3, Type> const& axis, Type const& angle)const CRAFT_ENGINE_NOEXCEPT { Quaternion q(angle, axis); return (*this) * q; }
			CRAFT_ENGINE_MATH_FUNC_DECL Quaternion addScaledVector(Vector<3, Type> const& sv)const CRAFT_ENGINE_NOEXCEPT { Type l = sv.length(); Quaternion q(l, (static_cast<Type>(1) / l) * sv);  return (*this) * q; }

		};



		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL Type dot(Quaternion<Type> const& x, Quaternion<Type> const& y) CRAFT_ENGINE_NOEXCEPT {
			return x.dot(y);
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL Quaternion<Type> mix(Quaternion<Type> const& from, Quaternion<Type> const& to, Type t) CRAFT_ENGINE_NOEXCEPT {
			return (static_cast<Type>(1) - t) * from + t * to;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL Quaternion<Type> lerp(Quaternion<Type> const& from, Quaternion<Type> const& to, Type t) CRAFT_ENGINE_NOEXCEPT {
			return (static_cast<Type>(1) - t) * from + t * to;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL Quaternion<Type> slerp(Quaternion<Type> const& from, Quaternion<Type> const& to, Type t) CRAFT_ENGINE_NOEXCEPT {
			Type b = std::sin(t * math::pi<Type>());
			return (static_cast<Type>(1) - b) * from + b * to;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL Quaternion<Type> splerp(Quaternion<Type> const& from, Quaternion<Type> const& to, Type t) CRAFT_ENGINE_NOEXCEPT {
			auto z = to;
			Type cosTheta = dot(from, to);
			// If cosTheta < 0, the interpolation will take the long way around the sphere.
			// To fix this, one quat must be negated.
			if (cosTheta < static_cast<Type>(0))
			{
				z = -to;
				cosTheta = -cosTheta;
			}
			// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
			if (cosTheta > static_cast<Type>(1) - math::epsilon<Type>())
			{
				// Linear interpolation
				return Quaternion<Type>(
					mix(from.x, z.x, t),
					mix(from.y, z.y, t),
					mix(from.z, z.z, t),
					mix(from.w, z.w, t));
			}
			else
			{
				// Essential Mathematics, page 467
				Type angle = acos(cosTheta);
				return (
					sin((static_cast<Type>(1) - t) * angle) * from 
					+ sin(t * angle) * z)
					/ sin(angle);
			}
		}



		/*
			Matrix2x2: Simplifid implement of Column-Major Order, 2 column with 2 row Matrix for GLSL
		*/
		template<typename Type>
		struct Matrix<2, 2, Type>
		{
			typedef Vector<2, Type>    col_vector;
			typedef Vector<2, Type>    row_vector;
			typedef Matrix<2, 2, Type> mat2_t;
			typedef Type               value_t;
			union
			{
				struct { col_vector c0, c1; };
				col_vector value[2];
			};

			CRAFT_ENGINE_CONSTEXPR Matrix() : value{ {1,0},{0,1} } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(col_vector const& C0, col_vector const& C1) :value{ C0,C1 } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(Matrix<2, 2, Type> const& m) : value{ m.c0,m.c1 } {}
			template<typename A, typename B> CRAFT_ENGINE_CONSTEXPR Matrix(Vector<2, A> const& C0, Vector<2, B> const& C1) : value{ C0,C1 } {}
			template<typename U> CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Matrix<2, 2, U> const& m) : value{ col_vector(m.c0),col_vector(m.c1) } {}
			template<
				typename T00, typename T01,
				typename T10, typename T11
			> CRAFT_ENGINE_CONSTEXPR Matrix(
				T00 const& V00, T01 const& V01,
				T10 const& V10, T11 const& V11
			) : value{ {V00, V01},{V10, V11} } {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Type v) : value{ {v,0},{0,v} } {}
			CRAFT_ENGINE_CONSTEXPR Matrix<2, 2, Type>& operator=(Matrix<2, 2, Type> const& m) { c0 = m.c0; c1 = m.c1; return *this; }
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Matrix<3, 3, Type> const& m) : value{ col_vector(m.c0),col_vector(m.c1) } {}


			col_vector& operator[](uint32_t index) { return value[index]; }
			col_vector const& operator[](uint32_t index)const { return value[index]; }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<2, 2, Type> transpose()const
			{
				return Matrix<2, 2, Type>(
					{ c0.x,c1.x },
					{ c0.y,c1.y }
				);
			}

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type determinant()const
			{
				return c0[0] * c1[1] - c1[0] * c1[0];
			}

			CRAFT_ENGINE_CONSTEXPR Matrix<2, 2, Type> inverse()const
			{
				auto inv_det = static_cast<Type>(1) / determinant();
				return *this * inv_det;
			}

			CRAFT_ENGINE_CONSTEXPR Matrix<2, 2, Type>& operator*=(Matrix<2, 2, Type> const& m) { return *this = *this * m; }
			CRAFT_ENGINE_CONSTEXPR Matrix<2, 2, Type>& operator*=(Type scalar) { c0 *= scalar; c1 *= scalar; return *this; }

			// scalar operator
			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<2, 2, Type> operator*(Matrix<2, 2, Type> const& m, Type scalar) { return Matrix<2, 2, Type>(m.c0 * scalar, m.c1 * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<2, 2, Type> operator*(Type scalar, Matrix<2, 2, Type> const& m) { return Matrix<2, 2, Type>(m.c0 * scalar, m.c1 * scalar); }
			// matrix operator
			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<2, 2, Type> operator*(Matrix<2, 2, Type> const& m1, Matrix<2, 2, Type> const& m2)
			{
				auto const& SrcA0 = m1[0];
				auto const& SrcA1 = m1[1];
				auto const& SrcB0 = m2[0];
				auto const& SrcB1 = m2[1];
				Matrix<2, 2, Type> Result;
				Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1];
				Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1];
				return Result;
			}
			// vector operator
			CRAFT_ENGINE_MATH_FUNC_DECL friend row_vector operator*(Matrix<2, 2, Type> const& m, col_vector const& col_vec) {
				return (col_vec[0] * m[0] + col_vec[1] * m[1]);
			}
			CRAFT_ENGINE_MATH_FUNC_DECL friend col_vector operator*(row_vector const& v, Matrix<2, 2, Type> const& m) {
				return Vector<2, Type>(dot(m[0], v), dot(m[1], v));
			}

		};



		/*
			Matrix3x3: Simplifid implement of Left-Hand, Column-Major Order, 3 column with 3 row Matrix for GLSL
		*/
		template<typename Type>
		struct Matrix<3, 3, Type>
		{
			typedef Vector<3, Type>    col_vector;
			typedef Vector<3, Type>    row_vector;
			typedef Matrix<3, 3, Type> mat3_t;
			typedef Type               value_t;
			union
			{
				struct { col_vector c0, c1, c2; };
				col_vector value[3];
			};

			CRAFT_ENGINE_CONSTEXPR Matrix() : value{ {1,0,0},{0,1,0},{0,0,1} } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(col_vector const& C0, col_vector const& C1, col_vector const& C2) :value{ C0,C1,C2 } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(Matrix<3, 3, Type> const& m) : value{ m.c0,m.c1,m.c2 } {}
			template<typename A, typename B, typename C> CRAFT_ENGINE_CONSTEXPR Matrix(Vector<3, A> const& C0, Vector<3, B> const& C1, Vector<3, C> const& C2) : value{ C0,C1,C2 } {}
			template<
				typename T00, typename T01, typename T02,
				typename T10, typename T11, typename T12,
				typename T20, typename T21, typename T22
			> CRAFT_ENGINE_CONSTEXPR Matrix(
				T00 const& V00, T01 const& V01, T02 const& V02,
				T10 const& V10, T11 const& V11, T12 const& V12, 
				T20 const& V20, T21 const& V21, T22 const& V22
			) : value{ {V00, V01, V02},{V10, V11, V12},{V20, V21, V22} } {}

			template<typename U> CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Matrix<3, 3, U> const& m) : value{ col_vector(m.c0),col_vector(m.c1),col_vector(m.c2) } {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Type v) : value{ {v,0,0},{0,v,0},{0,0,v} } {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Matrix<2, 2, Type> const& m) : value{ {m[0],0},{m[1],0},{0,0,1} } {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Matrix<4, 4, Type> const& m) : value{ col_vector(m.c0),col_vector(m.c1),col_vector(m.c2) } {}

			CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type>& operator=(Matrix<3, 3, Type> const& m) { c0 = m.c0; c1 = m.c1; c2 = m.c2; return *this; }

			col_vector& operator[](uint32_t index) { return value[index]; }
			col_vector const& operator[](uint32_t index)const { return value[index]; }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> transpose()const
			{
				return Matrix<3, 3, Type>(
					{ c0.x,c1.x,c2.x },
					{ c0.y,c1.y,c2.y },
					{ c0.z,c1.z,c2.z }
				);
			}

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type determinant()const
			{
				return triple(c0, c1, c2);
			}

			CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> inverse()const
			{
				auto cross_vec = cross(c1, c2);
				auto inv_det = static_cast<Type>(1) / dot(c0, cross_vec);
				auto result = inv_det * Matrix<3, 3, Type>(
					{ cross_vec },
					{ c0[2] * c2[1] - c0[1] * c2[2], c0[0] * c2[2] - c0[2] * c2[0], c0[1] * c2[0] - c0[0] * c2[1] },
					{ c0[1] * c1[2] - c0[2] * c1[1], c0[2] * c1[0] - c0[0] * c1[2], c0[0] * c1[1] - c0[1] * c1[0] }
				);
				return result;
			}


			CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type>& operator*=(Matrix<3, 3, Type> const& m) { return *this = *this * m; }
			CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type>& operator*=(Type scalar) { c0 *= scalar; c1 *= scalar; c2 *= scalar; return *this; }

			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<3, 3, Type> operator*(Matrix<3, 3, Type> const& m, Type scalar) { return Matrix<3, 3, Type>(m.c0 * scalar, m.c1 * scalar, m.c2 * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<3, 3, Type> operator*(Type scalar, Matrix<3, 3, Type> const& m) { return Matrix<3, 3, Type>(m.c0 * scalar, m.c1 * scalar, m.c2 * scalar); }

			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<3, 3, Type> operator*(Matrix<3, 3, Type> const& m1, Matrix<3, 3, Type> const& m2)
			{
				auto const& SrcA0 = m1[0];
				auto const& SrcA1 = m1[1];
				auto const& SrcA2 = m1[2];
				auto const& SrcB0 = m2[0];
				auto const& SrcB1 = m2[1];
				auto const& SrcB2 = m2[2];
				Matrix<3, 3, Type> Result;
				Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2];
				Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2];
				Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2];
				return Result;
			}

			CRAFT_ENGINE_MATH_FUNC_DECL friend row_vector operator*(Matrix<3, 3, Type> const& m, col_vector const& col_vec) {
				return (col_vec[0] * m[0] + col_vec[1] * m[1] + col_vec[2] * m[2]);
			}
			CRAFT_ENGINE_MATH_FUNC_DECL friend col_vector operator*(row_vector const& row_vec, Matrix<3, 3, Type> const& m) {
				return Vector<3, Type>(dot(m[0], row_vec), dot(m[1], row_vec), dot(m[2], row_vec));
			}

		};



		/*
			Matrix4x4: Simplifid implement of Left-Hand with Column-Major Order 4x4 Matrix for GLSL
		*/
		template<typename Type>
		struct Matrix<4, 4, Type>
		{

			typedef Vector<4, Type>    col_vector;
			typedef Vector<4, Type>    row_vector;
			typedef Matrix<4, 4, Type> mat4_t;
			typedef Type               value_t;
			union
			{
				struct { col_vector c0, c1, c2, c3; };
				col_vector value[4];
			};

			CRAFT_ENGINE_CONSTEXPR Matrix() : value{ {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(col_vector const& C0, col_vector const& C1, col_vector const& C2, col_vector const& C3) :value{ C0,C1,C2,C3 } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(Matrix<4, 4, Type> const& m) : value{ m.c0,m.c1,m.c2,m.c3 } {}
			template<typename A, typename B, typename C, typename D> CRAFT_ENGINE_CONSTEXPR
				Matrix(Vector<4, A> const& C0, Vector<4, B> const& C1, Vector<4, C> const& C2, Vector<4, D> const& C3) : value{ C0,C1,C2,C3 } {}
			template<
				typename T00, typename T01, typename T02, typename T03,
				typename T10, typename T11, typename T12, typename T13,
				typename T20, typename T21, typename T22, typename T23,
				typename T30, typename T31, typename T32, typename T33
			> CRAFT_ENGINE_CONSTEXPR Matrix(
				T00 const& V00, T01 const& V01, T02 const& V02, T03 const& V03,
				T10 const& V10, T11 const& V11, T12 const& V12, T13 const& V13,
				T20 const& V20, T21 const& V21, T22 const& V22, T23 const& V23,
				T30 const& V30, T31 const& V31, T32 const& V32, T33 const& V33
			) : value{ {V00, V01, V02, V03},{V10, V11, V12, V13},{V20, V21, V22, V23},{V30, V31, V32, V33} } {}

			template<typename U> CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Matrix<4, 4, U> const& m) : value{ col_vector(m.c0),col_vector(m.c1),col_vector(m.c2),col_vector(m.c3) } {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Type v) : value{ {v,0,0,0},{0,v,0,0},{0,0,v,0},{0,0,0,v} } {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Matrix<3, 3, Type> const& m) : value{ {m[0],0},{m[1],0},{m[2],0},{0,0,0,1} } {}
			CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type>& operator=(Matrix<4, 4, Type> const& m) { c0 = m.c0; c1 = m.c1; c2 = m.c2; c3 = m.c3; return *this; }

			col_vector& operator[](uint32_t index) { return value[index]; }
			col_vector const& operator[](uint32_t index)const { return value[index]; }

			Matrix<4, 4, Type> transpose()const
			{
				return Matrix<4, 4, Type>(
					{ c0.x,c1.x,c2.x,c3.x },
					{ c0.y,c1.y,c2.y,c3.y },
					{ c0.z,c1.z,c2.z,c3.z },
					{ c0.w,c1.w,c2.w,c3.w }
				);
			}

			Matrix<4, 4, Type> inverse()const
			{
				const mat4_t& m = *this;

				value_t coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
				value_t coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
				value_t coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

				value_t coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
				value_t coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
				value_t coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

				value_t coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
				value_t coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
				value_t coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

				value_t coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
				value_t coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
				value_t coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

				value_t coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
				value_t coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
				value_t coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

				value_t coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
				value_t coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
				value_t coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

				col_vector fac0(coef00, coef00, coef02, coef03);
				col_vector fac1(coef04, coef04, coef06, coef07);
				col_vector fac2(coef08, coef08, coef10, coef11);
				col_vector fac3(coef12, coef12, coef14, coef15);
				col_vector fac4(coef16, coef16, coef18, coef19);
				col_vector fac5(coef20, coef20, coef22, coef23);

				col_vector tec0(m[1][0], m[0][0], m[0][0], m[0][0]);
				col_vector tec1(m[1][1], m[0][1], m[0][1], m[0][1]);
				col_vector tec2(m[1][2], m[0][2], m[0][2], m[0][2]);
				col_vector tec3(m[1][3], m[0][3], m[0][3], m[0][3]);

				col_vector inv0(tec1 * fac0 - tec2 * fac1 + tec3 * fac2);
				col_vector inv1(tec0 * fac0 - tec2 * fac3 + tec3 * fac4);
				col_vector inv2(tec0 * fac1 - tec1 * fac3 + tec3 * fac5);
				col_vector inv3(tec0 * fac2 - tec1 * fac4 + tec2 * fac5);

				col_vector signA(+1, -1, +1, -1);
				col_vector signB(-1, +1, -1, +1);
				mat4_t inverseMatrix(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

				col_vector row0(inverseMatrix[0][0], inverseMatrix[1][0], inverseMatrix[2][0], inverseMatrix[3][0]);
				col_vector dot0(m[0] * row0);

				value_t dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);
				value_t OneOverDeterminant = static_cast<value_t>(1) / dot1;
				return inverseMatrix * OneOverDeterminant;
			}

			CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type>& operator*=(Matrix<4, 4, Type> const& m) { return *this = *this * m; }
			CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type>& operator*=(Type scalar) { c0 *= scalar; c1 *= scalar; c2 *= scalar, c3 *= scalar; return *this; }

			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<4, 4, Type> operator*(Matrix<4, 4, Type> const& m, Type scalar) { return Matrix<4, 4, Type>(m.c0 * scalar, m.c1 * scalar, m.c2 * scalar, m.c3 * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<4, 4, Type> operator*(Type scalar, Matrix<4, 4, Type> const& m) { return Matrix<4, 4, Type>(m.c0 * scalar, m.c1 * scalar, m.c2 * scalar, m.c3 * scalar); }

			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<4, 4, Type> operator*(Matrix<4, 4, Type> const& m1, Matrix<4, 4, Type> const& m2)
			{
				auto const& SrcA0 = m1[0];
				auto const& SrcA1 = m1[1];
				auto const& SrcA2 = m1[2];
				auto const& SrcA3 = m1[3];

				auto const& SrcB0 = m2[0];
				auto const& SrcB1 = m2[1];
				auto const& SrcB2 = m2[2];
				auto const& SrcB3 = m2[3];

				Matrix<4, 4, Type> Result;
				Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3];
				Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3];
				Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3];
				Result[3] = SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3];
				return Result;
			}

			CRAFT_ENGINE_MATH_FUNC_DECL friend row_vector operator*(Matrix<4, 4, Type> const& m, col_vector const& col_vec) {
				return (col_vec[0] * m[0] + col_vec[1] * m[1] + col_vec[2] * m[2] + col_vec[3] * m[3]);
			}
			CRAFT_ENGINE_MATH_FUNC_DECL friend col_vector operator*(row_vector const& row_vec, Matrix<4, 4, Type> const& m) {
				return Vector<4, Type>(dot(m[0], row_vec), dot(m[1], row_vec), dot(m[2], row_vec), dot(m[3], row_vec));
			}

		};



		/*
			Matrix3x4: Simplifid implement of Left-Hand, Column-Major Order, 3 column with 4 row Matrix for GLSL
		*/
		template<typename Type>
		struct Matrix<3, 4, Type>
		{
			typedef Vector<4, Type> col_vector;
			typedef Vector<3, Type> row_vector;
			typedef Type            value_t;
			union
			{
				struct { col_vector c0, c1, c2; };
				col_vector value[3];
			};

			CRAFT_ENGINE_CONSTEXPR Matrix() : value{ {1,0,0,0},{0,1,0,0},{0,0,1,0} } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(col_vector const& C0, col_vector const& C1, col_vector const& C2) :value{ C0,C1,C2 } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(Matrix<3, 4, Type> const& m) : value{ m.c0,m.c1,m.c2 } {}
			template<typename A, typename B, typename C> CRAFT_ENGINE_CONSTEXPR Matrix(Vector<4, A> const& C0, Vector<4, B> const& C1, Vector<4, C> const& C2) : value{ C0,C1,C2 } {}
			template<
				typename T00, typename T01, typename T02, typename T03,
				typename T10, typename T11, typename T12, typename T13,
				typename T20, typename T21, typename T22, typename T23
			> CRAFT_ENGINE_CONSTEXPR Matrix(
				T00 const& V00, T01 const& V01, T02 const& V02, T03 const& V03,
				T10 const& V10, T11 const& V11, T12 const& V12, T13 const& V13,
				T20 const& V20, T21 const& V21, T22 const& V22, T23 const& V23
			) : value{ {V00, V01, V02, V03},{V10, V11, V12, V13},{V20, V21, V22, V23} } {}

			CRAFT_ENGINE_CONSTEXPR Matrix(Matrix<3, 3, Type> const& m, Vector<3, Type> v) :
				Matrix(
					Vector<4, Type>(m[0], v[0]),
					Vector<4, Type>(m[1], v[1]),
					Vector<4, Type>(m[2], v[2])
				) {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Type v) : value{ {v,0,0},{0,v,0},{0,0,v} } {}
			CRAFT_ENGINE_CONSTEXPR Matrix<3, 4, Type>& operator=(Matrix<3, 4, Type> const& m) { c0 = m.c0; c1 = m.c1; c2 = m.c2; return *this; }

			col_vector& operator[](uint32_t index) { return value[index]; }
			col_vector const& operator[](uint32_t index)const { return value[index]; }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 3, Type> transpose()const
			{
				return Matrix<4, 3, Type>(
					{ c0.x,c1.x,c2.x },
					{ c0.y,c1.y,c2.y },
					{ c0.z,c1.z,c2.z },
					{ c0.w,c1.w,c2.w }
				);
			}

			CRAFT_ENGINE_CONSTEXPR Matrix<3, 4, Type>& operator*=(Type scalar) { c0 *= scalar; c1 *= scalar; c2 *= scalar; return *this; }
			
			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<3, 4, Type> operator*(Matrix<3, 4, Type> const& m, Type scalar) { return Matrix<3, 4, Type>(m.c0 * scalar, m.c1 * scalar, m.c2 * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<3, 4, Type> operator*(Type scalar, Matrix<3, 4, Type> const& m) { return Matrix<3, 4, Type>(m.c0 * scalar, m.c1 * scalar, m.c2 * scalar); }

			CRAFT_ENGINE_MATH_FUNC_DECL friend col_vector operator*(Matrix<3, 4, Type> const& m, row_vector const& col_vec) {
				return col_vector(m[0] * col_vec[0] + m[1] * col_vec[1] + m[2] * col_vec[2] + m[3] * col_vec[3]);
			}
			CRAFT_ENGINE_MATH_FUNC_DECL friend row_vector operator*(col_vector const& row_vec, Matrix<3, 4, Type> const& m) {
				return row_vector(dot(m[0], row_vec), dot(m[1], row_vec), dot(m[2], row_vec));
			}

		};


		/*
			Matrix4x3: Simplifid implement of Left-Hand, Column-Major Order, 3 column with 4 row Matrix for GLSL
		*/
		template<typename Type>
		struct Matrix<4, 3, Type>
		{
			typedef Vector<4, Type> row_vector;
			typedef Vector<3, Type> col_vector;
			typedef Type            value_t;
			union
			{
				struct { col_vector c0, c1, c2, c3; };
				col_vector value[4];
			};

			CRAFT_ENGINE_CONSTEXPR Matrix() : value{ {1,0,0},{0,1,0},{0,0,1},{0,0,0} } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(col_vector const& C0, col_vector const& C1, col_vector const& C2, col_vector const& C3) :value{ C0,C1,C2,C3 } {}
			CRAFT_ENGINE_CONSTEXPR Matrix(Matrix<4, 3, Type> const& m) : value{ m.c0,m.c1,m.c2,m.c3 } {}
			template<typename A, typename B, typename C> CRAFT_ENGINE_CONSTEXPR Matrix(Vector<4, A> const& C0, Vector<4, B> const& C1, Vector<4, C> const& C2) : value{ C0,C1,C2 } {}
			template<
				typename T00, typename T01, typename T02,
				typename T10, typename T11, typename T12,
				typename T20, typename T21, typename T22,
				typename T30, typename T31, typename T32
			> CRAFT_ENGINE_CONSTEXPR Matrix(
				T00 const& V00, T01 const& V01, T02 const& V02,
				T10 const& V10, T11 const& V11, T12 const& V12,
				T20 const& V20, T21 const& V21, T22 const& V22,
				T30 const& V30, T31 const& V31, T32 const& V32
			) : value{ {V00, V01, V02},{V10, V11, V12},{V20, V21, V22},{V30, V31, V32} } {}
			CRAFT_ENGINE_CONSTEXPR CRAFT_ENGINE_EXPLICIT Matrix(Type v) : value{ {v,0,0},{0,v,0},{0,0,v},{0,0,0} } {}
			CRAFT_ENGINE_CONSTEXPR Matrix<4, 3, Type>& operator=(Matrix<4, 3, Type> const& m) { c0 = m.c0; c1 = m.c1; c2 = m.c2; c3 = m.c3; return *this; }

			col_vector& operator[](uint32_t index) { return value[index]; }
			col_vector const& operator[](uint32_t index)const { return value[index]; }

			CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 4, Type> transpose()const
			{
				return Matrix<3, 4, Type>(
					{ c0.x,c1.x,c2.x,c3.x },
					{ c0.y,c1.y,c2.y,c3.y },
					{ c0.z,c1.z,c2.z,c3.z }
					);
			}

			CRAFT_ENGINE_CONSTEXPR Matrix<4, 3, Type>& operator*=(Type scalar) { c0 *= scalar; c1 *= scalar; c2 *= scalar; c3 *= scalar; return *this; }

			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<4, 3, Type> operator*(Matrix<4, 3, Type> const& m, Type scalar) { return Matrix<4, 3, Type>(m.c0 * scalar, m.c1 * scalar, m.c2 * scalar, m.c3 * scalar); }
			CRAFT_ENGINE_MATH_FUNC_DECL friend Matrix<4, 3, Type> operator*(Type scalar, Matrix<4, 3, Type> const& m) { return Matrix<4, 3, Type>(m.c0 * scalar, m.c1 * scalar, m.c2 * scalar, m.c3 * scalar); }

			CRAFT_ENGINE_MATH_FUNC_DECL friend col_vector operator*(Matrix<4, 3, Type> const& m, row_vector const& col_vec) {
				return col_vector(m[0] * col_vec[0] + m[1] * col_vec[1] + m[2] * col_vec[2]);
			}
			CRAFT_ENGINE_MATH_FUNC_DECL friend row_vector operator*(col_vector const& row_vec, Matrix<4, 3, Type> const& m) {
				return row_vector(dot(m[0], row_vec), dot(m[1], row_vec), dot(m[2], row_vec), dot(m[3], row_vec));
			}

		};



		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<2, 2, Type> inverse(const Matrix<2, 2, Type>& m) CRAFT_ENGINE_NOEXCEPT { return m.inverse(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> inverse(const Matrix<3, 3, Type>& m) CRAFT_ENGINE_NOEXCEPT { return m.inverse(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> inverse(const Matrix<4, 4, Type>& m) CRAFT_ENGINE_NOEXCEPT { return m.inverse(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<2, 2, Type> transpose(const Matrix<2, 2, Type>& m) CRAFT_ENGINE_NOEXCEPT { return m.transpose(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> transpose(const Matrix<3, 3, Type>& m) CRAFT_ENGINE_NOEXCEPT { return m.transpose(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> transpose(const Matrix<4, 4, Type>& m) CRAFT_ENGINE_NOEXCEPT { return m.transpose(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 4, Type> transpose(const Matrix<4, 3, Type>& m) CRAFT_ENGINE_NOEXCEPT { return m.transpose(); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 3, Type> transpose(const Matrix<3, 4, Type>& m) CRAFT_ENGINE_NOEXCEPT { return m.transpose(); }


		typedef Quaternion<float>   quat;
		typedef Vector<2, float>    vec2;
		typedef Vector<3, float>    vec3;
		typedef Vector<4, float>    vec4;
		typedef Matrix<2, 2, float> mat2;
		typedef Matrix<3, 3, float> mat3;
		typedef Matrix<4, 4, float> mat4;

		typedef Matrix<3, 4, float> mat3x4;
		typedef Matrix<4, 3, float> mat4x3;

		typedef Quaternion<double>   dquat;
		typedef Vector<2, double>    dvec2;
		typedef Vector<3, double>    dvec3;
		typedef Vector<4, double>    dvec4;
		typedef Matrix<2, 2, double> dmat2;
		typedef Matrix<3, 3, double> dmat3;
		typedef Matrix<4, 4, double> dmat4;

		typedef Vector<2, int> ivec2;
		typedef Vector<3, int> ivec3;
		typedef Vector<4, int> ivec4;

		typedef Vector<2, unsigned int> uvec2;
		typedef Vector<3, unsigned int> uvec3;
		typedef Vector<4, unsigned int> uvec4;
	


		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> quatTomat3(Quaternion<Type> const& quat) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Rotate(1);
			Type qxx(quat.i * quat.i);
			Type qyy(quat.j * quat.j);
			Type qzz(quat.k * quat.k);
			Type qxz(quat.i * quat.k);
			Type qxy(quat.i * quat.j);
			Type qyz(quat.j * quat.k);
			Type qwx(quat.r * quat.i);
			Type qwy(quat.r * quat.j);
			Type qwz(quat.r * quat.k);
			Rotate[0][0] = Type(1) - Type(2) * (qyy + qzz);
			Rotate[0][1] = Type(2) * (qxy + qwz);
			Rotate[0][2] = Type(2) * (qxz - qwy);
			Rotate[1][0] = Type(2) * (qxy - qwz);
			Rotate[1][1] = Type(1) - Type(2) * (qxx + qzz);
			Rotate[1][2] = Type(2) * (qyz + qwx);
			Rotate[2][0] = Type(2) * (qxz + qwy);
			Rotate[2][1] = Type(2) * (qyz - qwx);
			Rotate[2][2] = Type(1) - Type(2) * (qxx + qyy);
			return Rotate;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> quatTomat4(Quaternion<Type> const& quat) CRAFT_ENGINE_NOEXCEPT
		{
			return Matrix<4, 4, Type>(quatTomat3(quat));
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Quaternion<Type> mat3ToQuat(Matrix<3, 3, Type> const& m)
		{
			Type fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
			Type fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
			Type fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
			Type fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

			int biggestIndex = 0;
			Type fourBiggestSquaredMinus1 = fourWSquaredMinus1;
			if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
			{
				fourBiggestSquaredMinus1 = fourXSquaredMinus1;
				biggestIndex = 1;
			}
			if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
			{
				fourBiggestSquaredMinus1 = fourYSquaredMinus1;
				biggestIndex = 2;
			}
			if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
			{
				fourBiggestSquaredMinus1 = fourZSquaredMinus1;
				biggestIndex = 3;
			}

			Type biggestVal = sqrt(fourBiggestSquaredMinus1 + static_cast<Type>(1)) * static_cast<Type>(0.5);
			Type mult = static_cast<Type>(0.25) / biggestVal;

			switch (biggestIndex)
			{
			case 0:
				return Quaternion<Type>((m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult, biggestVal);
			case 1:
				return Quaternion<Type>(biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] - m[2][1]) * mult);
			case 2:
				return Quaternion<Type>((m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult, (m[2][0] - m[0][2]) * mult);
			case 3:
				return Quaternion<Type>((m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal, (m[0][1] - m[1][0]) * mult);
			default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
				assert(false);
				return Quaternion<Type>(0, 0, 0, 1);
			}
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Quaternion<Type> mat4ToQuat(Matrix<4, 4, Type> const& m)
		{
			return mat3ToQuat(Matrix<3, 3, Type>(m));
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> eulerToMat3(Vector<3, Type> const& eularAngle)
		{
			auto c = eularAngle.cos();
			auto s = eularAngle.sin();
			Type cc = c[0] * c[2];
			Type cs = c[0] * -s[2];
			Type sc = -s[0] * c[2];
			Type ss = s[0] * s[2];
			Matrix<3, 3, Type> Rotate(1);
			Rotate[0] = { c[1] * c[2], -s[1] * sc - cs, -s[1] * cc + ss };
			Rotate[1] = { c[1] * -s[2], -s[1] * ss + cc, -s[1] * cs - sc };
			Rotate[2] = { s[1]      , -c[1] * s[0]   , c[1] * c[0] };
			return Rotate;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> quatToEuler(Quaternion<Type> const& q)
		{
			return q.eulerAngles();
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Quaternion<Type> eulerToQuat(Vector<3, Type> const& euler)
		{
			return mat3ToQuat(eulerToMat3(euler));
		}





	}
}


namespace CraftEngine
{
	namespace math
	{
		/*
		 Matrix Transform 3D
		*/

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> translate(Vector<3, Type> const& v, const Matrix<4, 4, Type>& m) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Result;
			Result[0] = m[0] + Vector<4, Type>(v * m[0][3], static_cast<Type>(0));
			Result[1] = m[1] + Vector<4, Type>(v * m[1][3], static_cast<Type>(0));
			Result[2] = m[2] + Vector<4, Type>(v * m[2][3], static_cast<Type>(0));
			Result[3] = m[3] + Vector<4, Type>(v * m[3][3], static_cast<Type>(0));
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> translate(const Matrix<4, 4, Type>& m, Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Result = m;
			Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> translate(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Result(1);
			Result[3] = Vector<4, Type>(v, static_cast<Type>(1));
			return Result;
		}


		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> scale(Vector<3, Type> const& v, const Matrix<4, 4, Type>& m) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Result = m;
			Vector<4, Type> t = Vector<4, Type>(v, static_cast<Type>(1));
			Result[0] = m[0] * t;
			Result[1] = m[1] * t;
			Result[2] = m[2] * t;
			Result[3] = m[3] * t;
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> scale(const Matrix<4, 4, Type>& m, Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Result;
			Result[0] = m[0] * v[0];
			Result[1] = m[1] * v[1];
			Result[2] = m[2] * v[2];
			Result[3] = m[3];
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> scale(Vector<3, Type> const& v) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Result(1);
			Result[0][0] = v[0];
			Result[1][1] = v[1];
			Result[2][2] = v[2];
			return Result;
		}



		template<typename Type>
		/*
		 create a 3x3 matrix which can be used to rotate a vector by angle&axis
		*/
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> rotate(Type angle, Vector<3, Type> const& axis) CRAFT_ENGINE_NOEXCEPT
		{
			Type const a = angle;
			Type const c = cos(a);
			Type const s = sin(a);
			Vector<3, Type> naxis = axis.normalize();
			Vector<3, Type> temp((Type(1) - c) * axis);
			Matrix<3, 3, Type> Rotate(1);
			Rotate[0][0] = c + temp[0] * naxis[0];
			Rotate[0][1] = temp[0] * naxis[1] + s * naxis[2];
			Rotate[0][2] = temp[0] * naxis[2] - s * naxis[1];
			Rotate[1][0] = temp[1] * naxis[0] - s * naxis[2];
			Rotate[1][1] = c + temp[1] * naxis[1];
			Rotate[1][2] = temp[1] * naxis[2] + s * naxis[0];
			Rotate[2][0] = temp[2] * naxis[0] + s * naxis[1];
			Rotate[2][1] = temp[2] * naxis[1] - s * naxis[0];
			Rotate[2][2] = c + temp[2] * naxis[2];
			return Rotate;
		}

		template<typename Type>
		/*
		 create a 3x3 matrix which can be used to rotate a vector around eular angles order by x->y->z axis
		*/
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> rotate(Vector<3, Type> const& eularAngle) CRAFT_ENGINE_NOEXCEPT
		{
			auto c = eularAngle.cos();
			auto s = eularAngle.sin();
			Type cc = c[0] * c[2];
			Type cs = c[0] * -s[2];
			Type sc = -s[0] * c[2];
			Type ss = s[0] * s[2];
			Matrix<3, 3, Type> Rotate(1);
			Rotate[0] = { c[1] * c[2], -s[1] * sc - cs, -s[1] * cc + ss };
			Rotate[1] = { c[1] * -s[2], -s[1] * ss + cc, -s[1] * cs - sc };
			Rotate[2] = { s[1]      , -c[1] * s[0]   , c[1] * c[0] };
			return Rotate;
		}


		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> rotateX(Type angleX) CRAFT_ENGINE_NOEXCEPT
		{
			auto c = math::cos(angleX);
			auto s = math::sin(angleX);
			Matrix<3, 3, Type> Rotate(
				1, 0, 0,
				0, c, s,
				0, -s, c);
			return Rotate;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> rotateY(Type angleY) CRAFT_ENGINE_NOEXCEPT
		{
			auto c = math::cos(angleY);
			auto s = math::sin(angleY);
			Matrix<3, 3, Type> Rotate(
				c, 0, -s,
				0, 1, 0,
				s, 0, c);
			return Rotate;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> rotateZ(Type angleZ) CRAFT_ENGINE_NOEXCEPT
		{
			auto c = math::cos(angleZ);
			auto s = math::sin(angleZ);
			Matrix<3, 3, Type> Rotate(
				c, s, 0,
				-s, c, 0,
				0, 0, 1);
			return Rotate;
		}


		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> rotate(const Matrix<4, 4, Type>& m, Vector<3, Type> const& eularAngle) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Rotate(rotate(eularAngle));
			Matrix<4, 4, Type> Result;
			Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
			Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
			Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
			Result[3] = m[3];
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> rotate(const Matrix<4, 4, Type>& m, Quaternion<Type> const& quat) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Rotate(quatTomat3(quat));
			Matrix<4, 4, Type> Result;
			Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
			Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
			Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
			Result[3] = m[3];
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> rotate(const Matrix<4, 4, Type>& m, Type angle, Vector<3, Type> const& axis) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Rotate(rotate(angle, axis));
			Matrix<4, 4, Type> Result;
			Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
			Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
			Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
			Result[3] = m[3];
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> rotate(Vector<3, Type> const& eularAngle, const Matrix<4, 4, Type>& m) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Rotate(rotate(eularAngle));
			Matrix<4, 4, Type> Result;
			auto const& SrcA0 = Rotate[0];
			auto const& SrcA1 = Rotate[1];
			auto const& SrcA2 = Rotate[2];
			auto const& SrcB0 = m[0];
			auto const& SrcB1 = m[1];
			auto const& SrcB2 = m[2];
			auto const& SrcB3 = m[3];
			Result[0] = Vector<4, Type>(SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2], SrcB0[3]);
			Result[1] = Vector<4, Type>(SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2], SrcB1[3]);
			Result[2] = Vector<4, Type>(SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2], SrcB2[3]);
			Result[3] = Vector<4, Type>(SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2], SrcB3[3]);
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> rotate(Quaternion<Type> const& quat, const Matrix<4, 4, Type>& m) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Rotate(quatTomat3(quat));
			Matrix<4, 4, Type> Result;
			auto const& SrcA0 = Rotate[0];
			auto const& SrcA1 = Rotate[1];
			auto const& SrcA2 = Rotate[2];
			auto const& SrcB0 = m[0];
			auto const& SrcB1 = m[1];
			auto const& SrcB2 = m[2];
			auto const& SrcB3 = m[3];
			Result[0] = Vector<4, Type>(SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2], SrcB0[3]);
			Result[1] = Vector<4, Type>(SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2], SrcB1[3]);
			Result[2] = Vector<4, Type>(SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2], SrcB2[3]);
			Result[3] = Vector<4, Type>(SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2], SrcB3[3]);
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> rotate(Type angle, Vector<3, Type> const& axis, const Matrix<4, 4, Type>& m) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Rotate(rotate(angle, axis));
			Matrix<4, 4, Type> Result;
			auto const& SrcA0 = Rotate[0];
			auto const& SrcA1 = Rotate[1];
			auto const& SrcA2 = Rotate[2];
			auto const& SrcB0 = m[0];
			auto const& SrcB1 = m[1];
			auto const& SrcB2 = m[2];
			auto const& SrcB3 = m[3];
			Result[0] = Vector<4, Type>(SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2], SrcB0[3]);
			Result[1] = Vector<4, Type>(SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2], SrcB1[3]);
			Result[2] = Vector<4, Type>(SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2], SrcB2[3]);
			Result[3] = Vector<4, Type>(SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2], SrcB3[3]);
			return Result;
		}




		// Combine rotation and position to one matrix
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> model(Quaternion<Type> const& rotation, Vector<3, Type> const& position) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Result(rotate(rotation));
			Result[3] = Vector<4, Type>(position, static_cast<Type>(1));
			return Result;
		}

	}
}


namespace CraftEngine
{
	namespace math
	{
		/*
		 Clip Space Function 3D
		*/

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> lookAtRH(Vector<3, Type> const& eye, Vector<3, Type> const& center, Vector<3, Type> const& up) CRAFT_ENGINE_NOEXCEPT
		{
			Vector<3, Type> const f(normalize(eye - center));
			Vector<3, Type> const s(normalize(cross(up, f)));// right dir
			Vector<3, Type> const u(cross(f, s));//
			Matrix<4, 4, Type> Result(1);
			Result[0][0] = s.x;
			Result[1][0] = s.y;
			Result[2][0] = s.z;
			Result[0][1] = u.x;
			Result[1][1] = u.y;
			Result[2][1] = u.z;
			Result[0][2] = f.x;
			Result[1][2] = f.y;
			Result[2][2] = f.z;
			Result[3][0] = -dot(s, eye);
			Result[3][1] = -dot(u, eye);
			Result[3][2] = -dot(f, eye);
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> lookAtLH(Vector<3, Type> const& eye, Vector<3, Type> const& center, Vector<3, Type> const& up) CRAFT_ENGINE_NOEXCEPT
		{
			Vector<3, Type> const f(normalize(center - eye));// center - eye
			Vector<3, Type> const s(normalize(cross(up, f)));// right dir
			Vector<3, Type> const u(cross(f, s));//
			Matrix<4, 4, Type> Result(1);
			Result[0][0] = s.x;
			Result[1][0] = s.y;
			Result[2][0] = s.z;
			Result[0][1] = u.x;
			Result[1][1] = u.y;
			Result[2][1] = u.z;
			Result[0][2] = f.x;
			Result[1][2] = f.y;
			Result[2][2] = f.z;
			Result[3][0] = -dot(s, eye);
			Result[3][1] = -dot(u, eye);
			Result[3][2] = -dot(f, eye);
			return Result;
		}




		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> lookAt(Vector<3, Type> const& eye, Vector<3, Type> const& center, Vector<3, Type> const& up) CRAFT_ENGINE_NOEXCEPT
		{
			if (CRAFT_ENGINE_MATH_CLIP_CONTROL & CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_LH)
				return lookAtLH(eye, center, up);
			else
				return lookAtRH(eye, center, up);
		}


		template<typename Type> // 
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> perspectiveWorldRH_DeviceLH(Type fovy, Type aspect, Type zNear, Type zFar) CRAFT_ENGINE_NOEXCEPT
		{
			assert(abs(aspect - std::numeric_limits<Type>::epsilon()) > static_cast<Type>(0));
			Type const tanHalfFovy = tan(fovy / static_cast<Type>(2));
			Matrix<4, 4, Type> Result(static_cast<Type>(0));
			Result[0][0] = static_cast<Type>(1) / (aspect * tanHalfFovy);
			Result[1][1] = (static_cast<Type>(1) / (tanHalfFovy));
			Result[2][2] = -zFar / (zFar - zNear); // 
			Result[2][3] = -static_cast<Type>(1);
			Result[3][2] = -(zFar * zNear) / (zFar - zNear);
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> perspectiveWorldLH_DeviceLH(Type fovy, Type aspect, Type zNear, Type zFar) CRAFT_ENGINE_NOEXCEPT
		{
			assert(abs(aspect - std::numeric_limits<Type>::epsilon()) > static_cast<Type>(0));
			Type const tanHalfFovy = tan(fovy / static_cast<Type>(2));
			Matrix<4, 4, Type> Result(static_cast<Type>(0));
			Result[0][0] = static_cast<Type>(1) / (aspect * tanHalfFovy);
			Result[1][1] = (static_cast<Type>(1) / (tanHalfFovy));
			Result[2][2] = zFar / (zFar - zNear);
			Result[2][3] = static_cast<Type>(1);
			Result[3][2] = -(zFar * zNear) / (zFar - zNear);
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> perspectiveWorldRH_DeviceRH(Type fovy, Type aspect, Type zNear, Type zFar) CRAFT_ENGINE_NOEXCEPT
		{
			assert(abs(aspect - std::numeric_limits<Type>::epsilon()) > static_cast<Type>(0));
			Type const tanHalfFovy = tan(fovy / static_cast<Type>(2));
			Matrix<4, 4, Type> Result(static_cast<Type>(0));
			Result[0][0] = static_cast<Type>(1) / (aspect * tanHalfFovy);
			Result[1][1] = -(static_cast<Type>(1) / (tanHalfFovy)); // 修改为负值
			Result[2][2] = -zFar / (zFar - zNear); // 
			Result[2][3] = -static_cast<Type>(1); //
			Result[3][2] = -(zFar * zNear) / (zFar - zNear);
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> perspectiveWorldLH_DeviceRH(Type fovy, Type aspect, Type zNear, Type zFar) CRAFT_ENGINE_NOEXCEPT
		{
			assert(abs(aspect - std::numeric_limits<Type>::epsilon()) > static_cast<Type>(0));
			Type const tanHalfFovy = tan(fovy / static_cast<Type>(2));
			Matrix<4, 4, Type> Result(static_cast<Type>(0));
			Result[0][0] = static_cast<Type>(1) / (aspect * tanHalfFovy);
			Result[1][1] = -(static_cast<Type>(1) / (tanHalfFovy)); // 修改为负值
			Result[2][2] = zFar / (zFar - zNear);
			Result[2][3] = static_cast<Type>(1);
			Result[3][2] = -(zFar * zNear) / (zFar - zNear);
			return Result;
		}


		// * Perspective Projection Matrix, Left-Hand and Zero to One for Vulkan&GLSL Only
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> perspective(Type fovy, Type aspect, Type zNear, Type zFar) CRAFT_ENGINE_NOEXCEPT
		{
			if (CRAFT_ENGINE_MATH_CLIP_CONTROL & CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_LH)
				if (CRAFT_ENGINE_MATH_CLIP_CONTROL & CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_LH)
					return perspectiveWorldLH_DeviceLH(fovy, aspect, zNear, zFar);
				else
					return perspectiveWorldLH_DeviceRH(fovy, aspect, zNear, zFar); // vulkan
			else
				if (CRAFT_ENGINE_MATH_CLIP_CONTROL & CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_LH)
					return perspectiveWorldRH_DeviceLH(fovy, aspect, zNear, zFar); // opengl
				else
					return perspectiveWorldRH_DeviceRH(fovy, aspect, zNear, zFar);
		}



		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> orthoWorldRH_DeviceLH(Type left, Type right, Type top, Type bottom, Type zNear, Type zFar)
		{
			Matrix<4, 4, Type> Result(1);
			Result[0][0] = static_cast<Type>(2) / (right - left);
			Result[1][1] = static_cast<Type>(2) / (top - bottom);
			Result[2][2] = -static_cast<Type>(2) / (zFar - zNear);
			Result[3][0] = -(right + left) / (right - left);
			Result[3][1] = -(top + bottom) / (top - bottom);
			Result[3][2] = -zNear / (zFar - zNear); // zero-to-one
			return Result;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> orthoWorldRH_DeviceRH(Type left, Type right, Type top, Type bottom, Type zNear, Type zFar)
		{
			Matrix<4, 4, Type> Result(1);
			Result[0][0] = static_cast<Type>(2) / (right - left);
			Result[1][1] = -static_cast<Type>(2) / (top - bottom);//
			Result[2][2] = -static_cast<Type>(2) / (zFar - zNear);
			Result[3][0] = -(right + left) / (right - left);
			Result[3][1] = (top + bottom) / (top - bottom);//
			Result[3][2] = -zNear / (zFar - zNear); // zero-to-one
			return Result;
		}

		// * Ortho Projection Matrix, Left-Hand and Zero to One for Vulkan&GLSL Only
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> orthoWorldLH_DeviceRH(Type left, Type right, Type top, Type bottom, Type zNear, Type zFar) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Result(1);
			Result[0][0] = static_cast<Type>(2) / (right - left);
			Result[1][1] = -(static_cast<Type>(2) / (top - bottom)); // 修改为负值
			Result[2][2] = static_cast<Type>(1) / (zFar - zNear);
			Result[3][0] = -(right + left) / (right - left);
			Result[3][1] = (top + bottom) / (top - bottom); // 修改为负值
			Result[3][2] = -zNear / (zFar - zNear); // zero-to-one
			return Result;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> orthoWorldLH_DeviceLH(Type left, Type right, Type top, Type bottom, Type zNear, Type zFar) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<4, 4, Type> Result(1);
			Result[0][0] = static_cast<Type>(2) / (right - left);
			Result[1][1] = static_cast<Type>(2) / (top - bottom); // 修改为负值
			Result[2][2] = static_cast<Type>(1) / (zFar - zNear);
			Result[3][0] = -(right + left) / (right - left);
			Result[3][1] = -(top + bottom) / (top - bottom); // 修改为负值
			Result[3][2] = -zNear / (zFar - zNear); // zero-to-one
			return Result;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<4, 4, Type> ortho(Type left, Type right, Type top, Type bottom, Type zNear, Type zFar)
		{
			if (CRAFT_ENGINE_MATH_CLIP_CONTROL & CRAFT_ENGINE_MATH_CLIP_WORLD_SPACE_LH)
				if (CRAFT_ENGINE_MATH_CLIP_CONTROL & CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_LH)
					return orthoWorldLH_DeviceLH(left, right, top, bottom, zNear, zFar);
				else
					return orthoWorldLH_DeviceRH(left, right, top, bottom, zNear, zFar); // vulkan
			else
				if (CRAFT_ENGINE_MATH_CLIP_CONTROL & CRAFT_ENGINE_MATH_CLIP_DEVICE_SPACE_LH)
					return orthoWorldRH_DeviceLH(left, right, top, bottom, zNear, zFar); // opengl
				else
					return orthoWorldRH_DeviceRH(left, right, top, bottom, zNear, zFar);
		}



	}
}


namespace CraftEngine
{
	namespace math
	{

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> translate2D(Vector<2, Type> const& v, const Matrix<3, 3, Type>& m) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Result;
			Result[0] = m[0] + Vector<3, Type>(v * m[0][2], static_cast<Type>(0));
			Result[1] = m[1] + Vector<3, Type>(v * m[1][2], static_cast<Type>(0));
			Result[2] = m[2] + Vector<3, Type>(v * m[2][2], static_cast<Type>(0));
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> translate2D(const Matrix<3, 3, Type>& m, Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Result = m;
			Result[2] = m[0] * v[0] + m[1] * v[1] + m[2];
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> translate2D(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Result(1);
			Result[2] = Vector<3, Type>(v, static_cast<Type>(1));
			return Result;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> scale2D(Vector<2, Type> const& v, const Matrix<3, 3, Type>& m) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Result = m;
			Vector<3, Type> t = Vector<3, Type>(v, static_cast<Type>(1));
			Result[0] = m[0] * t;
			Result[1] = m[1] * t;
			Result[2] = m[2] * t;
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> scale2D(const Matrix<3, 3, Type>& m, Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Result(1);
			Result[0] = m[0] * v[0];
			Result[1] = m[1] * v[1];
			Result[2] = m[2];
			return Result;
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> scale2D(Vector<2, Type> const& v) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Result(1);
			Result[0][0] = v[0];
			Result[1][1] = v[1];
			return Result;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<2, 2, Type> rotate2D(Type angle) CRAFT_ENGINE_NOEXCEPT
		{
			auto c = math::cos(angle);
			auto s = math::sin(angle);
			Matrix<2, 2, Type> Rotate(
				c, s,
				-s, c);
			return Rotate;
		}
		template<typename Type>
		/*
		* rotate2D: left multiply
		* @param angle: angle
		* @param m:     src matrix
		*/
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> rotate2D(Type angle, const Matrix<3, 3, Type>& m) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<2, 2, Type> Rotate = rotate2D(angle);
			Matrix<3, 3, Type> Result;
			auto const& SrcA0 = Rotate[0];
			auto const& SrcA1 = Rotate[1];
			auto const& SrcB0 = m[0];
			auto const& SrcB1 = m[1];
			auto const& SrcB2 = m[2];
			Result[0] = Vector<3, Type>(SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1], SrcB0[2]);
			Result[1] = Vector<3, Type>(SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1], SrcB1[2]);
			Result[2] = Vector<3, Type>(SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1], SrcB2[2]);
			return Result;
		}

		template<typename Type>
		/*
		* rotate2D: right multiply
		* @param m:     src matrix
		* @param angle: angle
		*/
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> rotate2D(const Matrix<3, 3, Type>& m, Type angle) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Rotate(rotate2D(angle));
			Matrix<3, 3, Type> Result = m;
			Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1];
			Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1];
			Result[2] = m[2];
			return Result;
		}


		// Combine rotation and position to one matrix
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Matrix<3, 3, Type> model2D(Matrix<2, 2, Type> const& rotation, Vector<2, Type> const& position) CRAFT_ENGINE_NOEXCEPT
		{
			Matrix<3, 3, Type> Result(
				Vector<3, Type>(rotation[0], static_cast<Type>(0)),
				Vector<3, Type>(rotation[1], static_cast<Type>(0)),
				Vector<3, Type>(position, static_cast<Type>(1))
			);
			return Result;
		}


	}
}




namespace CraftEngine
{
	namespace math
	{

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> hsl2rgb(const Vector<3, Type>& color) CRAFT_ENGINE_NOEXCEPT
		{
			static_assert(std::numeric_limits<Type>::is_iec559, "only accept floating-point inputs");
			const Type& h = color[0];
			const Type& s = color[1];
			const Type& l = color[2];
			const Type q = l < 0.5 ? (l * (1 + s)) : (l + s - l * s);
			const Type p = 2 * l - q;
			Vector<3, Type> tc = Vector<3, Type>(h + 0.33333333333333333333, h, h - 0.33333333333333333333);
			for (int i = 0; i < 3; i++)
				if (tc[i] < 0)tc[i] = tc[i] + 1.0;
				else if (tc[i] > 0)tc[i] = tc[i] - 1.0;
			Vector<3, Type> c;
			for (int i = 0; i < 3; i++)
				if (tc[i] < (1.0 / 6.0))c[i] = p + ((q - p) * 6 * tc[i]);
				else if (tc[i] < (1.0 / 2.0))c[i] = q;
				else if (tc[i] < (2.0 / 3.0))c[i] = p + ((q - p) * 6 * (2.0 / 3.0 - tc[i]));
				else tc[i] = p;
			return c;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> hsv2rgb(const Vector<3, Type>& color) CRAFT_ENGINE_NOEXCEPT
		{
			static_assert(std::numeric_limits<Type>::is_iec559, "only accept floating-point inputs");
			const Type& h = color[0];
			const Type& s = color[1];
			const Type& v = color[2];
			const int hi = int((h * 6));
			const Type f = h * 6.0 - hi;
			float p = v * (1 - s);
			float q = v * (1 - f * s);
			float t = v * (1 - (1 - f) * s);
			switch (hi)
			{
			case 0: return Vector<3, Type>(v, t, p);
			case 1: return Vector<3, Type>(q, v, p);
			case 2: return Vector<3, Type>(p, v, t);
			case 3: return Vector<3, Type>(p, q, v);
			case 4: return Vector<3, Type>(t, p, v);
			case 5: return Vector<3, Type>(v, p, q);
			case 6:
			default: return Vector<3, Type>(v, t, p);
			}
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> rgb2hsl(const Vector<3, Type>& color) CRAFT_ENGINE_NOEXCEPT
		{
			static_assert(std::numeric_limits<Type>::is_iec559, "only accept floating-point inputs");
			const Type& r = color[0];
			const Type& g = color[1];
			const Type& b = color[2];
			const Type max = math::max(r, g, b);
			const Type min = math::min(r, g, b);
			const Type sum = max + min;
			const Type difference = max - min;
			Vector<3, Type> c;
			if (max == min)c[0] = 0;
			else if (max == r && g >= b)c[0] = 60.0 * (g - b) / difference;
			else if (max == r && g < b)c[0] = 60.0 * (g - b) / difference + 360.0;
			else if (max == g)c[0] = 60.0 * (b - r) / difference + 120.0;
			else if (max == b)c[0] = 60.0 * (r - g) / difference + 240.0;
			c[0] = c[0] / 360.0;
			c[1] = 0.5 * sum;
			if (c[1] == 0 || max == min) c[2] = 0;
			else if (c[1] <= 0.5) c[2] = difference / sum;
			else c[2] = difference / 2.0 - sum;
			return c;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> rgb2hsv(const Vector<3, Type>& color) CRAFT_ENGINE_NOEXCEPT
		{
			static_assert(std::numeric_limits<Type>::is_iec559, "only accept floating-point inputs");
			const Type& r = color[0];
			const Type& g = color[1];
			const Type& b = color[2];
			const Type max = math::max(r, g, b);
			const Type min = math::min(r, g, b);
			const Type sum = max + min;
			const Type difference = max - min;
			Vector<3, Type> c;
			if (max == min)c[0] = 0;
			else if (max == r && g >= b)c[0] = 60.0 * (g - b) / difference;
			else if (max == r && g < b)c[0] = 60.0 * (g - b) / difference + 360.0;
			else if (max == g)c[0] = 60.0 * (b - r) / difference + 120.0;
			else if (max == b)c[0] = 60.0 * (r - g) / difference + 240.0;
			c[0] = c[0] / 360.0;
			if (max == 0) c[1] = 0;
			else c[1] = difference / max;
			c[2] = max;
			return c;
		}

	}
}


namespace CraftEngine
{
	namespace math
	{
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> solve2(const Matrix<2, 2, Type>& coefficientMatrix, const Vector<2, Type>& constantVector) CRAFT_ENGINE_NOEXCEPT
		{
			static_assert(std::numeric_limits<Type>::is_iec559, "only accept floating-point inputs");
			auto cm = coefficientMatrix;
			auto cv = constantVector;
			ivec2 order = { 0,1 };
			if (cm[order[0]][0] == 0.0)
			{
				if (cm[order[1]][0] == 0.0)
					return Vector<2, Type>(0);
				order = { order[1], order[0] };
			}
			auto t = cm[order[1]][0] / cm[order[0]][0];
			cm[order[1]] -= cm[order[0]] * t;
			cv[order[1]] -= cv[order[0]] * t;
			if (cm[order[1]][1] == 0.0f)
				return Vector<2, Type>(0);
			Vector<2, Type> result;
			result[1] = cv[order[1]] / cm[order[1]][1];
			result[0] = (cv[order[0]] - result[1] * cm[order[0]][1]) / cm[order[0]][0];
			return result;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> solve3(const Matrix<3, 3, Type>& coefficientMatrix, const Vector<3, Type>& constantVector) CRAFT_ENGINE_NOEXCEPT
		{
			static_assert(std::numeric_limits<Type>::is_iec559, "only accept floating-point inputs");
			auto cm = coefficientMatrix;
			auto cv = constantVector;
			ivec3 order = { 0,1,2 };
			if (cm[order[0]][0] == 0.0)
			{
				if (cm[order[1]][0] == 0.0)
				{
					if (cm[order[2]][0] == 0.0)
						return Vector<3, Type>(0);
					order = { order[2], order[0], order[1] };
				}
				order = { order[1], order[0], order[2] };
			}
			auto t = cm[order[1]][0] / cm[order[0]][0];
			cm[order[1]] -= cm[order[0]] * t;
			cv[order[1]] -= cv[order[0]] * t;
			t = cm[order[2]][0] / cm[order[0]][0];
			cm[order[2]] -= cm[order[0]] * t;
			cv[order[2]] -= cv[order[0]] * t;
			if (cm[order[1]][1] == 0.0)
			{
				if (cm[order[2]][1] == 0.0)
					return Vector<3, Type>(0);
				order = { order[0], order[2], order[1] };
			}
			t = cm[order[2]][1] / cm[order[1]][1];
			cm[order[2]] -= cm[order[1]] * t;
			cv[order[2]] -= cv[order[1]] * t;
			if(cm[order[2]][2] == 0.0)
				return Vector<3, Type>(0);
			Vector<3, Type> result;
			result[2] = cv[order[2]] / cm[order[2]][2];
			result[1] = (cv[order[1]] - result[2] * cm[order[1]][2]) / cm[order[1]][1];
			result[0] = (cv[order[0]] - result[2] * cm[order[0]][2] - result[1] * cm[order[0]][1]) / cm[order[0]][0];
			return result;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<4, Type> solve4(const Matrix<4, 4, Type>& coefficientMatrix, const Vector<4, Type>& constantVector) CRAFT_ENGINE_NOEXCEPT
		{
			static_assert(std::numeric_limits<Type>::is_iec559, "only accept floating-point inputs");
			auto cm = coefficientMatrix;
			auto cv = constantVector;
			ivec4 order = { 0,1,2,3 };

			if (cm[order[0]][0] == 0)
			{
				if (cm[order[1]][0] == 0)
				{
					if (cm[order[2]][0] == 0)
					{
						if (cm[order[3]][0] == 0)
							return Vector<4, Type>(0);
						order = { order[3], order[0], order[1], order[2] };
					}
					order = { order[2], order[0], order[1], order[3] };
				}
				order = { order[1], order[0], order[2], order[3] };
			}
			auto t = cm[order[1]][0] / cm[order[0]][0];
			cm[order[1]] -= cm[order[0]] * t;
			cv[order[1]] -= cv[order[0]] * t;
			t = cm[order[2]][0] / cm[order[0]][0];
			cm[order[2]] -= cm[order[0]] * t;
			cv[order[2]] -= cv[order[0]] * t;
			t = cm[order[3]][0] / cm[order[0]][0];
			cm[order[3]] -= cm[order[0]] * t;
			cv[order[3]] -= cv[order[0]] * t;
			if (cm[order[1]][1] == 0)
			{
				if (cm[order[2]][1] == 0)
				{
					if (cm[order[3]][1] == 0)
						return Vector<4, Type>(0);
					order = { order[0], order[2], order[1], order[3] };
				}
				order = { order[0], order[3], order[1], order[2] };
			}
			t = cm[order[2]][1] / cm[order[1]][1];
			cm[order[2]] -= cm[order[1]] * t;
			cv[order[2]] -= cv[order[1]] * t;
			t = cm[order[3]][1] / cm[order[1]][1];
			cm[order[3]] -= cm[order[1]] * t;
			cv[order[3]] -= cv[order[1]] * t;
			if (cm[order[2]][2] == 0)
			{
				if (cm[order[3]][2] == 0)
					return Vector<4, Type>(0);
				order = { order[0], order[1], order[3], order[2] };
			}
			t = cm[order[3]][2] / cm[order[2]][2];
			cm[order[3]] -= cm[order[2]] * t;
			cv[order[3]] -= cv[order[2]] * t;
			if (cm[order[3]][3] == 0)
				return Vector<4, Type>(0);
			Vector<4, Type> result;
			result[3] = cv[order[3]] / cm[order[3]][3];
			result[2] = (cv[order[2]] - result[3] * cm[order[2]][3]) / cm[order[2]][2];
			result[1] = (cv[order[1]] - result[3] * cm[order[1]][3] - result[2] * cm[order[1]][2]) / cm[order[1]][1];
			result[0] = (cv[order[0]] - result[3] * cm[order[0]][3] - result[2] * cm[order[0]][2] - result[1] * cm[order[0]][1]) / cm[order[0]][0];
			return result;
		}

	}
}


namespace CraftEngine
{
	namespace math
	{

		CRAFT_ENGINE_MATH_FUNC_DECL uint32_t packUnorm2x16(vec2 v)
		{
			union PackStructure
			{
				uint16_t unpackedData[2];
				uint32_t packedData;
			};
			static_assert(sizeof(PackStructure) == 4, "error");
			PackStructure p;
			for (int i = 0; i < 2; i++)
				p.unpackedData[i] = v[i] * 65535;
			return p.packedData;
		}

		CRAFT_ENGINE_MATH_FUNC_DECL vec2 unpackUnorm2x16(uint32_t v)
		{
			union PackStructure
			{
				uint16_t unpackedData[2];
				uint32_t packedData;
			};
			static_assert(sizeof(PackStructure) == 4, "error");
			vec2 d;
			PackStructure p;
			p.packedData = v;
			for (int i = 0; i < 2; i++)
				d[i] = p.unpackedData[i] * (1.0f / 65535.0f);
			return d;
		}

		CRAFT_ENGINE_MATH_FUNC_DECL uint32_t packSnorm2x16(vec2 v)
		{
			union PackStructure
			{
				int16_t unpackedData[2];
				uint32_t packedData;
			};
			static_assert(sizeof(PackStructure) == 4, "error");
			PackStructure p;
			for (int i = 0; i < 2; i++)
				p.unpackedData[i] = v[i] * 32768;
			return p.packedData;
		}

		CRAFT_ENGINE_MATH_FUNC_DECL vec2 unpackSnorm2x16(uint32_t v)
		{
			union PackStructure
			{
				int16_t unpackedData[2];
				uint32_t packedData;
			};
			static_assert(sizeof(PackStructure) == 4, "error");
			vec2 d;
			PackStructure p;
			p.packedData = v;
			for (int i = 0; i < 2; i++)
				d[i] = p.unpackedData[i] * (1.0f / 32768.0f);
			return d;
		}

		CRAFT_ENGINE_MATH_FUNC_DECL uint32_t packUnorm4x8(vec4 v)
		{
			union PackStructure
			{
				uint8_t unpackedData[4];
				uint32_t packedData;
			};
			static_assert(sizeof(PackStructure) == 4, "error");
			PackStructure p;
			for (int i = 0; i < 4; i++)
				p.unpackedData[i] = v[i] * 255;
			return p.packedData;
		}

		CRAFT_ENGINE_MATH_FUNC_DECL vec4 unpackUnorm4x8(uint32_t v)
		{
			union PackStructure
			{
				uint8_t unpackedData[4];
				uint32_t packedData;
			};
			static_assert(sizeof(PackStructure) == 4, "error");
			vec4 d;
			PackStructure p;
			p.packedData = v;
			for (int i = 0; i < 4; i++)
				d[i] = p.unpackedData[i] * (1.0f / 255.0f);
			return d;
		}

		CRAFT_ENGINE_MATH_FUNC_DECL uint32_t packSnorm4x8(vec4 v)
		{
			union PackStructure
			{
				int8_t unpackedData[4];
				uint32_t packedData;
			};
			static_assert(sizeof(PackStructure) == 4, "error");
			PackStructure p;
			for (int i = 0; i < 4; i++)
				p.unpackedData[i] = v[i] * 128;
			return p.packedData;
		}

		CRAFT_ENGINE_MATH_FUNC_DECL vec4 unpackSnorm4x8(uint32_t v)
		{
			union PackStructure
			{
				uint8_t unpackedData[4];
				int32_t packedData;
			};
			static_assert(sizeof(PackStructure) == 4, "error");
			vec4 d;
			PackStructure p;
			p.packedData = v;
			for (int i = 0; i < 4; i++)
				d[i] = p.unpackedData[i] * (1.0f / 128.0f);
			return d;
		}

		//uint32_t packHalf2x16(const vec2& v)
		//{

		//}

		//vec2 unpackHalf2x16(uint32_t v)
		//{

		//}

		//uint64_t packHalf4x16(const vec4& v)
		//{

		//}

		//vec4 unpackHalf4x16(uint64_t v)
		//{

		//}
	}
}


namespace CraftEngine
{
	namespace math
	{

		/*
			template parameter:
			C: control point count
			L: vector length (DEFAULT = 2)
			T: vector element type (DEFAULT = float)
		*/
		template<size_t C, size_t L = 2, typename T = float>
		class BezierCurve
		{
		public:
			using val_t = T;
			using vec_t = Vector<L, T>;

			BezierCurve(const vec_t points[C])
			{
				for (int i = 0; i < order(); i++)
					m_controlPoints[i] = points[i];
			}

			vec_t pointAt(val_t t)
			{
				val_t a = t;
				val_t b = static_cast<val_t>(1) - t;
				val_t ab = a * b;
				val_t aa = a * a;
				val_t p[order()];
				p[0] = b;
				p[1] = a;
				for (int i = 2; i < order(); i++) {
					for (int j = 0; j < i; j++)
						p[j] *= b;
					p[i - 1] += ab;
					p[i] = aa;
				}
				vec_t r = vec_t(0);
				for (int i = 0; i < order(); i++)
					r += m_controlPoints[i] * p[i];
				return r;
			}

			static constexpr size_t order() { return C; }
			static constexpr size_t degree() { return C - 1; }
			static constexpr size_t size() { return C; }
			const vec_t& controlPoint(int idx) const { return m_controlPoints[idx]; }
		private:
			vec_t m_controlPoints[C];
		};



		/*
			template parameter:
			C: control point count
			N: degree (equals to [order - 1]) (DEFAULT = 3) [0, +..]
			L: vector length (DEFAULT = 2)
			T: vector element type (DEFAULT = float)
		*/
		template<size_t C, size_t N = 3, size_t L = 2, typename T = float>
		class BSplineCurve
		{
		public:
			using val_t = T;
			using vec_t = Vector<L, T>;

			BSplineCurve(const vec_t points[C])
			{
				for (int i = 0; i < size(); i++)
					m_controlPoints[i] = points[i];
				constexpr int nKnots = size() + order(); // 节点数=控制点数+次数（或阶数+1）n+k
				constexpr val_t step = static_cast<val_t>(1) / (size() + static_cast<val_t>(1) - order());
				for (int i = 0; i < nKnots; i++) {
					if (i < order()) m_knots[i] = 0;
					else if (i < size() + 1) m_knots[i] = m_knots[i - 1] + step;
					else m_knots[i] = 1;
				}
			}

			vec_t pointAt(val_t t)
			{
				vec_t r = vec_t(0);
				val_t u = t;
				for (int i = 0; i < size(); i++) {
					val_t Base_ik = deBoor_Cox(u, degree(), i);
					if (Base_ik > 0)
						r += m_controlPoints[i] * Base_ik;
				}
				return r;
			}

			static constexpr size_t order() { return N + 1; }
			static constexpr size_t degree() { return N; }
			static constexpr size_t size() { return C; }
			const vec_t& controlPoint(int idx) const { return m_controlPoints[idx]; }
		private:
			val_t deBoor_Cox(val_t u, int k, int i)
			{
				if (k == 0) {
					if (u >= m_knots[i] && u <= m_knots[i + 1]) return 1;
					else return 0;
				}
				else {
					val_t coef_1 = static_cast<val_t>(0);
					if ((m_knots[i + k] - m_knots[i] == 0)) coef_1 = 0;
					else coef_1 = (u - m_knots[i]) / (m_knots[i + k] - m_knots[i]);
					val_t coef_2 = static_cast<val_t>(0);
					if ((m_knots[i + k + 1] - m_knots[i + 1] == 0)) coef_2 = 0;
					else coef_2 = (m_knots[i + k + 1] - u) / (m_knots[i + k + 1] - m_knots[i + 1]);
					return coef_1 * deBoor_Cox(u, k - 1, i) + coef_2 * deBoor_Cox(u, k - 1, i + 1);
				}
			}

			vec_t m_controlPoints[C];
			val_t m_knots[C + N + 1];
		};


		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> distance(Vector<2, Type> const& a, Vector<2, Type> const& b) { return length(a - b); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> distance(Vector<3, Type> const& a, Vector<3, Type> const& b) { return length(a - b); }

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> reflect(Vector<2, Type> const& I, Vector<2, Type> const& N) { return I - N * dot(N, I) * Type(2); }
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> reflect(Vector<3, Type> const& I, Vector<3, Type> const& N) { return I - N * dot(N, I) * Type(2); }

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> refract(Vector<2, Type> const& I, Vector<2, Type> const& N, Type eta)
		{
			Type const dotValue(dot(N, I));
			Type const k(static_cast<Type>(1) - eta * eta * (static_cast<Type>(1) - dotValue * dotValue));
			return (eta * I - (eta * dotValue + sqrt(k)) * N) * static_cast<Type>(k >= static_cast<Type>(0));
		}
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> refract(Vector<3, Type> const& I, Vector<3, Type> const& N, Type eta)
		{
			Type const dotValue(dot(N, I));
			Type const k(static_cast<Type>(1) - eta * eta * (static_cast<Type>(1) - dotValue * dotValue));
			return (eta * I - (eta * dotValue + sqrt(k)) * N) * static_cast<Type>(k >= static_cast<Type>(0));
		}
		/*
		 x, y must be normalized.
		*/
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type angle(Vector<2, Type> const& x, Vector<2, Type> const& y)
		{
			return acos(clamp(dot(x, y), Type(-1), Type(1)));
		}
		/*
		 x, y must be normalized.
		*/
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type angle(Vector<3, Type> const& x, Vector<2, Type> const& y)
		{
			return acos(clamp(dot(x, y), Type(-1), Type(1)));
		}
		/*
		 x, y must be normalized.
		*/
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type orientedAngle(Vector<2, Type> const& x, Vector<2, Type> const& y)
		{
			Type const Angle(acos(clamp(dot(x, y), Type(-1), Type(1))));
			return cross(x, y) > Type(0) ? Angle : -Angle;
		}
		/*
		 x, y must be normalized.
		*/
		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type orientedAngle(Vector<3, Type> const& x, Vector<3, Type> const& y, Vector<3, Type> const& ref)
		{
			Type const Angle(acos(clamp(dot(x, y), Type(-1), Type(1))));
			return dot(ref, cross(x, y)) > Type(0) ? Angle : -Angle;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> lineClosestPoint
		(
			Vector<3, Type> const& point,
			Vector<3, Type> const& a,
			Vector<3, Type> const& b
		)
		{
			Type l = distance(a, b);
			Vector<3, Type> v = point - a;
			Vector<3, Type> dir = (b - a) / l;
			Type d = dot(v, dir);
			if (d <= Type(0)) return a;
			if (d >= l) return b;
			return a + dir * d;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> lineClosestPoint
		(
			Vector<2, Type> const& point,
			Vector<2, Type> const& a,
			Vector<2, Type> const& b
		)
		{
			Type l = distance(a, b);
			Vector<2, Type> v = point - a;
			Vector<2, Type> dir = (b - a) / l;
			Type d = dot(v, dir);
			if (d <= Type(0)) return a;
			if (d >= l) return b;
			return a + dir * d;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> lineIntersection(
			Vector<2, Type> const& p1,
			Vector<2, Type> const& p2,
			Vector<2, Type> const& q1,
			Vector<2, Type> const& q2
		)
		{
			Matrix<4, 4, Type> cm;
			cm[0] = Vector<4, Type>(1, 0, p1.x - p2.x, 0);
			cm[1] = Vector<4, Type>(0, 1, p1.y - p2.y, 0);
			cm[2] = Vector<4, Type>(1, 0, 0, q1.x - q2.x);
			cm[3] = Vector<4, Type>(0, 1, 0, q1.y - q2.y);
			Vector<4, Type> cf = Vector<4, Type>(p1.x, p1.y, q1.x, q1.y);
			auto result = solve4(cm, cf);
			return result.xy;
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> lineIntersectionX(
			Vector<2, Type> const& p1,
			Vector<2, Type> const& p2,
			Type x
		)
		{
			Type k1 = x;
			auto d = p1 - p2;
			Type k2 = p1.y + (k1 - p1.x) * d.y / d.x;
			return Vector<2, Type>(k1, k2);
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<2, Type> lineIntersectionY(
			Vector<2, Type> const& p1,
			Vector<2, Type> const& p2,
			Type y
		)
		{
			Type k1 = y;
			auto d = p1 - p2;
			Type k2 = p1.x + (k1 - p1.y) * d.x / d.y;
			return Vector<2, Type>(k2, k1);
		}

		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Vector<3, Type> lineSurfaceIntersection(
			Vector<3, Type> const& a,
			Vector<3, Type> const& b,
			Vector<3, Type> const& p,
			Vector<3, Type> const& n
		)
		{
			Matrix<4, 4, Type> cm;
			cm[0] = Vector<4, Type>(p.x, 0, 0, a.x - b.x);
			cm[1] = Vector<4, Type>(0, p.y, 0, a.y - b.y);
			cm[2] = Vector<4, Type>(0, 0, p.z, a.z - b.z);
			cm[3] = Vector<4, Type>(n, 0);
			Vector<4, Type> cf = Vector<4, Type>(a.x, a.y, a.z, dot(p, n));
			auto result = solve4(cm, cf);
			return result.xyz;
		}



		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR std::tuple<Vector<3, Type>, Vector<3, Type>> surfaceIntersection(
			Vector<3, Type> const& p1,
			Vector<3, Type> const& n1,
			Vector<3, Type> const& p2,
			Vector<3, Type> const& n2
		)
		{

			return {  };
		}


		union AABB
		{
			struct { vec3 mMin, mMax; };
			struct { float xMin, yMin, zMin, xMax, yMax, zMax; };
		};



		template<typename Type>
		CRAFT_ENGINE_MATH_FUNC_DECL CRAFT_ENGINE_CONSTEXPR Type lineAABBIntersection(
			Vector<3, Type> const& p,
			Vector<3, Type> const& n,
			AABB const& aabb
		)
		{

			return 0;
		}


	}
}

#endif // !CRAFT_ENGINE_INCLUDED