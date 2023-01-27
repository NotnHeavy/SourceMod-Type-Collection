//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// Requires "Vector.inc".

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CPLANE_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CPLANE_T

#include "Vector.inc"

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
    // This member is an unsigned char.
    property int type
    {
        public get() { return this.Dereference(CPLANE_T_OFFSET_TYPE, NumberType_Int8); }
        public set(int value) { this.Write(value, CPLANE_T_OFFSET_TYPE, NumberType_Int8); }
    }

    // signx + (signy << 1) + (signz << 1)
    property int signbits
    {
        public get() { return this.Dereference(CPLANE_T_OFFSET_SIGNBITS, NumberType_Int8); }
        public set(int value) { this.Write(value, CPLANE_T_OFFSET_SIGNBITS, NumberType_Int8); }
    }

    // Get byte pad.
    // This array uses unsigned chars.
    // Returns 0 if invalid index, otherwise returns 1 and fills value parameter.
    public int pad_Get(int index, int& value)
    {
        if (index < 0 || index >= 2)
            return 0;
        value = this.Dereference(CPLANE_T_OFFSET_PAD + index, NumberType_Int8);
        return 1;
    }

    // Set byte pad.
    // This array uses unsigned chars.
    // Returns 0 if invalid index, else 1.
    public int pad_Set(int index, int value)
    {
        if (index < 0 || index >= 2)
            return 0;
        this.Write(value, CPLANE_T_OFFSET_PAD + index, NumberType_Int8);
        return 1;
    }

    // Call constructor on existing type.
    public void ctor(Vector normal = NULL, float dist = 0.00, int type = 0, int signbits = 0, int pad[2] = { 0, 0 })
    {
        this.normal = normal;
        this.dist = dist;
        this.type = type;
        this.signbits = signbits;
        for (int i = 0; i < 2; ++i)
            this.Write(pad[i], CPLANE_T_OFFSET_PAD + i, NumberType_Int8);
    }

    // Constructor - allocate a new cplane_t onto the heap. Deallocation will be required when you are finished using free().
    public static CPlane_t Malloc(Vector normal = NULL, float dist = 0.00, int type = 0, int signbits = 0, int pad[2] = { 0, 0 })
    {
        CPlane_t pointer = malloc(SIZEOF_CPlane_t);
        pointer.ctor(normal, dist, type, signbits, pad);
        return pointer;
    }

    // Constructor - allocate a new cplane_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(Vector normal = NULL, float dist = 0.00, int type = 0, int signbits = 0, int pad[2] = { 0, 0 })
    {
        char buffer[SIZEOF_CPlane_t];
        view_as<CPlane_t>(AddressOfString(buffer)).ctor(normal, dist, type, signbits, pad);
        return buffer;
    }
}