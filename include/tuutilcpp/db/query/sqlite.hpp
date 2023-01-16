///----------------------------------
/// @file type.hpp
/// @brief DBフィールド用の型定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_QUERY_SQLITE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_QUERY_SQLITE_HPP

namespace tuutil::db::query
{
    struct sqlite
    {
        /**
         * @fn
         * @brief 指定したsql型に該当する型名の文字列を返す
        */
        template <class T>
        static constexpr auto get_type_name_string()
        {
            if constexpr (std::is_same_v<T, bit>)                       return str::cstr{"bit"};
            else if constexpr (std::is_same_v<T, tinyint>)              return str::cstr{"tinyint"};
            else if constexpr (std::is_same_v<T, smallint>)             return str::cstr{"smallint"};
            else if constexpr (std::is_same_v<T, integer>)              return str::cstr{"int"};
            else if constexpr (std::is_same_v<T, bigint>)               return str::cstr{"bigint"};
            else if constexpr (std::is_same_v<T, unsigned_tinyint>)     return str::cstr{"tinyint unsigned"};
            else if constexpr (std::is_same_v<T, unsigned_smallint>)    return str::cstr{"smallint unsigned"};
            else if constexpr (std::is_same_v<T, unsigned_integer>)     return str::cstr{"int unsigned"};
            else if constexpr (std::is_same_v<T, unsigned_bigint>)      return str::cstr{"bigint unsigned"};
            else if constexpr (is_varchar_v<T>)
                return str::concat(str::cstr{"varchar("}, str::to_string<get_sql_string_size_v<T>>(), str::cstr{")"});
            else if constexpr (is_character_v<T>)
                return str::concat(str::cstr{"char("}, str::to_string<get_sql_string_size_v<T>>(), str::cstr{")"});
            else return str::cstr{""};
        }

        template <bool IsAi>
        static constexpr auto get_auto_increment_string()
        {
            if constexpr (IsAi) return str::cstr{"autoincrement "};
            else return str::cstr{""};
        }

        template <bool IsNn>
        static constexpr auto get_not_null_string()
        {
            if constexpr (IsNn) return str::cstr{"not null "};
            else return str::cstr("");
        }

        /**
         * @fn
         * @brief カラム定義文字列を取得する
        */
        template <ColumnDefinable ColumnDefinition>
        static constexpr auto get_column_define_string()
        {
            return str::concat(
                str::cstr{"\""}, ColumnDefinition::name, str::cstr{"\" "},
                get_type_name_string<typename ColumnDefinition::field_type>(), str::cstr{" "},
                get_auto_increment_string<ColumnDefinition::auto_incriment>(),
                get_not_null_string<ColumnDefinition::not_null>()
            );
        }

    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_QUERY_SQLITE_HPP
