#pragma once
#ifndef CRAFT_ENGINE_COMMON_H_
#define CRAFT_ENGINE_COMMON_H_

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <stack>
#include <queue>
#include <functional>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <typeindex>
#include <fstream>
#include <string>
#include <atomic>
#include <list>
#include <sstream>
#include <stdexcept>
#include <exception>
#include <regex>

#define CRAFT_ENGINE_STATIC static
#define CRAFT_ENGINE_EXPLICIT explicit
#define CRAFT_ENGINE_CONSTEXPR constexpr
#define CRAFT_ENGINE_INLINE inline
#define CRAFT_ENGINE_NOEXCEPT noexcept

#define craft_engine_make_handle(type_name) \
struct type_name \
{ \
	void* unused; \
	type_name() CRAFT_ENGINE_NOEXCEPT :unused(nullptr) {} \
	CRAFT_ENGINE_EXPLICIT type_name(void* ptr) CRAFT_ENGINE_NOEXCEPT : unused(ptr) {} \
	operator void* ()const CRAFT_ENGINE_NOEXCEPT { return unused; } \
};

#define craft_engine_make_date(year, month, day) (year << 16 & month << 8 & day)


#define craft_engine_error(system_name, object_id, method_id, code, message) \
	throw std::runtime_error((system_name) + std::to_string(object_id) + std::to_string(method_id) + std::to_string(code) + (message))

#define craft_engine_error_v2(message) craft_engine_error("unknown" , 0, 0, -1, message)












namespace CraftEngine
{


}

#define USING_CRAFT_ENGINE using namespace CraftEngine;


#endif // !CRAFT_ENGINE_COMMON_H_