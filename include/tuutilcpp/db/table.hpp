///----------------------------------
/// @file table.hpp
/// @brief DB�e�[�u����`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_TABLE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_TABLE_HPP

namespace tuutil::db
{
    /**
     * @fn
     * @brief �e���v���[�g�������ؗp�̃��^�֐�
    */
    template <
        ColumnListDefinitionable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList
    >
    struct validate_define_table_constraint_arg : public std::bool_constant<
        ConstraintListDefinable<
            mpl::concat_list_t<
                ConstraintDefinitionList,
                extract_constraints_t<ColumnDefinitionList>
            >
        >
    > {};

    /**
     * @fn
     * @brief �e�[�u����`�p���^�֐�
     * @tparam ETableType �e�[�u���Ɨ�̎��ʎq�Ƃ��Ē�`�����X�R�[�v�t���񋓌^
     * @tparam Name str::cstr�ŕ��œn���������񃊃e����
     * @tparam ColumnDefinitionList define_column�ɂ����`���w�肷��
     * @tparam ConstraintDefinitionList define_column�ɂ����`���w�肷��
    */
    template <
        mpl::Enumeration ETableType,
        str::cstr Name,
        ColumnListDefinitionable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList = constraint_unspecified
    >
    requires (
        validate_define_table_constraint_arg<ColumnDefinitionList, ConstraintDefinitionList>::value
        && validate_sql_identity<Name>()
    )
    struct define_table
    {
    private:
        template <class T1, ColumnDefinable T2> struct is_match_column_id : public std::false_type {};
        template <auto V, ColumnDefinable T>
        requires std::same_as<ETableType, decltype(V)>
        struct is_match_column_id<mpl::value_constant<V>, T> : public std::bool_constant<V == T::id> {};

    public:
        static constexpr auto name = Name;

        /**
         * @fn
         * @brief V�ɍ��v������`���擾����
        */
        template <ETableType V>
        using get_column_definition_t = mpl::find_if_t<mpl::bind<mpl::quote<is_match_column_id>, mpl::value_constant<V>>, ColumnDefinitionList>;

        /**
         * @fn
         * @brief V�ɍ��v���������擾����
        */
        template <ETableType V>
        static constexpr auto get_column_info()
        {
            return get_column_definition_t<V>{};
        }
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_TABLE_HPP
