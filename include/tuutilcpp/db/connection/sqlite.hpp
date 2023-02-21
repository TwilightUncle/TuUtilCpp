///----------------------------------
/// @file db/query/sqlite.hpp
/// @brief sqliteクエリ生成
/// @details 本ファイルを使用する場合、sqlite3.hがインクルード可能で、sqlite3.libがリンク済みでない場合、未定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP

#if __has_include(<SQLiteCpp/SQLiteCpp.h>)  // SQLiteCppのヘッダが読み込めなければいけない
#include <SQLiteCpp/SQLiteCpp.h>
#include <tuutilcpp/str.hpp>

namespace tuutil::db::connection
{
    /**
     * @class
     * @brief sqlitecppのラッパー
     * @tparam DbFileName
    */
    template <str::cstr DbFileName>
    class sqlite
        : public SQLite::Database
    {
    public:
        sqlite()
            : SQLite::Database(DbFileName, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
        {}

        /**
         * @brief select count(*) from [table name] ...を引数に指定する
        */
        int count(const std::string& query)
        {
            SQLite::Statement st(this->db, query);
            return st.getColumn(0).getInt();
        }
    };
}

#else
    # warning <SQLiteCpp/SQLiteCpp.h> is not found
#endif  // __has_include(<SQLiteCpp/SQLiteCpp.h>)

#endif // TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP
