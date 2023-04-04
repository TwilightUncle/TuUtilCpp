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
    template <MetaCallable F, class TypeList> using map_t = typename map<F, TypeList>::type;

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
    template <MetaCallable Pred, MetaCallable F, class TypeList> using map_if_t = typename map_if<Pred, F, TypeList>::type;

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�ɑ΂��郁�^�֐��̈ꊇ�K�p���s���A�S�Ă̗v�f��l�ɕύX����
     * @tparam F �K�p���郁�^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam NewList �ϊ���̔�^�v�f���i�[���郊�X�g
    */
    template <MetaCallable F, class TypeList, LiftedvList NewList = liftv<value_list>> struct t_v_map;
    template <MetaCallable F, template <class...> class List, template <auto...> class NewList, class... Parameters>
    struct t_v_map<F, List<Parameters...>, liftv<NewList>> : public std::type_identity<NewList<apply_v<F, Parameters>...>> {};
    template <MetaCallable F, LiftedvList NewList>
    struct t_v_map<F, ignore_type, NewList> : public std::type_identity<ignore_type> {};

    /**
     * @fn
     * @brief �p�����[�^�p�b�N�ɑ΂��郁�^�֐��̈ꊇ�K�p���s���A�S�Ă̗v�f��l�ɕύX����
     * @tparam F �K�p���郁�^�֐�
     * @tparam TypeList �^�̃p�����[�^�p�b�N�����^
     * @tparam NewList �ϊ���̔�^�v�f���i�[���郊�X�g
    */
    template <MetaCallable F, class TypeList, LiftedvList NewList = liftv<value_list>> using t_v_map_t = typename t_v_map<F, TypeList, NewList>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_MAP_HPP
