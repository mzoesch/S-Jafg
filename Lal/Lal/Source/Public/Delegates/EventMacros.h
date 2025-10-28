// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# Makes a signature for an event.
//#
//# @param Owner The owner type of this event.
//# @param Name  The name of this event.
//# @param Ret   The return type of this event.
//# @param ...   The parameters (optional).
//#
#define EVENT_SIG(Owner, Name, Ret, ...) typedef ::TEvent<Owner, Ret(__VA_ARGS__)> Name;
#define MULTI_EVENT_SIG(Owner, Name, ...) typedef ::TMulticastEvent<Owner, void(__VA_ARGS__)> Name;

//# Declare an event. Only for jcxx objects.
#define EVENT_DECL(Name, Ret, ...) EVENT_DECL_VERBOSE(Derived, Name, Ret, __VA_ARGS__)
#define MULTI_EVENT_DECL(Name, ...) MULTI_EVENT_DECL_VERBOSE(Derived, Name, __VA_ARGS__)

#define EVENT_DECL_VERBOSE(Owner, Name, Ret, ...) ::TEvent<Owner, Ret(__VA_ARGS__)> Name;
#define MULTI_EVENT_DECL_VERBOSE(Owner, Name, ...) ::TMulticastEvent<Owner, void(__VA_ARGS__)> Name;
