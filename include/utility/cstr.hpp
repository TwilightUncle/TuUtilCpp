#pragma once
#include <array>

namespace tudb
{
    /**
     * @class
     * @brief パラメータパックで文字列リテラルを使用するための型
    */
    template <std::size_t N>
    struct cstr
    {
        char buf[N];
        // 終端文字も格納済みであるものとして扱う
        static constexpr auto size = N - 1;
    };

    template <std::size_t N1, std::size_t N2>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2)
    {
        // s1の終端文字は除去するため、-1
        cstr<N1 + N2 - 1> _s{};
        for (int i = 0; i < N1 - 1; i++) _s.buf[i] = s1.buf[i];
        for (int i = 0; i < N2; i++) _s.buf[i + N1 - 1] = s2.buf[i];
        return _s;
    }
    template <std::size_t N1, std::size_t N2, std::size_t... Sizes>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2, const cstr<Sizes>&... strs)
    {
        return concat(concat(s1, s2), strs...);
    }

    template <std::size_t N1, std::size_t N2>
    constexpr bool operator==(const cstr<N1>& s1, const cstr<N2>& s2)
    {
        // 同じ要素数の場合、arrayに比較演算子の定義があるのでそれを適用
        if constexpr (N1 == N2)
            return std::to_array(s1.buf) == std::to_array(s2.buf);
        return false;
    }

    /**
     * @fn
     * @brief テンプレート引数で渡した整数値の桁数を取得する。第二引数は任意の進数を指定する
    */
    template <std::integral auto V, std::size_t Hex = 10>
    requires (Hex >= 2)
    constexpr auto get_digit()
    {
        std::size_t digit = 1;
        for (auto val = V; val /= static_cast<decltype(V)>(Hex); digit++);
        return digit;
    }

    /**
     * @fn
     * @brief テンプレート引数で渡した整数値をcstrに変換する。進数を渡すことで、(2,8,16)進数リテラルのような文字列に変換する
    */
    template <std::integral auto V, int Hex = 10, bool UsePrefix = false>
    requires (V >= 0 && Hex >= 2)
    constexpr auto to_string()
    {
        // 10進数として桁数を取得
        constexpr auto len = get_digit<V, Hex>();
        // 終端文字の長さも配列の要素数に含める
        cstr<len + 1> s;
        s.buf[len] = '\0';
        auto val = V;
        for (auto i = len; i > 0; val /= Hex) {
            const auto code = static_cast<char>(val % Hex);
            s.buf[--i] = (code < 10)
                ? '0' + code
                : 'A' + code - 10;
        }

        // 進数リテラルの接頭子を付与
        if constexpr (UsePrefix) {
            if constexpr (Hex == 2) return concat(cstr{"0b"}, s);
            else if constexpr (Hex == 8) return concat(cstr{"0"}, s);
            else if constexpr (Hex == 16) return concat(cstr{"0x"}, s);
            else return s;
        }
        else return s;
    }
    // 負数の場合
    template <std::integral auto V, std::size_t Hex = 10, bool UsePrefix = false>
    constexpr auto to_string() { return concat(cstr{"-"}, to_string<-V, Hex, UsePrefix>()); }

    template <auto V> struct to_cstr;

    /**
     * @fn
     * @brief cstrであるかのメタ関数
    */
    template <class T> struct is_cstr : public std::false_type{};
    template <std::size_t N> struct is_cstr<cstr<N>> : public std::true_type{};

    /**
     * @brief cstrであることを強制するコンセプト
    */
    template <class T>
    concept StringLiteralSpecificable = is_cstr<T>::value;

    // 関数でテンプレート引数に指定可能であった場合は不要なので消す
    // 型として文字を保持し続けるコンテナとして残したほうが良いか？
    template <StringLiteralSpecificable auto... Strs>
    struct concat_cstr { static constexpr auto value = concat(Strs...); };
    template <StringLiteralSpecificable auto... Strs>
    constexpr auto concat_cstr_v = concat_cstr<Strs...>::value;

    template <StringLiteralSpecificable FormatStr, auto... Args>
    struct format_cstr
    {

    };
}
