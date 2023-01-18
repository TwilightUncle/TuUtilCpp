///----------------------------------
/// @file type.hpp
/// @brief DBフィールド用の型定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_SQLITE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_SQLITE_HPP

#include <tuutilcpp/str.hpp>
#include <tuutilcpp/db.hpp>
#include <tuutilcpp/db/connection/sqlite.hpp>
#include <tuutilcpp/db/query/sqlite.hpp>

namespace tuutil::db
{
    /**
     * @class
     * @brief sqliteについて指定したテーブル定義とDB名をもとに接続とクエリ作成をカプセル化したもの
     * @tparam DbName データベース名称
     * @tparam TableDefinitionList テーブル定義型
    */
    template <str::cstr DbName, TableListDefinable TableDefinitionList>
    struct sqlite
    {
        static constexpr auto name = DbName;

        /**
         * @fn
         * @brief TableDefinitionに含まれる全てのcreate文を発行する
        */
        void create_table_all() const
        {
            for (auto s : create_querys_v) this->con.exec<nullptr>(std::string{s}, nullptr, nullptr);
        }

        /**
         * @fn
         * @brief 指定したID型のテーブルが存在しているか判定
         * @tparam テーブル識別子の列挙型
        */
        template <mpl::Enumeration ETableType>
        bool exists_table() const
        {
            using table_definition_type = get_table_def_t<ETableType ,TableDefinitionList>;
            static_assert(!std::is_same_v<table_definition_type, mpl::ignore_type>, "Not found database table definition.");
            return bool(this->count(std::string(query::sqlite::make_exists_table_string_t_v<table_definition_type>)));
        }

        /**
         * @fn
         * @brief countのクエリを発行
        */
        int count(const std::string& query) const
        {
            constexpr auto callback = [](void* out, int, char** result, char**) {
                auto cnt = (int*)out;
                *cnt = std::atoi(*result);
            };
            int cnt = 0;
            this->con.exec<callback>(query, (void*)&cnt, nullptr);
            return cnt;
        }

    private:
        connection::sqlite<DbName> con{};

        /**
         * @fn
         * @brief 全テーブルに対して一括で実行したいクエリを配列として展開する
         * @tparam TableDefinitionList テーブル定義リスト
        */
        template <TableListDefinable TableDefinitionList> struct expansion_querys;
        template <template <class...> class List, TableDefinable... TableDefinitions>
        struct expansion_querys<List<TableDefinitions...>>
        {
            static constexpr auto create_querys = std::array{ query::sqlite::make_create_table_string_t_v<TableDefinitions>.view()... };
            static constexpr auto exists_querys = std::array{ query::sqlite::make_exists_table_string_t_v<TableDefinitions>.view()... };
        };
        static constexpr auto create_querys_v = expansion_querys<TableDefinitionList>::create_querys;
    };

}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_SQLITE_HPP
