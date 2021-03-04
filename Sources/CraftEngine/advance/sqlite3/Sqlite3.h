#pragma once
#include "../../3rdparty/ThirdPartyImportSqlite3.h"

#include "../../util/Core.h"
#include <vector>
#include <string>
#include <stdexcept>

namespace CraftEngine
{
	namespace sqlite
	{

		typedef int64_t              SqliteInteger64;
		typedef double               SqliteDouble;
		typedef std::string          SqliteText;
		typedef std::vector<uint8_t> SqliteBlob;
		typedef nullptr_t            SqliteNull;
		typedef util::Variant<
			SqliteInteger64,
			SqliteDouble,
			SqliteBlob,
			SqliteText,
			SqliteNull
		> SqliteValue;
	

		class SqliteDB
		{
		private:

			sqlite3* m_sqlite3 = nullptr;

		public:

			struct SQLStatement
			{
			private:
				friend class SqliteDB;
				void* unused;
				SQLStatement(void* v):unused(v)
				{
					
				}

				bool isValid()
				{
					return unused != nullptr;
				}
			};

			bool open(const char* filename)
			{
				int code = sqlite3_open(filename, &m_sqlite3);
				if (code == SQLITE_OK)
					return true;
				else
				{
					return false;
				}
			}

			bool close() throw (std::runtime_error)
			{
				if (m_sqlite3 != nullptr)
				{
					int code = sqlite3_close(m_sqlite3);
					while (code == SQLITE_BUSY)
					{
						sqlite3_stmt* stmt = sqlite3_next_stmt(m_sqlite3, nullptr);
						if (stmt == nullptr)
							break;
						code = sqlite3_finalize(stmt);
						if (code == SQLITE_OK)
							code = sqlite3_close(m_sqlite3);
						else
							throw std::runtime_error("");
					}
					return true;
				}
				return false;
			}

			bool execute(const std::string& sql)
			{
				auto code = sqlite3_exec(m_sqlite3, sql.c_str(), nullptr, nullptr, nullptr);
				if (code == SQLITE_OK)
					return true;
				else
				{
					return false;
				}
			}

			template<typename... Args>
			bool execute(const std::string& sql, Args&&... args)
			{
				SQLStatement stmt = prepareStmt(sql);
				auto code = bindStmtParams(stmt, args);
				bool success = false;
				if (code == SQLITE_OK)
					success = execute(stmt);
				releaseStmt(stmt);
				return success;
			}

			bool execute(SQLStatement stmt)
			{
				auto code = sqlite3_step((sqlite3_stmt*)stmt.unused);
				if (code != SQLITE_DONE)
					return false;
				return true;
			}



			template<typename ResultType = int64_t>
			ResultType executeScalar(const std::string& sql)
			{
				static_assert((
					std::is_same<ResultType, int64_t>::value ||
					std::is_same<ResultType, double>::value ||
					std::is_same<ResultType, std::string>::value ||
					std::is_same<ResultType, std::vector<uint8_t>>::value
					), "unsupport type.");
				ResultType result = ResultType();
				SQLStatement stmt = prepareStmt(sql);
				if (!stmt.isValid())
				{
					throw std::runtime_error("sql syntax error.");
				}
				auto code = sqlite3_step((sqlite3_stmt*)stmt.unused);
				if (code == SQLITE_ROW)
				{
					result = getStmtValue(stmt, 0).get<ResultType>();
				}
				releaseStmt(stmt);
				return result;
			}


			SqliteValue getStmtValue(SQLStatement stmt, int index)
			{
				sqlite3_stmt* s = (sqlite3_stmt*)stmt.unused;
				int column_type = sqlite3_column_type((sqlite3_stmt*)stmt.unused, index);
				switch (column_type)
				{
				case SQLITE_INTEGER:		
					return sqlite3_column_int64(s, index); break;
				case SQLITE_FLOAT:
					return sqlite3_column_double(s, index); break;
				case SQLITE_TEXT:
					return std::string((const char*)sqlite3_column_text(s, index)); break;
				case SQLITE_BLOB:
				{
					std::vector<uint8_t> data(strlen((const char*)sqlite3_column_blob(s, index)));
					memcpy(data.data(), sqlite3_column_blob(s, index), data.size());
					return data;
					break;
				}
				case SQLITE_NULL:return nullptr; break;
				default:return nullptr;
				}
			}



			template<typename Type, typename... Args>
			int bindStmtParams(SQLStatement stmt, Args&&... args)
			{
				return _Bind_Params((sqlite3_stmt*)stmt.unused, 1, args);
			}

			SQLStatement prepareStmt(const std::string& sql)
			{
				sqlite3_stmt* stmt = nullptr;
				auto code = sqlite3_prepare_v2(m_sqlite3, sql.c_str(), -1, &stmt, nullptr);
				if (code != SQLITE_OK)
					return SQLStatement(nullptr);
				else
					return SQLStatement(stmt);
			}

			int releaseStmt(SQLStatement stmt)
			{
				return sqlite3_finalize((sqlite3_stmt*)stmt.unused);
			}
		private:


			int _Bind_Params(sqlite3_stmt* stmt, int index)
			{
				return SQLITE_OK;
			}

			template<typename Type, typename... Args>
			int _Bind_Params(sqlite3_stmt* stmt, int index, Type&& value, Args&&... args)
			{
				auto code = _Bind_Value(stmt, index, value);
				if (code != SQLITE_OK)
				{
					return code;
				}
				_Bind_Params(stmt, index + 1, std::forward<Args>(args)...);
				return code;
			}

			template<typename Type>
			typename std::enable_if<std::is_same<Type, int32_t>::value, int>::type
				_Bind_Value(sqlite3_stmt* stmt, int index, Type value)
			{
				return sqlite3_bind_int(stmt, index, std::forward<Type>(value));
			}

			template<typename Type>
			typename std::enable_if<std::is_same<Type, int64_t>::value, int>::type
				_Bind_Value(sqlite3_stmt* stmt, int index, Type value)
			{
				return sqlite3_bind_int64(stmt, index, std::forward<Type>(value));
			}

			template<typename Type>
			typename std::enable_if<std::is_floating_point<Type>::value, int>::type
				_Bind_Value(sqlite3_stmt* stmt, int index, Type value)
			{
				return sqlite3_bind_double(stmt, index, std::forward<Type>(value));
			}

			template<typename Type>
			typename std::enable_if<std::is_same<Type, std::string>::value, int>::type
				_Bind_Value(sqlite3_stmt* stmt, int index, const Type& value)
			{
				return sqlite3_bind_text(stmt, index, value.data(), value.length(), SQLITE_TRANSIENT);
			}

			template<typename Type>
			typename std::enable_if<std::is_same<Type, char*>::value || std::is_same<Type, const char*>::value, int>::type
				_Bind_Value(sqlite3_stmt* stmt, int index, Type value)
			{
				return sqlite3_bind_text(stmt, index, value, strlen(value), SQLITE_TRANSIENT);
			}

			template<typename Type>
			typename std::enable_if<std::is_same<Type, std::vector<uint8_t>>::value, int>::type
				_Bind_Value(sqlite3_stmt* stmt, int index, const Type& value)
			{
				return sqlite3_bind_blob(stmt, index, value.data(), value.size(), SQLITE_TRANSIENT);
			}

			template<typename Type>
			typename std::enable_if<std::is_same<Type, nullptr_t>::value, int>::type
				_Bind_Value(sqlite3_stmt* stmt, int index, Type value)
			{
				return sqlite3_bind_null(stmt, index, value);
			}
		};




	}
}

