//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_UTIL
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_UTIL

#include "Pointer.inc"
#include "Ray_t.inc"
#include "vtable.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// UITL.INC DATA                                                            //
//////////////////////////////////////////////////////////////////////////////

// The standard trace filter... NOTE: Most normal traces inherit from CTraceFilter!!!
enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

enum
{
    ITRACEFILTER_OFFSET_VTABLE = 0,                                    // void* vtable;

    SIZEOF_ITraceFilter = ITRACEFILTER_OFFSET_VTABLE + SIZEOF_Pointer, // sizeof(ITraceFilter);
    SIZEOF_CTraceFilter = SIZEOF_ITraceFilter,                         // sizeof(CTraceFilter);
    SIZEOF_CTraceFilterEntitiesOnly = SIZEOF_ITraceFilter,             // sizeof(CTraceFilterEntitiesOnly);
    SIZEOF_CTraceFilterWorldOnly = SIZEOF_ITraceFilter,                // sizeof(CTraceFilterWorldOnly);
    SIZEOF_CTraceFilterWorldAndPropsOnly = SIZEOF_ITraceFilter,        // sizeof(CTraceFilterWorldAndPropsOnly);
    SIZEOF_CTraceFilterHitAll = SIZEOF_ITraceFilter                    // sizeof(CTraceFilterHitAll);
}

static char windows_CTraceFilter_GetTraceType[] = "\x33\xC0\xC3";
static char windows_CTraceFilterEntitiesOnly_GetTraceType[] = "\xB8\x02\x00\x00\x00\xC3";
static char windows_CTraceFilterWorldOnly_ShouldHitEntity[] = "\x32\xC0\xC2\x08\x00";
static char windows_CTraceFilterWorldOnly_GetTraceType[] = "\xB8\x01\x00\x00\x00\xC3";
static char windows_CTraceFilterWorldAndPropsOnly_ShouldHitEntity[] = "\x32\xC0\xC2\x08\x00";
static char windows_CTraceFilterWorldAndPropsOnly_GetTraceType[] = "\x33\xC0\xC3";
static char windows_CTraceFilterHitAll_ShouldHitEntity[] = "\xB0\x01\xC2\x08\x00";

static char linux_CTraceFilter_GetTraceType[] = "\x55\x31\xC0\x89\xE5\x5D\xC3"; // @_ZNK12CTraceFilter12GetTraceTypeEv
static char linux_CTraceFilterEntitiesOnly_GetTraceType[] = "\x55\xB8\x02\x00\x00\x00\x89\xE5\x5D\xC3"; // @_ZNK24CTraceFilterEntitiesOnly12GetTraceTypeEv
static char linux_CTraceFilterWorldOnly_ShouldHitEntity[] = "\x55\x31\xC0\x5D\xC3";
static char linux_CTraceFilterWorldOnly_GetTraceType[] = "\x55\xB8\x01\x00\x00\x00\x89\xE5\x5D\xC3";
static char linux_CTraceFilterWorldAndPropsOnly_ShouldHitEntity[] = "\x55\x31\xC0\x5D\xC3";
static char linux_CTraceFilterWorldAndPropsOnly_GetTraceType[] = "\x55\x31\xC0\x89\xE5\x5D\xC3";
static char linux_CTraceFilterHitAll_ShouldHitEntity[] = "\x55\xB8\x01\x00\x00\x00\x89\xE5\x5D\xC3";

//////////////////////////////////////////////////////////////////////////////
// SMTC_UTILForward()                                                       //
//////////////////////////////////////////////////////////////////////////////

