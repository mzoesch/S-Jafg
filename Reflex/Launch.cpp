// Copyright mzoesch. All rights reserved.

#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/AST/AST.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include "Common.h"

using namespace clang;

namespace
{

std::string GetFullyQualifiedName(CXXRecordDecl const* Decl)
{
    if (Decl == nullptr)
    {
        return {};
    }

    std::string FullName;
    if (DeclContext const* Dc = Decl->getDeclContext())
    {
        if (NamespaceDecl const* Nsd = dyn_cast<NamespaceDecl>(Dc))
        {
            if (!Nsd->isAnonymousNamespace())
            {
                FullName = Nsd->getNameAsString() + "::";
            }
        }
        else if (RecordDecl const* RD = dyn_cast<RecordDecl>(Dc))
        {
            FullName = RD->getNameAsString() + "::";
        }
    }

    FullName += Decl->getNameAsString();
    return FullName;
}

//# This will also check rules, such as no double inheritance...
bool DerivesFromJObjectBase(CXXRecordDecl const* Derived)
{
    if (Derived == nullptr)
    {
        return false;
    }

    int Cursor { -1 };
    bool bDerives { false };
    for (auto const& Base : Derived->bases())
    {
        ++Cursor;

        CXXRecordDecl const* BaseDecl { Base.getType()->getAsCXXRecordDecl() };
        if (BaseDecl == nullptr)
        {
            continue;
        }

        if (GetFullyQualifiedName(BaseDecl) == "Jafg::JObjectBase")
        {
            if (bDerives)
            {
                const std::string Err{ "Expected only one direct base class of [" + GetFullyQualifiedName(Derived) + "] to be (indirectly) derived from Jafg::JObjectBase." };
                llvm::report_fatal_error(Err.c_str());
            }

            if (Cursor != 0)
            {
                const std::string Err{ "Expected first base class of [" + GetFullyQualifiedName(Derived) + "] to be Jafg::JObjectBase." };
                llvm::report_fatal_error(Err.c_str());
            }

            bDerives = true;
            continue;
        }

        if (DerivesFromJObjectBase(BaseDecl))
        {
            if (bDerives)
            {
                const std::string Err{ "Expected only one direct base class of [" + GetFullyQualifiedName(Derived) + "] to be derived from Jafg::JObjectBase." };
                llvm::report_fatal_error(Err.c_str());
            }

            if (Cursor != 0)
            {
                const std::string Err{ "Expected first base class of [" + GetFullyQualifiedName(Derived) + "] to be (indirectly) derived from Jafg::JObjectBase." };
                llvm::report_fatal_error(Err.c_str());
            }

            bDerives = true;
            continue;
        }

        continue;
    }

    return bDerives;
}

bool DerivesFromOrIsJObjectBase(CXXRecordDecl const* Derived)
{
    if (Derived == nullptr)
    {
        return false;
    }

    if (GetFullyQualifiedName(Derived) == "Jafg::JObjectBase")
    {
        //# We are not that stupid to fuck up the root. So just leave it here.
        return true;
    }

    return DerivesFromJObjectBase(Derived);
}

void ReflectThis(CXXRecordDecl const* Decl)
{
    if (Decl == nullptr)
    {
        return;
    }

    std::string const Name { GetFullyQualifiedName(Decl) };
    llvm::errs() << "Reflecting class: " << Name << "\n";

    return;
}

class MyVisitor : public RecursiveASTVisitor<MyVisitor>
{
public:

    explicit MyVisitor(ASTContext& Context, std::string InFile) noexcept
        : Context(Context), InFile(std::move(InFile)) {}

    bool VisitCXXRecordDecl(CXXRecordDecl *Decl)
    {
        if (Decl == nullptr || Decl->isImplicit() || Decl->isAnonymousStructOrUnion() || !Decl->isThisDeclarationADefinition())
        {
            return true;
        }

        const std::string DisplayName { Decl->getNameAsString() };

        //
        // Naming is already enforced by the Motor itself.
        // Therefore, rechecking if classes inherit from Jafg::JObjectBase without the correct prefix is not needed.
        // We can safely skip those classes here.
        //
        if
        (
            const std::vector AllowedPrefixes { 'J', 'W', 'A', };
               DisplayName.empty()
            || std::find_if(AllowedPrefixes.begin(), AllowedPrefixes.end(), [&](char const Prefix){ return DisplayName[0] == Prefix; }) == AllowedPrefixes.end()
        )
        {
            return true;
        }

        const FullSourceLoc FullLocation = Context.getFullLoc(Decl->getLocation());
        if (FullLocation.isInvalid() || FullLocation.getFileEntry() == nullptr)
        {
            return true;
        }

        const StringRef RealPath { FullLocation.getFileEntry()->tryGetRealPathName() };
        if (RealPath.empty())
        {
            return true;
        }

        size_t BeginSub { RealPath.find_last_of('/') };
        if (BeginSub == StringRef::npos)
        {
            BeginSub = 0;
        }
        else
        {
            BeginSub += 1;
        }

        StringRef FileName;

        if (const size_t EndSub { RealPath.find_last_of('.') }; EndSub <= BeginSub)
        {
            FileName = RealPath.substr(BeginSub);
        }
        else
        {
            FileName = RealPath.substr(BeginSub, EndSub - BeginSub);
        }

        if
        (
            const std::string ExpectedGeneratedFileName { FileName.str() + ".generated.cpp" };
            Reflex::StrEndsWith(this->InFile, ExpectedGeneratedFileName) == false
        )
        {
            return true;
        }

        if (GetFullyQualifiedName(Decl) == "Jafg::JObjectBase")
        {
            //# No need to check for double shit; we are not that stupid.
            ReflectThis(Decl);
            return true;
        }

        if (DerivesFromJObjectBase(Decl) == false)
        {
            return true;
        }

        ReflectThis(Decl);

        return true;
    }

private:

    ASTContext& Context;
    std::string InFile;
};

class MyPluginASTConsumer final : public ASTConsumer
{
public:

    explicit MyPluginASTConsumer(ASTContext& Context, std::string InFile) noexcept
        : Visitor(Context, std::move(InFile)) {}

    void HandleTranslationUnit(ASTContext& Context) override
    {
        this->Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:

    MyVisitor Visitor;
};

class MyFrontendAction final : public PluginASTAction
{
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& CI, StringRef InFile) override
    {
        if (InFile.ends_with(".generated.cpp"))
        {
            return std::make_unique<MyPluginASTConsumer>(CI.getASTContext(), InFile.str());
        }

        return std::make_unique<ASTConsumer>();
    }

public:

    bool ParseArgs(CompilerInstance const& Ci, std::vector<std::string> const& Args) override
    {
        return true;
    }
};

} /* ~Namespace <Anonymous> */

static FrontendPluginRegistry::Add<MyFrontendAction> X("jafg_reflex", "Helper plugin for jafg reflex until C++26 supports reflection natively.");
