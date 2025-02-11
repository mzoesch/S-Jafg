// Copyright mzoesch. All rights reserved.

#pragma once

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        MyClassName,                                                                               \
        MyClassSpaces,                                                                             \
        SuperClassName,                                                                            \
        Line,                                                                                      \
        ...                                                                                        \
    )                                                                                              \
    struct PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                                      \
        L,                                                                                         \
        _,                                                                                         \
        JAFG_PRIVATE_FILE_ID,                                                                      \
        _,                                                                                         \
        MyClassName,                                                                               \
        _,                                                                                         \
        Line,                                                                                      \
        _,                                                                                         \
        ConstructionHelper                                                                         \
    ) final                                                                                        \
    {                                                                                              \
        PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                                         \
        L,                                                                                         \
        _,                                                                                         \
        JAFG_PRIVATE_FILE_ID,                                                                      \
        _,                                                                                         \
        MyClassName,                                                                               \
        _,                                                                                         \
        Line,                                                                                      \
        _,                                                                                         \
        ConstructionHelper                                                                         \
        )();                                                                                       \
        inline static ::EClassFlags::Type ClassFlags = EClassFlags::CombineFlags(__VA_ARGS__);     \
    };

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        MyClassName,                                                                                \
        MyClassSpaces,                                                                              \
        SuperClassName,                                                                             \
        Line,                                                                                       \
        ...                                                                                         \
    )                                                                                               \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION(      \
        MyClassName,                                                                                \
        MyClassSpaces,                                                                              \
        SuperClassName,                                                                             \
        Line,                                                                                       \
        __VA_ARGS__                                                                                 \
    )

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
    MyClassName, \
    MyClassSpaces,\
    SuperClassName, \
    Line, \
    FactoryType, \
    ... \
    ) \
    struct PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                                      \
        L,                                                                                         \
        _,                                                                                         \
        JAFG_PRIVATE_FILE_ID,                                                                      \
        _,                                                                                         \
        MyClassName,                                                                               \
        _,                                                                                         \
        Line,                                                                                      \
        _,                                                                                         \
        ConstructionHelper                                                                         \
    ) final                                                                                        \
    {                                                                                              \
        PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                                         \
        L,                                                                                         \
        _,                                                                                         \
        JAFG_PRIVATE_FILE_ID,                                                                      \
        _,                                                                                         \
        MyClassName,                                                                               \
        _,                                                                                         \
        Line,                                                                                      \
        _,                                                                                         \
        ConstructionHelper                                                                         \
        )();                                                                                       \
        inline static ::EClassFlags::Type ClassFlags = EClassFlags::CombineFlags(__VA_ARGS__);     \
        template <typename TNode> \
        using TWidgetFactoryTy = FactoryType<TNode>; \
    };

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
        MyClassName,                                                                              \
        MyClassSpaces,                                                                            \
        LineOfDeclaration,                                                                        \
        SuperClassName                                                                            \
    )                                                                                             \
                                                                                                  \
    PRIVATE_JAFG_CORE_JOIN_INNER_THREE(                                                           \
        PRIVATE_JAFG_CORE_JOIN_INNER_TWO(                                                         \
            MyClassSpaces,                                                                        \
            PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                                    \
                L,                                                                                \
                _,                                                                                \
                JAFG_PRIVATE_FILE_ID,                                                             \
                _,                                                                                \
                MyClassName,                                                                      \
                _,                                                                                \
                LineOfDeclaration,                                                                \
                _,                                                                                \
                ConstructionHelper                                                                \
            )                                                                                     \
        ),                                                                                        \
        ::,                                                                                       \
        PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                                        \
            L,                                                                                    \
            _,                                                                                    \
            JAFG_PRIVATE_FILE_ID,                                                                 \
            _,                                                                                    \
            MyClassName,                                                                          \
            _,                                                                                    \
            LineOfDeclaration,                                                                    \
            _,                                                                                    \
            ConstructionHelper                                                                    \
        )                                                                                         \
    )()                                                                                           \
    {                                                                                             \
        ::Jafg::Private::RegisterNewObjectType<                                                   \
                PRIVATE_JAFG_CORE_JOIN_INNER_TWO(MyClassSpaces, MyClassName)                      \
        >(                                                                                        \
            PRIVATE_JAFG_CORE_CAT_OUTER_TWO(#MyClassSpaces, #MyClassName),                        \
            [] (void) -> ::Jafg::JObjectBase*                                            \
            {                                                                                     \
                return                                                                            \
                new PRIVATE_JAFG_CORE_JOIN_INNER_TWO(MyClassSpaces, MyClassName)                  \
                    (::Jafg::GetDefaultObjectInitializer());                                      \
            },                                                                                    \
            [] (::Jafg::LObjectClass* StaticClass) -> void                                        \
            {                                                                                     \
                ::Jafg::Private::LRegistrationCallbackHelper::DoRegisterContentsForClass<         \
                    PRIVATE_JAFG_CORE_JOIN_INNER_TWO(MyClassSpaces, MyClassName)                  \
                >(                                                                                \
                    StaticClass,                                                                  \
                    PRIVATE_JAFG_CORE_JOIN_INNER_TWO(                                             \
                        MyClassSpaces,                                                            \
                        PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                        \
                            L,                                                                    \
                            _,                                                                    \
                            JAFG_PRIVATE_FILE_ID,                                                 \
                            _,                                                                    \
                            MyClassName,                                                          \
                            _,                                                                    \
                            LineOfDeclaration,                                                    \
                            _,                                                                    \
                            ConstructionHelper                                                    \
                        )                                                                         \
                    )::ClassFlags,                                                                \
                    #SuperClassName                                                               \
                );                                                                                \
                return;                                                                           \
            }                                                                                     \
        );                                                                                        \
        return;                                                                                   \
    }                                                                                             \
                                                                                                  \
    namespace                                                                                     \
    {                                                                                             \
                                                                                                  \
    PRIVATE_JAFG_CORE_JOIN_INNER_TWO(                                                             \
        MyClassSpaces,                                                                            \
        PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                                        \
            L,                                                                                    \
            _,                                                                                    \
            JAFG_PRIVATE_FILE_ID,                                                                 \
            _,                                                                                    \
            MyClassName,                                                                          \
            _,                                                                                    \
            LineOfDeclaration,                                                                    \
            _,                                                                                    \
            ConstructionHelper                                                                    \
        )                                                                                         \
    )                                                                                             \
        PRIVATE_JAFG_CORE_JOIN_OUTER_TWO(                                                         \
            PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                                    \
                L,                                                                                \
                _,                                                                                \
                JAFG_PRIVATE_FILE_ID,                                                             \
                _,                                                                                \
                MyClassName,                                                                      \
                _,                                                                                \
                LineOfDeclaration,                                                                \
                _,                                                                                \
                ConstructionHelper                                                                \
            ),                                                                                    \
            Instance                                                                              \
        );                                                                                        \
                                                                                                  \
    } /* ~Namespace <Anonymous> */


#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(                                    \
        MyClassName,                                                                                \
        MyClassSpaces,                                                                              \
        SuperClassName,                                                                             \
        ConstructionHelperLine                                                                      \
    )                                                                                               \
                                                                                                    \
private:                                                                                            \
                                                                                                    \
    typedef SuperClassName Super;                                                                   \
    typedef PRIVATE_JAFG_CORE_JOIN_INNER_TWO(MyClassSpaces, MyClassName) Derived;                   \
    friend class  ::Jafg::Private::LObjectRegistry;                                                 \
    friend struct ::Jafg::Private::LRegistrationCallbackHelper;                                     \
    friend struct ::Jafg::Private::LObjectMiscellaneousAccessor;                                    \
    friend PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(                                                       \
      L,                                                                                            \
      _,                                                                                            \
      JAFG_PRIVATE_FILE_ID,                                                                         \
      _,                                                                                            \
      MyClassName,                                                                                  \
      _,                                                                                            \
      ConstructionHelperLine,                                                                       \
      _,                                                                                            \
      ConstructionHelper                                                                            \
      );                                                                                            \
    inline static ::Jafg::LObjectClass* StaticClassReferrer = nullptr;                              \
    inline static auto GetMutableClassReferrer() -> MyClassName*                                    \
    {                                                                                               \
        check( MyClassName :: StaticClassReferrer)                                                  \
        return MyClassName :: StaticClassReferrer->GetMutableDefaultPackageReferrer<MyClassName>(); \
    }                                                                                               \
    inline static auto GetImmutableClassReferrer() -> const MyClassName*                            \
    {                                                                                               \
        check( MyClassName :: StaticClassReferrer)                                                  \
        return MyClassName :: StaticClassReferrer->GetDefaultPackageReferrer<MyClassName>();        \
    }                                                                                               \
                                                                                                    \
