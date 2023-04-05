///----------------------------------
/// @file str/cstr.hpp 
/// @brief 定数式に置ける文字列
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_STR_CSTR_HPP
#define TUUTILCPP_INCLUDE_GUARD_STR_CSTR_HPP

namespace tuutil::str
{
    /**
     * @fn
     * @brief subjectのposで指定した位置にsearchで指定した文字列が存在委するか判定
     * @param search 検索する文字列
     * @param subject 対象文字列
     * @param pos 判定位置
    */
    inline constexpr auto exists_in_position(
        std::string_view search,
        std::string_view subject,
        std::size_t pos
    ) {
        // そもそもpos以降の文字数が、searchより短い場合存在のしようがない
        if (search.size() > subject.size() - pos) return false;
        // 一致の確認
        for (std::size_t i = 0; i < search.size(); i++) {
            if (subject[i + pos] != search[i]) return false;
        }
        return true;
    }

    /**
     * @fn
     * @brief subjectの中にsearchが含まれているか検索する
     * @param search 検索する文字列
     * @param subject 対象文字列
     * @param offset 検索開始位置
    */
    inline constexpr auto find(
        std::string_view search,
        std::string_view subject,
        std::size_t offset = 0
    ) {
        for (std::size_t i = offset; i < subject.size(); i++)
            if (exists_in_position(search, subject, i)) return i;
        return std::string_view::npos;
    }

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
        constexpr operator std::string() const noexcept { return std::string(this->data()); }

        /**
         * @fn
         * @brief 保持している文字列の長さを取得
        */
        constexpr auto size() const noexcept { return view().size(); }

        /**
         * @fn
         * @brief 保持している文字列が空の場合真
        */
        constexpr auto empty() const noexcept { return view().empty(); }

        /**
         * @fn
         * @brief sの文字列が出現する数をカウント
        */
        constexpr auto count(std::string_view s) const
        {
            const auto sv = this->view();
            std::size_t cnt = 0;
            for (int i = 0; i < sv.size(); i++)
                if (exists_in_position(s, sv, i)) cnt++;
            return cnt;
        }

        /**
         * @fn
         * @brief 文字列が最初に出現する場所を検索する
        */
        constexpr auto find(std::string_view s, std::size_t offset = 0) const { return str::find(s, this->view(), offset); }

        /**
         * @fn
         * @brief 指定の文字、文字列が含まれるとき真
        */
        constexpr auto contains(std::string_view s) const { return this->find(s) != std::string_view::npos; }

        /**
         * @fn
         * @brief 文字集合を検索し、最初に見つけた文字の位置と、そこから連続して合致し続けた数のペアを返す
         * @param char_set 文字集合
         * @param offset 開始位置
         * @param is_allow falseを指定すると文字集合にマッチしないときの範囲を返す
         * @return 見つからない場合、位置がstd::string_view::npos
        */
        constexpr auto get_charset_match_range(std::string_view char_set, std::size_t offset = 0, bool is_allow = true) const
        {
            std::size_t s_pos = std::string_view::npos, cnt{};
            for (std::size_t i = offset; i < this->size(); i++) {
                const auto is_find = str::find(cstr{{this->at(i), '\0'}}, char_set) != std::string_view::npos;
                if (is_allow != is_find) break;
                s_pos = (std::min)(s_pos, i);
                cnt++;
            }
            return std::array{s_pos, cnt};
        }

        /**
         * @fn
         * @brief 文字集合を検索し、最初に見つからなかった文字の位置と、そこから連続して合致し続けなかった数のペアを返す
         * @param char_set 文字集合
         * @param offset 開始位置
        */
        constexpr auto get_charset_unmatch_range(std::string_view char_set, std::size_t offset = 0) const
        {
            return this->get_charset_match_range(char_set, offset, false);
        }

        /**
         * @fn
         * @brief 全ての要素が指定された文字集合に含まれているとき真
        */
        constexpr auto match_all_charset(std::string_view char_set, bool is_allow = true) const
        {
            auto [s_pos, cnt] = this->get_charset_match_range(char_set, 0, is_allow);
            return s_pos == 0 && cnt == this->size();
        }

        /**
         * @fn
         * @brief 全ての要素が指定した文字集合に含まれていないとき真
        */
        constexpr auto unmatch_all_charset(std::string_view char_set) const { return this->match_all_charset(char_set, false); }

        /**
         * @fn
         * @brief 部分文字列を取得する
        */
        template <std::size_t Offset, std::size_t Count = max_size>
        requires (Offset <= max_size)
        constexpr auto substr() const
        {
            constexpr auto len = (std::min)(Count, max_size - Offset);
            cstr<len + 1> _s{};
            for (auto i = 0; i < len; i++) _s[i] = this->data()[i + Offset];
            return _s;
        }

        /**
         * @fn
         * @brief 指定文字数、文字列の前後から削る
        */
        template <std::size_t Prefix, std::size_t Suffix>
        requires (Prefix + Suffix <= max_size)
        constexpr auto remove_prefix_suffix() const
        {
            return this->template substr<Prefix, max_size - Prefix - Suffix>();
        }

