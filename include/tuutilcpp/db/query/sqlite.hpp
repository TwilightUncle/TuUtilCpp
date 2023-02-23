///----------------------------------
/// @file db/query/sqlite.hpp
/// @brief sqlite�N�G������
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_QUERY_SQLITE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_QUERY_SQLITE_HPP

namespace tuutil::db::query
{
    struct sqlite
    {
        /**
         * @fn
         * @brief �w�肵��sql�^�ɊY������^���̕������Ԃ�
        */
        template <class T>
        struct make_type_name_string : public std::type_identity<mpl::value_constant<
            []() {
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
                    return "varchar(" + str::to_string<get_sql_string_size_v<T>>() + ')';
                else if constexpr (is_character_v<T>)
                    return "char(" + str::to_string<get_sql_string_size_v<T>>() + ')';
                else return str::cstr{""};
            }()
        >> {};
        template <class T>
        static constexpr auto make_type_name_string_t_v = make_type_name_string<T>::type::value;

        /**
         * @fn
         * @brief auto increment�̏ꍇ�A��`�������Ԃ�
         * @tparam IsAi auto increment���ǂ����̐^�U�l
        */
        template <bool IsAi>
        static constexpr auto make_auto_increment_string()
        {
            if constexpr (IsAi) return str::cstr{"autoincrement "};
            else return str::cstr{""};
        }

        /**
         * @fn
         * @brief not null�̏ꍇ�A��`�������Ԃ�
         * @tparam IsNn not null���ǂ����̐^�U�l
        */
        template <bool IsNn>
        static constexpr auto make_not_null_string()
        {
            if constexpr (IsNn) return str::cstr{"not null "};
            else return str::cstr("");
        }

        /**
         * @fn
         * @brief DB�J������`�������Ԃ�
         * @tparam ColumnDefinition �J������`�N���X
        */
        template <ColumnDefinable ColumnDefinition>
        using make_column_define_string = std::type_identity<mpl::value_constant<
            []() {
                return ('"' + ColumnDefinition::name + "\" "
                    + make_type_name_string_t_v<typename ColumnDefinition::field_type> + ' '
                    + make_auto_increment_string<ColumnDefinition::auto_increment>()
                    + make_not_null_string<ColumnDefinition::not_null>()
                ).remove_suffix<1>();
            }()
        >>;
        template <ColumnDefinable ColumnDefinition>
        static constexpr auto make_column_define_string_t_v = make_column_define_string<ColumnDefinition>::type::value;

        /**
         * @fn
         * @brief DB�e�[�u����create�����쐬����
         * @tparam TableDefinition �e�[�u����`�N���X
        */
        template <TableDefinable TableDefinition>
        using make_create_table_string = std::type_identity<mpl::value_constant<
            []() {
                using column_define_strings = mpl::unwrap_value_elements_t<
                    mpl::map_t<
                        mpl::quote<make_column_define_string>,
                        typename TableDefinition::column_list
                    >
                >;
                return "create table \"" + TableDefinition::name + '"'
                    + '(' + str::cstrs_join_v<", ", column_define_strings> + ')';
            }()
        >>;
        template <TableDefinable TableDefinition>
        static constexpr auto make_create_table_string_t_v = make_create_table_string<TableDefinition>::type::value;

        /**
         * @fn
         * @brief DB�e�[�u����drop�����쐬����
         * @tparam TableDefinition �e�[�u����`�N���X
        */
        template <TableDefinable TableDefinition>
        using make_drop_table_string = std::type_identity<mpl::value_constant<"drop table \"" + TableDefinition::name + '"'>>;
        template <TableDefinable TableDefinition>
        static constexpr auto make_drop_table_string_t_v = make_drop_table_string<TableDefinition>::type::value;
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_QUERY_SQLITE_HPP
