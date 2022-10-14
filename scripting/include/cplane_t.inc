//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// Requires "Vector.inc".

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CPLANE_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CPLANE_T

#include "Pointer.inc"
#include "Vector.inc"
#include "ctypes.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CPLANE_T DATA                                                            //
//////////////////////////////////////////////////////////////////////////////

// typedef unsigned char byte;
enum
{
    CPLANE_T_OFFSET_NORMAL = 0,                                    // Vector normal;
    CPLANE_T_OFFSET_DIST = CPLANE_T_OFFSET_NORMAL + SIZEOF_Vector, // float dist;
    CPLANE_T_OFFSET_TYPE = CPLANE_T_OFFSET_DIST + 4,               // byte type;
    CPLANE_T_OFFSET_SIGNBITS = CPLANE_T_OFFSET_TYPE + 1,           // byte signbits;
    CPLANE_T_OFFSET_PAD = CPLANE_T_OFFSET_SIGNBITS + 1,            // byte pad[2];

    SIZEOF_CPlane_t = CPLANE_T_OFFSET_PAD + 2                      // sizeof(cplane_t);
};

//////////////////////////////////////////////////////////////////////////////
// CPLANE_T METHODMAP                                                       //
//////////////////////////////////////////////////////////////////////////////

// what if you wanted to use "cplane_t", but god said "error 109: methodmap name must start with an uppercase letter"?
// A plane_t structure.
methodmap CPlane_t < Pointer
{
    // Constructor - wrap around an existing address to make way for a cplane_t object.
    public CPlane_t(Address address)
    {
        return view_as<CPlane_t>(address);
    }

    // Normal vector.
    property Vector normal
    {
        public get() { return view_as<Vector>(this.Address + CPLANE_T_OFFSET_NORMAL); }
        public set(Vector value) { memcpy(this.Address + CPLANE_T_OFFSET_NORMAL, value, SIZEOF_Vector); }
    }

    // Distance.
    property float dist
    {
        public get() { return this.Dereference(CPLANE_T_OFFSET_DIST); }
        public set(float value) { this.Write(value, CPLANE_T_OFFSET_DIST); }
    }

    // For fast side tests.
    property BYTE type
    {
        public get() { return this.Dereference(CPLANE_T_OFFSET_TYPE, NumberType_Int8); }
        public set(BYTE value) { this.Write(value, CPLANE_T_OFFSET_TYPE, NumberType_Int8); }
    }

    // signx + (signy << 1) + (signz << 1)
    property BYTE signbits
    {
        public get() { return this.Dereference(CPLANE_T_OFFSET_SIGNBITS, NumberType_Int8); }
        public set(BYTE value) { this.Write(value, CPLANE_T_OFFSET_SIGNBITS, NumberType_Int8); }
    }

    // Get byte pad.
    public BYTE pad_Get(BYTE index)
    {
        if (index < 0 || index >= 2)
            return BYTE(0);
        return this.Dereference(CPLANE_T_OFFSET_PAD + index, NumberType_Int8);
    }

    // Set byte pad.
    public void pad_Set(int index, BYTE value)
    {
        if (index < 0 || index >= 2)
            return;
        this.Write(value, CPLANE_T_OFFSET_PAD + index, NumberType_Int8);
    }

    // Call constructor on existing type.
    public void ctor(Vector normal = view_as<Vector>(NULL), float dist = 0.00, BYTE type = view_as<BYTE>(0), BYTE signbits = view_as<BYTE>(0), BYTE pad[2] = { view_as<BYTE>(0), view_as<BYTE>(0) })
    {
        this.normal = normal;
        this.dist = dist;
        this.type = type;
        this.signbits = signbits;
        memcpy(this.Address + CPLANE_T_OFFSET_PAD, AddressOfArray(pad), 2);
    }

    // Constructor - allocate a new cplane_t onto the heap. Deallocation will be required when you are finished using free().
    public static CPlane_t Malloc(Vector normal = view_as<Vector>(NULL), float dist = 0.00, BYTE type = view_as<BYTE>(0), BYTE signbits = view_as<BYTE>(0), BYTE pad[2] = { view_as<BYTE>(0), view_as<BYTE>(0) })
    {
        CPlane_t pointer = malloc(SIZEOF_CPlane_t);
        pointer.ctor(normal, dist, type, signbits, pad);
        return pointer;
    }
}