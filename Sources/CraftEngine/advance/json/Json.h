#pragma once
#include "../../3rdparty/ThirdPartyImportRapidJSON.h"

#include <string>

namespace CraftEngine
{
	namespace json
	{

		class JsonFile
		{
		private:
			typedef rapidjson::Writer<rapidjson::StringBuffer> Writer;
			rapidjson::StringBuffer m_buf;
			Writer m_writer;
			rapidjson::Document m_doc;
		public:

			JsonFile() : m_writer(m_buf)
			{

			}

			void startArray()
			{
				m_writer.StartArray();
			}
			void endArray()
			{
				m_writer.EndArray();
			}

			void startObject()
			{
				m_writer.StartObject();
			}
			void endObject()
			{
				m_writer.EndObject();
			}

			template<typename Type>
			void write(const char* key, Type&& value)
			{
				m_writer.String(key);
				_Write_Value(value);
			}
			
			template<typename Type>
			void write(const std::string& key, Type&& value)
			{
				m_writer.String(key);
				_Write_Value(value);
			}


			template<typename Type>
			void value(Type&& value)
			{
				_Write_Value(value);
			}

			void key(const char* key)
			{
				m_writer.Key(key);
			}
			void key(const std::string& key)
			{
				m_writer.Key(key.c_str());
			}

			const char* getString()
			{
				return m_buf.GetString();
			}

			const rapidjson::Document& getDoc()
			{
				return m_doc;
			}

		private:
			template<typename Type>
			typename std::enable_if<std::is_same<Type, int32_t>::value, void>::type
				_Write_Value(Type value) { m_writer.Int(value); }
			template<typename Type>
			typename std::enable_if<std::is_same<Type, uint32_t>::value, void>::type
				_Write_Value(Type value) { m_writer.Uint(value); }
			template<typename Type>
			typename std::enable_if<std::is_same<Type, int64_t>::value, void>::type
				_Write_Value(Type value) { m_writer.Int64(value); }
			template<typename Type>
			typename std::enable_if<std::is_same<Type, uint64_t>::value, void>::type
				_Write_Value(Type value) { m_writer.Uint64(value); }
			template<typename Type>
			typename std::enable_if<std::is_floating_point<Type>::value, void>::type
				_Write_Value(Type value) { m_writer.Double(value); }
			template<typename Type>
			typename std::enable_if<std::is_same<Type, bool>::value, void>::type
				_Write_Value(Type value) { m_writer.Bool(value); }
			template<typename Type>
			typename std::enable_if<std::is_same<Type, std::string>::value, void>::type
				_Write_Value(const Type& value) { m_writer.String(value.c_str(), value.size(), true); }
			template<typename Type>
			typename std::enable_if<std::is_pointer<Type>::value, void>::type
				_Write_Value(Type value) { m_writer.String(value); }
		};





		template<typename T>
		bool writeJson(JsonFile& file, T& obj);
		template<typename T>
		bool readJson(const rapidjson::Value& value, const T& obj);



		template<typename T>
		bool fromJson(const std::string& json, T& obj)
		{
			rapidjson::Document doc;
			doc.Parse(json.c_str());
			if (doc.HasParseError())
				return false;
			return readJson(doc, obj);
		}
		template<typename T>
		bool toJson(const T& obj, std::string& json)
		{
			JsonFile& file;
			auto success = writeJson(file, obj);
			if (!success)
				return false;
			json = file.getString();
			return true;
		}



	}
}

