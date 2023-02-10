//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// Requires "csurface_t.inc" and "CBaseTrace.inc".

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CGAMETRACE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CGAMETRACE

#include "CBaseTrace.inc"
#include "csurface_t.inc"

#pragma semicolon true 
#pragma newdecls required

#define TRACE_T CGameTrace
#define SIZEOF_TRACE_T SIZEOF_CGameTrace

//////////////////////////////////////////////////////////////////////////////
// CGAMETRACE DATA                                                          //
//////////////////////////////////////////////////////////////////////////////

enum
{
    CGAMETRACE_OFFSET_FRACTIONLEFTSOLID = SIZEOF_CBaseTrace,                    // float fractionleftsolid;
    CGAMETRACE_OFFSET_SURFACE = CGAMETRACE_OFFSET_FRACTIONLEFTSOLID + 4,        // csurface_t surface;

    CGAMETRACE_OFFSET_HITGROUP = CGAMETRACE_OFFSET_SURFACE + SIZEOF_CSurface_t, // int hitgroup;
    CGAMETRACE_OFFSET_PHYSICSBONE = CGAMETRACE_OFFSET_HITGROUP + 4,             // short physicsbone;

    CGAMETRACE_OFFSET_M_PENT = CGAMETRACE_OFFSET_PHYSICSBONE + 4,               // CBaseEntity* m_pEnt;

    CGAMETRACE_OFFSET_HITBOX = CGAMETRACE_OFFSET_M_PENT + SIZEOF_Pointer,       // int hitbox;

    SIZEOF_CGameTrace = CGAMETRACE_OFFSET_HITBOX + 4,                           // sizeof(CGameTrace);
};

//////////////////////////////////////////////////////////////////////////////
// CGAMETRACE METHODMAP                                                     //
//////////////////////////////////////////////////////////////////////////////

// A trace that is returned when a box is swept through the world.
// CGameTrace is typically referred to by the typedef "trace_t" internally.
methodmap CGameTrace < CBaseTrace
{
    // Constructor - wrap around an existing address to make way for a CGameTrace object.
    public CGameTrace(Address address)
    {
        return view_as<CGameTrace>(address);
    }

    // time we left a solid, only valid if we started in solid
    property float fractionleftsolid
    {
        public get() { return this.Dereference(CGAMETRACE_OFFSET_FRACTIONLEFTSOLID); }
        public set(float value) { this.Write(value, CGAMETRACE_OFFSET_FRACTIONLEFTSOLID); }
    }

    // surface hit (impact surface)
    property CSurface_t surface
    {
        public get() { return view_as<CSurface_t>(this.Address + CGAMETRACE_OFFSET_SURFACE); }
        public set(CSurface_t value) { memcpy(this.Address + CGAMETRACE_OFFSET_SURFACE, value, SIZEOF_CSurface_t); } 
    }

    // 0 == generic, non-zero is specific body part
    property int hitgroup
    {
        public get() { return this.Dereference(CGAMETRACE_OFFSET_HITGROUP); }
        public set(int value) { this.Write(value, CGAMETRACE_OFFSET_HITGROUP); }
    }

    // physics bone hit by trace in studio
    // This member is a short.
    property int physicsbone
    {
        public get() { return this.Dereference(CGAMETRACE_OFFSET_PHYSICSBONE, NumberType_Int16); }
        public set(int value) { this.Write(value, CGAMETRACE_OFFSET_PHYSICSBONE, NumberType_Int16); } 
    }

    // Entity hit. Returns entity index on success, returns -1 on NULL.
    property CBaseEntity m_pEnt
    {
        public get() { return this.Dereference(CGAMETRACE_OFFSET_M_PENT); }
        public set(CBaseEntity value) { this.Write(value, CGAMETRACE_OFFSET_M_PENT); }
    }

    // Box hit by trace in studio.
    // NOTE: this member is overloaded.
	// If hEnt points at the world entity, then this is the static prop index.
	// Otherwise, this is the hitbox index.
    property int hitbox
    {
        public get() { return this.Dereference(CGAMETRACE_OFFSET_HITBOX); }
        public set(int value) { this.Write(value, CGAMETRACE_OFFSET_HITBOX); }
    }

    // Constructor - allocate a new CGameTrace onto the heap. Deallocation will be required when you are finished using free().
    public static CGameTrace Malloc()
    {
        return malloc(SIZEOF_CGameTrace);
    }

    // Constructor - allocate a new CGameTrace onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_CGameTrace] = { 0, ... };
        return buffer;
    }

    // Returns true if there was any kind of impact at all.
    public bool DidHit()
    {
        return this.fraction < 1.0 || this.allsolid || this.startsolid;
    }

    // Returns true if hEnt points at the world entity.
	// If this returns true, then you can't use GetHitBoxIndex().
    public bool DidHitWorld()
    {
        return this.m_pEnt == GetWorldEntity();
    }

    // Returns true if we hit something and it wasn't the world.
    public bool DidHitNonWorldEntity()
    {
        return this.m_pEnt != NULL && !this.DidHitWorld();
    }

    // Gets the entity's network index if the trace has hit an entity.
	// If not, returns -1.
    public int GetEntityIndex()
    {
        return this.m_pEnt.entindex();
    }
}