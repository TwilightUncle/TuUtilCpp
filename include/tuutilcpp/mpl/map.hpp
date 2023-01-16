///----------------------------------
/// @file mpl/map.hpp 
/// @brief ���^�֐��ɂ��^�p�����[�^�p�b�N�̈ꊇ�K�p
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief �p�����[�^�p�b�N�ɑ΂��郁�^�֐��̈ꊇ�K�p
     * @tparam F �K�p���郁�^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class TypeList> struct map;
    template <MetaCallable F, template <class...> class List, class... Parameters>
    struct map<F, List<Parameters...>> : public std::type_identity<List<apply_t<F, Parameters>...>> {};
    template <MetaCallable F>
    struct map<F, ignore_type> : public std::type_identity<ignore_type> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�ɑ΂��郁�^�֐��̈ꊇ�K�p
     * @tparam F �K�p���郁�^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable F, class TypeList> using map_t = map<F, TypeList>::type;

    /**
     * @fn
     * @brief �����ɍ��v����p�����[�^�p�b�N�S�ĂɃ��^�֐���K�p����
     * @tparam Pred ����p�֐�
     * @tparam F �K�p���郁�^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, MetaCallable F, class TypeList> using map_if = map<bind<quote<apply_if>, Pred, F>, TypeList>;

    /**
     * @fn
     * @brief �����ɍ��v����p�����[�^�p�b�N�S�ĂɃ��^�֐���K�p����
     * @tparam Pred ����p�֐�
     * @tparam F �K�p���郁�^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, MetaCallable F, class TypeList> using map_if_t = map_if<Pred, F, TypeList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP
