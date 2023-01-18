///----------------------------------
/// @file db/query/sqlite.hpp
/// @brief sqlite�N�G������
/// @details �{�t�@�C�����g�p����ꍇ�Asqlite3.h���C���N���[�h�\�ŁAsqlite3.lib�������N�ς݂łȂ��ꍇ�A����`
///----------------------------------
#ifndef TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP
#define TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP

#if __has_include(<sqlite3.h>)  // sqlite3�̃w�b�_���ǂݍ��߂Ȃ���΂����Ȃ�
#include <sqlite3.h>
#include <tuutilcpp/str.hpp>

namespace tuutil::db::connection
{
    /**
     * @class
     * @brief sqlite3�̃��b�p�[
     * @tparam DbFileName
    */
    template <str::cstr DbFileName>
    class sqlite
    {
    public:
        using sqlite_callback_t = int (*)(void*, int, char**, char**);

        sqlite() { this->open(); }

        ~sqlite()
        {
            // sql�X�e�[�g�����g�̏I�������������K�v����
            sqlite3_close(this->con);
        }

        /**
         * @fn
         * @brief �e���v���[�g�����Ŏw�肳�ꂽDB�t�@�C�������݈ς��邩����
        */
        bool exists() const { return std::ifstream(DbFileName).is_open(); }

        /**
         * @fn
         * @brief ������Ƃ��č\�z���ꂽsql�N�G�������s
         * @tparam Func nullptr or �����_�� or �֐��|�C���^
         * @param sql �N�G��������
         * @param data [out] �f�[�^�o�̓o�b�t�@
         * @param errmsg [out] �G���[�擾�o�b�t�@
        */
        template <auto Func>
        void exec(const std::string& sql, void* data, char** errmsg = nullptr) const
        {
            this->error(
                sqlite3_exec(this->con, sql.c_str(), nullptr, data, nullptr)
            );
        }
        template <auto Func>
        requires (std::invocable<decltype(Func) ,void*, int, char**, char**>)
        void exec(const std::string& sql, void* data, char** errmsg = nullptr) const
        {
            this->error(
                sqlite3_exec(this->con, sql.c_str(), &exec_callback_template<Func>, data, nullptr)
            );
        }

    private:
        sqlite3* con = nullptr;

        /**
         * @fn
         * @brief DB�Ƃ̐ڑ����J��
        */
        void open()
        {
            this->error(
                sqlite3_open_v2(std::string(DbFileName).c_str(), &this->con, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr)
            );
        }

        /**
         * @fn
         * @brief ��O���o
        */
        void error(int code) const
        {
            if (code != SQLITE_OK)
                throw std::runtime_error(
                    std::string("[sqlite3 error]code ")
                    + std::to_string(code) + ", "
                    + '"' + sqlite3_errmsg(this->con) + '"'
                );
        }

        /**
         * @fn
         * @brief �R�[���o�b�N�֐��̌^�����z���e���v���[�g
         * @tparam Func �����_���A�֐��|�C���^��
         * @param data �f�[�^�o�̓o�b�t�@
         * @param argc select�����ꍇ��
         * @param argv select�����ꍇ��s�̌��ʂ̒l�z��
         * @param asColName select�����ꍇ�J�������̔z��
         * 
        */
        template <std::invocable<void*, int, char**, char**> auto Func>
        static int exec_callback_template(void* data, int argc, char **argv, char **azColName)
        {
            Func(data, argc, argv, azColName);
            return SQLITE_OK;
        }
    };
}

#else
# warning <sqlite3.h> is not found
#endif  // __has_include(<sqlite3.h>)

#endif // TUUTILCPP_INCLUDE_GUARD_DB_CONNECTION_SQLITE_HPP
