// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace rhi
{

//# The state that is required after a resource load function was called.
enum struct resource_state_bits
{
    //#
    //# No state is required. Just allocate the resource. The client will handle the rest.
    //# If another client already requested host or device memory then the resource will be in the same
    //# state as requested previously by the other client.
    //#
    None = 0 << 0,
    //#
    //# The resource data will be loaded to host memory. If another client already requested device memory
    //# then the resource will be loaded to host and device memory.
    //#
    Host = 1 << 0,
    //#
    //# The resource data will be loaded to device memory. Host memory will be orphaned if host memory was
    //# not requested by another client.
    //#
    Device = 1 << 1,
};
ENUM_STRUCT_FLAGS(resource_state_bits, resource_state_flags)

//# Use this as we might want to handle resources differently later on.
template<typename T> using shared_ref = std::shared_ptr<T>;

} /* ~Namespace rhi */
