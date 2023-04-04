///----------------------------------
/// @file column.hpp
/// @brief DB���`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_COLUMN_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_COLUMN_HPP

namespace tuutil::db
{
    /**
     * @fn
     * @brief ���`�p���^�֐�(�����ɒ�`���e�Q�Ɨp�N���X)
     * @tparam ColID �X�R�[�v�t���񋓑̗v�f���w��
     * @tparam Name str::cstr�ŕ�񂾕����񃊃e������n���B�������1�����ȏ�
     * @tparam FieldType �i�[����f�[�^�^
     * @tparam Constraints �񐧖��C�Ӑ��w�肷��(�w��Ȃ���OK)
    */
    template <
        mpl::Enumeration auto ColID,
        str::cstr Name,
        class FieldType,
        ColumnConstraintDefinable... Constraints
    >
    requires (
        (std::is_integral_v<FieldType> || !mpl::exists_v<ai, mpl::type_list<Constraints...>>) // auto incriment�ɂ��Ă̐���(�ݒ肳��Ă���ꍇ�A�t�B�[���h�^�͐����łȂ���΂Ȃ�Ȃ�)
        && validate_sql_identity<Name>()
    )
    struct define_column
    {
        // �e�[�u�����
        static constexpr auto name = Name;
        static constexpr auto id = ColID;
        using id_type = decltype(ColID);
        using field_type = FieldType;

        // ����̃t���O
        static constexpr auto auto_increment = mpl::exists_v<ai, mpl::type_list<Constraints...>>;
        static constexpr auto not_null = mpl::exists_v<db::not_null, mpl::type_list<Constraints...>>;
        
        // �e�[�u���Ƃ��Ă��Q�Ƃ��K�v�Ȑ��񃊃X�g
        using constraint_list = std::conditional_t<
            (sizeof...(Constraints) > 0),
            mpl::type_list<to_table_constraint_t<Constraints, ColID>...>,
            constraint_unspecified
        >;
    };

    /**
     * @fn
     * @brief column_definition�^���ǂ����𔻒肷�郁�^�֐�
    */
    template <class T> struct is_column_definition : public std::false_type {};
    template <mpl::Enumeration auto ColID, str::cstr Name, typename FieldType, ColumnConstraintDefinable... Constraints>
    struct is_column_definition<define_column<ColID, Name, FieldType, Constraints...>> : public std::true_type {};

    /**
     * @brief define_column�ł��邱�Ƃ��������鐧��
    */
    template <class T>
    concept ColumnDefinable = is_column_definition<T>::value;

    /**
     * @fn
     * @brief �J������`����J����ID�������񋓑̂̓����N���X���擾����
    */
    template <ColumnDefinable T>
    struct get_column_id : public std::type_identity<mpl::value_constant<T::id>> {};

    /**
     * @fn
     * @brief �J������`����J����ID�������񋓑̂̌^���擾
    */
    template <class T> using get_column_id_t = typename get_column_id<T>::type;

    /**
     * @fn
     * @brief �J������`����J����ID�������񋓑̂̒l���擾
    */
    template <class T> constexpr auto get_column_id_v = get_column_id_t<T>::value;

    /**
     * @fn
     * @brief �J������`����J�������̓����N���X���擾
    */
    template <ColumnDefinable T>
    struct get_column_name : public std::type_identity<mpl::value_constant<T::name>> {};

    /**
     * @fn
     * @brief �J������`����J�������̒l���擾
    */
    template <class T> constexpr auto get_column_name_v = get_column_name<T>::value;

    /**
     * @fn
     * @brief define_column�Ŏw�肵����������o���B�ق��̔C�ӂ̌^��n�����ꍇ�́Aconstraint_unspecified��Ԃ�
    */
    template <class T> struct get_constraint_list : public std::type_identity<constraint_unspecified>{};
    template <class T>
    requires ColumnDefinable<T>
    struct get_constraint_list<T> : public std::type_identity<typename T::constraint_list> {};

    /**
     * @fn
     * @brief define_column�Ŏw�肵����������o���B�ق��̔C�ӂ̌^��n�����ꍇ�́Aconstraint_unspecified��Ԃ�
    */
    template <class T> using get_constraint_list_t = typename get_constraint_list<T>::type;

    /**
     * �J������`���X�g�ł��邱��
    */
    template <class T>
    concept ColumnListDefinable = mpl::has_type_parameters_v<T>                                             // T�͌^�̃p�����[�^�p�b�N��������
        && mpl::is_unique_v<T>                                                                              // T�����p�����[�^�p�b�N�͏d�����Ȃ�����
        && mpl::apply_list_v<mpl::quote<std::conjunction>, mpl::map_t<mpl::quote<is_column_definition>, T>> // T�����p�����[�^�p�b�N�͑S��column_definition�ł��邱��
        && mpl::is_unique_v<mpl::map_t<mpl::quote<get_column_id>, T>>;                                      // T�����p�����[�^�p�b�N�v�f�̃����oid�͑S�ĈقȂ�l�ł��邱��

    /**
     * @fn
     * @brief �J�����̒�`���X�g����A�e�J�����Ɏw�肳�ꂽ������𒊏o����(����Ȃ����type_list<ignore_type>���Ԃ�)
    */
    template <ColumnListDefinable T>
    struct extract_constraints : public mpl::relay<
        T,
        mpl::type_list<
            mpl::bind<mpl::quote<mpl::map>, mpl::quote<get_constraint_list>>,
            mpl::bind<mpl::quote<mpl::apply_list>, mpl::quote<mpl::concat_list>>,
            mpl::bind<mpl::quote<mpl::remove_if>, mpl::bind<mpl::quote<std::is_same>, table_constraint>>
        >
    > {};

    /**
     * @fn
     * @brief �J�����̒�`���X�g����A�e�J�����Ɏw�肳�ꂽ������𒊏o����(����Ȃ����type_list<ignore_type>���Ԃ�)
    */
    template <ColumnListDefinable T> using extract_constraints_t = typename extract_constraints<T>::type;

    /**
     * @fn
     * @brief �w�肵��ColID�ɍ��v����e�[�u����`���e�[�u����`���X�g����擾����Bvalue�ɂ͌��������ʒu�̃C���f�b�N�X�������Ă���
     * @tparam ColID ��`�ɊY������񋓑�
     * @tparam T �J������`���X�g
    */
    template <mpl::Enumeration auto ColID, class T> struct get_column_def;

    // ���ڒ�`���X�g�����Ă������ꍇ�̓��ꉻ
    template <mpl::Enumeration auto ColID, ColumnListDefinable T>
    struct get_column_def<ColID, T> : public mpl::find_if<
        mpl::bind<
            mpl::quote<mpl::flip>,
            mpl::quote<mpl::relay>,
            mpl::type_list<
                mpl::quote<get_column_id>,
                mpl::bind<mpl::quote<std::is_same>, mpl::value_constant<ColID>>
            >
        >,
        T
    > {};

    /**
     * @fn
     * @brief �w�肵��ColID�ɍ��v����e�[�u����`���e�[�u����`���X�g����擾����
    */
    template <mpl::Enumeration auto ColID, class T>
    using get_column_def_t = typename get_column_def<ColID, T>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_COLUMN_HPP
