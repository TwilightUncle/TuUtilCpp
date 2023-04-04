///----------------------------------
/// @file mpl/extract_if.hpp 
/// @brief ���^�֐��ɂ��^�p�����[�^�p�b�N�̒��o
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP

namespace tuutil::mpl
{
    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���^�𒊏o
     * @tparam Pred ����p���^�֐�
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class List> struct extract_if;
    template <MetaCallable Pred, template <class...> class List, class Head, class... Parameters>
    struct extract_if<Pred, List<Head, Parameters...>> : public std::conditional_t<
        apply_v<Pred, Head>,
        foldl<bind<quote<push_back_if>, Pred>, List<Head>, List<Parameters...>>,
        extract_if<Pred, List<Parameters...>>
    > {};
    template <MetaCallable Pred, template <class...> class List, class Head>
    struct extract_if<Pred, List<Head>>
        : public std::conditional<apply_v<Pred, Head>, List<Head>, ignore_type>
    {};
    template <MetaCallable Pred, template <auto...> class List, auto... Parameters>
    struct extract_if<Pred, List<Parameters...>>
        : public behave_as_type_list_arg<bind<quote<extract_if>, Pred>, List<Parameters...>>
    {};

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���^�𒊏o
     * @tparam Pred ����p���^�֐�
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class List> using extract_if_t = typename extract_if<Pred, List>::type;

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���^�����O(extract_if�̔��莮�̌��ʂ𔽓]�����Ă���)
     * @tparam Pred ����p���^�֐�
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class List> using remove_if = extract_if<bind<quote<flip>, quote<relay>, type_list<Pred, quote<std::negation>>>, List>;

    /**
     * @fn
     * @brief ���^�֐��ɂ�锻�肪�^�ł���^�����O
     * @tparam Pred ����p���^�֐�
     * @tparam List �p�����[�^�p�b�N�����^
    */
    template <MetaCallable Pred, class List> using remove_if_t = typename remove_if<Pred, List>::type;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_EXTRACT_IF_HPP