        /**
         * @fn
         * @brief 先頭のN文字を削除する
        */
        template <std::size_t Prefix>
        constexpr auto remove_prefix() const { return this->template remove_prefix_suffix<Prefix, 0>(); }

        /**
         * @fn
         * @brief 末尾のN文字を削除する
        */
        template <std::size_t Suffix>
        constexpr auto remove_suffix() const { return this->template remove_prefix_suffix<0, Suffix>(); }
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
    constexpr auto substr(const cstr<N>& s) { return s.template substr<Offset, Count>(); }
    // Offsetから文字列の最後尾まで
    template <std::size_t Offset, std::size_t N>
    constexpr auto substr(const cstr<N>& s) { return s.template substr<Offset>(); }

    /**
     * @fn
     * @brief targetを区切り文字で分割した結果をviewの配列で返却
    */
    template <cstr target, cstr delimiter>
    constexpr auto split()
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

    template <std::size_t N1, std::size_t N2>
    constexpr bool operator==(const cstr<N1>& s1, const cstr<N2>& s2) { return s1.view() == s2.view(); }

    template <std::size_t N1, std::size_t N2>
    constexpr std::strong_ordering operator<=>(const cstr<N1>& s1, const cstr<N2>& s2) { return s1.view() <=> s2.view(); }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator+(const cstr<N1>& s1, const cstr<N2>& s2) { return concat(s1, s2); }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator+(const char (&s1)[N1], const cstr<N2>& s2) { return cstr<N1>{s1} + s2; }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator+(const cstr<N1>& s1, const char (&s2)[N2]) { return s1 + cstr<N2>{s2}; }

    template <std::size_t N>
    constexpr auto operator+(const char c, const cstr<N>& s) { return char_to_cstr(c) + s; }

    template <std::size_t N>
    constexpr auto operator+(const cstr<N>& s, const char c) { return s + char_to_cstr(c); }

    /**
     * @fn
     * @brief パラメータパックの固定長文字列を、区切り文字列を挟んで一つの文字へと結合する
     * @param separator 区切り文字列
     * @param head パラメータパックの先頭文字列(特にstr特別する必要はない)
     * @param strs 固定長文字列のパラメータパック
    */
    template <std::size_t SepN, std::size_t HeadN, std::size_t... Sizes>
    constexpr auto join(const cstr<SepN>& separator, const cstr<HeadN>& head, const cstr<Sizes>&... strs)
    {
        return head + ((separator + strs) + ...);
    }

    template <std::size_t SepN, std::size_t... Sizes>
    constexpr auto join(const char (&separator)[SepN], const cstr<Sizes>&... strs) { return join(cstr<SepN>{separator}, strs...); }

    template <std::size_t... Sizes>
    constexpr auto join(const char c, const cstr<Sizes>&... strs) { return join(char_to_cstr(c), strs...); }

    /**
     * @fn
     * @brief パラメータパックの固定長文字列を、区切り文字列を挟んで一つの文字へと結合する
     * @tparam Separator 区切り文字列
     * @tparam List 固定長文字列のパラメータパックを持つ型
    */
    template <cstr Separator, class List> struct cstrs_join;
    template <cstr Separator, template <cstr...> class List, cstr... Strs>
    struct cstrs_join<Separator, List<Strs...>> : public mpl::value_constant<join(Separator, Strs...)> {};

    /**
     * @fn
     * @brief パラメータパックの固定長文字列を、区切り文字列を挟んで一つの文字へと結合する
     * @tparam Separator 区切り文字列
     * @tparam Strs 固定長文字列のパラメータパック
    */
    template <cstr Separator, class List> constexpr auto cstrs_join_v = cstrs_join<Separator, List>::value;

    /**
     * @fn
     * @brief テンプレート引数で渡した整数値をcstrに変換する。進数を渡すことで、(2,8,16)進数リテラルのような文字列に変換する
    */
    template <auto V, int Hex = 10, bool UsePrefix = false>
    requires (V >= 0 && Hex >= 2 && (std::integral<decltype(V)> || std::convertible_to<decltype(V), utility::big_int<2>>))
    constexpr auto to_string()
    {
        // 10進数として桁数を取得
        constexpr auto len = utility::get_digit<V, Hex>();
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
            if constexpr (Hex == 2) return "0b" + s;
            else if constexpr (Hex == 8) return "0" + s;
            else if constexpr (Hex == 16) return "0x" + s;
            else return s;
        }
        else return s;
    }
    // 負数の場合
    template <std::integral auto V, std::size_t Hex = 10, bool UsePrefix = false>
    requires (V < 0)
    constexpr auto to_string() { return "-" + to_string<-V, Hex, UsePrefix>(); }

    /**
     * @fn
     * @brief 文字列を整数型に変換
    */
    template <std::integral T>
    constexpr T to_int(std::string_view s)
    {
        T val{};
        for (const auto c: s) {
            if (c < '0' || c > '9') break;
            val *= 10;
            val += (c - '0');
        }
        return val;
    }

    /**
     * @fn
     * @brief cstrであるかのメタ関数
    */
    template <class T> struct is_cstr : public std::false_type{};
    template <std::size_t N> struct is_cstr<cstr<N>> : public std::true_type{};
}

#endif // TUUTILCPP_INCLUDE_GUARD_STR_CSTR_HPP
