// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"


/*-----------------------------------------------------------------------------
    Generated macro bodies.
-----------------------------------------------------------------------------*/

#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
        MyClassName,                                                                               \
        MyClassSpaces,                                                                             \
        SuperClassName,                                                                            \
        Line,                                                                                      \
        ...                                                                                        \
    )                                                                                              \
    struct JAFG_JOIN_OUTER_NINE(                                                                    \
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
        JAFG_JOIN_OUTER_NINE(                                                                       \
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
        inline static ::ECxxClassFlags::Type Flags { ::Jafg::CombineCxxClassFlags(__VA_ARGS__) };  \
    };

#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
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

#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION( \
    MyClassName,                                                                                                 \
    MyClassSpaces,                                                                                               \
    SuperClassName,                                                                                              \
    Line,                                                                                                        \
    FactoryType,                                                                                                 \
    ...                                                                                                          \
    )                                                                                                            \
    struct JAFG_JOIN_OUTER_NINE(                                                                                  \
        L,                                                                                                       \
        _,                                                                                                       \
        JAFG_PRIVATE_FILE_ID,                                                                                    \
        _,                                                                                                       \
        MyClassName,                                                                                             \
        _,                                                                                                       \
        Line,                                                                                                    \
        _,                                                                                                       \
        ConstructionHelper                                                                                       \
    ) final                                                                                                      \
    {                                                                                                            \
        JAFG_JOIN_OUTER_NINE(                                                                                     \
        L,                                                                                                       \
        _,                                                                                                       \
        JAFG_PRIVATE_FILE_ID,                                                                                    \
        _,                                                                                                       \
        MyClassName,                                                                                             \
        _,                                                                                                       \
        Line,                                                                                                    \
        _,                                                                                                       \
        ConstructionHelper                                                                                       \
        )();                                                                                                     \
        inline static ::ECxxClassFlags::Type Flags { ::Jafg::CombineCxxClassFlags(__VA_ARGS__) };                \
        typedef FactoryType LFactory;                                                             \
    };

