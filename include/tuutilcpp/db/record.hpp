///----------------------------------
/// @file record.hpp
/// @brief DB�f�[�^��1�s������̊i�[�p�I�u�W�F�N�g
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_RECORD_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_RECORD_HPP

namespace tuutil::db
{
    /**
     * @class
     * @brief ���ʂ��i�[������Ainsert, update�œn�����肷��
     * @tparam ColumnDefinitions �Y���̃J������`
    */
    template <ColumnDefinable... ColumnDefinitions>
    struct record
        : public std::tuple<typename ColumnDefinitions::field_type...>
    {
    private:
        using parent = std::tuple<typename ColumnDefinitions::field_type...>;
    
    public:
        // �p���R���X�g���N�^
        using parent::parent;

        // �w�肳�ꂽ�J������`�̒��Ɏ����̔Ԃ��܂܂�Ă����ꍇ�^
        static constexpr bool has_auto_increment_field = ((ColumnDefinitions::auto_increment) || ...);
    };

    /**
     * @fn
     * @brief record�^���ǂ�������
    */
    template <class T> struct is_record : public std::false_type {};
    template <ColumnDefinable... ColumnDefinitions>
    struct is_record<record<ColumnDefinitions...>> : public std::bool_constant<mpl::is_unique_v<record<ColumnDefinitions...>>> {};

    template <class T>
    concept DbRecordHoldable = is_record<T>::value;

    /**
     * @brief insert�\�ȃf�[�^�^�����肷��
    */
    template <class T, class TableDefinition>
    concept DbRecordInsertable = TableDefinable<TableDefinition>
        && ColumnListDefinableForTable<T>
        && DbRecordHoldable<T>
        && !T::has_auto_increment_field;

    /**
     * @fn
     * @brief record��ColID�ɊY������v�f���擾����
     * @tparam ColID �J�������ʗp�̗񋓌^�̒l�B������Q�Ƃ��Ēl��������v�f�����肷��
     * @param re �Ώۃ��R�[�h
    */
    template <mpl::Enumeration auto ColID, ColumnDefinable... ColumnDefinitions>
    requires (!std::same_as<get_column_def_t<ColID, mpl::type_list<ColumnDefinitions...>>, mpl::ignore_type>)
    constexpr auto& get(record<ColumnDefinitions...>& re)
    {
        using column_def_type = get_column_def<ColID, mpl::type_list<ColumnDefinitions...>>;
        return std::get<column_def_type::value>(re);
    }

    /**
     * @fn
     * @brief record�̃C���X�^���X�ɒl���Z�b�g����
     * @tparam ColID �J�������ʗp�̗񋓌^�̒l�B������Q�Ƃ��Ēl��������v�f�����肷��
     * @param re �l���Z�b�g����Ώ�
     * @param value �Z�b�g����l
    */
    template <mpl::Enumeration auto ColID, ColumnDefinable... ColumnDefinitions>
    constexpr void set(
        record<ColumnDefinitions...>& re,
        const typename get_column_def_t<ColID, mpl::type_list<ColumnDefinitions...>>::field_type& value
    ) {
        get<ColID>(re) = value;
    }
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_RECORD_HPP
