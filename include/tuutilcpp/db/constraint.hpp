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
    struct table_constraint {};

    /**
     * @class
     * @brief �J�����̐���Ƃ��Ďw��\�Ȍ^�̊��B�������A���ځA������w�肵�Ă����������
    */
    struct column_constraint {};

    /**
     * @brief �J����ID���X�g�Ƃ��Đ������`��������
    */
    template <class ColIdList>
    concept ColIdListSpecifiable = mpl::has_value_parameters_v<ColIdList>   // ColIdList�͔�^�e���v���[�g�p�����[�^�p�b�N��������
        && mpl::is_same_types_v<ColIdList>                                  // ColIdList�̃p�����[�^�p�b�N�v�f�̌^�͑S�ē����ł��邱��
        && mpl::is_unique_v<ColIdList>                                      // ColIdList�̃p�����[�^�p�b�N�̒l�ɏd���������Ă͂����Ȃ�
        && mpl::Enumeration<mpl::get_front_t<ColIdList>>;                   // ColIdList�̌^�͗񋓑̂łȂ���΂����Ȃ�

    /**
     * @class
     * @brief �e�[�u���Ɏ�L�[������w�肷��
     * @tparam ColIds ��L�[�Ƃ��Đݒ肷��J����ID��񋓂���
    */
    template <mpl::Enumeration auto... ColIds>
    requires (ColIdListSpecifiable<mpl::value_list<ColIds...>>)
    struct primary_key : public table_constraint
    {
        using id_type = mpl::get_front_t<mpl::value_list<ColIds...>>;
    };

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
    struct pk : public column_constraint {};

    /**
     * @brief �O���L�[����̐���������������
    */
    template <class... ColIdLists>
    concept ForeignKeyArgsSpecifiable = (sizeof...(ColIdLists) == 2)
        && (ColIdListSpecifiable<ColIdLists> && ...)            // ColIdLists�͂��ꂼ���ColIdListSpecifiable�̐���𖞂����Ȃ���Έɍ���
        && mpl::is_unique_v<mpl::concat_list_t<ColIdLists...>>  // ���ID���X�g�Ɉ�v����ID���܂܂�Ă��Ă͂Ȃ�Ȃ�
        && (mpl::count_v<ColIdLists> == ...);                   // ���ID���X�g�����p�����[�^�p�b�N�̐��͓������Ȃ���΂����Ȃ�

    /**
     * @class
     * @brief �e�[�u���ɊO���L�[������w�肷��
     * @tparam ColIdList �O���L�[�Ƃ���ID�̑g
     * @tparam RefColIdList �O���L�[�Ƃ��ĎQ�Ƃ���ID�̑g
    */
    template <class ColIdList, class RefColIdList>
    requires ForeignKeyArgsSpecifiable<ColIdList, RefColIdList>
    struct foreign_key : public table_constraint
    {
        using id_type = mpl::get_front_t<ColIdList>;
    };

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
    template <mpl::Enumeration auto RefColId> struct fk : public column_constraint {};

    /**
     * @class
     * @brief �e�[�u���Ɉ�Ӑ�������w�肷��
     * @tparam ColIds ��Ӑ���������������̑g
    */
    template <mpl::Enumeration auto... ColIds>
    requires (ColIdListSpecifiable<mpl::value_list<ColIds...>>)
    struct unique : public table_constraint
    {
        using id_type = mpl::get_front_t<mpl::value_list<ColIds...>>;
    };

    /**
     * @fn
     * @brief ��Ӑ����񂩔���
    */
    template <class T> struct is_unique : public std::false_type {};
    template <mpl::Enumeration auto... ColIds> struct is_unique<unique<ColIds...>> : public std::true_type {};

    /**
     * @class
     * @brief �J�����Ɉ�Ӑ�������w�肷��
    */
    struct uni : public column_constraint {};

    /**
     * @class
     * @brief �����̔ԁB�W�v�ړI�̂��߁A����𒼐ڃe�[�u���Ɏw�肵�Ă͂����Ȃ�
    */
    template <mpl::Enumeration auto ColId>
    struct auto_increment : public table_constraint
    {
        using id_type = decltype(ColId);
    };

    /**
     * @class
     * @brief �J�����Ɏ����̔Ԃ�ݒ肷��
    */
    struct ai : public column_constraint {};

    /**
     * @fn
     * @brief �����̔Ԃ�����
    */
    template <class T> struct is_auto_increment : public std::false_type {};
    template <mpl::Enumeration auto ColId>
    struct is_auto_increment<auto_increment<ColId>> : public std::true_type {};

    /**
     * @class
     * @brief NOT NULL����
    */
    struct not_null : public column_constraint {};

    /**
     * @brief �J�����֎w��\�Ȑ���̃R���Z�v�g
    */
    template <class T>
    concept ColumnConstraintDefinable = std::is_base_of_v<column_constraint, T> && !std::is_same_v<column_constraint, T>;

    /**
     * @fn
     * @brief �e�[�u���̐��񂩔���
    */
    template <class T> using is_constraint_definition = std::is_base_of<table_constraint, T>;

    /**
     * @brief �e�[�u���̐���ł��邱��
    */
    template <class T>
    concept ConstraintDefinable = is_constraint_definition<T>::value;

    /**
     * @fn
     * @brief �e�[�u���̎��ʎq�^���擾����
    */
    template <class T> struct get_table_id;
    template <ConstraintDefinable T>
    struct get_table_id<T> : public std::type_identity<typename T::id_type> {};

    /**
     * @brief �e�[�u���ւ̐��񃊃X�g�ł��邱��
    */
    template <class T>
    concept ConstraintListDefinable = mpl::has_type_parameters_v<T>                                             // T�͌^�̃p�����[�^�p�b�N��������
        && mpl::is_unique_v<T>                                                                                  // T�̎��p�����[�^�p�b�N�ɏd���͂����Ă͂Ȃ�Ȃ�
        && mpl::apply_list_v<mpl::quote<std::conjunction>, mpl::map_t<mpl::quote<is_constraint_definition>, T>> // T�̎��p�����[�^�p�b�N�v�f�͑S��constraint_definition�łȂ���΂Ȃ�Ȃ�
        && mpl::is_same_types_v<
            mpl::map_t<
                mpl::quote<get_table_id>,
                mpl::remove_if_t<mpl::bind<mpl::quote<std::is_same>, table_constraint>, T>
            >>                                                                                                  // T�����p�����[�^�p�b�N�v�f�̃����oid_type�͑S�ē����^�ł��邱��(���^������)
        && mpl::count_if_v<mpl::quote<is_primary_key>, T> < 2                                                   // �����̎�L�[����̃e�[�u���ɑ��݂��Ă͂����Ȃ�
        && mpl::count_if_v<mpl::quote<is_auto_increment>, T> < 2;                                               // �����̎����̔ԗ񂪈�̃e�[�u���ɑ��݂��Ă͂����Ȃ�

    /**
     * @fn
     * @brief �J������`�p����ʏ�̃e�[�u����`�p�ɐ����ϊ�����
     * @tparam Constraint �J�����Ɏw�肵�Ă��鐧��
     * @tparam ColId Constraint���w�肵�Ă���Ώۂ̃J����ID
     * @details �J�����݂̂œ��Ƀe�[�u���Ɏw�肷����̂ł͂Ȃ��ꍇ�A�f�t�H���g��table_constraint��Ԃ�
    */
    template <ColumnConstraintDefinable Constraint, mpl::Enumeration auto ColId>
    struct to_table_constraint : public std::type_identity<table_constraint> {};

    // �e����ɑ΂�����ꉻ
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<pk, ColId> : public std::type_identity<primary_key<ColId>> {};
    template <mpl::Enumeration auto ColId, mpl::Enumeration auto ColId2>
    struct to_table_constraint<fk<ColId2>, ColId>
        : public std::type_identity<foreign_key<mpl::value_list<ColId>, mpl::value_list<ColId2>>> {};
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<uni, ColId> : public std::type_identity<unique<ColId>> {};
    template <mpl::Enumeration auto ColId>
    struct to_table_constraint<ai, ColId> : public std::type_identity<auto_increment<ColId>> {};

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
    using constraint_unspecified = mpl::type_list<table_constraint>;
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_CONSTRAINT_HPP
