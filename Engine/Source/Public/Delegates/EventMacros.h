// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# Makes a signature for an event.
//#
//# @param Owner The owner type of this event.
//# @param Name  The name of this event.
//# @param ...   The signature.
//#
#define EVENT_SIG(Owner, Name, ...) typedef ::TEvent<Owner, __VA_ARGS__> Name;
#define MULTI_EVENT_SIG(Owner, Name, ...) typedef ::TMulticastEvent<Owner, bool(__VA_ARGS__)> Name;

//# Declare an event. Only for jcxx objects.
#define EVENT_DECL(Name, ...) EVENT_DECL_VERBOSE(Derived, Name, __VA_ARGS__)
#define MULTI_EVENT_DECL(Name, ...) MULTI_EVENT_DECL_VERBOSE(Derived, Name, __VA_ARGS__)

#define EVENT_DECL_VERBOSE(Owner, Name, ...) ::TEvent<Owner, __VA_ARGS__> Name;
#define MULTI_EVENT_DECL_VERBOSE(Owner, Name, ...) ::TMulticastEvent<Owner, bool(__VA_ARGS__)> Name;