public void SMTC_UTILForward()
{
    // i don't fucking know what i'm doing anymore

    /*
    Addie — Today at 17:13
    > that seriously seems like over-engineering
    that's basically NotnHeavy's code in a nutshell every time
    */

    // abstract_class ITraceFilter
    VTable.CreateVTable("ITraceFilter", 2);
    VTable.RegisterVPointer("ITraceFilter", 0, NULL); // virtual bool ShouldHitEntity(IHandleEntity* pEntity, int contentsMask) = 0;
    VTable.RegisterVPointer("ITraceFilter", 1, NULL); // virtual TraceType_t GetTraceType() const = 0;

    // class CTraceFilter : public ITraceFilter
    VTable.CreateVTable("CTraceFilter", 2, "ITraceFilter");
    VTable.RegisterVPointer("CTraceFilter", 1, AddressOfString(SMTC.GetOperatingSystem() == OSTYPE_WINDOWS ? windows_CTraceFilter_GetTraceType : linux_CTraceFilter_GetTraceType));

    // class CTraceFilterEntitiesOnly : public ITraceFilter
    VTable.CreateVTable("CTraceFilterEntitiesOnly", 2, "ITraceFilter");
    VTable.RegisterVPointer("CTraceFilterEntitiesOnly", 1, AddressOfString(SMTC.GetOperatingSystem() == OSTYPE_WINDOWS ? windows_CTraceFilterEntitiesOnly_GetTraceType : linux_CTraceFilterEntitiesOnly_GetTraceType));

    // class CTraceFilterWorldOnly: public ITraceFilter
    VTable.CreateVTable("CTraceFilterWorldOnly", 2, "ITraceFilter");
    VTable.RegisterVPointer("CTraceFilterWorldOnly", 0, AddressOfString(SMTC.GetOperatingSystem() == OSTYPE_WINDOWS ? windows_CTraceFilterWorldOnly_ShouldHitEntity : linux_CTraceFilterWorldOnly_ShouldHitEntity));
    VTable.RegisterVPointer("CTraceFilterWorldOnly", 1, AddressOfString(SMTC.GetOperatingSystem() == OSTYPE_WINDOWS ? windows_CTraceFilterWorldOnly_GetTraceType : linux_CTraceFilterWorldOnly_GetTraceType));

    // class CTraceFilterWorldAndPropsOnly : public ITraceFilter
    VTable.CreateVTable("CTraceFilterWorldAndPropsOnly", 2, "ITraceFilter");
    VTable.RegisterVPointer("CTraceFilterWorldAndPropsOnly", 0, AddressOfString(SMTC.GetOperatingSystem() == OSTYPE_WINDOWS ? windows_CTraceFilterWorldAndPropsOnly_ShouldHitEntity : linux_CTraceFilterWorldAndPropsOnly_ShouldHitEntity));
    VTable.RegisterVPointer("CTraceFilterWorldAndPropsOnly", 1, AddressOfString(SMTC.GetOperatingSystem() == OSTYPE_WINDOWS ? windows_CTraceFilterWorldAndPropsOnly_GetTraceType : linux_CTraceFilterWorldAndPropsOnly_GetTraceType));

    // class CTraceFilterHitAll : public CTraceFilter
    VTable.CreateVTable("CTraceFilterHitAll", 2, "ITraceFilter");
    VTable.RegisterVPointer("CTraceFilterHitAll", 0, AddressOfString(SMTC.GetOperatingSystem() == OSTYPE_WINDOWS ? windows_CTraceFilterHitAll_ShouldHitEntity : linux_CTraceFilterHitAll_ShouldHitEntity));
}

//////////////////////////////////////////////////////////////////////////////
// ITraceFilter methodmap                                                   //
//////////////////////////////////////////////////////////////////////////////

// This is the base object for all the trace filters, which determines what to consider within a trace.
methodmap ITraceFilter < Pointer
{
    // Constructor - wrap around an existing address to make way for an ITraceFilter object.
    public ITraceFilter(Address address)
    {
        return view_as<ITraceFilter>(address);
    }

    // Call constructor on existing type.
    public void ctor()
    {
        VTable.HookOntoObject("ITraceFilter", this.Address);
    }

    // Constructor - allocate a new ITraceFilter onto the heap. Deallocation will be required when you are finished using this using free().
    public static ITraceFilter Malloc()
    {
        ITraceFilter pointer = malloc(SIZEOF_ITraceFilter);
        pointer.ctor();
        return pointer;
    }
    
    // Constructor - allocate a new ITraceFilter onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        STACK_ALLOC(pointer, ITraceFilter, SIZEOF_ITraceFilter);
        pointer.ctor();
        STACK_RETURN(pointer);
    }
}

//////////////////////////////////////////////////////////////////////////////
// CTraceFilter methodmap                                                   //
//////////////////////////////////////////////////////////////////////////////

// This trace filter considers everything in the game. This is intended to be derived from.
methodmap CTraceFilter < ITraceFilter
{
    // Constructor - wrap around an existing address to make way for an CTraceFilter object.
    public CTraceFilter(Address address)
    {
        return view_as<CTraceFilter>(address);
    }

    // Call constructor on existing type.
    public void ctor()
    {
        VTable.HookOntoObject("CTraceFilter", this.Address);
    }

    // Constructor - allocate a new CTraceFilter onto the heap. Deallocation will be required when you are finished using this using free().
    public static CTraceFilter Malloc()
    {
        CTraceFilter pointer = malloc(SIZEOF_CTraceFilter);
        pointer.ctor();
        return pointer;
    }

    // Constructor - allocate a new CTraceFilter onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        STACK_ALLOC(pointer, CTraceFilter, SIZEOF_CTraceFilter);
        pointer.ctor();
        STACK_RETURN(pointer);
    }
}

//////////////////////////////////////////////////////////////////////////////
// CTraceFilterEntitiesOnly methodmap                                       //
//////////////////////////////////////////////////////////////////////////////