public:                                                                                             \
                                                                                                    \
    static const MyClassName* ContentDefault()                                                      \
    {                                                                                               \
        return MyClassName :: GetImmutableClassReferrer();                                          \
    }                                                                                               \
    static const ::Jafg::LObjectClass* StaticClass()                                                \
    {                                                                                               \
        check( MyClassName :: StaticClassReferrer)                                                  \
        return MyClassName :: StaticClassReferrer;                                                  \
    }                                                                                               \
    MyClassName() = delete;                                                                         \
    PROHIBIT_REALLOC_OF_ANY_FORM( MyClassName )                                                     \
    /* void operator delete(void* Ptr) = delete; */                                                 \
                                                                                                    \
private: /* Restore default visibility. */

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_BODY_IMPL( \
    MyClassName,                                                  \
    MyClassSpaces,                                                \
    SuperClassName,                                               \
    ConstructionHelperLine                                        \
    )                                                             \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(      \
        MyClassName,                                              \
        MyClassSpaces,                                            \
        SuperClassName,                                           \
        ConstructionHelperLine                                    \
    )                                                             \
public:                                                           \
    template <typename TNode>                                     \
    using TWidgetFactoryTy = Super::TWidgetFactoryTy<TNode>;      \
    using TWidgetFactory = TWidgetFactoryTy<Derived>;             \
private: /* Restore default visibility. */

#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_BODY_IMPL( \
    MyClassName,                                                  \
    MyClassSpaces,                                                \
    SuperClassName,                                               \
    ConstructionHelperLine                                        \
    )                                                             \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(      \
        MyClassName,                                              \
        MyClassSpaces,                                            \
        SuperClassName,                                           \
        ConstructionHelperLine                                    \
    )                                                             \
public:                                                           \
    template <typename TNode>                                     \
    using TWidgetFactoryTy = PRIVATE_JAFG_CORE_JOIN_OUTER_NINE(   \
        L,                                                        \
        _,                                                        \
        JAFG_PRIVATE_FILE_ID,                                     \
        _,                                                        \
        MyClassName,                                              \
        _,                                                        \
        ConstructionHelperLine,                                   \
        _,                                                        \
        ConstructionHelper                                        \
    )::TWidgetFactoryTy<TNode>;                                         \
    using TWidgetFactory = TWidgetFactoryTy<Derived>;             \
private: /* Restore default visibility. */

//#
//# Required before the jafg-class declaration.
//# @see Engine/Object.h, for example.
//#
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
    )( __VA_ARGS__ )

//#
//# Required as the first statement of a jafg-class declaration.
//# @see Engine/Object.h, for example.
//#
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

//#
//# Required before the jafg-widget-class declaration
//# @see Widgets/WidgetNode.h
//#
#ifdef DECLARE_JAFG_WIDGET
    #undef DECLARE_JAFG_WIDGET
#endif /* DECLARE_JAFG_WIDGET */
#define DECLARE_JAFG_WIDGET(...) DECLARE_JAFG_CLASS(__VA_ARGS__)

//#
//# Required before the jafg-widget-class declaration
//# @see Widgets/WidgetNode.h
//#
#ifdef DECLARE_JAFG_WIDGET_WITH_FACTORY
    #undef DECLARE_JAFG_WIDGET_WITH_FACTORY
#endif /* DECLARE_JAFG_WIDGET_WITH_FACTORY */
#define DECLARE_JAFG_WIDGET_WITH_FACTORY(TFactoryTy, ...)              \
    PRIVATE_JAFG_CORE_JOIN_OUTER_FIVE(                                 \
        JAFG_PRIVATE_FILE_ID,                                          \
        _,                                                             \
        __LINE__,                                                      \
        _,                                                             \
        MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION \
        )(TFactoryTy, __VA_ARGS__ )

//#
//# Required as the first statement of a jafg-widget-class declaration.
//# @see Widgets/WidgetNode.h
//#
#ifdef GENERATED_WIDGET_BODY
    #undef GENERATED_WIDGET_BODY
#endif /* GENERATED_WIDGET_BODY */
#define GENERATED_WIDGET_BODY(...) GENERATED_CLASS_BODY(__VA_ARGS__)

//#
//# Default constructor for an JObject. Mandatory. It may be used to declare extra information for all objects of this
//# type. The new constructor is prohibited from changing the arguments of the constructor.
//#
#define DEFAULT_OBJECT_CONSTRUCTOR(MyClassName)                                                              \
    explicit MyClassName(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) { return; }
