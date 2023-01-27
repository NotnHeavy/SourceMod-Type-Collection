//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// these members are aligned!! (ok ty valve xx)

// Requires "Vector.inc" and "cplane_t.inc".

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CBASETRACE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CBASETRACE

#include "Vector.inc"
#include "cplane_t.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CBASETRACE DATA                                                          //
//////////////////////////////////////////////////////////////////////////////

#if !defined DISPSURF_FLAG_SURFACE
#define DISPSURF_FLAG_SURFACE		(1<<0)
#define DISPSURF_FLAG_WALKABLE		(1<<1)
#define DISPSURF_FLAG_BUILDABLE		(1<<2)
#define DISPSURF_FLAG_SURFPROP1		(1<<3)
#define DISPSURF_FLAG_SURFPROP2		(1<<4)
#endif

enum
{
    CBASETRACE_OFFSET_STARTPOS = 0,                                         // Vector startpos;
    CBASETRACE_OFFSET_ENDPOS = CBASETRACE_OFFSET_STARTPOS + SIZEOF_Vector,  // Vector endpos;
    CBASETRACE_OFFSET_PLANE = CBASETRACE_OFFSET_ENDPOS + SIZEOF_Vector,     // cplane_t plane;

    CBASETRACE_OFFSET_FRACTION = CBASETRACE_OFFSET_PLANE + SIZEOF_CPlane_t, // float fraction;

    CBASETRACE_OFFSET_CONTENTS = CBASETRACE_OFFSET_FRACTION + 4,            // int contents;
    CBASETRACE_OFFSET_DISPFLAGS = CBASETRACE_OFFSET_CONTENTS + 4,           // unsigned short dispFlags;

    CBASETRACE_OFFSET_ALLSOLID = CBASETRACE_OFFSET_DISPFLAGS + 2,           // bool allsolid;
    CBASETRACE_OFFSET_STARTSOLID = CBASETRACE_OFFSET_ALLSOLID + 1,          // bool startsolid;

    SIZEOF_CBaseTrace = CBASETRACE_OFFSET_STARTSOLID + 1                    // sizeof(CBaseTrace);
};

//////////////////////////////////////////////////////////////////////////////
// CBASETRACE METHODMAP                                                     //
//////////////////////////////////////////////////////////////////////////////

// Base trace structure. Used behind the scenes for CGameTrace.
methodmap CBaseTrace < Pointer
{
    // Constructor - wrap around an existing address to make way for a CBaseTrace object.
    public CBaseTrace(Address address)
    {
        return view_as<CBaseTrace>(address);
    }

    // start position
    property Vector startpos
    {
        public get() { return view_as<Vector>(this.Address + CBASETRACE_OFFSET_STARTPOS); }
        public set(Vector value) { memcpy(this.Address + CBASETRACE_OFFSET_STARTPOS, value, SIZEOF_Vector); }
    }

    // final position
    property Vector endpos
    {
        public get() { return view_as<Vector>(this.Address + CBASETRACE_OFFSET_ENDPOS); }
        public set(Vector value) { memcpy(this.Address + CBASETRACE_OFFSET_ENDPOS, value, SIZEOF_Vector); }
    }

    // surface normal at impact
    property CPlane_t plane
    {
        public get() { return view_as<CPlane_t>(this.Address + CBASETRACE_OFFSET_PLANE); }
        public set(CPlane_t value) { memcpy(this.Address + CBASETRACE_OFFSET_PLANE, value, SIZEOF_CPlane_t); }
    }

    // time completed, 1.0 = didn't hit anything
    property float fraction
    {
        public get() { return this.Dereference(CBASETRACE_OFFSET_FRACTION); }
        public set(float value) { this.Write(value, CBASETRACE_OFFSET_FRACTION); }
    }

    // contents on other side of surface hit
    property int contents
    {
        public get() { return this.Dereference(CBASETRACE_OFFSET_CONTENTS); }
        public set(int value) { this.Write(value, CBASETRACE_OFFSET_CONTENTS); }
    }

    // displacement flags for marking surfaces with data
    // This member is an unsigned short.
    property int dispFlags
    {
        public get() { return this.Dereference(CBASETRACE_OFFSET_DISPFLAGS, NumberType_Int16); }
        public set(int value) { this.Write(value, CBASETRACE_OFFSET_DISPFLAGS, NumberType_Int16); }
    }

    // if true, plane is not valid
    property bool allsolid
    {
        public get() { return this.Dereference(CBASETRACE_OFFSET_ALLSOLID, NumberType_Int8); }
        public set(bool value) { this.Write(value, CBASETRACE_OFFSET_ALLSOLID, NumberType_Int8); }
    }

    // if true, the initial point was in a solid area
    property bool startsolid
    {
        public get() { return this.Dereference(CBASETRACE_OFFSET_STARTSOLID, NumberType_Int8); }
        public set(bool value) { this.Write(value, CBASETRACE_OFFSET_STARTSOLID, NumberType_Int8); }
    }

    // Constructor - allocate a new CBaseTrace onto the heap. Deallocation will be required when you are finished using free().
    public static CBaseTrace Malloc()
    {
        return malloc(SIZEOF_CBaseTrace);
    }

    // Constructor - allocate a new CBaseTrace onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_CBaseTrace] = { 0, ... };
        return buffer;
    }

    // Displacement flag tests.
    public bool IsDispSurface()
    {
        return this.dispFlags & DISPSURF_FLAG_SURFACE != 0;
    }
    public bool IsDispSurfaceWalkable()
    {
        return this.dispFlags & DISPSURF_FLAG_WALKABLE != 0;
    }
    public bool IsDispSurfaceBuildable()
    {
        return this.dispFlags & DISPSURF_FLAG_BUILDABLE != 0;
    }
    public bool IsDispSurfaceProp1()
    {
        return this.dispFlags & DISPSURF_FLAG_SURFPROP1 != 0;
    }
    public bool IsDispSurfaceProp2()
    {
        return this.dispFlags & DISPSURF_FLAG_SURFPROP2 != 0;
    }
}