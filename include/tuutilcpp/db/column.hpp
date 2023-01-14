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
    requires (validate_sql_identity<Name>())
    struct define_column
    {
        static constexpr auto name = Name;
        static constexpr auto id = ColID;
        using id_type = decltype(ColID);
        using field_type = FieldType;
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

    template <ColumnDefinable T>
    struct get_column_id { static constexpr auto value = T::id; };

    template <ColumnDefinable T>
    struct get_column_name { static constexpr auto value = T::name; };

    /**
     * @fn
     * @brief define_column�Ŏw�肵����������o���B�ق��̔C�ӂ̌^��n�����ꍇ�́Aconstraint_unspecified��Ԃ�
    */
    template <class T> struct get_constraint_list : public std::type_identity<constraint_unspecified>{};
    template <class T>
    requires ColumnDefinable<T>
    struct get_constraint_list<T> : public std::type_identity<typename T::constraint_list> {};

    template <class T> using get_constraint_list_t = get_constraint_list<T>::type;

    /**
     * �J������`���X�g�ł��邱��
    */
    template <class T>
    concept ColumnListDefinitionable = requires {
        // T�̓p�����[�^�p�b�N�������Ă���
        requires mpl::has_type_parameters_v<T>;

        // ������`�̃J���������݂��Ă͂����Ȃ�
        requires mpl::is_unique_v<T>;

        // �e���v���[�g�������S�ăJ������`�ł��邱��(�S�Ẵp�����[�^���J������`�ł��邩�����X�g�Ƃ��Ď擾���A�_���ςŌ��ʂ��m�F)
        requires mpl::apply_list<
            mpl::quote<std::conjunction>,
            mpl::map_t<mpl::quote<is_column_definition>, T>
        >::value;

        // name�����Ԃ��Ă͂����Ȃ�
        // id�����Ԃ��Ă͂����Ȃ�
        // id�̌^�͑S�ē������K�v������
    };

    /**
     * @fn
     * @brief �J�����̒�`���X�g����A�e�J�����Ɏw�肳�ꂽ������𒊏o����(����Ȃ����type_list<ignore_type>���Ԃ�)
    */
    template <ColumnListDefinitionable T>
    struct extract_constraints : public mpl::relay<
        T,
        mpl::type_list<
            mpl::bind<mpl::quote<mpl::map>, mpl::quote<get_constraint_list>>,
            mpl::bind<mpl::quote<mpl::apply_list>, mpl::quote<mpl::concat_list>>,
            mpl::bind<mpl::quote<mpl::remove_if>, mpl::bind<mpl::quote<std::is_same>, constraint>>
        >
    > {};

    template <ColumnListDefinitionable T> using extract_constraints_t = extract_constraints<T>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_COLUMN_HPP
