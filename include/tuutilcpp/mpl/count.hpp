///----------------------------------
/// @file mpl/reverse.hpp 
/// @brief �^�p�����[�^�p�b�N�̕��т𔽓]
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_COUNT_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_COUNT_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̐��𐔂���
     * @tparam T �^�p�����[�^�p�b�N�����^
    */
    template <class T> struct count;
    template <template <class...> class List, class... Parameters>
    struct count<List<Parameters...>> : public std::integral_constant<std::size_t, sizeof...(Parameters)> {};
    template <template <auto...> class List, auto... Parameters>
    struct count<List<Parameters...>> : public std::integral_constant<std::size_t, sizeof...(Parameters)> {};
    template <>
    struct count<ignore_type> : public std::integral_constant<std::size_t, 0> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�̐��𐔂���
     * @tparam T �^�p�����[�^�p�b�N�����^
    */
    template <class T> constexpr auto count_v = count<T>::value;

    /**
     * @fn
     * @brief �����ɍ��v����p�����[�^�p�b�N�̐��𐔂���
     * @tparam Pred ����p���^�֐�
     * @tparam T �^�p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class T> using count_if = relay<
        T,
        type_list<
            bind<quote<extract_if>, Pred>,
            quote<count>
        >
    >;

    template <MetaCallable Pred, class T> constexpr auto count_if_v = count_if<Pred, T>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_COUNT_HPP
