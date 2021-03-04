#pragma once
#include "./codecvt/Codecvt.h"

namespace CraftEngine
{
	namespace core
	{



		typedef wchar_t Char;
		typedef std::wstring String;

		class StringRef
		{
		private:
			const Char* m_data;
			uint32_t m_size;
		public:
			StringRef() :m_data(nullptr), m_size(0) {}
			StringRef(const String& string) :m_data(string.data()), m_size(string.size()) {}
			StringRef(const String& string, uint32_t count, uint32_t offset) :m_data(string.data() + offset), m_size(count) {}
			StringRef(const Char* pString, uint32_t size) :m_data(pString), m_size(size) {}

			uint32_t size()const { return m_size; }
			const Char* data()const { return m_data; }
			const Char& operator[](uint32_t index)const { return m_data[index]; }
			const Char& at(uint32_t index)const { return m_data[index]; }
			StringRef substr_ref(uint32_t count, uint32_t offset)const { return StringRef(data() + offset, count); }
		};

		class StringTool
		{
		public:

#ifdef _WIN32
			static String fromUtf8(const void* byte_array, uint32_t byte_count)
			{
				std::wstring ws = codecvt::utf8_to_utf16le((const char*)byte_array, byte_count);
				return fromStdWString(ws);
			}

			static String fromUtf8(const std::string& utf8str)
			{
				return fromUtf8(utf8str.c_str(), utf8str.size());
			}
			static String fromUtf16(const std::wstring& utf16str)
			{
				return utf16str;
			}

			static const String& fromStdWString(const std::wstring& wstr)
			{
				return wstr;
			}

			static std::string toUtf8(const String& str)
			{
				std::string&& ws = codecvt::utf16le_to_utf8(str);
				return ws;
			}

#endif

			template<typename Type>
			static String fromValue(Type val)
			{
				return fromStdWString(std::to_wstring(val));
			}

			template<class Type>
			static bool toValue(const String& str, Type* pValueOutput)
			{
				if (pValueOutput == nullptr)
					return false;
				int& errno_ref = errno; // Nonzero cost, pay it once
				wchar_t* end_ptr;
				errno_ref = 0;
				const double double_value = std::wcstod(str.c_str(), &end_ptr);
				if (str.c_str() == end_ptr)
					return false;
				Type val = Type(double_value);
				*pValueOutput = val;
				return true;
			}

			template<typename Type>
			static String fromValue(Type val, const String& format)
			{
				wchar_t val_str[256];
				if (swprintf_s(val_str, 256, format.c_str(), val))
					return String(val_str);
				return String();
			}


		};



	}
}