///----------------------------------
/// @file constraint.hpp
/// @brief �񐧖��`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP

namespace tuutil::db
{
    /**
     * @class
     * @brief �e�[�u���̐���Ƃ��Ďw��\�Ȍ^�̊��B�������A���ڂ�����w�肵�Ă����������
    */
    struct constraint {};

    /**
     * @class
     * @brief �J�����̐���Ƃ��Ďw��\�Ȍ^�̊��B�������A���ځA������w�肵�Ă����������
    */
    struct constraint_c {};

    /**
     * @brief �J����ID���X�g�Ƃ��Đ������`��������
    */
    template <class ColIdList>
    concept ColIdListSpecifiable = mpl::has_value_parameters_v<ColIdList>
        && mpl::is_same_types_v<ColIdList>
        && mpl::is_unique_v<ColIdList>
        && mpl::Enumeration<mpl::get_front_t<ColIdList>>;

    /**
     * @class
     * @brief �e�[�u���Ɏ�L�[������w�肷��
     * @tparam ColIds ��L�[�Ƃ��Đݒ肷��J����ID��񋓂���
    */
    template <mpl::Enumeration auto... ColIds>
    requires (ColIdListSpecifiable<mpl::value_list<ColIds...>>)
    struct primary_key : public constraint {};

    /**
     * @fn
     * @brief ��L�[���񂩔���
    */
    template <class T> struct is_primary_key : public std::false_type {};
    template <mpl::Enumeration auto... ColIds>
    struct is_primary_key<primary_key<ColIds...>> : public std::true_type {};

    /**
     * @fn
     * @brief ��L�[���񂩔���
    */
    template <class T> constexpr auto is_primary_key_v = is_primary_key<T>::value;
    
    /**
     * @class
     * @brief �J�����Ɏ�L�[������w�肷��
    */
    struct pk : public constraint_c {};

    /**
     * @brief �O���L�[����̐���������������
    */
    template <class... ColIdLists>
    concept ForeignKeyArgsSpecifiable = (sizeof...(ColIdLists) == 2)
        && (ColIdListSpecifiable<ColIdLists> && ...)
        && mpl::is_unique_v<mpl::concat_list_t<ColIdLists...>> // ���ID���X�g�Ɉ�v����ID���܂܂�Ă��Ă͂Ȃ�Ȃ�
        && (mpl::count_v<ColIdLists> == ...);

    /**
     * @class
     * @brief �e�[�u���ɊO���L�[������w�肷��
     * @tparam ColIdList �O���L�[�Ƃ���ID�̑g
     * @tparam RefColIdList �O���L�[�Ƃ��ĎQ�Ƃ���ID�̑g
    */
    template <class ColIdList, class RefColIdList>
    requires ForeignKeyArgsSpecifiable<ColIdList, RefColIdList>
    struct foreign_key : public constraint {};

    /**
     * @fn
     * @brief �O���L�[���񂩔���
    */
    template <class T> struct is_foreign_key : public std::false_type {};
    template <class... ColIdLists>
    struct is_foreign_key<foreign_key<ColIdLists...>> : public std::true_type {};

    /**
     * @fn
     * @brief �O���L�[���񂩔���
    */
    template <class T> constexpr auto is_foreign_key_v = is_foreign_key<T>::value;

    /**
     * @class
     * @brief �J�����ɊO���L�[������w�肷��
     * @tparam RefColId �Q�Ɛ�̃J����ID
    */
    template <mpl::Enumeration auto RefColId> struct fk : public constraint_c {};

    /**
     * @brief �J�����֎w��\�Ȑ���̃R���Z�v�g
    */
    template <class T>
    concept ColumnConstraintDefinable = std::is_base_of_v<constraint_c, T> && !std::is_same_v<constraint_c, T>;

    /**
     * @fn
     * @brief �e�[�u���̐��񂩔���
    */
    template <class T> using is_constraint_definition = std::is_base_of<constraint, T>;

    /**
     * @brief �e�[�u���̐���ł��邱��
    */
    template <class T>
    concept ConstraintDefinable = is_constraint_definition<T>::value;

    /**
     * @brief �e�[�u���ւ̐��񃊃X�g�ł��邱��
    */
    template <class T>
    concept ConstraintListDefinable = mpl::has_type_parameters_v<T>
        && mpl::is_unique_v<T>
        && mpl::apply_list_v<mpl::quote<std::conjunction>, mpl::map_t<mpl::quote<is_constraint_definition>, T>>
        && mpl::count_if_v<mpl::quote<is_primary_key>, T> < 2; // �����̎�L�[����̃e�[�u���ɑ��݂��Ă͂����Ȃ�

    /**
     * @fn
     * @brief �J������`�p����ʏ�̃e�[�u����`�p�ɐ����ϊ�����
     * @tparam Constraint �J�����Ɏw�肵�Ă��鐧��
     * @tparam ColId Constraint���w�肵�Ă���Ώۂ̃J����ID
    */
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId> struct to_table_constraint;

    // �e����ɑ΂�����ꉻ
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<pk, ColId> : public std::type_identity<primary_key<ColId>> {};
    template <mpl::Enumeration auto ColId, mpl::Enumeration auto ColId2>
    struct to_table_constraint<fk<ColId2>, ColId>
        : public std::type_identity<foreign_key<mpl::value_list<ColId>, mpl::value_list<ColId2>>> {};

    /**
     * @fn
     * @brief �J������`�p����ʏ�̃e�[�u����`�p�ɐ����ϊ�����
     * @tparam Constraint �J�����Ɏw�肵�Ă��鐧��
     * @tparam ColId Constraint���w�肵�Ă���Ώۂ̃J����ID
    */
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId>
    using to_table_constraint_t = to_table_constraint<Constraint, ColId>::type;

    /**
     * �e�[�u���Ƃ��Đ���𖢎w��ł��邱��
    */
    using constraint_unspecified = mpl::type_list<constraint>;
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
