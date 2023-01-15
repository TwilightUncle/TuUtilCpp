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
     * @brief �e�[�u����`�p���^�֐�
     * @tparam ETableType �e�[�u���Ɨ�̎��ʎq�Ƃ��Ē�`�����X�R�[�v�t���񋓌^
     * @tparam Name str::cstr�ŕ��œn���������񃊃e����
     * @tparam ColumnDefinitionList define_column�ɂ����`���w�肷��
     * @tparam ConstraintDefinitionList define_column�ɂ����`���w�肷��
    */
    template <
        mpl::Enumeration ETableType,
        str::cstr Name,
        ColumnListDefinable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList = constraint_unspecified
    >
    requires (
        ConstraintListDefinable<
            mpl::concat_list_t<
                ConstraintDefinitionList,
                extract_constraints_t<ColumnDefinitionList>
            >
        >
        && validate_sql_identity<Name>()
    )
    struct define_table
    {
        static constexpr auto name = Name;
    };

    // �e�[�u����`����J������`���擾����ꍇ�̓��ꉻ
    template <
        mpl::Enumeration auto ColID,
        mpl::Enumeration ETableType,
        str::cstr Name,
        ColumnListDefinable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList
    >
    struct get_column_definition<ColID, define_table<ETableType, Name, ColumnDefinitionList, ConstraintDefinitionList>>
        : public get_column_definition<ColID, ColumnDefinitionList> {};
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_TABLE_HPP
