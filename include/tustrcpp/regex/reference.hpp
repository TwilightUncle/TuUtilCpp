///----------------------------------
/// @file quantifier.hpp 
/// @brief regex.hpp関連
///----------------------------------
#ifndef TUSTRCPP_INCLUDE_GUARD_REGEX_REFERENCE_HPP
#define TUSTRCPP_INCLUDE_GUARD_REGEX_REFERENCE_HPP

namespace tustr
{
    template <cstr Pattern, std::size_t Pos>
    struct regex_reference_parser
    {
        static constexpr auto begin_pos = Pos;
        static constexpr auto end_pos = Pos + Pattern.get_charset_match_range("0123456789", Pos)[1];
        static constexpr auto reference_index = to_int<std::size_t>(Pattern.substr<Pos, end_pos - Pos>().view()) - 1;

        /**
         * @fn
         * @brief 解析結果生成された処理。既にキャプチャが存在することが前提のため、is_pos_lockの真偽に関わらず位置は常に固定でOK
        */
        template <std::size_t N>
        static constexpr regex_match_range generated_func(std::string_view s, std::size_t offset, bool is_pos_lock, regex_capture_store<N>& cs)
        {
            const auto search_str = cs.get(reference_index);
            return exists_in_position(search_str, s, offset)
                ? regex_match_range{offset, search_str.size()}
                : regex_match_range::make_unmatch();
        }
    };
}

#endif // TUSTRCPP_INCLUDE_GUARD_REGEX_REFERENCE_HPP
