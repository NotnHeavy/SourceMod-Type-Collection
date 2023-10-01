//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_BURNED_ENTITY_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_BURNED_ENTITY_T

// Requires "CHandle.inc".
#include "CHandle.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// BURNED_ENTITY_T DATA                                                     //
//////////////////////////////////////////////////////////////////////////////

// WARNING!
// I have completely assumed the names for members that I have managed to make sense of,
// because this is completely reverse engineered without any other sources.
enum
{
    BURNED_ENTITY_T_OFFSET_M_HENTITY = 0,                                                   // EHANDLE m_hEntity;
    BURNED_ENTITY_T_OFFSET_V4 = BURNED_ENTITY_T_OFFSET_M_HENTITY + SIZEOF_CHandle,          // ?
    BURNED_ENTITY_T_OFFSET_V8 = BURNED_ENTITY_T_OFFSET_V4 + SIZEOF_int,                     // ?
    BURNED_ENTITY_T_OFFSET_M_FLLASTHIT = BURNED_ENTITY_T_OFFSET_V8 + SIZEOF_float,          // float m_flLastHit;
    BURNED_ENTITY_T_OFFSET_M_FLDENSITY = BURNED_ENTITY_T_OFFSET_M_FLLASTHIT + SIZEOF_float, // float m_flDensity;    

    SIZEOF_Burned_Entity_t = BURNED_ENTITY_T_OFFSET_M_FLDENSITY + 4                         // sizeof(burned_entity_t);
}

//////////////////////////////////////////////////////////////////////////////
// BURNED_ENTITY_T METHODMAP                                                //
//////////////////////////////////////////////////////////////////////////////

// REVERT IT!
// The actual class name is burned_entity_t.
methodmap Burned_Entity_t < Pointer
{
    // Constructor - wrap around an existing address to make way for a burned_entity_t object.
    public Burned_Entity_t(Address address)
    {
        return view_as<Burned_Entity_t>(address);
    }

    // The handle of the entity that this struct possesses.
    property CHandle m_hEntity
    {
        public get() { return view_as<any>(this) + BURNED_ENTITY_T_OFFSET_M_HENTITY; }
        public set(CHandle value) { memcmp(view_as<any>(this) + BURNED_ENTITY_T_OFFSET_M_HENTITY, value, SIZEOF_CHandle); }
    }

    // ?
    property int v4
    {
        public get() { return this.Dereference(BURNED_ENTITY_T_OFFSET_V4); }
        public set(int value) { this.Write(value, BURNED_ENTITY_T_OFFSET_V4); }
    }

    // ?
    property int v8
    {
        public get() { return this.Dereference(BURNED_ENTITY_T_OFFSET_V8); }
        public set(int value) { this.Write(value, BURNED_ENTITY_T_OFFSET_V8); }
    }

    // The time that this entity was last hit.
    property float m_flLastHit
    {
        public get() { return this.Dereference(BURNED_ENTITY_T_OFFSET_M_FLLASTHIT); }
        public set(float value) { this.Write(value, BURNED_ENTITY_T_OFFSET_M_FLLASTHIT); }
    }

    // Used as a multiplier for flame density while damaging this entity.
    property float m_flDensity
    {
        public get() { return this.Dereference(BURNED_ENTITY_T_OFFSET_M_FLLASTHIT); }
        public set(float value) { this.Write(value, BURNED_ENTITY_T_OFFSET_M_FLLASTHIT); }
    }

    // Constructor - allocate a new burned_entity_t onto the heap. Deallocation will be required when you are finished using this using free().
    public static Burned_Entity_t Malloc()
    {
        return malloc(SIZEOF_Burned_Entity_t);
    }

    // Constructor - allocate a new burned_entity_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_Burned_Entity_t] = { 0, ... };
        return buffer;
    }
}