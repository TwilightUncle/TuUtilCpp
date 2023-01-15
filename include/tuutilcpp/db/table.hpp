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
        && std::is_same_v<typename mpl::get_front_t<ColumnDefinitionList>::id_type, ETableType>
        && validate_sql_identity<Name>()
    )
    struct define_table
    {
        using id_type = ETableType;
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
    struct get_column_def<ColID, define_table<ETableType, Name, ColumnDefinitionList, ConstraintDefinitionList>>
        : public get_column_def<ColID, ColumnDefinitionList> {};

    /**
     * @fn
     * @brief �e�[�u����`�N���X�����肷��
    */
    template <class T> struct is_table_definition : public std::false_type {};
    template <
        mpl::Enumeration ETableType,
        str::cstr Name,
        ColumnListDefinable ColumnDefinitionList,
        ConstraintListDefinable ConstraintDefinitionList
    >
    struct is_table_definition<define_table<ETableType, Name, ColumnDefinitionList, ConstraintDefinitionList>>
        : public std::true_type {};

    /**
     * @brief define_table�ł��邱�Ƃ̐���
    */
    template <class T>
    concept TableDefinable = is_table_definition<T>::value;

    /**
     * @fn
     * @brief �e�[�u����`����e�[�u�����̓����N���X���擾
    */
    template <TableDefinable T>
    struct get_table_name : public std::type_identity<mpl::value_constant<T::name>> {};

    /**
     * @fn
     * @brief �e�[�u����`����e�[�u�������ʂ���񋓌^���擾����
    */
    template <TableDefinable T>
    struct get_table_id : public std::type_identity<typename T::id_type> {};
    
    /**
     * @fn
     * @brief �e�[�u����`����e�[�u�������ʂ���񋓌^���擾����
    */
    template <class T> using get_talbe_id_t = get_table_id<T>::type;

    /**
     * @fn
     * @brief �e�[�u����`����e�[�u�����̓����N���X���擾
    */
    template <class T> constexpr auto get_table_name_v = get_table_name<T>::value;

    template <class T>
    concept TableListDefinable = mpl::has_type_parameters_v<T>                                              // T�͌^�̃p�����[�^�p�b�N��������
        && mpl::is_unique_v<T>                                                                              // T�����p�����[�^�p�b�N�͏d�����Ȃ�����
        && mpl::apply_list_v<mpl::quote<std::conjunction>, mpl::map_t<mpl::quote<is_table_definition>, T>>  // T�����p�����[�^�p�b�N�͑S��define_table�ł��邱��
        && mpl::is_unique_v<mpl::map_t<mpl::quote<get_table_id>, T>>                                        // T�����p�����[�^�p�b�N�v�f�̃����oid�͑S�ĈقȂ�^�ł��邱��
        && mpl::is_unique_v<mpl::map_t<mpl::quote<get_table_name>, T>>;                                     // T�����p�����[�^�p�b�N�v�f�̃����oname�͑S�ĈقȂ�l�ł��邱��
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_TABLE_HPP
