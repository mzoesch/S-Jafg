// Copyright 2024 mzoesch. All rights reserved.

package BuildTool

import (
    "Jafg/Shared"
    "fmt"
    "os"
    "strings"
)

func GetJustNameOfFileNoExtensions(file *os.File) string {
    var name string = file.Name()

    if strings.Contains(name, "\\") {
        name = strings.ReplaceAll(name, "\\", "/")
    }

    var start int = strings.LastIndex(name, "/") + 1
    var end int = strings.LastIndex(name, Shared.GeneratedHeadersExtension)

    return name[start:end]
}

// MakeHeaderContentFinal make the final output of content for a header file with all of its comments and guards.
func MakeHeaderContentFinal(file *os.File, bDoHeaderGuard bool, bIsHeaderGuardModuleSpecific bool, content string) string {
    if GBuildInfo == nil {
        panic("BuildInfo not initialized.")
    }

    var justFilename string = GetJustNameOfFileNoExtensions(file)

    var headerGuard string = ""
    if bDoHeaderGuard {
        if bIsHeaderGuardModuleSpecific {
            headerGuard = GetHeaderGuardForModule(GBuildInfo.ModuleName, justFilename)
        } else {
            headerGuard = GetHeaderGuardGeneric(justFilename)
        }
    }

    return fmt.Sprintf(
        "%s\n%s\n%s\n",
        GeneratedHeaderFileStub,
        headerGuard,
        content,
    )
}

func GetHeaderGuardForModule(moduleName string, justFilename string) string {
    return fmt.Sprintf(`
#ifdef PRIVATE_JAFG_%s_%s_GENERATED_HEADER
#error "Generated header from %s-file inside project %s already exists. Missing #ifndef or #pragma once directive?"
#endif /* PRIVATE_JAFG_%s_%s_GENERATED_HEADER */
#define PRIVATE_JAFG_%s_%s_GENERATED_HEADER
`,
        moduleName, justFilename,
        justFilename, moduleName,
        moduleName, justFilename,
        moduleName, justFilename,
    )
}

func GetHeaderGuardGeneric(justFilename string) string {
    return fmt.Sprintf(`
#ifdef PRIVATE_JAFG_%s_GENERATED_HEADER
#error "Generated header from %s-file already exists. Missing #ifndef or #pragma once directive?"
#endif /* PRIVATE_JAFG_%s_GENERATED_HEADER */
#define PRIVATE_JAFG_%s_GENERATED_HEADER
`,
        justFilename,
        justFilename,
        justFilename,
        justFilename,
    )
}

func MakeModuleCallSpecs() string {
    var builder strings.Builder = strings.Builder{}

    for _, mod := range Shared.GApp.GetAllModules() {
        builder.WriteString(fmt.Sprintf("#if PRIVATE_JAFG_CURRENT_MODULE_PREPROC_IDENT == 0b%s /* Alias for proj: %s. */\n",
            mod.GetPreProcIntAsStringBase2(), strings.ToUpper(mod.GetUsableName())))

        for _, modApi := range Shared.GApp.GetAllModules() {
            if mod.GetUsableName() == modApi.GetUsableName() {
                builder.WriteString("    ")
                builder.WriteString(GetCallSpecForModule(modApi, modApi.GetKind().IsShared()))
                builder.WriteString("    ")
                builder.WriteString(GetExternSpecForModule(modApi, modApi.GetKind().IsShared()))
            } else {
                builder.WriteString("    ")
                builder.WriteString(GetCallSpecForModule(modApi, false))
                builder.WriteString("    ")
                builder.WriteString(GetExternSpecForModule(modApi, false))
            }

            continue
        }

        builder.WriteString(fmt.Sprintf(
            "#endif /* PRIVATE_JAFG_CURRENT_MODULE_PREPROC_IDENT == 0b%s */\n",
            mod.GetPreProcIntAsStringBase2(),
        ))

        continue
    }

    return builder.String()
}

func MakeModuleAssertCallSpecs() string {
    var builder strings.Builder = strings.Builder{}

    for _, mod := range Shared.GApp.GetAllModules() {
        builder.WriteString(fmt.Sprintf("#ifndef %s_API\n", strings.ToUpper(mod.GetUsableName())))
        builder.WriteString(fmt.Sprintf("#pragma message( \\\n" +
            "    \"Callspec API for module %s is not defined. PreProcIdent: \" \\\n" +
            "    PRIVATE_JAFG_BUILD_GENERATED_STR(PRIVATE_JAFG_CURRENT_MODULE_PREPROC_IDENT \\\n" +
            ") )\n",
            mod.GetUsableName()))
        builder.WriteString(fmt.Sprintf("    #error message \"Callspec API for module %s is not defined " +
            "[PreProcIdent :: {#PRIVATE_JAFG_CURRENT_MODULE_PREPROC_IDENT}].\"\n", mod.GetUsableName()))
        builder.WriteString(fmt.Sprintf("#endif /* %s_API */\n\n", strings.ToUpper(mod.GetUsableName())))

        continue
    }

    return builder.String()
}


func GetCallSpecForModule(mod *Shared.Module, bDoExport bool) string {
    var spec string = ""
    if bDoExport {
        spec = "PLATFORM_CALLSPEC_OUT"
    } else {
        spec = "PLATFORM_CALLSPEC_IN"
    }

    return fmt.Sprintf("#define %s_API                  %s\n",
        strings.ToUpper(mod.GetUsableName()),
        spec,
    )
}

func GetExternSpecForModule(mod *Shared.Module, bDoExport bool) string {
    var spec string = ""
    if bDoExport {
        spec = "PLATFORM_EXTERNSPEC_OUT"
    } else {
        spec = "PLATFORM_EXTERNSPEC_IN"
    }

    return fmt.Sprintf("#define %s_EXTERN              %s\n",
        strings.ToUpper(mod.GetUsableName()),
        spec,
    )
}
