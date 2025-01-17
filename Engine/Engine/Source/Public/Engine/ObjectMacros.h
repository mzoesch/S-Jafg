// Copyright mzoesch. All rights reserved.

#pragma once

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(                  \
        MyClassName,                                                                                                \
        SuperClassName,                                                                                             \
        Line,                                                                                                       \
        ...                                                                                                         \
    )                                                                                                               \
                                                                                                                    \
    struct PRIVATE_JAFG_CORE_JOIN_OUTER_SIX(L_, MyClassName, _, Line, _, ConstructionHelper) final  \
    {                                                                                                               \
        PRIVATE_JAFG_CORE_JOIN_OUTER_SIX(L_, MyClassName, _, Line, _, ConstructionHelper)();                      \
        inline static ::EClassFlags::Type ClassFlags = EClassFlags::CombineFlags(__VA_ARGS__ EClassFlags::None);    \
    };                                                                                                              \
    typedef PRIVATE_JAFG_CORE_JOIN_OUTER_SIX(L_, MyClassName, _, Line, _, ConstructionHelper) \
        PRIVATE_JAFG_CORE_JOIN_OUTER_FOUR(L_, MyClassName, _, ConstructionHelper);

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_BODY_IMPL(                                                      \
        MyClassName,                                                                                            \
        SuperClassName,                                                                                          \
        ConstructionHelper \
    )                                                                                                           \
                                                                                                                \
private:                                                                                                        \
                                                                                                                \
    typedef SuperClassName Super;                                                                         \
    typedef MyClassName    Derived;                                                                       \
    friend class  ::Jafg::Private::LObjectRegistry;                                                             \
    friend struct ::Jafg::Private::LRegistrationCallbackHelper;                                                 \
    friend struct ::Jafg::Private::LObjectMiscellaneousAccessor;                                                \
    friend ConstructionHelper;                            \
    inline static auto GetMutableClassReferrer() -> MyClassName*                                          \
    {                                                                                                           \
        check( MyClassName :: StaticClassReferrer)                                                        \
        return MyClassName :: StaticClassReferrer->GetMutableDefaultPackageReferrer<MyClassName>(); \
    }                                                                                                           \
    inline static auto GetImmutableClassReferrer() -> const MyClassName*                                  \
    {                                                                                                           \
        check( MyClassName :: StaticClassReferrer)                                                        \
        return MyClassName :: StaticClassReferrer->GetDefaultPackageReferrer<MyClassName>();        \
    }                                                                                                           \
    inline static ::Jafg::LObjectClass* StaticClassReferrer = nullptr;                                          \
                                                                                                                \
public:                                                                                                         \
                                                                                                                \
    static const MyClassName* ContentDefault()                                                            \
    {                                                                                                           \
        return MyClassName :: GetImmutableClassReferrer();                                                \
    }                                                                                                           \
    static const ::Jafg::LObjectClass* StaticClass()                                                            \
    {                                                                                                           \
        check( MyClassName :: StaticClassReferrer)                                                        \
        return MyClassName :: StaticClassReferrer;                                                        \
    }                                                                                                           \
    MyClassName() = delete;                                                                                     \
    PROHIBIT_REALLOC_OF_ANY_FORM( MyClassName )                                   \
    /* void operator delete(void* Ptr) = delete; */                                                             \
                                                                                                                \
private: /* Restore default visibility. */

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

#ifdef DECLARE_JAFG_CLASS
    #undef DECLARE_JAFG_CLASS
#endif /* DECLARE_JAFG_CLASS */
#define DECLARE_JAFG_CLASS(...)                                        \
    PRIVATE_JAFG_CORE_JOIN_OUTER_FIVE(                                 \
        JAFG_PRIVATE_FILE_ID,                                          \
        _,                                                             \
        __LINE__,                                                      \
        _,                                                             \
        MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION \
    )(__VA_ARGS__)

#ifdef GENERATED_CLASS_BODY
    #undef GENERATED_CLASS_BODY
#endif /* GENERATED_CLASS_BODY */
#define GENERATED_CLASS_BODY(...)      \
    PRIVATE_JAFG_CORE_JOIN_OUTER_FIVE( \
        JAFG_PRIVATE_FILE_ID,          \
        _,                             \
        __LINE__,                      \
        _,                             \
        MY_GENERATED_CLASS_BODY        \
    )( __VA_ARGS__ )

#define DEFAULT_OBJECT_CONSTRUCTOR(MyClassName)                                                              \
    explicit MyClassName(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) { return; }
