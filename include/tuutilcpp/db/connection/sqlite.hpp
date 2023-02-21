///----------------------------------
/// @file db/query/sqlite.hpp
/// @brief sqlite�N�G������
/// @details �{�t�@�C�����g�p����ꍇ�Asqlite3.h���C���N���[�h�\�ŁAsqlite3.lib�������N�ς݂łȂ��ꍇ�A����`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP

#if __has_include(<SQLiteCpp/SQLiteCpp.h>)  // SQLiteCpp�̃w�b�_���ǂݍ��߂Ȃ���΂����Ȃ�
#include <SQLiteCpp/SQLiteCpp.h>
#include <tuutilcpp/str.hpp>

namespace tuutil::db::connection
{
    /**
     * @class
     * @brief sqlitecpp�̃��b�p�[
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
         * @brief select count(*) from [table name] ...�������Ɏw�肷��
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
