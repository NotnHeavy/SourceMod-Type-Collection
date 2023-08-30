//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_TF_POINT_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_TF_POINT_T

// Requires "Vector.inc".
#include "Vector.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// TF_POINT_T DATA                                                          //
//////////////////////////////////////////////////////////////////////////////

enum
{
	TF_POINT_T_OFFSET_VTABLE = 0,                                                       // void* vtable;
	TF_POINT_T_OFFSET_M_VECSTARTPOS = TF_POINT_T_OFFSET_VTABLE + 4,                     // Vector m_vecStartPos;
	TF_POINT_T_OFFSET_M_VECVELOCITY = TF_POINT_T_OFFSET_M_VECSTARTPOS + SIZEOF_Vector,  // Vector m_vecVelocity;
	TF_POINT_T_OFFSET_M_FLSPAWNTIME = TF_POINT_T_OFFSET_M_VECVELOCITY + SIZEOF_Vector,  // Vector m_flSpawnTime;
	TF_POINT_T_OFFSET_M_FLLIFETIME = TF_POINT_T_OFFSET_M_FLSPAWNTIME + 4,               // float m_flLifeTime;
	TF_POINT_T_OFFSET_M_IINDEX = TF_POINT_T_OFFSET_M_FLLIFETIME + 4,                    // int m_iIndex;
	TF_POINT_T_OFFSET_M_NTOUCHES = TF_POINT_T_OFFSET_M_IINDEX + 4,                      // int m_nTouches;
	TF_POINT_T_OFFSET_M_VECENDPOS = TF_POINT_T_OFFSET_M_NTOUCHES + 4,                   // Vector m_vecEndPos;

	SIZEOF_TF_Point_t = TF_POINT_T_OFFSET_M_VECENDPOS + SIZEOF_Vector                   // sizeof(tf_point_t);
}

//////////////////////////////////////////////////////////////////////////////
// TF_POINT_T METHODMAP                                                     //
//////////////////////////////////////////////////////////////////////////////

// where were you when sourcemod die
// i was at home writing methodmap when dvander ring
// "methodmap dont start with lowercase letter"
// "no"
//
// forgive me i am russia
//
// The actual name is tf_point_t.
methodmap TF_Point_t < Pointer
{
    // Constructor - wrap around an existing address to make way for a tf_point_t object.
    public TF_Point_t(Address address)
    {
        return view_as<TF_Point_t>(address);
    }

    // tf_point_t vtable.
    property Pointer vtable
    {
        public get() { return this.Dereference(TF_POINT_T_OFFSET_VTABLE); }
        public set(Pointer value) { this.Write(value, TF_POINT_T_OFFSET_VTABLE); }
    }

    // The starting position.
    property Vector m_vecStartPos
    {
        public get() { return view_as<Vector>(this.Address + TF_POINT_T_OFFSET_M_VECSTARTPOS); }
        public set(Vector value) { memcpy(this.Address + TF_POINT_T_OFFSET_M_VECSTARTPOS, value, SIZEOF_Vector); }
    }

    // The point velocity.
    property Vector m_vecVelocity
    {
        public get() { return view_as<Vector>(this.Address + TF_POINT_T_OFFSET_M_VECVELOCITY); }
        public set(Vector value) { memcpy(this.Address + TF_POINT_T_OFFSET_M_VECVELOCITY, value, SIZEOF_Vector); }
    }

    // When did this point spawn?
    property float m_flSpawnTime
    {
        public get() { return this.Dereference(TF_POINT_T_OFFSET_M_FLSPAWNTIME); }
        public set(float value) { this.Write(value, TF_POINT_T_OFFSET_M_FLSPAWNTIME); }
    }

    // How long does this point last?
    property float m_flLifeTime
    {
        public get() { return this.Dereference(TF_POINT_T_OFFSET_M_FLLIFETIME); }
        public set(float value) { this.Write(value, TF_POINT_T_OFFSET_M_FLLIFETIME); }
    }

    // Index in the CTFPointManager's m_Points.
    property int m_iIndex
    {
        public get() { return this.Dereference(TF_POINT_T_OFFSET_M_IINDEX); }
        public set(int value) { this.Write(value, TF_POINT_T_OFFSET_M_IINDEX); }
    }

    // How many times has this point touched a player?
    property int m_nTouches
    {
        public get() { return this.Dereference(TF_POINT_T_OFFSET_M_NTOUCHES); }
        public set(int value) { this.Write(value, TF_POINT_T_OFFSET_M_NTOUCHES); }
    }

    // The starting position.
    property Vector m_vecEndPos
    {
        public get() { return view_as<Vector>(this.Address + TF_POINT_T_OFFSET_M_VECENDPOS); }
        public set(Vector value) { memcpy(this.Address + TF_POINT_T_OFFSET_M_VECENDPOS, value, SIZEOF_Vector); }
    }

    // Call constructor on existing type.
    public void ctor()
    {
        this.vtable = SMTC_tf_point_t_vtable;
        this.m_vecStartPos = vec3_origin;
        this.m_vecVelocity = vec3_origin;
        this.m_flSpawnTime = 0.0;
        this.m_flLifeTime = 0.0;
        this.m_iIndex = 0;
        this.m_nTouches = 0;
        this.m_vecEndPos = vec3_origin;
    }

    // Constructor - allocate a new tf_point_t onto the heap. Deallocation will be required when you are finished using free().
    public static TF_Point_t Malloc()
    {
        TF_Point_t pointer = malloc(SIZEOF_TF_Point_t);
        pointer.ctor();
        return pointer;
    }

    // Constructor - allocate a new tf_point_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_TF_Point_t];
        view_as<TF_Point_t>(AddressOfString(buffer)).ctor();
        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////////
// HOOKS                                                                    //
//////////////////////////////////////////////////////////////////////////////

// Post-call CTFPointManager::AddPoint().
// If a new point has been instantiated, call:
// "void SMTC_AddPoint(const CBaseEntity manager, const TF_Point_t point)".
MRESReturn SMTC_CTFPointManager_AddPoint(int entity, DHookReturn returnValue, DHookParam parameters)
{
    CBaseEntity manager = CBaseEntity.FromIndex(entity);
    if (manager == NULL)
        return MRES_Ignored;

    CBaseEntity owner = manager.GetEntPropHandle(Prop_Send, "m_hAttacker");
    if (owner == NULL)
        return MRES_Ignored;

    CBaseEntity weapon = manager.GetEntPropHandle(Prop_Send, "m_hOwnerEntity");
    if (weapon == NULL)
        return MRES_Ignored;

    CUtlVector vector = CUtlVector(manager.Address + SMTC_CTFPointManager_m_Points);
    int index = manager.GetEntProp(Prop_Send, "m_unNextPointIndex");
    index = ((index == 0) ? (SDKCall(SMTC_SDKCall_CTFPointManager_GetMaxPoints, manager) - 1) : (index - 1));

    for (int i = 0, size = vector.m_Size; i < size; ++i)
    {
        TF_Point_t point = vector.Get(i).Dereference();
        if (point == NULL)
            continue;

        if (point.m_iIndex == index)
        {
            SMTC_HookData data;
            SMTC_GetHookData(entity, FORWARDTYPE_ADDPOINT, data);
            if (data.entity == INVALID_ENT_REFERENCE)
                return MRES_Ignored;

            Call_StartFunction(null, data.callback);
            Call_PushCell(manager);
            Call_PushCell(point);
            Call_Finish();
            break;
        }
    }

    return MRES_Ignored;
}