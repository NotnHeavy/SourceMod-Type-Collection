//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_FLAME_POINT_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_FLAME_POINT_T

// Requires "tf_point_t.inc".
#include "tf_point_t.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// FLAME_POINT_T DATA                                                       //
//////////////////////////////////////////////////////////////////////////////

enum
{
	FLAME_POINT_T_OFFSET_M_VECATTACKERVEL = SIZEOF_TF_Point_t,                                      // Vector m_vecAttackerVel;
    FLAME_POINT_T_OFFSET_M_VECATTACKERPOS = FLAME_POINT_T_OFFSET_M_VECATTACKERVEL + SIZEOF_Vector,  // Vector m_vecAttackerPos;

    SIZEOF_Flame_Point_t = FLAME_POINT_T_OFFSET_M_VECATTACKERPOS + SIZEOF_Vector                    // sizeof(flame_point_t);
}

//////////////////////////////////////////////////////////////////////////////
// FLAME_POINT_T METHODMAP                                                  //
//////////////////////////////////////////////////////////////////////////////

// The actual name is flame_point_t.
methodmap Flame_Point_t < TF_Point_t
{
    // Constructor - wrap around an existing address to make way for a flame_point_t object.
    public Flame_Point_t(Address address)
    {
        return view_as<Flame_Point_t>(address);
    }

    // The stored attacker's velocity when this point was created.
    property Vector m_vecAttackerVel
    {
        public get() { return view_as<Vector>(this.Address + FLAME_POINT_T_OFFSET_M_VECATTACKERVEL); }
        public set(Vector value) { memcpy(this.Address + FLAME_POINT_T_OFFSET_M_VECATTACKERVEL, value, SIZEOF_Vector); }
    }

    // The stored attacker's position when this point was created.
    property Vector m_vecAttackerPos
    {
        public get() { return view_as<Vector>(this.Address + FLAME_POINT_T_OFFSET_M_VECATTACKERPOS); }
        public set(Vector value) { memcpy(this.Address + FLAME_POINT_T_OFFSET_M_VECATTACKERPOS, value, SIZEOF_Vector); }
    }

    // Call constructor on existing type.
    public void ctor()
    {
        view_as<TF_Point_t>(this).ctor();
        this.m_vecAttackerVel = vec3_origin;
        this.m_vecAttackerPos = vec3_origin;
    }

    // Constructor - allocate a new flame_point_t onto the heap. Deallocation will be required when you are finished using free().
    public static Flame_Point_t Malloc()
    {
        Flame_Point_t pointer = malloc(SIZEOF_Flame_Point_t);
        pointer.ctor();
        return pointer;
    }

    // Constructor - allocate a new flame_point_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_Flame_Point_t];
        view_as<Flame_Point_t>(AddressOfString(buffer)).ctor();
        return buffer;
    }
}