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
        using parent::parent;
    };

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
