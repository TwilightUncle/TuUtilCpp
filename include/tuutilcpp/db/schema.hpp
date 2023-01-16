///----------------------------------
/// @file column.hpp
/// @brief DB—ñ’è‹`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_SCHEMEA_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_SCHEMEA_HPP

namespace tuutil::db
{
    template <str::cstr Name, TableListDefinable TableDefinitionList>
    struct define_schema
    {
        static constexpr auto name = Name;
    };

}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_SCHEMEA_HPP