// This trace filter considers entities only. This is intended to be derived from.
methodmap CTraceFilterEntitiesOnly < ITraceFilter
{
    // Constructor - wrap around an existing address to make way for an CTraceFilterEntitiesOnly object.
    public CTraceFilterEntitiesOnly(Address address)
    {
        return view_as<CTraceFilterEntitiesOnly>(address);
    }

    // Call constructor on existing type.
    public void ctor()
    {
        VTable.HookOntoObject("CTraceFilterEntitiesOnly", this.Address);
    }

    // Constructor - allocate a new CTraceFilterEntitiesOnly onto the heap. Deallocation will be required when you are finished using this using free().
    public static CTraceFilterEntitiesOnly Malloc()
    {
        CTraceFilterEntitiesOnly pointer = malloc(SIZEOF_CTraceFilterEntitiesOnly);
        pointer.ctor();
        return pointer;
    }

    // Constructor - allocate a new CTraceFilterEntitiesOnly onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        STACK_ALLOC(pointer, CTraceFilterEntitiesOnly, SIZEOF_CTraceFilterEntitiesOnly);
        pointer.ctor();
        STACK_RETURN(pointer);
    }
}

//////////////////////////////////////////////////////////////////////////////
// CTraceFilterWorldOnly methodmap                                          //
//////////////////////////////////////////////////////////////////////////////

// This trace filter considers the world entity only. This is not intended to be derived from.
methodmap CTraceFilterWorldOnly < ITraceFilter
{
    // Constructor - wrap around an existing address to make way for an CTraceFilterWorldOnly object.
    public CTraceFilterWorldOnly(Address address)
    {
        return view_as<CTraceFilterWorldOnly>(address);
    }

    // Call constructor on existing type.
    public void ctor()
    {
        VTable.HookOntoObject("CTraceFilterWorldOnly", this.Address);
    }

    // Constructor - allocate a new CTraceFilterWorldOnly onto the heap. Deallocation will be required when you are finished using this using free().
    public static CTraceFilterWorldOnly Malloc()
    {
        CTraceFilterWorldOnly pointer = malloc(SIZEOF_CTraceFilterWorldOnly);
        pointer.ctor();
        return pointer;
    }

    // Constructor - allocate a new CTraceFilterWorldOnly onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        STACK_ALLOC(pointer, CTraceFilterWorldOnly, SIZEOF_CTraceFilterWorldOnly);
        pointer.ctor();
        STACK_RETURN(pointer);
    }
}

//////////////////////////////////////////////////////////////////////////////
// CTraceFilterWorldAndPropsOnly methodmap                                  //
//////////////////////////////////////////////////////////////////////////////

// This trace filter considers the world entity and props only. This is not intended to be derived from.
methodmap CTraceFilterWorldAndPropsOnly < ITraceFilter
{
    // Constructor - wrap around an existing address to make way for an CTraceFilterWorldAndPropsOnly object.
    public CTraceFilterWorldAndPropsOnly(Address address)
    {
        return view_as<CTraceFilterWorldAndPropsOnly>(address);
    }

    // Call constructor on existing type.
    public void ctor()
    {
        VTable.HookOntoObject("CTraceFilterWorldAndPropsOnly", this.Address);
    }

    // Constructor - allocate a new CTraceFilterWorldAndPropsOnly onto the heap. Deallocation will be required when you are finished using this using free().
    public static CTraceFilterWorldAndPropsOnly Malloc()
    {
        CTraceFilterWorldAndPropsOnly pointer = malloc(SIZEOF_CTraceFilterWorldAndPropsOnly);
        pointer.ctor();
        return pointer;
    }

    // Constructor - allocate a new CTraceFilterWorldAndPropsOnly onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        STACK_ALLOC(pointer, CTraceFilterWorldAndPropsOnly, SIZEOF_CTraceFilterWorldAndPropsOnly);
        pointer.ctor();
        STACK_RETURN(pointer);
    }
}

//////////////////////////////////////////////////////////////////////////////
// CTraceFilterHitAll methodmap                                             //
//////////////////////////////////////////////////////////////////////////////

// This trace filter considers everything. This is not intended to be derived from.
methodmap CTraceFilterHitAll < ITraceFilter
{
    // Constructor - wrap around an existing address to make way for an CTraceFilterHitAll object.
    public CTraceFilterHitAll(Address address)
    {
        return view_as<CTraceFilterHitAll>(address);
    }

    // Call constructor on existing type.
    public void ctor()
    {
        VTable.HookOntoObject("CTraceFilterHitAll", this.Address);
    }

    // Constructor - allocate a new CTraceFilterHitAll onto the heap. Deallocation will be required when you are finished using this using free().
    public static CTraceFilterHitAll Malloc()
    {
        CTraceFilterHitAll pointer = malloc(SIZEOF_CTraceFilterHitAll);
        pointer.ctor();
        return pointer;
    }

    // Constructor - allocate a new CTraceFilterHitAll onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        STACK_ALLOC(pointer, CTraceFilterHitAll, SIZEOF_CTraceFilterHitAll);
        pointer.ctor();
        STACK_RETURN(pointer);
    }
}

//////////////////////////////////////////////////////////////////////////////
// UITL.INC GLOBALS                                                         //
//////////////////////////////////////////////////////////////////////////////

public void UTIL_TraceLine(const Vector vecAbsStart, const Vector vecAbsEnd, int mask, ITraceFilter pFilter, TRACE_T& ptr)
{
    Ray_t ray = STACK_GETRETURN(Ray_t, Ray_t.StackAlloc());
    ray.Init(vecAbsStart, vecAbsEnd);
}