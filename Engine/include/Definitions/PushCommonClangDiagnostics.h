//
// Customize clang warnings.
// @see https://clang.llvm.org/docs/DiagnosticsReference.html
//

#pragma clang diagnostic ignored "-Wc++20-extensions"
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wc2y-extensions"
#pragma clang diagnostic ignored "-Wctad-maybe-unsupported"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-deprecated-sync"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wdouble-promotion" // Maybe?
#pragma clang diagnostic ignored "-Wexit-time-destructors" // We might want to look into this at a later time.
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wglobal-constructors" // We currently require this. but with c++26 reflection maybe not anymore??
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct" // All our compilers support this.
#pragma clang diagnostic ignored "-Wmissing-designated-field-initializers"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wmissing-noreturn" // Maybe active this if we get around PURE_VIRTUAL()?
#pragma clang diagnostic ignored "-Wmissing-prototypes" // Only for del llmm ops. can we get around this?
#pragma clang diagnostic ignored "-Wnested-anon-types" // All our compilers support this.
#pragma clang diagnostic ignored "-Wnrvo" // Very interesting. We might want this later. But very annoying to deal with.
#pragma clang diagnostic ignored "-Wpadded" // Considering: -Wno-error=padded
#pragma clang diagnostic ignored "-Wreserved-identifier"
#pragma clang diagnostic ignored "-Wreserved-macro-identifier"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wshadow-field"
#pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
#pragma clang diagnostic ignored "-Wshadow-header" // That's how it should be. Why is this a warning??
#pragma clang diagnostic ignored "-Wsign-conversion" // To pedantic.
#pragma clang diagnostic ignored "-Wswitch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wunique-object-duplication" // Mmm, this one we might want.
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage-in-libc-call"
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#pragma clang diagnostic ignored "-Wunused-member-function"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wweak-vtables" // Just trust the compiler here.

#if !JAFG_IN_SHIPPING
    #pragma clang diagnostic ignored "-Wunused-variable"
#endif /* !JAFG_IN_SHIPPING */
