///----------------------------------
/// @file type.hpp
/// @brief DBフィールド用の型定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_SQLITE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_SQLITE_HPP

#include <fstream>
#include <filesystem>
#include <tuutilcpp/str.hpp>
#include <tuutilcpp/db.hpp>
#include <tuutilcpp/db/connection/sqlite.hpp>
#include <tuutilcpp/db/query/sqlite.hpp>
#include <tuutilcpp/db/query/builder.hpp>

namespace tuutil::db
{
    /**
     * @class
     * @brief sqliteについて指定したテーブル定義とDB名をもとに接続とクエリ作成をカプセル化したもの。
     * DB構造については型定義と同時に実施されることを想定しているため、テーブル構造変更などの機能は持たせないものとする。
     * データベースファイルが存在しない場合、インスタンス化した際に自動的に作成される
     * @tparam DbName データベース名称
     * @tparam TableDefinitionList テーブル定義型
    */
    template <str::cstr DbName, TableListDefinable TableDefinitionList>
    struct sqlite
    {
        static constexpr auto name = DbName;

        sqlite(): con{}
        {
            number_of_valid_connections++;

            // 一つでもテーブルが作成されていたら、構造が完成しているものとしてテーブル追加を実施しない
            if (!exists_table<mpl::get_front_t<TableDefinitionList>>())
                for (auto s : create_querys_v) this->con.exec(std::string(s));
        }
        ~sqlite() { number_of_valid_connections--; }

        /**
         * @fn
         * @brief DbNameで指定したファイルに対する現在行われている有効な接続の数をカウント
        */
        static int count_valid_connections() { return number_of_valid_connections; }

        /**
         * @fn
         * @brief テンプレート引数で指定されたDBファイルが存在委するか判定
        */
        static bool exists_db() { return std::ifstream(std::string(DbName)).is_open(); }

        /**
         * @fn
         * @brief データベースを作成する
         * @return 作成に成功した場合真
        */
        static bool create_db()
        {
            if (!exists_db()) sqlite{};
            return exists_db();
        }

        /**
         * @fn
         * @brief データベースを削除する
        */
        static void drop_db()
        {
            if (count_valid_connections() > 0)
                throw std::runtime_error("Unable to drop database as valid connections exist.");
            std::filesystem::remove(std::string(DbName));
        }

        /**
         * @fn
         * @brief 指定したID型のテーブルが存在しているか判定
         * @tparam テーブル識別子の列挙型
        */
        template <mpl::Enumeration ETableType>
        bool exists_table() const
        {
            using table_definition_type = get_table_def_t<ETableType, TableDefinitionList>;
            static_assert(!std::is_same_v<table_definition_type, mpl::ignore_type>, "Not found database table definition.");
            return this->exists_table<table_definition_type>();
        }

        /**
         * @fn
         * @brief 指定した定義のテーブルが存在しているか判定
         * @tparam テーブル定義型
        */
        template <TableDefinable TableDefinition>
        bool exists_table() const
        {
            return this->con.tableExists(TableDefinition::name);
        }

        /**
         * @fn
         * @brief countのクエリを発行
        */
        int count(const std::string& query)
        {
            return this->con.count(query);
        }

    private:
        connection::sqlite<DbName> con;
        inline static int number_of_valid_connections = 0;

        /**
         * @fn
         * @brief 全テーブルに対して一括で実行したいクエリを配列として展開する
         * @tparam TableDefinitionList テーブル定義リスト
        */
        template <TableListDefinable TableDefinitionList> struct expansion_querys;
        template <template <class...> class List, TableDefinable... TableDefinitions>
        struct expansion_querys<List<TableDefinitions...>>
        {
            static constexpr auto create_querys = std::array{ query::sqlite::make_create_table_string_v<TableDefinitions>.view()... };
        };
        static constexpr auto create_querys_v = expansion_querys<TableDefinitionList>::create_querys;
    };

}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_SQLITE_HPP
