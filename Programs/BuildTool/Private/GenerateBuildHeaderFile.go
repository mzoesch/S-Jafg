// Copyright 2024 mzoesch. All rights reserved.

package Private

import (
    "Jafg/BuildTool/Info"
    "Jafg/Shared"
    "fmt"
    "strings"
)

func GetProjectCallSpecs() string {
    Info.CheckIfCurrentProjectExists()

    var builder strings.Builder = strings.Builder{}

    for _, proj := range Shared.GApp.Projects {
        builder.WriteString(fmt.Sprintf("#if PRIVATE_JAFG_CURRENT_PROJECT_PREPROC_IDENT == 0b%s /* Alias for proj: %s. */\n",
            proj.GetPreProcIntAsStringBase2(), strings.ToUpper(proj.Name)))

        for _, projApi := range Shared.GApp.Projects {
            if proj.Name == projApi.Name {
                builder.WriteString("    ")
                builder.WriteString(GetCallSpecForProject(&projApi, projApi.Kind.IsShared()))
                builder.WriteString("    ")
                builder.WriteString(GetExternSpecForProject(&projApi, projApi.Kind.IsShared()))
            } else {
                builder.WriteString("    ")
                builder.WriteString(GetCallSpecForProject(&projApi, false))
                builder.WriteString("    ")
                builder.WriteString(GetExternSpecForProject(&projApi, false))
            }
        }

        builder.WriteString(fmt.Sprintf("#endif /* PRIVATE_JAFG_CURRENT_PROJECT_PREPROC_IDENT == 0b%s */\n", proj.GetPreProcIntAsStringBase2()))

        continue
    }


    return builder.String()
}

func AssertProjectCallSpecs() string {
    var builder strings.Builder = strings.Builder{}

    for _, proj := range Shared.GApp.Projects {
        builder.WriteString(fmt.Sprintf("#ifndef %s_API\n", strings.ToUpper(proj.Name)))
        builder.WriteString(fmt.Sprintf("#pragma message( \\\n" +
            "    \"Callspec API for project %s is not defined. PreProcIdent: \" \\\n" +
            "    PRIVATE_JAFG_BUILD_GENERATED_STR(PRIVATE_JAFG_CURRENT_PROJECT_PREPROC_IDENT \\\n" +
            ") )\n",
        proj.Name))
        builder.WriteString(fmt.Sprintf("    #error message \"Callspec API for project %s is not defined " +
            "[PreProcIdent :: {#PRIVATE_JAFG_CURRENT_PROJECT_PREPROC_IDENT}].\n", proj.Name))
        builder.WriteString(fmt.Sprintf("#endif /* %s_API */\n\n", strings.ToUpper(proj.Name)))

        continue
    }

    return builder.String()
}

func GetCallSpecForProject(proj *Shared.Project, bDoExport bool) string {
    var spec string = ""
    if bDoExport {
        spec = "PLATFORM_CALLSPEC_OUT"
    } else {
        spec = "PLATFORM_CALLSPEC_IN"
    }

    return fmt.Sprintf("#define %s_API                  %s\n",
        strings.ToUpper(proj.Name),
        spec,
    )
}

func GetExternSpecForProject(proj *Shared.Project, bDoExport bool) string {
    var spec string = ""
    if bDoExport {
        spec = "PLATFORM_EXTERNSPEC_OUT"
    } else {
        spec = "PLATFORM_EXTERNSPEC_IN"
    }

    return fmt.Sprintf("#define %s_EXTERN              %s\n",
        strings.ToUpper(proj.Name),
        spec,
    )
}
