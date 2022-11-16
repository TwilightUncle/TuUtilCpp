#pragma once

namespace tudb
{
    /**
     * @class
     * @brief パラメータパックで文字列リテラルを使用するための型
    */
    template <std::size_t N>
    struct cstr : public std::array<char, N>
    {
        // 終端文字も格納済みであるものとして扱う
        static constexpr auto size = N - 1;

        constexpr cstr() noexcept : std::array<char, N>{} {}
        constexpr cstr(const char (&str_literal)[N]) noexcept
            : std::array<char, N>{}
        {
            for (auto i = 0; i < size; i++)
                this->data()[i] = str_literal[i];
            this->data()[size] = '\0';
        }

        constexpr auto view() const noexcept { return std::string_view{this->data()}; }
    };

    /**
     * @fn
     * @brief 二つのcstr型文字列を結合する
    */
    template <std::size_t N1, std::size_t N2>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2)
    {
        // s1の終端文字は除去するため、-1
        cstr<N1 + N2 - 1> _s{};
        for (int i = 0; i < N1 - 1; i++) _s[i] = s1[i];
        for (int i = 0; i < N2; i++) _s[i + N1 - 1] = s2[i];
        return _s;
    }

    /**
     * @fn
     * @brief n個のcstr型文字列を一つに結合する
    */
    template <std::size_t N1, std::size_t N2, std::size_t... Sizes>
    constexpr auto concat(const cstr<N1>& s1, const cstr<N2>& s2, const cstr<Sizes>&... strs)
    {
        return concat(concat(s1, s2), strs...);
    }

    /**
     * @fn
     * @brief OffsetからCountの文字数分を新規文字列として切り出す
    */
    template <std::size_t Offset, std::size_t Count, std::size_t N>
    requires (Offset < cstr<N>::size)
    constexpr auto substr(const cstr<N>& s)
    {
        constexpr auto len = (std::min)(Count, cstr<N>::size - Offset);
        cstr<len + 1> _s{};
        for (auto i = 0; i < len; i++) _s[i] = s[i + Offset];
        return _s;
    }
    // Offsetから文字列の最後尾まで
    template <std::size_t Offset, std::size_t N>
    constexpr auto substr(const cstr<N>& s) { return substr<Offset, cstr<N>::size - Offset, N>(s); }

    /**
     * @fn
     * @brief 演算子オーバーロード(一致。不一致は導出されるので不要。細かい仕様はc++20の一貫比較で確認すること)
    */
    template <std::size_t N1, std::size_t N2>
    constexpr bool operator==(const cstr<N1>& s1, const cstr<N2>& s2) { return s1.view() == s2.view(); }

    /**
     * @fn
     * @brief 演算子オーバーロード(宇宙船。細かい仕様はc++20の一貫比較で確認すること)
    */
    template <std::size_t N1, std::size_t N2>
    constexpr std::strong_ordering operator<=>(const cstr<N1>& s1, const cstr<N2>& s2) { return s1.view() <=> s2.view(); }

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
        cstr<len + 1> s{};
        auto val = V;
        for (auto i = len; i > 0; val /= Hex) {
            const auto code = static_cast<char>(val % Hex);
            s[--i] = (code < 10)
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

    // template <auto V> struct to_cstr;

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
    // template <StringLiteralSpecificable auto... Strs>
    // struct concat_cstr { static constexpr auto value = concat(Strs...); };
    // template <StringLiteralSpecificable auto... Strs>
    // constexpr auto concat_cstr_v = concat_cstr<Strs...>::value;

    // template <StringLiteralSpecificable FormatStr, auto... Args>
    // struct format_cstr
    // {

    // };
}
