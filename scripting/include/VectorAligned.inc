//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VECTORALIGNED
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VECTORALIGNED

#include "Pointer.inc"
#include "Vector.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// VECTORALIGNED DATA                                                       //
//////////////////////////////////////////////////////////////////////////////

enum
{
    VECTORALIGNED_OFFSET_W = SIZEOF_Vector,                      // float w; (this space is used anyway)

    SIZEOF_VectorAligned = VECTORALIGNED_OFFSET_W + SIZEOF_float // sizeof(VectorAligned);
}

//////////////////////////////////////////////////////////////////////////////
// VECTORALIGNED METHODMAP                                                  //
//////////////////////////////////////////////////////////////////////////////

// A 3 co-ordinate vector. This object is aligned to take up 16 bytes of memory, rather than a typical vector's 12 bytes.
methodmap VectorAligned < Vector
{
    // Constructor - wrap around an existing address to make way for a VectorAligned object.
    public VectorAligned(Address address)
    {
        return view_as<VectorAligned>(address);
    }

    // Constructor - allocate a new vector in the accumulator, a temporary storage location.
    public static VectorAligned Accumulator(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        return view_as<VectorAligned>(Vector.Accumulator(x, y, z));
    }

    // Call constructor on existing type.
    public void ctor(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        this.X = x;
        this.Y = y;
        this.Z = z;
    }

    // Constructor - allocate a new VectorAlligned onto the heap. Deallocation will be required when you are finished using this using free().
    public static VectorAligned Malloc(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        VectorAligned pointer = malloc(SIZEOF_VectorAligned);
        pointer.ctor(x, y, z);
        return pointer;
    }

    // Constructor - allocate a new VectorAlligned onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        char buffer[SIZEOF_VectorAligned];
        view_as<VectorAligned>(AddressOfString(buffer)).ctor(x, y, z);
        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////////
// VECTORALIGNED OPERATORS                                                  //
//////////////////////////////////////////////////////////////////////////////

stock VectorAligned operator-(const VectorAligned vector)
{
    return VectorAligned.Accumulator(-vector.X, -vector.Y, -vector.Z);
}

stock VectorAligned operator*(const VectorAligned left, const float right)
{
    return VectorAligned.Accumulator(left.X * right, left.Y * right, left.Z * right);
}

stock VectorAligned operator+(const VectorAligned left, const Vector right)
{
    return VectorAligned.Accumulator(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
}

stock VectorAligned operator-(const VectorAligned left, const Vector right)
{
    return VectorAligned.Accumulator(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
}

stock VectorAligned operator*(const VectorAligned left, const Vector right)
{
    return VectorAligned.Accumulator(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
}

stock VectorAligned operator/(const VectorAligned left, const Vector right)
{
    return VectorAligned.Accumulator(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
}

stock VectorAligned operator+(const VectorAligned left, const VectorAligned right)
{
    return VectorAligned.Accumulator(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
}

stock VectorAligned operator-(const VectorAligned left, const VectorAligned right)
{
    return VectorAligned.Accumulator(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
}

stock VectorAligned operator*(const VectorAligned left, const VectorAligned right)
{
    return VectorAligned.Accumulator(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
}

stock VectorAligned operator/(const VectorAligned left, const VectorAligned right)
{
    return VectorAligned.Accumulator(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
}

stock Vector operator+(const Vector left, const VectorAligned right)
{
    return Vector.Accumulator(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
}

stock Vector operator-(const Vector left, const VectorAligned right)
{
    return Vector.Accumulator(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
}

stock Vector operator*(const Vector left, const VectorAligned right)
{
    return Vector.Accumulator(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
}

stock Vector operator/(const Vector left, const VectorAligned right)
{
    return Vector.Accumulator(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
}