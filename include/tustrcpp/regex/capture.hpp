///----------------------------------
/// @file capture.hpp 
/// @brief regex.hpp関連
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_CAPUTURE_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_CAPUTURE_HPP

namespace tustr
{
    /**
     * @class
     * @brief キャプチャグループの解析
    */
    template <cstr Pattern, std::size_t Pos>
    struct regex_capture_parser
    {
        using brancket_inner = regex_bracket_inner<Pattern, Pos>;
        static constexpr auto inner_pattern = brancket_inner::value;

        static_assert(inner_pattern.size() > 0 && is_collect_regex_back_slash(inner_pattern.view()));

        static constexpr auto begin_pos = Pos;
        static constexpr auto end_pos = brancket_inner::end_pos;

        static constexpr auto is_capture = !(inner_pattern.find("?:") == 0);

    private:
        /**
         * @fn
         * @brief キャプチャの名称を取得
        */
        template <char C> requires (C == '?' && inner_pattern.find("?<") == 0)
        static constexpr auto extract_capture_name()
        {
            using named_bracket = regex_bracket_inner<inner_pattern, 1>;
            constexpr auto named_inner = named_bracket::value;

            static_assert(
                named_inner.size() > 0 && named_inner.match_all_charset(regex_char_class::get_const_char_set<'w'>()),
                "An error has occurred. [regex_capture_parser]"
            );

            return named_inner;
        }
        template <char C>
        static constexpr auto extract_capture_name() { return cstr{""}; }

    public:
        static constexpr auto name = extract_capture_name<inner_pattern[0]>();
        static constexpr auto is_named = name.size() > 0;

        // キャプチャの有無、キャプチャ名指定部分を取り除いたパターン
        static constexpr auto capture_pattern = inner_pattern.remove_prefix<(is_capture ? 0 : 2) + (is_named ? name.size() + 3 : 0)>();

        /**
         * @fn
         * @brief 解析結果生成された処理
        */
        static constexpr std::size_t generated_func(const std::string_view& s, std::size_t offset, bool is_pos_lock)
        {
            using parser = regex<capture_pattern, regex_parser>;
            const auto result = parser::run(s, offset, is_pos_lock);
            return (result == std::string_view::npos)
                ? result
                : result + offset;
        }
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_CAPUTURE_HPP
