///----------------------------------
/// @file concepts.hpp
/// @brief 汎用の制約定義
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_CONCEPTS_HPP
#define TUUTILCPP_INCLUDE_GUARD_CONCEPTS_HPP

namespace tudb
{
    // 前方宣言
    template <class Head, class... Parameters> struct is_unique;

    /**
     * @brief 判定を行うメタ関数であることの制約(bool_constantを継承していれば通過)
    */
    template <typename T>
    concept Testable = requires {
        T::value;
        typename T::value_type;
        requires std::is_same_v<bool, typename T::value_type>;
    };

    /**
     * @brief 二つの型引数を受け取り判定するメタ関数であることを制約する
    */
    template <template <class, class> class T, class ArgT1, class ArgT2>
    concept TwoTypeArgsTestable = Testable<T<ArgT1, ArgT2>>;

    /**
     * @brief 非型引数と型引数を一つずつ受け取り判定するメタ関数であることを制約する
    */
    template <template <auto, class> class T, auto ArgV, class ArgT>
    concept FirstValueSecondTypeArgsTestable = Testable<T<ArgV, ArgT>>;

    /**
     * @brief 型取得メタ関数である制約
    */
    template <typename T>
    concept TypeGetable = requires {
        typename T::type;
    };

    /**
     * @brief 二つの型引数を受け取り型を返すメタ関数であることを制約する
    */
    template <template <class, class> class T, class ArgT1, class ArgT2>
    concept TwoTypeArgsTypeGetable = TypeGetable<T<ArgT1, ArgT2>>;

    /**
     * @brief 重複なしのパラメータパックをもつ制約
    */
    template <class... Types>
    concept TemplateTypesUniqueable = requires {
        requires (sizeof...(Types) > 0);
        requires is_unique<Types...>::value;
    };
}

#endif // TUUTILCPP_INCLUDE_GUARD_CONCEPTS_HPP
