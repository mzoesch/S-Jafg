// Copyright mzoesch. All rights reserved.

#pragma once

/**
 * Makes a signature for a delegate.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param RetTy      The return type of the delegate (can be void).
 * @param ...        The parameter types of the delegate (can be void / or empty).
 */
#define MAKE_DELEGATE_SIGNATURE(DelNameSig, RetTy, ...)       \
    typedef ::Jafg::TDelegate<RetTy(__VA_ARGS__)> DelNameSig;

/**
 * Declares a delegate by its signature.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param DelName    The name of the delegate.
 */
#define DECLARE_DELEGATE_BY_SIG(DelNameSig, DelName) \
    DelNameSig DelName;

/**
 * Declares a delegate by its signature. The signature will be created automatically.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param DelName    The name of the delegate.
 * @param RetTy      The return type of the delegate (can be void).
 * @param ...        The parameter types of the delegate (can be void / or empty).
 */
#define DECLARE_DELEGATE(DelNameSig, DelName, RetTy, ...)     \
    MAKE_DELEGATE_SIGNATURE(DelNameSig, RetTy, ##__VA_ARGS__) \
    DECLARE_DELEGATE_BY_SIG(DelNameSig, DelName);

/**
 * Declares an inline delegate by its signature.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param DelName    The name of the delegate.
 */
#define DECLARE_INLINE_DELEGATE_BY_SIG(DelNameSig, DelName) \
    inline DelNameSig DelName;

/**
 * Declares an inline delegate by its signature. The signature will be created automatically.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param DelName    The name of the delegate.
 * @param RetTy      The return type of the delegate (can be void).
 * @param ...        The parameter types of the delegate (can be void / or empty).
 */
#define DECLARE_INLINE_DELEGATE(DelNameSig, DelName, RetTy, ...) \
    MAKE_DELEGATE_SIGNATURE(DelNameSig, RetTy, ##__VA_ARGS__)    \
    DECLARE_INLINE_DELEGATE_BY_SIG(DelNameSig, DelName);

/**
 * Declares a multicast delegate by its signature. The return value is always void.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param ...        The parameter types of the delegate (can be void / or empty).
 */
#define MAKE_MULTICAST_SIGNATURE(DelNameSig, ...)                     \
    typedef ::Jafg::TMulticastDelegate<void(__VA_ARGS__)> DelNameSig;

/**
 * Declares a multicast delegate by its signature.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param DelName    The name of the delegate.
 */
#define DECLARE_MULTICAST_DELEGATE_BY_SIG(DelNameSig, DelName) \
    DelNameSig DelName;

/**
 * Declares a multicast delegate by its signature. The signature will be created automatically.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param DelName    The name of the delegate.
 * @param ...        The parameter types of the delegate (can be void / or empty).
 */
#define DECLARE_MULTICAST_DELEGATE(DelNameSig, DelName, ...) \
    MAKE_MULTICAST_SIGNATURE(DelNameSig, ##__VA_ARGS__)      \
    DECLARE_MULTICAST_DELEGATE_BY_SIG(DelNameSig, DelName);

/**
 * Declares an inline multicast delegate by its signature.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param DelName    The name of the delegate.
 */
#define DECLARE_INLINE_MULTICAST_DELEGATE_BY_SIG(DelNameSig, DelName) \
    inline DelNameSig DelName;

/**
 * Declares an inline multicast delegate by its signature. The signature will be created automatically.
 *
 * @param DelNameSig The name of the delegate signature.
 * @param DelName    The name of the delegate.
 * @param ...        The parameter types of the delegate (can be void / or empty).
 */
#define DECLARE_INLINE_MULTICAST_DELEGATE(DelNameSig, DelName, ...) \
    MAKE_MULTICAST_SIGNATURE(DelNameSig, ##__VA_ARGS__)             \
    DECLARE_INLINE_MULTICAST_DELEGATE_BY_SIG(DelNameSig, DelName);
