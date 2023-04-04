///----------------------------------
/// @file mpl/meta_callback.hpp 
/// @brief ���^�֐����^�e���v���[�g�p�����[�^�̕����ł��g�p�ł���悤�ɂ���
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_MPL_META_CALLBACK_HPP
#define TUUTILCPP_INCLUDE_GUARD_MPL_META_CALLBACK_HPP

namespace tuutil::mpl
{
    template <class T>
    concept ReturenValueMetaFunction = requires {
        T::value;
    };

    /**
     * @fn
     * @brief �^T�� constexpr �����o�ϐ��������Ă��邩����
    */
    template <class T> using has_constexpr_value_member = std::bool_constant<ReturenValueMetaFunction<T>>;

    /**
     * @fn
     * @brief �^T�� constexpr �����o�ϐ��������Ă��邩����
    */
    template <class T> constexpr auto has_constexpr_value_member_v = has_constexpr_value_member<T>::value;

    template <class T>
    concept ReturenTypeMetaFunction = requires {
        typename T::type;
    };

    template <class T>
    concept MetaFunction = ReturenValueMetaFunction<T> || ReturenTypeMetaFunction<T>;

    /**
     * @fn
     * @brief ���^�֐����^�Ɠ��l�Ɉ�����悤�ɂ���
     * @tparam MetaF ���^�֐�
    */
    template <template <class...> class MetaF> struct quote {};

    /**
     * @fn
     * @brief ���^�֐������̕����K�p
     * @tparam QuotedMetaF quote�܂���bind���ꂽ���^�֐�
     * @tparam PartialArgs �����K�p����^�p�����[�^���
    */
    template <class QuotedMetaF, class... PartialArgs> struct bind;
    template <template <class...> class MetaF, class... PartialArgs>
    struct bind<quote<MetaF>, PartialArgs...> {};
    // bind�������bind
    template <class QuotedMetaF, class... PartialArgs, class... PartialArgs2>
    struct bind<bind<QuotedMetaF, PartialArgs...>, PartialArgs2...> {};

    /**
     * @fn
     * @brief quote�܂���bind������
     * @tparam T �����Ώی^
    */
    template <class T> struct is_meta_callback : public std::false_type {};
    template <template <class...> class MetaF>
    struct is_meta_callback<quote<MetaF>> : public std::true_type {};
    template <class QuotedMetaF, class... PartialArgs>
    struct is_meta_callback<bind<QuotedMetaF, PartialArgs...>> : public std::true_type {};

    /**
     * @fn
     * @brief quote�܂���bind������
     * @tparam T �����Ώی^
    */
    template <class T> constexpr auto is_meta_callback_v = is_meta_callback<T>::value;

    /**
     * @brief quote�܂���bind�ł��鐧��
    */
    template <class T>
    concept MetaCallable = is_meta_callback_v<T>;

    /**
     * @fn
     * @brief quote�܂���bind��n���B type�Ăяo���Ŏ��s
     * @tparam F quote�܂���bind���ꂽ���^�֐�
     * @tparam Args �K�p�������
    */
    template <class T, class... Args> struct apply;
    // ������n���ă��^�֐����s
    template <template <class...> class MetaF, class... Args>
    struct apply<quote<MetaF>, Args...> : public MetaF<Args...>
    {
        static_assert(MetaFunction<MetaF<Args...>>, "Invalid argment. Non-metafunction specified for argument F.");
    };
    // �����K�p���ꂽ������W�J
    template <class QuotedMetaF, class... PartialArgs, class... Args>
    struct apply<bind<QuotedMetaF, PartialArgs...>, Args...> : public apply<QuotedMetaF, PartialArgs..., Args...> {};

    template <class T, class... Args>
    using apply_t = typename apply<T, Args...>::type;

    template <MetaCallable F, class... Args>
    requires (ReturenValueMetaFunction<apply<F, Args...>>)
    constexpr auto apply_v = apply<F, Args...>::value;
}

#endif // TUUTILCPP_INCLUDE_GUARD_MPL_META_CALLBACK_HPP
