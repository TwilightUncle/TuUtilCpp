///----------------------------------
/// @file mpl/unique.hpp 
/// @brief �^�p�����[�^�p�b�N����ӂɂ���
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_UNIQUE_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_UNIQUE_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief ��������
    */
    template <class TypeList1, class TypeList2> struct unique_impl;
    template <template <class...> class List, class Dest, class Head, class... Parameters>
    struct unique_impl<List<Head, Parameters...>, Dest> : public unique_impl<
        remove_if_t<bind<quote<std::is_same>, Head>, List<Parameters...>>,
        push_back_t<Dest, Head>
    > {};
    template <template <class...> class List, class Dest, class Head>
    struct unique_impl<List<Head>, Dest> : public unique_impl<ignore_type, push_back_t<Dest, Head>> {};
    template <class Dest>
    struct unique_impl<ignore_type, Dest> : public std::type_identity<Dest> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̏d����r������
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <class List> struct unique;
    // �^�o�[�W����
    template <template <class...> class List, class... Parameters>
    struct unique<List<Parameters...>> : public unique_impl<List<Parameters...>, lift<List>> {};
    // ��^�o�[�W����
    template <template <auto...> class List, auto... Parameters>
    struct unique<List<Parameters...>> : public behave_as_type_list_arg<quote<unique>, List<Parameters...>> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̏d����r������
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <class List> using unique_t = typename unique<List>::type;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N����ӂ�����
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <class List> using is_unique = std::is_same<List, unique_t<List>>;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N����ӂ�����
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <class List> constexpr auto is_unique_v = is_unique<List>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_UNIQUE_HPP
