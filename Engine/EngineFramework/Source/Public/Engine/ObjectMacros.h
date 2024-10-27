// Copyright mzoesch. All rights reserved.

#pragma once

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_BODY_IMPL(\
        MyClassName,\
        MyClassSpacedName,\
        SuperClassName,\
        SuperClassSpacedName\
    )               \
                    \
private:\
    typedef SuperClassSpacedName Super; \
    typedef MyClassSpacedName Derived; \
    friend struct ::Jafg::Private::LRegistrationCallbackHelper; \
    friend struct ::Jafg::LTemporalStructForCreatingObjects; \
    friend PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper); \
    friend class ::Jafg::Private::LObjectRegistry; \
    inline static LSimpleString ClassName = ""; \
    inline static MyClassSpacedName * StaticClassReferrer = nullptr; \
    static auto GetImmutableClassReferrer() -> const MyClassSpacedName * { return StaticClassReferrer; } \
    static auto GetMutableClassReferrer() -> MyClassSpacedName * { return StaticClassReferrer; } \
public:\
    static LSimpleString& StaticClassName() { return PRIVATE_JAFG_CORE_JOIN_INNER_TWO(MyClassSpacedName, ::ClassName); } \
    static const MyClassSpacedName * StaticClass() { return GetImmutableClassReferrer(); } \
    MyClassSpacedName() = delete; \
    PROHIBIT_REALLOC_OF_ANY_FROM_NAMESPACED( MyClassSpacedName, MyClassName ) \
private: /* Restore default visibility. */

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        MyClassName, \
        MyClassSpacedName, \
        SuperClassName, \
        SuperClassSpacedName \
    )               \
                    \
    struct PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper) final {\
        PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper)();\
    }; \

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
        MyClassName, \
        MyClassSpacedName, \
        MyClassNamespaces, \
        SuperClassName, \
        SuperClassSpacedName \
    )               \
                    \
    MyClassNamespaces PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper)::PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper)() \
    {\
        ::Jafg::Private::RegisterNewObjectType< MyClassSpacedName >(\
            #MyClassSpacedName, \
            [](void) -> ::Jafg::Private::JObjectBase* { return new MyClassSpacedName(::Jafg::GetDefaultObjectInitializer()); }, \
            [] (::Jafg::Private::JObjectBase * DefaultPackageReferrer) -> void {\
            ::Jafg::Private::LRegistrationCallbackHelper::DoRegisterContentsForClass< MyClassSpacedName >( \
                dynamic_cast< MyClassSpacedName *>(DefaultPackageReferrer));\
        return; });\
        return;\
    } \
    namespace { \
    MyClassNamespaces PRIVATE_JAFG_CORE_JOIN_INNER_THREE(L_, MyClassName, _ConstructionHelper) \
    PRIVATE_JAFG_CORE_JOIN_INNER_FOUR(L_, MyClassName, _ConstructionHelper, Instance); \
    }\

#define DEFAULT_OBJECT_CONSTRUCTOR(MyClassName) \
    explicit MyClassName(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) { return; }
