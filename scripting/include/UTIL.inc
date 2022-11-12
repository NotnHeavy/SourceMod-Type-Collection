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
// BSPFLAGS.H                                                               //
//////////////////////////////////////////////////////////////////////////////

#if !defined CONTENTS_EMPTY
// contents flags are seperate bits
// a given brush can contribute multiple content bits
// multiple brushes can be in a single leaf

// these definitions also need to be in q_shared.h!

// lower bits are stronger, and will eat weaker brushes completely
#define	CONTENTS_EMPTY			0		// No contents

#define	CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_BLOCKLOS		0x40	// block AI line of sight
#define CONTENTS_OPAQUE			0x80	// things that cannot be seen through (may be non-solid though)
#define	LAST_VISIBLE_CONTENTS	0x80

#define ALL_VISIBLE_CONTENTS (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define CONTENTS_TESTFOGVOLUME	0x100
#define CONTENTS_UNUSED			0x200	

// unused 
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
#define CONTENTS_UNUSED6		0x400

#define CONTENTS_TEAM1			0x800	// per team contents used to differentiate collisions 
#define CONTENTS_TEAM2			0x1000	// between players and objects on different teams

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE		0x4000

// remaining contents are non-visible, and don't eat brushes
#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000	// use accurate hitboxes on trace
#endif

#if !defined MASK_ALL
#define	MASK_ALL					        (0xFFFFFFFF)
// everything that is normally solid
#define	MASK_SOLID						(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// everything that blocks player movement
#define	MASK_PLAYERSOLID				        (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks npc movement
#define	MASK_NPCSOLID					        (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// water physics in these contents
#define	MASK_WATER						(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
// everything that blocks lighting
#define	MASK_OPAQUE						(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS			                (MASK_OPAQUE|CONTENTS_MONSTER)
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS					        (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS			                (MASK_BLOCKLOS|CONTENTS_MONSTER)
// everything that blocks line of sight for players
#define	MASK_VISIBLE					        (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS			                (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
// bullets see these as solid
#define	MASK_SHOT						(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL					        (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL				        (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY				        (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY			        (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY				        (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC					(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL				        (CONTENTS_WATER|CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT						(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define	MASK_DEADSOLID						(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)
#endif

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

static char CTraceFilter_GetTraceType[] = "\x33\xC0\xC3";
static char CTraceFilterEntitiesOnly_GetTraceType[] = "\xB8\x02\x00\x00\x00\xC3";
static char CTraceFilterWorldOnly_ShouldHitEntity[] = "\x32\xC0\xC2\x08\x00";
static char CTraceFilterWorldOnly_GetTraceType[] = "\xB8\x01\x00\x00\x00\xC3";
static char CTraceFilterWorldAndPropsOnly_ShouldHitEntity[] = "\x32\xC0\xC2\x08\x00";
static char CTraceFilterWorldAndPropsOnly_GetTraceType[] = "\x33\xC0\xC3";
static char CTraceFilterHitAll_ShouldHitEntity[] = "\xB0\x01\xC2\x08\x00";

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
    VTable.RegisterVPointer("CTraceFilter", 1, AddressOfString(CTraceFilter_GetTraceType));

    // class CTraceFilterEntitiesOnly : public ITraceFilter
    VTable.CreateVTable("CTraceFilterEntitiesOnly", 2, "ITraceFilter");
    VTable.RegisterVPointer("CTraceFilterEntitiesOnly", 1, AddressOfString(CTraceFilterEntitiesOnly_GetTraceType));

    // class CTraceFilterWorldOnly: public ITraceFilter
    VTable.CreateVTable("CTraceFilterWorldOnly", 2, "ITraceFilter");
    VTable.RegisterVPointer("CTraceFilterWorldOnly", 0, AddressOfString(CTraceFilterWorldOnly_ShouldHitEntity));
    VTable.RegisterVPointer("CTraceFilterWorldOnly", 1, AddressOfString(CTraceFilterWorldOnly_GetTraceType));

    // class CTraceFilterWorldAndPropsOnly : public ITraceFilter
    VTable.CreateVTable("CTraceFilterWorldAndPropsOnly", 2, "ITraceFilter");
    VTable.RegisterVPointer("CTraceFilterWorldAndPropsOnly", 0, AddressOfString(CTraceFilterWorldAndPropsOnly_ShouldHitEntity));
    VTable.RegisterVPointer("CTraceFilterWorldAndPropsOnly", 1, AddressOfString(CTraceFilterWorldAndPropsOnly_GetTraceType));

    // class CTraceFilterHitAll : public CTraceFilter
    VTable.CreateVTable("CTraceFilterHitAll", 2, "ITraceFilter");
    VTable.RegisterVPointer("CTraceFilterHitAll", 0, AddressOfString(CTraceFilterHitAll_ShouldHitEntity));
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
        char buffer[SIZEOF_ITraceFilter];
        view_as<ITraceFilter>(AddressOfString(buffer)).ctor();
        return buffer;
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
        char buffer[SIZEOF_CTraceFilter];
        view_as<CTraceFilter>(AddressOfString(buffer)).ctor();
        return buffer;
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
        char buffer[SIZEOF_CTraceFilterEntitiesOnly];
        view_as<CTraceFilterEntitiesOnly>(AddressOfString(buffer)).ctor();
        return buffer;
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
        char buffer[SIZEOF_CTraceFilterWorldOnly];
        view_as<CTraceFilterWorldOnly>(AddressOfString(buffer)).ctor();
        return buffer;
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
        char buffer[SIZEOF_CTraceFilterWorldAndPropsOnly];
        view_as<CTraceFilterWorldAndPropsOnly>(AddressOfString(buffer)).ctor();
        return buffer;
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
        char buffer[SIZEOF_CTraceFilterHitAll];
        view_as<CTraceFilterHitAll>(AddressOfString(buffer)).ctor();
        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////////
// UITL.INC GLOBALS                                                         //
//////////////////////////////////////////////////////////////////////////////

public void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, int mask, ITraceFilter& pFilter, TRACE_T& ptr)
{
    Ray_t ray = STACK_GETRETURN(Ray_t.StackAlloc());
    ray.Init(vecAbsStart, vecAbsEnd);
    SDKCall(SMTC_SDKCall_IEngineTrace_TraceRay, enginetrace, ray, mask, pFilter, ptr);
}