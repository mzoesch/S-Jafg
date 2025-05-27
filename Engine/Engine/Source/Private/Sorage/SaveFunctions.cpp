// Copyright mzoesch. All rights reserved.

#include "Storage/SaveFunctions.h"
#include "Containers/MyString.h"
#include "Vendor/sqlite3.h"

#define EMIT_ERROR(InDescription) \
    if (OutError) \
    { \
        *OutError = Jafg::LString::SprintF("SQL {}: [{}].", InDescription, sqlite3_errmsg(Con)); \
    } \
    else \
    { \
        LOG_ERROR(LogStorage, "SQL {}: [{}].", InDescription, sqlite3_errmsg(Con)); \
    }

namespace
{

struct LSql3Con final
{
    LSql3Con()
        : Db(nullptr)
    {
    }

    LSql3Con(const Jafg::LPath& InPath, Jafg::LString* OutError /* = nullptr */)
    {
        const int Rc = sqlite3_open(InPath.ToPtr(), &this->Db);

        if (Rc)
        {
            if (OutError)
            {
                *OutError = Jafg::LString::SprintF("Failed to open database. Reason: [{}].", sqlite3_errmsg(Db));
            }
            else
            {
                LOG_ERROR(LogStorage, "Failed to open database. Reason: [{}].", sqlite3_errmsg(Db));
            }
        }

        return;
    }

    ~LSql3Con() noexcept
    {
        if (this->IsValid())
        {
            this->Close();
        }

        return;
    }

    FORCEINLINE bool IsValid() const noexcept
    {
        return this->Db != nullptr;
    }

    FORCEINLINE void Close() noexcept
    {
        check( this->IsValid() )
        sqlite3_close(this->Db);
        this->Db = nullptr;

        return;
    }

    FORCEINLINE operator        bool() const noexcept { return  this->IsValid(); }
    FORCEINLINE sqlite3*  operator *()       noexcept { return  this->Db; }
    FORCEINLINE sqlite3*  operator->()       noexcept { return  this->Db; }
    FORCEINLINE sqlite3** operator &()       noexcept { return &this->Db; }
    FORCEINLINE operator    sqlite3*()       noexcept { return  this->Db; }

    FORCEINLINE sqlite3* GetDb() noexcept { return this->Db; }

private:

    sqlite3* Db;
};

struct LSqlSmt final
{
    LSqlSmt() noexcept
        : Stmt(nullptr)
    {
    }

    ~LSqlSmt() noexcept
    {
        if (this->IsValid())
        {
            this->Close();
        }

        return;
    }

    FORCEINLINE bool IsValid() const noexcept
    {
        return this->Stmt != nullptr;
    }

    FORCEINLINE void Close() noexcept
    {
        check( this->IsValid() )
        sqlite3_finalize(this->Stmt);
        this->Stmt = nullptr;

        return;
    }

    FORCEINLINE operator             bool() const noexcept { return  this->IsValid(); }
    FORCEINLINE sqlite3_stmt*  operator *()       noexcept { return  this->Stmt; }
    FORCEINLINE sqlite3_stmt*  operator->()       noexcept { return  this->Stmt; }
    FORCEINLINE sqlite3_stmt** operator &()       noexcept { return &this->Stmt; }
    FORCEINLINE operator    sqlite3_stmt*()       noexcept { return  this->Stmt; }

    FORCEINLINE sqlite3_stmt* GetStmt() noexcept { return this->Stmt; }

private:

    sqlite3_stmt* Stmt;
};

} /* ~Anonymous Namespace */

Jafg::TOptional<Jafg::LString> Jafg::Saves::GetDisplayName(const LPath& InPath, LString* OutError /* = nullptr */)
{
    LSql3Con Con(InPath, OutError);
    if (Con.IsValid() == false)
    {
        return { };
    }

    LSqlSmt Stmt;

    const LString Sql
    {
        "SELECT DisplayName FROM Meta "
        "WHERE EXISTS (SELECT 1 FROM sqlite_master WHERE type='table' AND name=?) LIMIT 1"
    };

    if (sqlite3_prepare_v2(Con, Sql.ToPtr(), -1, &Stmt, nullptr) != SQLITE_OK)
    {
        EMIT_ERROR("Prepare")
        return { };
    }

    if (sqlite3_bind_text(Stmt, 1, "Meta", -1, SQLITE_STATIC) != SQLITE_OK)
    {
        EMIT_ERROR("Bind")
        return { };
    }

    if (const int Rc = sqlite3_step(Stmt); Rc == SQLITE_ROW)
    {
        if (const unsigned char* Val = sqlite3_column_text(Stmt, 0); Val)
        {
            return { reinterpret_cast<const char*>(Val) };
        }
        EMIT_ERROR("Invalid DisplayName")
    }
    else if (Rc == SQLITE_DONE)
    {
        EMIT_ERROR("Rows Invalid")
    }
    else
    {
        EMIT_ERROR("Step")
    }

    return { };
}

#undef EMIT_ERROR
