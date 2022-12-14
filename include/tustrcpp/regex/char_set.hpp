///----------------------------------
/// @file char_set.hpp 
/// @brief regex.hpp関連
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_CHAR_SET_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_CHAR_SET_HPP

namespace tustr
{
    /**
     * @class
     * @brief 文字集合を解析する
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_char_set_parser
    {
        using brancket_inner = regex_bracket_inner<Pattern, Pos>;
        static constexpr auto char_range = brancket_inner::value;

        static_assert(char_range.size() > 0 && is_collect_regex_back_slash(char_range.view()));

        static constexpr auto allow_or_deny = char_range[0] != '^';
        static constexpr auto begin_index = int(!allow_or_deny);
        static constexpr auto size = char_range.size();
        static constexpr auto begin_pos = Pos;
        static constexpr auto end_pos = brancket_inner::end_pos;

    private:
        /**
         * @fn
         * @brief 文字集合を構築する
        */
        template <int N>
        static constexpr auto make_regex_char_list()
        {
            if constexpr (N >= size) return cstr{""};
            else return concat(char_range.substr<N, 1>(), make_regex_char_list<N + 1>());
        }
        template <int N>
        requires (char_range[N] == '-')
        static constexpr auto make_regex_char_list()
        {
            if constexpr (begin_index == N || size - 1 == N)
                return concat(cstr{"-"}, make_regex_char_list<N + 1>());
            else {
                constexpr char begin_char = (std::min)(char_range[N - 1], char_range[N + 1]) + 1;
                constexpr char end_char = (std::max)(char_range[N - 1], char_range[N + 1]);
                cstr<2 + end_char - begin_char> str{};
                for (char c = 0; c < str.max_size; c++) str[c] = c + begin_char;
                return concat(str, make_regex_char_list<N + 2>());
            }
        }
        template <int N>
        requires (char_range[N] == '\\')
        static constexpr auto make_regex_char_list()
        {
            // 文字範囲の括弧内では「.」は特殊文字ではない
            if constexpr (char_range[N + 1] != '.')
                return concat(regex_char_class::get_const_char_set<char_range[N + 1]>(), make_regex_char_list<N + 2>());
            else return concat(cstr{"."}, make_regex_char_list<N + 2>());
        }

        /**
         * @fn
         * @brief バックスラッシュとセットの場合の文字集合を構築する
        */
        template <int N>
        static constexpr auto make_regex_bk_char_list()
        {
            if constexpr (N >= size) return cstr{""};
            else if constexpr (char_range[N] == '\\' && char_range[N + 1] != '.')
                return concat(regex_char_class::get_const_bk_char_set<char_range[N + 1]>(), make_regex_bk_char_list<N + 2>());
            else return make_regex_bk_char_list<N + 1>();
        }

    public:
        // 文字集合
        static constexpr auto value = make_regex_char_list<begin_index>();

        // エスケープされた文字に合致する文字集合
        static constexpr auto bk_value = make_regex_bk_char_list<begin_index>();

        /**
         * @fn
         * @brief 解析結果生成された処理
        */
        static constexpr std::size_t generated_func(const std::string_view& s, std::size_t offset)
        {
            const auto is_contains = value.contains(char_to_cstr(s[offset]));
            return allow_or_deny == is_contains
                ? offset + 1
                : std::string_view::npos;
        }
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_CHAR_SET_HPP