#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION( \
        MyClassName,                                                                              \
        MyClassSpaces,                                                                            \
        LineOfDeclaration,                                                                        \
        SuperClassName,                                                                           \
        ...                                                                                       \
    )                                                                                             \
                                                                                                  \
    JAFG_JOIN_SCOPE_RESOLUTION_OUTER_TWO(                                                          \
        JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(                                                      \
            MyClassSpaces,                                                                        \
            JAFG_JOIN_OUTER_NINE(                                                                  \
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
        JAFG_JOIN_OUTER_NINE(                                                                      \
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
        Jafg::Detail::GetGlobalCxxRecordRegistry().AddNewPendingClassPackage(\
            std::make_unique<::Jafg::Detail::LRegistryClassPackage>( \
            JAFG_JOIN_SCOPE_RESOLUTION_OUTER_THREE(MyClassSpaces, MyClassName, MutableStaticClass)()) \
             \
        ); \
                                                           \
        return;                                                                                   \
    }                                                                                             \
                                                                                                  \
    namespace                                                                                     \
    {                                                                                             \
                                                                                                  \
    JAFG_JOIN_SCOPE_RESOLUTION_OUTER_TWO(                                                          \
        MyClassSpaces,                                                                            \
        JAFG_JOIN_OUTER_NINE(                                                                      \
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
        JAFG_JOIN_OUTER_TWO(                                                                       \
            JAFG_JOIN_OUTER_NINE(                                                                  \
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
    } /* ~Namespace <Anonymous> */                                                                \
    ::Jafg::LCxxClass& JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(MyClassSpaces, MyClassName)            \
        ::MutableStaticClass() noexcept \
    {\
        typedef JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(MyClassSpaces, MyClassName) _JAFG_OHGCRCHD_TObj; \
        static auto MallocCxxFn{[](::Jafg::LCxxDynamicInit const& Init) -> ::Jafg::JCxxClass* \
        { \
            return new\
                JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(MyClassSpaces, MyClassName)\
                (typename JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(MyClassSpaces, MyClassName)::LDynamicInitProj{}(Init)); \
        }}; \
        static auto BeginClassLifeFn{[](::Jafg::LBeginClassLifeInfo const& Info) -> void \
        { \
            JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(MyClassSpaces, MyClassName)::BeginClassLife(Info);\
        }}; \
        static auto EndClassLifeFn{[](::Jafg::LEndClassLifeInfo const& Info) -> void \
        { \
        JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(MyClassSpaces, MyClassName)::EndClassLife(Info);\
        }}; \
        static ::Jafg::LCxxClass Instance{ \
              #MyClassSpaces "::" #MyClassName \
            , MallocCxxFn, BeginClassLifeFn, EndClassLifeFn \
, #SuperClassName, JAFG_JOIN_SCOPE_RESOLUTION_INNER_TWO(                                          \
MyClassSpaces,                                                            \
JAFG_JOIN_OUTER_NINE(                                                      \
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
)::Flags, {__VA_ARGS__} \
            }; \
        return Instance; \
    }

#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_SET
#error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_SET already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_SET */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_SET(ObjectType, Name) \
    ::Jafg::LSetCxxClassField::CreateStrong([](::Jafg::JCxxClass* Object, ::LStringView Value) -> void \
    {\
         ObjectType * CastedObject{StaticCastChecked< ObjectType >(Object)}; \
         ObjectType ::\
            JAFG_JOIN_OUTER_TWO(_SetField_, Name)\
            (CastedObject, Value); \
    })

#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_GET
#error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_GET already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_GET */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DEFINITION_GET(ObjectType, Name) \
    ::Jafg::LGetCxxClassField::CreateStrong([](::Jafg::JCxxClass const& Object) -> ::LString \
    {\
         ObjectType const& CastedObject{*StaticCastChecked< ObjectType >(&Object)}; \
         return ObjectType ::\
            JAFG_JOIN_OUTER_TWO(_GetField_, Name)\
            (CastedObject); \
    })

#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(                             \
        MyClassName,                                                                         \
        MyClassSpaces,                                                                       \
        SuperClassName,                                                                      \
        ConstructionHelperLine,                                                              \
        ... /*OptionalAPI*/                                                                                 \
    )                                                                                        \
                                                                                             \
public:                                                                                     \
    typedef SuperClassName Super;                                                            \
    typedef MyClassName    Derived;                                                          \
private:                                                                                     \
    friend ::Jafg::Detail::NewStaticCxxFn;                                           \
    friend JAFG_JOIN_OUTER_NINE(                                                             \
      L,                                                                                     \
      _,                                                                                     \
      JAFG_PRIVATE_FILE_ID,                                                                  \
      _,                                                                                     \
      MyClassName,                                                                           \
      _,                                                                                     \
      ConstructionHelperLine,                                                                \
      _,                                                                                     \
      ConstructionHelper                                                                     \
      );                                                                                     \
                                                                                             \
public:                                                                                      \
    inline static ::Jafg::LCxxClass const& StaticClass() noexcept                                     \
    {                                                                                        \
        return MyClassName::MutableStaticClass();                                                 \
    }                                                                                        \
    __VA_ARGS__/*OptionalAPI*/ static ::Jafg::LCxxClass& MutableStaticClass() noexcept;      \
    MyClassName() = delete;                                                                  \
    PROHIBIT_REALLOC_OF_ANY_FORM(MyClassName)                                                               \
                                                                                             \
private: /* Restore default visibility. */


#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WORLD_BODY_IMPL
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WORLD_BODY_IMPL already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WORLD_BODY_IMPL */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WORLD_BODY_IMPL( \
    MyClassName,                                                  \
    MyClassSpaces,                                                \
    SuperClassName,                                               \
    ConstructionHelperLine,                                       \
    .../*OptionalAPI*/                                                          \
    )                                                             \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(      \
        MyClassName,                                              \
        MyClassSpaces,                                            \
        SuperClassName,                                           \
        ConstructionHelperLine,                                   \
        __VA_ARGS__                                               \
    )                                                             \
public:                                                           \
    typedef ::Jafg::Detail::LOuter2WorldProj LDynamicInitProj; \
private: /* Restore default visibility. */


#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_BODY_IMPL
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_BODY_IMPL already defined."
#endif /* !PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_BODY_IMPL */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_BODY_IMPL( \
    MyClassName,                                                  \
    MyClassSpaces,                                                \
    SuperClassName,                                               \
    ConstructionHelperLine,                                       \
    ... /*OptionalAPI*/                                                          \
    )                                                             \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(      \
        MyClassName,                                              \
        MyClassSpaces,                                            \
        SuperClassName,                                           \
        ConstructionHelperLine,                                   \
        __VA_ARGS__                                               \
    )                                                             \
public:                                                           \
    typedef typename Super::LFactory LFactory;\
    typedef ::Jafg::Detail::LOuter2ViewportProj LDynamicInitProj; \
private: /* Restore default visibility. */


#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_BODY_IMPL
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_BODY_IMPL already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_BODY_IMPL */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_WIDGET_WITH_FACTORY_BODY_IMPL( \
    MyClassName,                                                               \
    MyClassSpaces,                                                             \
    SuperClassName,                                                            \
    ConstructionHelperLine,                                                    \
    ...  /*OptionalAPI*/                                                                      \
    )                                                                          \
    PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_BODY_IMPL(                   \
        MyClassName,                                                           \
        MyClassSpaces,                                                         \
        SuperClassName,                                                        \
        ConstructionHelperLine,                                                \
        __VA_ARGS__                                                            \
    )                                                                          \
public:                                                                        \
    typedef JAFG_JOIN_OUTER_NINE(                              \
        L,                                                                     \
        _,                                                                     \
        JAFG_PRIVATE_FILE_ID,                                                  \
        _,                                                                     \
        MyClassName,                                                           \
        _,                                                                     \
        ConstructionHelperLine,                                                \
        _,                                                                     \
        ConstructionHelper                                                     \
    )::LFactory LFactory;                                                \
    typedef ::Jafg::Detail::LOuter2ViewportProj LDynamicInitProj; \
private: /* Restore default visibility. */


#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_Config
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_Config already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_Config */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_Config(MyClassMember) \
    inline static void JAFG_JOIN_OUTER_FOUR(_, SetField, _, MyClassMember)(Derived* Class, ::LStringView _InValue) \
    {                check( Class )                                                                         \
        ::Serde::FromString(&Class->MyClassMember, _InValue);                          \
    }                                                                                         \
    inline static ::LString JAFG_JOIN_OUTER_FOUR(_, GetField, _, MyClassMember)(Derived const& Class)                       \
    {                                                                                      \
        return ::Serde::ToString(Class.MyClassMember);                                \
    }

#ifdef PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_DefaultOnly
    #error "PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_DefaultOnly already defined."
#endif /* PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_DefaultOnly */
#define PRIVATE_JAFG_OBJECT_HIERARCHY_GENERATED_CLASS_FIELD_DECLARATION_DefaultOnly(MyClassMember) \
    void JAFG_JOIN_OUTER_FOUR(_, MallocField, _, MyClassMember)(void* InMemory)                     \
    {                                                                                              \
        ::Jafg::OnDefaultOnlyMallocMember(&static_cast<Derived*>(InMemory)->MyClassMember);        \
    }

//#
//# Required before the jafg-class declaration.
//# @see Engine/Object.h, for example.
//#
#ifdef DECLARE_JAFG_CLASS
    #error "DECLARE_JAFG_CLASS already defined."
#endif /* DECLARE_JAFG_CLASS */
#define DECLARE_JAFG_CLASS(...)                                        \
    JAFG_JOIN_OUTER_FIVE(                                               \
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
//# You may optionally provide an API as argument.
//#
#ifdef GENERATED_CLASS_BODY
    #error "GENERATED_CLASS_BODY already defined."
#endif /* GENERATED_CLASS_BODY */
#define GENERATED_CLASS_BODY(...)   \
    JAFG_JOIN_OUTER_FIVE(               \
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
    #error "DECLARE_JAFG_WIDGET already defined."
#endif /* DECLARE_JAFG_WIDGET */
#define DECLARE_JAFG_WIDGET(...) DECLARE_JAFG_CLASS(__VA_ARGS__)

//#
//# Required before the jafg-widget-class declaration
//# @see Widgets/WidgetNode.h
//#
#ifdef DECLARE_JAFG_WIDGET_WITH_FACTORY
    #error "DECLARE_JAFG_WIDGET_WITH_FACTORY already defined."
#endif /* DECLARE_JAFG_WIDGET_WITH_FACTORY */
#define DECLARE_JAFG_WIDGET_WITH_FACTORY(TFactoryTy, ...)              \
    JAFG_JOIN_OUTER_FIVE(                                               \
        JAFG_PRIVATE_FILE_ID,                                          \
        _,                                                             \
        __LINE__,                                                      \
        _,                                                             \
        MY_GENERATED_CLASS_REGISTRATION_CONSTRUCTOR_HELPER_DECLARATION \
        )(TFactoryTy, __VA_ARGS__ )

//#
//# Default behavior for all memory allocations.
//#
#ifdef DEFAULT_OBJECT_CONSTRUCTORS
    #error "DEFAULT_OBJECT_CONSTRUCTORS already defined."
#endif /* DEFAULT_OBJECT_CONSTRUCTORS */
#define DEFAULT_OBJECT_CONSTRUCTORS(MyClassName) \
    DEFAULT_OBJECT_DYNAMIC_CTOR(MyClassName)            \
    DEFAULT_OBJECT_STATIC_CTOR(MyClassName)

//#
//# Default behavior for all memory allocations with an additional body to execute code.
//#
//# Use as follows:
//#     DEFAULT_OBJECT_CONSTRUCTORS_BODY(CxxClass)
//#     {
//#         <...>
//#     }
//#
#ifdef DEFAULT_OBJECT_CONSTRUCTORS_BODY
    #error "DEFAULT_OBJECT_CONSTRUCTORS_BODY already defined."
#endif /* DEFAULT_OBJECT_CONSTRUCTORS_BODY */
#define DEFAULT_OBJECT_CONSTRUCTORS_BODY(MyClassName) \
    DEFAULT_OBJECT_DYNAMIC_CTOR_BODY(MyClassName) { this->_CommonCtorLogic(Init); }           \
    DEFAULT_OBJECT_STATIC_CTOR_BODY(MyClassName) { this->_CommonCtorLogic(Init); } \
    void _CommonCtorLogic(auto const& Init)

//#
//# Default behavior for all memory allocations.
//#
#ifdef DEFAULT_WORLD_CONSTRUCTORS
    #error "DEFAULT_WORLD_CONSTRUCTORS already defined."
#endif /* DEFAULT_WORLD_CONSTRUCTORS */
#define DEFAULT_WORLD_CONSTRUCTORS(MyClassName) \
    DEFAULT_WORLD_DYNAMIC_CTOR(MyClassName)            \
    DEFAULT_WORLD_STATIC_CTOR(MyClassName)

//#
//# Default behavior for all memory allocations with an additional body to execute code.
//#
//# Use as follows:
//#     DEFAULT_WORLD_CONSTRUCTORS_BODY(CxxClass)
//#     {
//#         <...>
//#     }
//#
#ifdef DEFAULT_WORLD_CONSTRUCTORS_BODY
    #error "DEFAULT_WORLD_CONSTRUCTORS_BODY already defined."
#endif /* DEFAULT_WORLD_CONSTRUCTORS_BODY */
#define DEFAULT_WORLD_CONSTRUCTORS_BODY(MyClassName) \
    DEFAULT_WORLD_DYNAMIC_CTOR_BODY(MyClassName) { this->_CommonCtorLogic(Init); }           \
    DEFAULT_WORLD_STATIC_CTOR_BODY(MyClassName) { this->_CommonCtorLogic(Init); } \
    void _CommonCtorLogic(auto const& Init)

//#
//# Default behavior for all memory allocations.
//#
#ifdef DEFAULT_NODE_CONSTRUCTORS
    #error "DEFAULT_NODE_CONSTRUCTORS already defined."
#endif /* DEFAULT_NODE_CONSTRUCTORS */
#define DEFAULT_NODE_CONSTRUCTORS(MyClassName) \
    DEFAULT_NODE_DYNAMIC_CTOR(MyClassName)            \
    DEFAULT_NODE_STATIC_CTOR(MyClassName)

//#
//# Default behavior for all memory allocations with an additional body to execute code.
//#
//# Use as follows:
//#     DEFAULT_NODE_CONSTRUCTORS_BODY(CxxClass)
//#     {
//#         <...>
//#     }
//#
#ifdef DEFAULT_NODE_CONSTRUCTORS_BODY
    #error "DEFAULT_NODE_CONSTRUCTORS_BODY already defined."
#endif /* DEFAULT_NODE_CONSTRUCTORS_BODY */
#define DEFAULT_NODE_CONSTRUCTORS_BODY(MyClassName) \
    DEFAULT_NODE_DYNAMIC_CTOR_BODY(MyClassName) { this->_CommonCtorLogic(Init); }           \
    DEFAULT_NODE_STATIC_CTOR_BODY(MyClassName) { this->_CommonCtorLogic(Init); } \
    void _CommonCtorLogic(auto const& Init)

//#
//# Default behavior for all memory allocations.
//#
#ifdef DEFAULT_WIDGET_CONSTRUCTORS
    #error "DEFAULT_WIDGET_CONSTRUCTORS already defined."
#endif /* DEFAULT_WIDGET_CONSTRUCTORS */
#define DEFAULT_WIDGET_CONSTRUCTORS(MyClassName) \
    DEFAULT_WIDGET_DYNAMIC_CTOR(MyClassName)            \
    DEFAULT_WIDGET_STATIC_CTOR(MyClassName)
//#
//# Default behavior for all memory allocations with an additional body to execute code.
//#
//# Use as follows:
//#     DEFAULT_WIDGET_CONSTRUCTORS_BODY(CxxClass)
//#     {
//#         <...>
//#     }
//#
#ifdef DEFAULT_WIDGET_CONSTRUCTORS_BODY
    #error "DEFAULT_WIDGET_CONSTRUCTORS_BODY already defined."
#endif /* DEFAULT_WIDGET_CONSTRUCTORS_BODY */
#define DEFAULT_WIDGET_CONSTRUCTORS_BODY(MyClassName) \
    DEFAULT_WIDGET_DYNAMIC_CTOR_BODY(MyClassName) { this->_CommonCtorLogic(Init); }           \
    DEFAULT_WIDGET_STATIC_CTOR_BODY(MyClassName) { this->_CommonCtorLogic(Init); } \
    void _CommonCtorLogic(auto const& Init)

//#
//# Default dynamic ctor for a JCxxClasses. Mandatory. It may be used to declare extra information for all
//# objects of this type. The new ctor is prohibited from changing the arguments of the super dynamic ctor.
//#
#ifdef DEFAULT_OBJECT_DYNAMIC_CTOR
    #error "DEFAULT_OBJECT_DYNAMIC_CTOR already defined."x
#endif /* DEFAULT_OBJECT_DYNAMIC_CTOR */
#define DEFAULT_OBJECT_DYNAMIC_CTOR(MyClassName)                                         \
    explicit MyClassName(::Jafg::LCxxDynamicInit const& Init) noexcept : Super{Init} {}
//#
//# Default static ctor for a JCxxClass. Not mandatory but default implementation is highly recommended.
//# The first argument must be the same as the first argument of the super static ctor.
//#
#ifdef DEFAULT_OBJECT_STATIC_CTOR
    #undef DEFAULT_OBJECT_STATIC_CTOR
#endif /* DEFAULT_OBJECT_STATIC_CTOR */
#define DEFAULT_OBJECT_STATIC_CTOR(MyClassName) \
    template<typename TCxxClass> \
    explicit MyClassName(::Jafg::TCxxStaticInit<TCxxClass> const& Init) noexcept : Super{Init} {}

//# Default jxx ctors for subclasses of Jafg::JCxxClass with no body attached.
#ifdef DEFAULT_OBJECT_DYNAMIC_CTOR_BODY
    #error "DEFAULT_OBJECT_DYNAMIC_CTOR_BODY already defined."x
#endif /* DEFAULT_OBJECT_DYNAMIC_CTOR_BODY */
#define DEFAULT_OBJECT_DYNAMIC_CTOR_BODY(MyClassName)                                         \
    explicit MyClassName(::Jafg::LCxxDynamicInit const& Init) noexcept : Super{Init}
#ifdef DEFAULT_OBJECT_STATIC_CTOR_BODY
    #undef DEFAULT_OBJECT_STATIC_CTOR_BODY
#endif /* DEFAULT_OBJECT_STATIC_CTOR_BODY */
#define DEFAULT_OBJECT_STATIC_CTOR_BODY(MyClassName) \
    template<typename TCxxClass> \
    explicit MyClassName(::Jafg::TCxxStaticInit<TCxxClass> const& Init) noexcept : Super{Init}

//# Default world object ctors for subclasses of Jafg::AWorldObjects.
#ifdef DEFAULT_WORLD_DYNAMIC_CTOR
    #error "DEFAULT_WORLD_DYNAMIC_CTOR already defined."
#endif /* DEFAULT_WORLD_DYNAMIC_CTOR */
#define  DEFAULT_WORLD_DYNAMIC_CTOR(MyClassName) \
    explicit MyClassName(::Jafg::LWorldDynamicInit const& Init) noexcept : Super{Init} {}
#ifdef DEFAULT_WORLD_STATIC_CTOR
    #undef DEFAULT_WORLD_STATIC_CTOR
#endif /* DEFAULT_WORLD_STATIC_CTOR */
#define DEFAULT_WORLD_STATIC_CTOR(MyClassName) \
    template<typename TCxxClass> \
    explicit MyClassName(::Jafg::TWorldStaticInit<TCxxClass> const& Init) noexcept : Super{Init} {}

//# Default world object ctors for subclasses of Jafg::AWorldObjects with no body attached.
#ifdef DEFAULT_WORLD_DYNAMIC_CTOR_BODY
    #error "DEFAULT_WORLD_DYNAMIC_CTOR_BODY already defined."
#endif /* DEFAULT_WORLD_DYNAMIC_CTOR_BODY */
#define  DEFAULT_WORLD_DYNAMIC_CTOR_BODY(MyClassName) \
    explicit MyClassName(::Jafg::LWorldDynamicInit const& Init) noexcept : Super{Init}
#ifdef DEFAULT_WORLD_STATIC_CTOR_BODY
    #undef DEFAULT_WORLD_STATIC_CTOR_BODY
#endif /* DEFAULT_WORLD_STATIC_CTOR_BODY */
#define DEFAULT_WORLD_STATIC_CTOR_BODY(MyClassName) \
    template<typename TCxxClass> \
    explicit MyClassName(::Jafg::TWorldStaticInit<TCxxClass> const& Init) noexcept : Super{Init}

//# Default node ctors for subclasses of Jafg::WNode.
#ifdef DEFAULT_NODE_DYNAMIC_CTOR
    #error "DEFAULT_NODE_DYNAMIC_CTOR already defined."
#endif /* DEFAULT_NODE_DYNAMIC_CTOR */
#define DEFAULT_NODE_DYNAMIC_CTOR(MyClassName)                                         \
    explicit MyClassName(::Jafg::LNodeDynamicInit const& Init) noexcept : Super{Init} {}
#ifdef DEFAULT_NODE_STATIC_CTOR
    #undef DEFAULT_NODE_STATIC_CTOR
#endif /* DEFAULT_NODE_STATIC_CTOR */
#define DEFAULT_NODE_STATIC_CTOR(MyClassName) \
    template<typename TCxxClass> \
    explicit MyClassName(::Jafg::TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init} {}

//# Default node ctors for subclasses of Jafg::WNode with no body attached.
#ifdef DEFAULT_NODE_DYNAMIC_CTOR_BODY
    #error "DEFAULT_NODE_DYNAMIC_CTOR_BODY already defined."
#endif /* DEFAULT_NODE_DYNAMIC_CTOR_BODY */
#define DEFAULT_NODE_DYNAMIC_CTOR_BODY(MyClassName)                                         \
    explicit MyClassName(::Jafg::LNodeDynamicInit const& Init) noexcept : Super{Init}
#ifdef DEFAULT_NODE_STATIC_CTOR_BODY
    #undef DEFAULT_NODE_STATIC_CTOR_BODY
#endif /* DEFAULT_NODE_STATIC_CTOR_BODY */
#define DEFAULT_NODE_STATIC_CTOR_BODY(MyClassName) \
    template<typename TCxxClass> \
    explicit MyClassName(::Jafg::TNodeStaticInit<TCxxClass> const& Init) noexcept : Super{Init}

//# Default widget ctors for subclasses of Jafg::WUserWidget.
#ifdef DEFAULT_WIDGET_DYNAMIC_CTOR
    #error "DEFAULT_WIDGET_DYNAMIC_CTOR already defined."
#endif /* DEFAULT_WIDGET_DYNAMIC_CTOR */
#define DEFAULT_WIDGET_DYNAMIC_CTOR(MyClassName)                                         \
    explicit MyClassName(::Jafg::LWidgetDynamicInit const& Init) noexcept : Super{Init} {}
#ifdef DEFAULT_WIDGET_STATIC_CTOR
    #undef DEFAULT_WIDGET_STATIC_CTOR
#endif /* DEFAULT_WIDGET_STATIC_CTOR */
#define DEFAULT_WIDGET_STATIC_CTOR(MyClassName) \
    template<typename TCxxClass> \
    explicit MyClassName(::Jafg::TWidgetStaticInit<TCxxClass> const& Init) noexcept : Super{Init} {}

//# Default widget ctors for subclasses of Jafg::WUserWidget with no body attached.
#ifdef DEFAULT_WIDGET_DYNAMIC_CTOR_BODY
    #error "DEFAULT_WIDGET_DYNAMIC_CTOR_BODY already defined."
#endif /* DEFAULT_WIDGET_DYNAMIC_CTOR_BODY */
#define DEFAULT_WIDGET_DYNAMIC_CTOR_BODY(MyClassName)                                         \
    explicit MyClassName(::Jafg::LWidgetDynamicInit const& Init) noexcept : Super{Init}
#ifdef DEFAULT_WIDGET_STATIC_CTOR_BODY
    #undef DEFAULT_WIDGET_STATIC_CTOR_BODY
#endif /* DEFAULT_WIDGET_STATIC_CTOR_BODY */
#define DEFAULT_WIDGET_STATIC_CTOR_BODY(MyClassName) \
    template<typename TCxxClass> \
    explicit MyClassName(::Jafg::TWidgetStaticInit<TCxxClass> const& Init) noexcept : Super{Init}

//#
//# Mark a member of a j-class as something special.
//# Allowed values are:
//#   Config      - The member of a configuration. Only affects the default class referrer. (Requires that the class is
//#                 also marked as a "Config".)
//#   DefaultOnly - On j-object class allocation, this member will usually be zeroed out.
//#                 (@see Engine/ObjectBaseUtility.h::OnJObjectDefaultOnlyMallocMember)
//#
#ifdef CLASS_FIELD
    #undef CLASS_FIELD
#endif /* CLASS_FIELD */
#define CLASS_FIELD(...)                     \
    JAFG_JOIN_OUTER_FIVE(                     \
        JAFG_PRIVATE_FILE_ID,                \
        _,                                   \
        __LINE__,                            \
        _,                                   \
        MY_GENERATED_CLASS_FIELD_DECLARATION \
        )()
