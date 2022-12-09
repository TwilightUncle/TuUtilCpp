#pragma once

namespace tudb
{
    /**
     * @class
     * @brief 固定長領域の文字列
    */
    template <std::size_t N>
    struct cstr : public std::array<char, N>
    {
    private:
        using parent_type = std::array<char, N>;

    public:
        // 終端文字も格納済みであるものとして扱う
        static constexpr auto max_size = N - 1;

        constexpr cstr() : parent_type{} {}
        constexpr cstr(const char (&str_literal)[N])
            : parent_type{}
        {
            for (auto i = 0; i < max_size; i++)
                this->data()[i] = str_literal[i];
        }

        /**
         * @fn
         * @brief string_viewを取得
        */
        constexpr auto view() const noexcept { return std::string_view{this->data()}; }
        constexpr operator std::string_view() const noexcept { return this->view(); }

        /**
         * @fn
         * @brief 保持している文字列の長さを取得
        */
        constexpr auto size() const noexcept { return view().size(); }

        /**
         * @fn
         * @brief sの文字列が出現する数をカウント
        */
        constexpr auto count(const std::string_view& s) const
        {
            const auto sv = this->view();
            std::size_t cnt = 0;
            for (int i = 0; i < sv.size(); i++) {
                if (sv[i] == s[0]) {
                    for (int j = 0; j < s.size() && i + j < sv.size(); j++) {
                        if (sv[i + j] != s[j]) break;
                        if (j == s.size() - 1) cnt++;
                    }
                }
            }
            return cnt;
        }

        /**
         * @fn
         * @brief 文字列が最初に出現する場所を検索する
        */
        constexpr auto find(const std::string_view& s, std::size_t offset = 0) const
        {
            const auto sv = this->view();
            for (std::size_t i = offset; i < sv.size(); i++) {
                if (sv[i] == s[0]) {
                    for (std::size_t j = 0; j < s.size() && i + j < sv.size(); j++) {
                        if (sv[i + j] != s[j]) break;
                        if (j == s.size() - 1) return i;
                    }
                }
            }
            return std::string_view::npos;
        }
    };

    /**
     * @fn
     * @brief 文字から長さ1の文字列を作成
    */
    inline constexpr auto char_to_cstr(char ch) { return cstr{{ch, '\0'}}; }

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
    requires (Offset < cstr<N>::max_size)
    constexpr auto substr(const cstr<N>& s)
    {
        constexpr auto len = (std::min)(Count, cstr<N>::max_size - Offset);
        cstr<len + 1> _s{};
        for (auto i = 0; i < len; i++) _s[i] = s[i + Offset];
        return _s;
    }
    // Offsetから文字列の最後尾まで
    template <std::size_t Offset, std::size_t N>
    constexpr auto substr(const cstr<N>& s) { return substr<Offset, cstr<N>::max_size - Offset, N>(s); }

    /**
     * @fn
     * @brief targetを区切り文字で分割した結果をviewの配列で返却
    */
    template <cstr target, cstr delimiter>
    constexpr auto devide_by_delimiter()
    {
        constexpr auto size = target.count(delimiter);
        auto res = std::array<std::string_view, size + 1>{};
        auto sv = target.view();

        for (std::size_t pos{}, prev{}, i{}; (pos = target.find(delimiter, pos)) != std::string_view::npos; prev = pos) {
            res[i++] = sv.substr(0, pos - prev);
            sv = sv.substr((pos += delimiter.size()) - prev);
        }
        res[size] = sv;
        return res;
    }

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
     * @brief テンプレート引数で渡した整数値をcstrに変換する。進数を渡すことで、(2,8,16)進数リテラルのような文字列に変換する
    */
    template <auto V, int Hex = 10, bool UsePrefix = false>
    requires (V >= 0 && Hex >= 2 && (std::integral<decltype(V)> || tudb::is_big_int<decltype(V)>::value))
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

    /**
     * @fn
     * @brief 文字列を整数型に変換
    */
    template <std::integral T>
    constexpr T to_int(const std::string& s)
    {
        T val{};
        for (const auto c: s) {
            val *= 10;
            val += (c - '0');
        }
        return val;
    }
    template <std::integral T>
    constexpr T to_int(const std::string_view& s) { return to_int<T>(std::string(s)); }

    /**
     * @fn
     * @brief cstrであるかのメタ関数
    */
    template <class T> struct is_cstr : public std::false_type{};
    template <std::size_t N> struct is_cstr<cstr<N>> : public std::true_type{};
}
