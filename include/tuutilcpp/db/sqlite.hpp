///----------------------------------
/// @file type.hpp
/// @brief DB�t�B�[���h�p�̌^��`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_SQLITE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_SQLITE_HPP

#include <fstream>
#include <filesystem>
#include <tuutilcpp/str.hpp>
#include <tuutilcpp/db.hpp>
#include <tuutilcpp/db/connection/sqlite.hpp>
#include <tuutilcpp/db/query/sqlite.hpp>
#include <tuutilcpp/db/query/builder.hpp>

namespace tuutil::db
{
    /**
     * @class
     * @brief sqlite�ɂ��Ďw�肵���e�[�u����`��DB�������Ƃɐڑ��ƃN�G���쐬���J�v�Z�����������́B
     * DB�\���ɂ��Ă͌^��`�Ɠ����Ɏ��{����邱�Ƃ�z�肵�Ă��邽�߁A�e�[�u���\���ύX�Ȃǂ̋@�\�͎������Ȃ����̂Ƃ���B
     * �f�[�^�x�[�X�t�@�C�������݂��Ȃ��ꍇ�A�C���X�^���X�������ۂɎ����I�ɍ쐬�����
     * @tparam DbName �f�[�^�x�[�X����
     * @tparam TableDefinitionList �e�[�u����`�^
    */
    template <str::cstr DbName, TableListDefinable TableDefinitionList>
    struct sqlite
    {
        static constexpr auto name = DbName;

        sqlite(): con{}
        {
            number_of_valid_connections++;

            // ��ł��e�[�u�����쐬����Ă�����A�\�����������Ă�����̂Ƃ��ăe�[�u���ǉ������{���Ȃ�
            if (!exists_table<mpl::get_front_t<TableDefinitionList>>())
                for (auto s : create_querys_v) this->con.exec(std::string(s));
        }
        ~sqlite() { number_of_valid_connections--; }

        /**
         * @fn
         * @brief DbName�Ŏw�肵���t�@�C���ɑ΂��錻�ݍs���Ă���L���Ȑڑ��̐����J�E���g
        */
        static int count_valid_connections() { return number_of_valid_connections; }

        /**
         * @fn
         * @brief �e���v���[�g�����Ŏw�肳�ꂽDB�t�@�C�������݈ς��邩����
        */
        static bool exists_db() { return std::ifstream(std::string(DbName)).is_open(); }

        /**
         * @fn
         * @brief �f�[�^�x�[�X���쐬����
         * @return �쐬�ɐ��������ꍇ�^
        */
        static bool create_db()
        {
            if (!exists_db()) sqlite{};
            return exists_db();
        }

        /**
         * @fn
         * @brief �f�[�^�x�[�X���폜����
        */
        static void drop_db()
        {
            if (count_valid_connections() > 0)
                throw std::runtime_error("Unable to drop database as valid connections exist.");
            std::filesystem::remove(std::string(DbName));
        }

        /**
         * @fn
         * @brief �w�肵��ID�^�̃e�[�u�������݂��Ă��邩����
         * @tparam �e�[�u�����ʎq�̗񋓌^
        */
        template <mpl::Enumeration ETableType>
        bool exists_table() const
        {
            using table_definition_type = get_table_def_t<ETableType, TableDefinitionList>;
            static_assert(!std::is_same_v<table_definition_type, mpl::ignore_type>, "Not found database table definition.");
            return this->exists_table<table_definition_type>();
        }

        /**
         * @fn
         * @brief �w�肵����`�̃e�[�u�������݂��Ă��邩����
         * @tparam �e�[�u����`�^
        */
        template <TableDefinable TableDefinition>
        bool exists_table() const
        {
            return this->con.tableExists(TableDefinition::name);
        }

        /**
         * @fn
         * @brief count�̃N�G���𔭍s
        */
        int count(const std::string& query)
        {
            return this->con.count(query);
        }

    private:
        connection::sqlite<DbName> con;
        inline static int number_of_valid_connections = 0;

        /**
         * @fn
         * @brief �S�e�[�u���ɑ΂��Ĉꊇ�Ŏ��s�������N�G����z��Ƃ��ēW�J����
         * @tparam TableDefinitionList �e�[�u����`���X�g
        */
        template <TableListDefinable TableDefinitionList> struct expansion_querys;
        template <template <class...> class List, TableDefinable... TableDefinitions>
        struct expansion_querys<List<TableDefinitions...>>
        {
            static constexpr auto create_querys = std::array{ query::sqlite::make_create_table_string_v<TableDefinitions>.view()... };
        };
        static constexpr auto create_querys_v = expansion_querys<TableDefinitionList>::create_querys;
    };

}

#endif // TUUTILCPP_INCLUDE_GUARD_DB_SQLITE_HPP
