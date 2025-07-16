# Copyright mzoesch. All rights reserved.

function(JsonfyList InList OutString)
    set(out "")
    foreach(item IN LISTS ${InList})
        string(APPEND out "\"${item}\", ")
    endforeach()

    string(REGEX REPLACE ", $" "" out "${out}")
    set(${OutString} "[${out}]" PARENT_SCOPE)
endfunction()
