// Copyright mzoesch. All rights reserved.

#pragma once

template<typename T, typename Deleter = std::default_delete<T>>
using TUnique = std::unique_ptr<T, Deleter>;
