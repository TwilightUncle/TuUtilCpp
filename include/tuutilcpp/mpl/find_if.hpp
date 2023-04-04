///----------------------------------
/// @file mpl/find_if.hpp 
/// @brief ���^�֐��ɂ��^�p�����[�^�p�b�N�̌���
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_FIND_IF_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_FIND_IF_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief ��������
    */
    template <MetaCallable Pred, int N, class... Parameters> struct find_if_impl;
    template <MetaCallable Pred, int N, class Head, class... Parameters>
    struct find_if_impl<Pred, N, Head, Parameters...> : public std::conditional_t<
        apply_v<Pred, Head>,
        type_value_pair<Head, N>,
        find_if_impl<Pred, N + 1, Parameters...>
    > {};
    template <MetaCallable Pred, int N>
    struct find_if_impl<Pred, N> : public type_value_pair<ignore_type, int(-1)> {};

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���ŏ��̌^�܂��͈ʒu�̃C���f�b�N�X���擾
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> struct find_if;
    template <MetaCallable Pred, template <class...> class List, class... Parameters>
    struct find_if<Pred, List<Parameters...>> : public find_if_impl<Pred, 0, Parameters...> {};

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���ŏ��̌^���擾
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> using find_if_t = typename find_if<Pred, TypeList>::type;

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���ŏ��̌^�̈ʒu���擾 
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> constexpr auto find_if_v = find_if<Pred, TypeList>::value;

    /**
     * @fn
     * @brief �w������̌^�����݂��Ă��邩����
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> using exists_if = std::negation<std::is_same<find_if_t<Pred, TypeList>, ignore_type>>;
    
    /**
     * @fn
     * @brief �w������̌^�����݂��Ă��邩����
     * @tparam Pred ����p���^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class TypeList> constexpr auto exists_if_v = exists_if<Pred, TypeList>::value;

    /**
     * @fn
     * @brief �w��̌^�����݂��Ă��邩����
     * @tparam T �����Ώۂ̌^
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class T, class TypeList> using exists = exists_if<bind<quote<std::is_same>, T>, TypeList>;
    
    /**
     * @fn
     * @brief �w��̌^�����݂��Ă��邩����
     * @tparam T �����Ώۂ̌^
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class T, class TypeList> constexpr auto exists_v = exists<T, TypeList>::value;

    /**
     * @fn
     * @brief �w�肵��T��TypeList�̉��Ԗڂ̗v�f�ł��邩���擾����B������Ȃ��ꍇ��-1
     * @tparam T �����Ώۂ̌^
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class T, class TypeList> using index_of = find_if<bind<quote<std::is_same>, T>, TypeList>;

    /**
     * @fn
     * @brief �w�肵��T��TypeList�̉��Ԗڂ̗v�f�ł��邩���擾����B������Ȃ��ꍇ��-1
     * @tparam T �����Ώۂ̌^
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <class T, class TypeList> constexpr auto index_of_v = index_of<T, TypeList>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_FIND_IF_HPP
