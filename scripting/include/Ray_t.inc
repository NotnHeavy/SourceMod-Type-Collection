//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_RAY_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_RAY_T

#include "VectorAligned.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// RAY_T DATA                                                               //
//////////////////////////////////////////////////////////////////////////////

enum
{
    RAY_T_OFFSET_M_START = 0,                                                   // VectorAligned m_Start; (starting point, centered within the extents)
    RAY_T_OFFSET_M_DELTA = RAY_T_OFFSET_M_START + SIZEOF_VectorAligned,         // VectorAligned m_Delta; (direction + length of the ray)
    RAY_T_OFFSET_M_STARTOFFSET = RAY_T_OFFSET_M_DELTA + SIZEOF_VectorAligned,   // VectorAligned m_StartOffset; (Add this to m_Start to get the actual ray start)
    RAY_T_OFFSET_M_EXTENTS = RAY_T_OFFSET_M_STARTOFFSET + SIZEOF_VectorAligned, // VectorAligned m_Extents; (Describes an axis aligned box etruded along a ray)
    RAY_T_OFFSET_M_ISRAY = RAY_T_OFFSET_M_EXTENTS + SIZEOF_VectorAligned,       // bool m_IsRay; (are the extents zero?)
    RAY_T_OFFSET_M_ISSWEPT = RAY_T_OFFSET_M_ISRAY + SIZEOF_bool,                // bool m_IsSwept; (is delta != 0?)

    SIZEOF_Ray_t = RAY_T_OFFSET_M_ISSWEPT + SIZEOF_bool * 3,                    // sizeof(Ray_t);
}

//////////////////////////////////////////////////////////////////////////////
// RAY_T METHODMAP                                                          //
//////////////////////////////////////////////////////////////////////////////

// A direction vector of a starting and ending point.
methodmap Ray_t < Pointer
{
    // Constructor - wrap around an existing address to make way for a Ray_t object.
    public Ray_t(Address address)
    {
        return view_as<Ray_t>(address);
    }

    // starting point, centered within the extents
    property VectorAligned m_Start
    {
        public get() { return view_as<VectorAligned>(this.Address + RAY_T_OFFSET_M_START); }
        public set (VectorAligned value) { memcpy(this.Address + RAY_T_OFFSET_M_START, value, SIZEOF_VectorAligned); }
    }

    // direction + length of the ray
    property VectorAligned m_Delta
    {
        public get() { return view_as<VectorAligned>(this.Address + RAY_T_OFFSET_M_DELTA); }
        public set (VectorAligned value) { memcpy(this.Address + RAY_T_OFFSET_M_DELTA, value, SIZEOF_VectorAligned); }
    }

    // Add this to m_Start to get the actual ray start
    property VectorAligned m_StartOffset
    {
        public get() { return view_as<VectorAligned>(this.Address + RAY_T_OFFSET_M_STARTOFFSET); }
        public set (VectorAligned value) { memcpy(this.Address + RAY_T_OFFSET_M_STARTOFFSET, value, SIZEOF_VectorAligned); }
    }

    // Describes an axis aligned box extruded along a ray
    property VectorAligned m_Extents
    {
        public get() { return view_as<VectorAligned>(this.Address + RAY_T_OFFSET_M_EXTENTS); }
        public set (VectorAligned value) { memcpy(this.Address + RAY_T_OFFSET_M_EXTENTS, value, SIZEOF_VectorAligned); }
    }

    // are the extents zero?
    property bool m_IsRay
    {
        public get() { return this.Dereference(RAY_T_OFFSET_M_ISRAY, NumberType_Int8); }
        public set(bool value) { this.Write(value, RAY_T_OFFSET_M_ISRAY, NumberType_Int8); }
    }

    // is delta != 0?
    property bool m_IsSwept
    {
        public get() { return this.Dereference(RAY_T_OFFSET_M_ISSWEPT, NumberType_Int8); }
        public set(bool value) { this.Write(value, RAY_T_OFFSET_M_ISSWEPT, NumberType_Int8); }
    }

    // Constructor - allocate a new Ray_t onto the heap. Deallocation will be required when you are finished using this using free().
    public static Ray_t Malloc()
    {
        return malloc(SIZEOF(Ray_t));
    }

    // Constructor - allocate a new Ray_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_Ray_t] = { 0, ... };
        return buffer;
    }

    // Initialize this ray with just the start and end.
    public void Init(const Vector start, const Vector end)
    {
        VectorSubtract(end, start, this.m_Delta);
        
        this.m_IsSwept = this.m_Delta.LengthSqr() != 0;
        
        VectorClear(this.m_Extents);
        this.m_IsRay = true;

        // Offset m_Start to be in the center of the box...
        VectorClear(this.m_StartOffset);
        VectorCopy(start, this.m_Start);
    }

    // Initialize the ray with the start/end and mins/maxs.
    public void InitHull(const Vector start, const Vector end, const Vector mins, const Vector maxs)
    {
        VectorSubtract(end, start, this.m_Delta);
        
        this.m_IsSwept = this.m_Delta.LengthSqr() != 0;

        VectorSubtract(maxs, mins, this.m_Extents);
        this.m_Extents *= 0.5;
        this.m_IsRay = this.m_Extents.LengthSqr() < 0.000001; // 1e-6

        // Offset m_Start to be in the center of the box...
        VectorAdd(mins, maxs, this.m_StartOffset);
        this.m_StartOffset *= 0.5;
        VectorAdd(start, this.m_StartOffset, this.m_Start);
        this.m_StartOffset *= -1.0;
    }

    // compute inverse delta
    // This vector is stored onto the accumulator and must be assigned to your desired vector, unless allocate is set to true.
    public Vector InvDelta(bool allocate = false)
    {
        Vector vecInvDelta = STACK_GETRETURN(Vector.StackAlloc());
        for (int iAxis = 0; iAxis < 3; ++iAxis)
        {
            int delta = this.m_Delta.Dereference(iAxis * SIZEOF(int));
            if (delta != 0.00)
                vecInvDelta.Write(1.0 / delta, iAxis * SIZEOF(int));
            else
                vecInvDelta.Write(FLT_MAX, iAxis * SIZEOF(int));
        }
        return allocate ? Vector.Malloc(vecInvDelta.X, vecInvDelta.Y, vecInvDelta.Z) : Vector.Accumulator(vecInvDelta.X, vecInvDelta.Y, vecInvDelta.Z);
    }
}