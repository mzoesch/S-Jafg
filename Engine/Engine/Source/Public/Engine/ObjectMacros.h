// Copyright mzoesch. All rights reserved.

#pragma once

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_BODY_IMPL(                                                      \
        MyClassName,                                                                                            \
        MyClassSpacedName,                                                                                      \
        SuperClassName,                                                                                         \
        SuperClassSpacedName                                                                                    \
    )                                                                                                           \
                                                                                                                \
private:                                                                                                        \
                                                                                                                \
    typedef SuperClassSpacedName Super;                                                                         \
    typedef MyClassSpacedName    Derived;                                                                       \
    friend class  ::Jafg::Private::LObjectRegistry;                                                             \
    friend struct ::Jafg::Private::LRegistrationCallbackHelper;                                                 \
    friend struct ::Jafg::Private::LObjectMiscellaneousAccessor;                                                \
    friend PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper);                            \
    inline static auto GetMutableClassReferrer() -> MyClassSpacedName*                                          \
    {                                                                                                           \
        check( MyClassSpacedName :: StaticClassReferrer)                                                        \
        return MyClassSpacedName :: StaticClassReferrer->GetMutableDefaultPackageReferrer<MyClassSpacedName>(); \
    }                                                                                                           \
    inline static auto GetImmutableClassReferrer() -> const MyClassSpacedName*                                  \
    {                                                                                                           \
        check( MyClassSpacedName :: StaticClassReferrer)                                                        \
        return MyClassSpacedName :: StaticClassReferrer->GetDefaultPackageReferrer<MyClassSpacedName>();        \
    }                                                                                                           \
    inline static ::Jafg::LObjectClass* StaticClassReferrer = nullptr;                                          \
                                                                                                                \
public:                                                                                                         \
                                                                                                                \
    static const MyClassSpacedName* ContentDefault()                                                            \
    {                                                                                                           \
        return MyClassSpacedName :: GetImmutableClassReferrer();                                                \
    }                                                                                                           \
    static const ::Jafg::LObjectClass* StaticClass()                                                            \
    {                                                                                                           \
        check( MyClassSpacedName :: StaticClassReferrer)                                                        \
        return MyClassSpacedName :: StaticClassReferrer;                                                        \
    }                                                                                                           \
    MyClassSpacedName() = delete;                                                                               \
    PROHIBIT_REALLOC_OF_ANY_FORM_NAMESPACED( MyClassSpacedName, MyClassName )                                   \
    /* void operator delete(void* Ptr) = delete; */                                                             \
                                                                                                                \
private: /* Restore default visibility. */

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(                  \
        MyClassName,                                                                                                \
        MyClassSpacedName,                                                                                          \
        SuperClassName,                                                                                             \
        SuperClassSpacedName,                                                                                       \
        ...                                                                                                         \
    )                                                                                                               \
                                                                                                                    \
    struct PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper) final                           \
    {                                                                                                               \
        PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper)();                                 \
        inline static ::EClassFlags::Type ClassFlags = EClassFlags::CombineFlags(EClassFlags::None, ##__VA_ARGS__); \
    };                                                                                                              \

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION(              \
        MyClassName,                                                                                           \
        MyClassSpacedName,                                                                                     \
        MyClassNamespaces,                                                                                     \
        SuperClassName,                                                                                        \
        SuperClassSpacedName                                                                                   \
    )                                                                                                          \
                                                                                                               \
    MyClassNamespaces                                                                                          \
        PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper)                               \
        ::PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper)()                           \
    {                                                                                                          \
        ::Jafg::Private::RegisterNewObjectType< MyClassSpacedName >(                                           \
            #MyClassSpacedName,                                                                                \
            [] (void) -> ::Jafg::Private::JObjectBase*                                                         \
            {                                                                                                  \
                return new MyClassSpacedName(::Jafg::GetDefaultObjectInitializer());                           \
            },                                                                                                 \
            [] (::Jafg::LObjectClass* StaticClass) -> void                                                     \
            {                                                                                                  \
                ::Jafg::Private::LRegistrationCallbackHelper::DoRegisterContentsForClass< MyClassSpacedName >( \
                    StaticClass,                                                                               \
                    PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper)::ClassFlags,      \
                    #SuperClassSpacedName                                                                      \
                );                                                                                             \
                return;                                                                                        \
            }                                                                                                  \
        );                                                                                                     \
                                                                                                               \
        return;                                                                                                \
    }                                                                                                          \
    namespace                                                                                                  \
    {                                                                                                          \
                                                                                                               \
    MyClassNamespaces PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper)                 \
    PRIVATE_JAFG_CORE_JOIN_INNER_FOUR(L_, MyClassName, _ConstructionHelper, Instance);                         \
                                                                                                               \
    } /* ~Namespace <Anonymous> */

#define DEFAULT_OBJECT_CONSTRUCTOR(MyClassName)                                                              \
    explicit MyClassName(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) { return; }
