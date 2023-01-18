///----------------------------------
/// @file mpl/argment.hpp 
/// @brief ���^�֐������֘A�̑���
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief ��������^�֐��̈����̏��Ԃ����ւ��������Ŏ��s
     * @tparam F ���^�֐�
     * @tparam Arg2 ���^�֐��̓�ڂɎw�肵��������
     * @tparam Arg1 ���^�֐��̈�ڂɎw�肵��������
    */
    template <MetaCallable F, class Arg2, class Arg1> using flip = apply<F, Arg1, Arg2>;

    /**
     * @fn
     * @brief ��������^�֐��̈����̏��Ԃ����ւ��������Ŏ��s
     * @tparam F ���^�֐�
     * @tparam Arg2 ���^�֐��̓�ڂɎw�肵��������
     * @tparam Arg1 ���^�֐��̈�ڂɎw�肵��������
    */
    template <MetaCallable F, class Arg2, class Arg1> using flip_t = flip<F, Arg2, Arg1>::type;

    template <MetaCallable F, class Arg2, class Arg1> constexpr auto flip_v = flip<F, Arg2, Arg1>::value;

    /**
     * @fn
     * @brief ���^�֐����X�g��Arg��H�킹�A���ʂ𒀎����s����
     * @tparam Arg �����l
     * @tparam MetaFuncList ���^�֐����X�g
    */
    template <class Arg, class MetaFuncList> struct relay;
    template <class Arg, template <class...> class List, MetaCallable... Funcs>
    struct relay<Arg, List<Funcs...>> : public foldr<quote<apply>, Arg, reverse_t<List<Funcs...>>> {};

    /**
     * @fn
     * @brief ���^�֐����X�g��Arg��H�킹�A���ʂ𒀎����s����
     * @tparam Arg �����l
     * @tparam MetaFuncList ���^�֐����X�g
    */
    template <class Arg, class MetaFuncList> using relay_t = relay<Arg, MetaFuncList>::type;

    /**
     * @fn
     * @brief ���^�֐����X�g��Arg��H�킹�A���ʂ𒀎����s����
     * @tparam Arg �����l
     * @tparam MetaFuncList ���^�֐����X�g
    */
    template <class Arg, class MetaFuncList> constexpr auto relay_v = relay<Arg, MetaFuncList>::value;

    /**
     * @fn
     * @brief ���X�g�Ƃ��ēn���ꂽ�^���X�g�������Ƃ��Ċ֐��ɓK�p����
     * @tparam F ���^�֐�
     * @tparam ArgList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class ArgList> struct apply_list;
    template <MetaCallable F, template <class...> class List, class... Args>
    struct apply_list<F, List<Args...>> : public apply<F, Args...> {};

    /**
     * @fn
     * @brief ���X�g�Ƃ��ēn���ꂽ�^���X�g�������Ƃ��Ċ֐��ɓK�p����
     * @tparam F ���^�֐�
     * @tparam ArgList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class ArgList> using apply_list_t = apply_list<F, ArgList>::type;

    /**
     * @fn
     * @brief ���X�g�Ƃ��ēn���ꂽ�^���X�g�������Ƃ��Ċ֐��ɓK�p����
     * @tparam F ���^�֐�
     * @tparam ArgList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class ArgList> constexpr auto apply_list_v = apply_list<F, ArgList>::value;

    /**
     * @fn
     * @brief �l���X�g�̑S�Ă̗v�f���^�Ƃ��Ĉ�����悤��value_constant�ŕ��
     * @tparam ValueList ��^�e���v���[�g�p�����[�^�p�b�N�����^
     * @tparam NewList value_constant�ŕ�񂾗v�f���󂯎��A�^�e���v���[�g�p�����[�^�p�b�N���w��\�ȃe���v���[�g�^
    */
    template <class ValueList, LiftedList NewList> struct wrap_value_elements;
    template <template <auto...> class List, template <class...> class NewList, auto... ValueElements>
    struct wrap_value_elements<List<ValueElements...>, lift<NewList>>
        : public std::type_identity<NewList<value_constant<ValueElements>...>>
    {};

    /**
     * @fn
     * @brief �l���X�g�̑S�Ă̗v�f���^�Ƃ��Ĉ�����悤��value_constant�ŕ��
     * @tparam ValueList ��^�e���v���[�g�p�����[�^�p�b�N�����^
     * @tparam NewList value_constant�ŕ�񂾗v�f���󂯎��A�^�e���v���[�g�p�����[�^�p�b�N���w��\�ȃe���v���[�g�^
    */
    template <class ValueList, LiftedList NewList> using wrap_value_elements_t = wrap_value_elements<ValueList, NewList>::type;

    /**
     * @fn
     * @brief �^���X�g�̑S�Ă̗v�f��l�֕ϊ�����B�S�Ă̗v�f�^��constexpr value�����o�ϐ��������Ă��Ȃ��ƃR���p�C���G���[
     * @tparam TypeList �^�e���v���[�g�p�����[�^�p�b�N�����^
     * @tparam NewList �l�̗v�f���i�[�\�ȃe���v���[�g�^
    */
    template <class TypeList, LiftedvList NewList> struct unwrap_value_elements;
    template <template <class...> class List, template <auto...> class NewList, class... TypeElements>
    requires (has_constexpr_value_member_v<TypeElements> && ...)
    struct unwrap_value_elements<List<TypeElements...>, liftv<NewList>>
        : public std::type_identity<NewList<TypeElements::value...>>
    {};
    template <template <auto...> class NewList>
    struct unwrap_value_elements<ignore_type, liftv<NewList>> : public std::type_identity<ignore_type> {};

    /**
     * @fn
     * @brief �^���X�g�̑S�Ă̗v�f��l�֕ϊ�����B�S�Ă̗v�f�^��constexpr value�����o�ϐ��������Ă��Ȃ��ƃR���p�C���G���[
     * @tparam TypeList �^�e���v���[�g�p�����[�^�p�b�N�����^
     * @tparam NewList �l�̗v�f���i�[�\�ȃe���v���[�g�^
    */
    template <class TypeList, LiftedvList NewList> using unwrap_value_elements_t = unwrap_value_elements<TypeList, NewList>::type;

    /**
     * @fn
     * @brief �l���X�g���^���X�g�Ƃ��Ĉ������Ƃɂ���āA���X�g����p���^�֐���K�p�ł���悤�ɂ���
    */
    template <MetaCallable F, class ValueList> struct behave_as_type_list_arg;
    template <MetaCallable F, template <auto...> class List, auto... Parameters>
    struct behave_as_type_list_arg<F, List<Parameters...>> : public relay<
        List<Parameters...>,
        type_list<
            bind<quote<flip>, quote<wrap_value_elements>, lift<type_list>>,
            F,
            bind<quote<flip>, quote<unwrap_value_elements>, liftv<List>>
        >
    > {};

    // �ʃt�@�C���Ő錾����Ă�����

    template <template <auto...> class List, auto... Parameters>
    struct reverse<List<Parameters...>> : public behave_as_type_list_arg<quote<reverse>, List<Parameters...>> {};

    template <template <auto...> class List, auto... Parameters>
    struct rotatel<List<Parameters...>> : public behave_as_type_list_arg<quote<rotatel>, List<Parameters...>> {};

    template <template <auto...> class List, auto... Parameters>
    struct rotater<List<Parameters...>> : public behave_as_type_list_arg<quote<rotater>, List<Parameters...>> {};

    template <class Head, class... Lists>
    requires (has_value_parameters_v<Head> && (has_value_parameters_v<Lists> && ...))
    struct concat_list<Head, Lists...> : public apply<
        bind<quote<flip>, quote<unwrap_value_elements>, get_empty_list_t<Head>>,
        concat_list_t<wrap_value_elements_t<Head, lift<type_list>>, wrap_value_elements_t<Lists, lift<type_list>>...>
    > {};
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_ARGMENT_HPP
