///----------------------------------
/// @file mpl/parameter_pack.hpp 
/// @brief �^�p�����[�^�p�b�N�̑���┻��
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief �e���v���[�g�N���X���^�p�����[�^�Ƃ��ė��p�ł���悤�ɂ���
     * @tparam List �p�����[�^�p�b�N���w��̃e���v���[�g�^
    */
    template <template <class...> class List> struct lift {};

    template <template <class...> class List, class T>
    struct push_back<lift<List>, T> : public std::type_identity<List<T>> {};

    template <template <class...> class List, class T>
    struct push_front<lift<List>, T> : public std::type_identity<List<T>> {};

    /**
     * @fn
     * @brief lift�ŕ�܂ꂽ�e���v���[�g�^������
     * @tparam T �����Ώ�
    */
    template <class T> struct is_lifted : public std::false_type {};
    template <template <class...> class List>
    struct is_lifted<lift<List>> : public std::true_type {};

    template <class T>
    concept LiftedList = is_lifted<T>::value;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�����^�̃K���̕������擾����
     * @return lift or liftv�ŕ�܂ꂽ�e���v���[�g�^
    */
    template <class T> struct get_empty_list;
    template <template <class...> class List, class... Parameters>
    struct get_empty_list<List<Parameters...>> : public std::type_identity<lift<List>> {};
    template <template <auto...> class List, auto... Parameters>
    struct get_empty_list<List<Parameters...>> : public std::type_identity<liftv<List>> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�����^�̃K���̕������擾����
     * @return lift or liftv�ŕ�܂ꂽ�e���v���[�g�^
    */
    template <class T> using get_empty_list_t = get_empty_list<T>::type;

    /**
     * @fn
     * @brief lift�ŕ�܂ꂽ�e���v���[�g�N���X�Ƀp�����[�^�p�b�N��K�p����
    */
    template <template <class...> class List, class... Parameters>
    struct apply<lift<List>, Parameters...> : public std::type_identity<List<Parameters...>> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̕��т𔽓]������
     * @tparam List �^�̃p�����[�^�p�b�N�����^
    */
    template <class List> struct reverse;
    template <template <class...> class List, class Head, class... Parameters>
    struct reverse<List<Head, Parameters...>> : public foldl<quote<push_front>, List<Head>, List<Parameters...>> {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т𔽓]������
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class List> using reverse_t = reverse<List>::type;

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��������Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> struct rotatel;
    template <template <class...> class List, class Head, class... Parameters>
    struct rotatel<List<Head, Parameters...>> : public std::type_identity<List<Parameters..., Head>> {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��������Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> using rotatel_t = rotatel<TypeList>::type;

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��E�����Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> struct rotater;
    template <template <class...> class List, class... Parameters>
    struct rotater<List<Parameters...>> : public reverse<rotatel_t<reverse_t<List<Parameters...>>>> {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N�̕��т��E�����Ɉ���炵�A�擪�̌^���Ō���Ɉڂ�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class TypeList> using rotater_t = rotater<TypeList>::type;

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N����������
     * @tparam T �Ώ�
    */
    template <class T> struct has_type_parameters : public std::false_type {};
    template <template <class...> class List, class... Parameters>
    struct has_type_parameters<List<Parameters...>> : public std::true_type {};

    /**
     * @fn
     * @brief ��^�̃p�����[�^�p�b�N����������
     * @tparam T �Ώ�
    */
    template <class T> struct has_value_parameters : public std::false_type {};
    template <template <auto...> class List, auto... Parameters>
    struct has_value_parameters<List<Parameters...>> : public std::true_type {};

    /**
     * @fn
     * @brief �^�̃p�����[�^�p�b�N����������
     * @tparam T �����Ώ�
    */
    template <class T> constexpr auto has_type_parameters_v = has_type_parameters<T>::value;

    /**
     * @fn
     * @brief ��^�̃p�����[�^�p�b�N����������
     * @tparam T �Ώ�
    */
    template <class T> constexpr auto has_value_parameters_v = has_value_parameters<T>::value;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̓��e��Dest�փR�s�[
     * @tparam Src �R�s�[���̌^�p�����[�^�p�b�N�����^
     * @tparam Dest �R�s�[�Ώۂ̃e���v���[�g�N���X
    */
    template <class Src, LiftedList Dest> struct copy;
    template <template <class...> class Src, class Dest, class... Parameters>
    struct copy<Src<Parameters...>, Dest> : public apply<Dest, Parameters...> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̓��e��Dest�փR�s�[
     * @tparam Src �R�s�[���̌^�p�����[�^�p�b�N�����^
     * @tparam Dest �R�s�[�Ώۂ̃e���v���[�g�N���X
    */
    template <class Src, LiftedList Dest> using copy_t = copy<Src, Dest>::type;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̌^���S�ē����Ƃ��^
     * @tparam T �����Ώۂ̌^�p�����[�^�p�b�N�����^
    */
    template <class T> struct is_same_types;
    template <template <class...> class List, class Head, class... Parameters>
    struct is_same_types<List<Head, Parameters...>> : public std::bool_constant<(std::is_same_v<Head, Parameters> && ...)> {};
    // ��^�e���v���[�g�p�����[�^�̌^�����݂̂̔�r(��̓I�Ȓl�ɂ��Ă͊֒m���Ȃ�)
    template <template <auto...> class List, auto... Parameters>
    struct is_same_types<List<Parameters...>> : public is_same_types<type_list<decltype(Parameters)...>> {};
    template <> struct is_same_types<ignore_type> : public std::true_type {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̌^���S�ē����Ƃ��^
     * @tparam T �����Ώۂ̌^�p�����[�^�p�b�N�����^
    */
    template <class T> constexpr auto is_same_types_v = is_same_types<T>::value;

    /**
     * @fn
     * @brief �^T��List�̃p�����[�^�p�b�N�Ɋ܂܂�Ă���Ƃ��^
     * @tparam T �܂܂�Ă��邩�m�F�������^
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <class T, class List> struct contains;
    template <class T, template <class...> class List, class... Parameters>
    struct contains<T, List<Parameters...>> : public std::bool_constant<(std::is_same_v<T, Parameters> || ...)> {};

    /**
     * @fn
     * @brief �^T��List�̃p�����[�^�p�b�N�Ɋ܂܂�Ă���Ƃ��^
     * @tparam T �܂܂�Ă��邩�m�F�������^
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <class T, class List> constexpr auto contains_v = contains<T, List>::value;

    /**
     * @fn
     * @brief �擪�v�f���擾����
     * @tparam �p�����[�^�p�b�N�����^
    */
    template <class List> struct get_front;
    template <template <class...> class List, class Head, class... Parameters>
    struct get_front<List<Head, Parameters...>> : public std::type_identity<Head> {};
    template <template <auto...> class List, auto Head, auto... Parameters>
    struct get_front<List<Head, Parameters...>> : public value_constant<Head> {};

    template <class List> using get_front_t = get_front<List>::type;
    template <class ValueList> constexpr auto get_front_v = get_front<ValueList>::value;

    /**
     * @fn
     * @brief �Ō���v�f���擾����
     * @tparam �p�����[�^�p�b�N�����^
    */
    template <class List> using get_back = get_front<reverse_t<List>>;

    template <class List> using get_back_t = get_back<List>::type;
    template <class ValueList> constexpr auto get_back_v = get_back<ValueList>::value;

    /**
     * @fn
     * @brief 1�ȏ�̃p�����[�^�p�b�N�̓��e����������
    */
    template <class... Lists>
    requires (sizeof...(Lists) > 0)
    struct concat_list;
    // ���type_list������
    template <template <class...> class List, class... Types1, class... Types2>
    struct concat_list<List<Types1...>, List<Types2...>> : std::type_identity<List<Types1..., Types2...>> {};
    // �C�Ӑ���type_list����������
    template <class Head, class... Lists>
    requires (has_type_parameters_v<Lists> && ...)
    struct concat_list<Head, Lists...> : public foldl<quote<concat_list>, Head, type_list<Lists...>> {};

    template <class... Lists> using concat_list_t = concat_list<Lists...>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_REVERSE_HPP
