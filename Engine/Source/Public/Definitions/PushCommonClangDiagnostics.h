//
// Customize clang warnings.
// @see https://clang.llvm.org/docs/DiagnosticsReference.html
//

/*-----------------------------------------------------------------------------
    Raise.
-----------------------------------------------------------------------------*/
#pragma clang diagnostic error "-Wbraced-scalar-init"
#pragma clang diagnostic error "-Wbuiltin-macro-redefined"
#pragma clang diagnostic error "-Wc99-designator"
#pragma clang diagnostic error "-Wdangling-else"
#pragma clang diagnostic error "-Wdeprecated-literal-operator"
#pragma clang diagnostic error "-Wduplicate-decl-specifier"
#pragma clang diagnostic error "-Wdynamic-class-memaccess"
#pragma clang diagnostic error "-Wextra-qualification"
#pragma clang diagnostic error "-Wextra-semi"
#pragma clang diagnostic error "-Wextra-tokens"
#pragma clang diagnostic error "-Wignored-attributes"
#pragma clang diagnostic error "-Winconsistent-missing-override"
#pragma clang diagnostic error "-Winfinite-recursion"
#pragma clang diagnostic error "-Winline-new-delete"
#pragma clang diagnostic error "-Winvalid-noreturn"
#pragma clang diagnostic error "-Wkeyword-macro"
#pragma clang diagnostic error "-Wlogical-op-parentheses"
#pragma clang diagnostic error "-Wmacro-redefined"
#pragma clang diagnostic error "-Wmismatched-new-delete"
#pragma clang diagnostic error "-Wmismatched-tags"
#pragma clang diagnostic error "-Wmissing-braces"
#pragma clang diagnostic error "-Wnontrivial-memcall"
#pragma clang diagnostic error "-Wnull-character"
#pragma clang diagnostic error "-Wnull-pointer-subtraction"
#pragma clang diagnostic error "-Wparentheses"
#pragma clang diagnostic error "-Wpessimizing-move"
#pragma clang diagnostic error "-Wpragmas"
#pragma clang diagnostic error "-Wpragma-once-outside-header"
#pragma clang diagnostic error "-Wreorder-init-list"
#pragma clang diagnostic error "-Wreturn-stack-address"
#pragma clang diagnostic error "-Wreturn-type"
#pragma clang diagnostic error "-Wself-assign-field"
#pragma clang diagnostic error "-Wsign-compare"
#pragma clang diagnostic error "-Wswitch"
#pragma clang diagnostic error "-Wundefined-inline"
#pragma clang diagnostic error "-Wuninitialized"
#pragma clang diagnostic error "-Wunknown-pragmas"
#pragma clang diagnostic error "-Wunknown-warning-option"
#pragma clang diagnostic error "-Wunnecessary-virtual-specifier"
#pragma clang diagnostic error "-Wunused-comparison"
#pragma clang diagnostic error "-Wunused-lambda-capture"
#pragma clang diagnostic error "-Wunused-result"

/*-----------------------------------------------------------------------------
    Ignore.
-----------------------------------------------------------------------------*/
#pragma clang diagnostic ignored "-Wcomment"
#pragma clang diagnostic ignored "-Wcomments"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma clang diagnostic ignored "-Wmissing-designated-field-initializers"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wnullability-extension"
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#pragma clang diagnostic ignored "-Wunused-parameter"

/*-----------------------------------------------------------------------------
    Shipping only.
-----------------------------------------------------------------------------*/
#if JAFG_DO_ENABLE_SHIPPING_WARNINGS
    #pragma clang diagnostic warning "-Wundefined-var-template"
    #pragma clang diagnostic warning "-Wunused-but-set-variable"
    #pragma clang diagnostic warning "-Wunused-private-field"
    #pragma clang diagnostic warning "-Wunused-function"
    #pragma clang diagnostic warning "-Wunused-variable"
#else /* JAFG_DO_ENABLE_SHIPPING_WARNINGS */
    #pragma clang diagnostic ignored "-Wundefined-var-template"
    #pragma clang diagnostic ignored "-Wunused-but-set-variable"
    #pragma clang diagnostic ignored "-Wunused-private-field"
    #pragma clang diagnostic ignored "-Wunused-function"
    #pragma clang diagnostic ignored "-Wunused-variable"
#endif /* !JAFG_DO_ENABLE_SHIPPING_WARNINGS */
