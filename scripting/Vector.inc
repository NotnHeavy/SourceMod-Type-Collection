//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VECTOR
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VECTOR

#include "Pointer.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// VECTOR DATA                                                              //
//////////////////////////////////////////////////////////////////////////////

enum vectorOffsets (+= 0x04)
{
    VECTOR_OFFSET_X,
    VECTOR_OFFSET_Y,
    VECTOR_OFFSET_Z,
    VECTOR_SIZE
}

static float accumulator[3];
static Address accumulatorAddress;

//////////////////////////////////////////////////////////////////////////////
// VECTOR METHODMAP                                                         //
//////////////////////////////////////////////////////////////////////////////

// A 3 co-ordinate vector. When assigning a result vector of an arithmetic operation to another, please do not directly use the result vector, as it is stored onto an accumulator buffer that is overwrited on next operation. Assign the result vector to another vector stored elsewhere.
methodmap Vector < Pointer
{
    // Constructor - wrap around an existing address to make way for a vector object.
    public Vector(Address address)
    {
        return view_as<Vector>(address);
    }

    // Constructor - allocate a new vector in the accumulator, a temporary storage location.
    public static Vector Accumulator(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        if (!accumulatorAddress)
            accumulatorAddress = AddressOfArray(accumulator);
        accumulator[0] = x;
        accumulator[1] = y;
        accumulator[2] = z;
        return Vector(accumulatorAddress);
    }

    // X co-ordinate.
    property float X
    {
        public get() { return this.Dereference(VECTOR_OFFSET_X); }
        public set(float x) { this.Write(x, VECTOR_OFFSET_X); }
    }

    // Y co-ordinate.
    property float Y
    {
        public get() { return this.Dereference(VECTOR_OFFSET_Y); }
        public set(float y) { this.Write(y, VECTOR_OFFSET_Y); }
    }
    
    // Z co-ordinate.
    property float Z
    {
        public get() { return this.Dereference(VECTOR_OFFSET_Z); }
        public set(float z) { this.Write(z, VECTOR_OFFSET_Z); }
    }

    // Call constructor on existing type.
    public void ctor(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        this.X = x;
        this.Y = y;
        this.Z = z;
    }

    // Constructor - allocate a new vector onto the heap. Deallocation will be required when you are finished using this using free().
    public static Vector Malloc(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        Vector pointer = malloc(VECTOR_SIZE);
        pointer.ctor();
        return pointer;
    }

    // Store the vector into a buffer.
    public void GetBuffer(float buffer[3])
    {
        memcpy(AddressOfArray(buffer), this, VECTOR_SIZE);
    }

    // Copy from a buffer into this vector.
    public void SetFromBuffer(float buffer[3])
    {
        memcpy(this, AddressOfArray(buffer), VECTOR_SIZE);
    }

    // Get the vector's magnitude squared.
    public float LengthSqr()
    {
        float buffer[3];
        this.GetBuffer(buffer);
        return this.X * this.X + this.Y * this.Y + this.Z + this.Z;
    }

    // Get the vector's magnitude.
    public float Length()
    {
        return SquareRoot(this.LengthSqr());
    }

    // Normalize a vector.
    public float NormalizeInPlace()
    {
        float magnitude = this.Length();
        this.X /= magnitude;
        this.Y /= magnitude;
        this.Z /= magnitude;
        return magnitude;
    }

    // Get the distance from this vector to the other one squared. The returned vector is stored onto the accumulator.
    public float DistToSqr(const Vector other)
    {
        return Vector.Accumulator(this.X - other.X, this.Y - other.Y, this.Z - other.Z).LengthSqr();
    }

    // Get the distance from this vector to the other one. The returned vector is stored onto the accumulator.
    public float DistTo(const Vector other)
    {
        return Vector.Accumulator(this.X - other.X, this.Y - other.Y, this.Z - other.Z).Length();
    }

    // Return the dot product between two vectors.
    public float Dot(const Vector other)
    {
        return this.X * other.X + this.Y * other.Y + this.Z * other.Z;
    }

    // Get the cross product between two vectors. The returned vector is stored onto the accumulator.
    public Vector Cross(const Vector other)
    {
        return Vector.Accumulator(this.Y * other.Z - this.Z * other.Y, this.Z * other.X - this.X * other.Z, this.X * other.Y - this.Y * other.X);
    }

    // Returns a vector with the min in X, Y and Z. The returned vector is stored onto the accumulator.
    public Vector Min(const Vector other)
    {
        return Vector.Accumulator(this.X < other.X ? this.X : other.X, this.Y < other.Y ? this.Y : other.Y, this.Z < other.Z ? this.Z : other.Z);
    }

    // Returns a vector with the max in X, Y and Z. The returned vector is stored onto the accumulator.
    public Vector Max(const Vector other)
    {
        return Vector.Accumulator(this.X > other.X ? this.X : other.X, this.Y > other.Y ? this.Y : other.Y, this.Z > other.Z ? this.Z : other.Z);
    }

    // Vector assignation (would be operator= instead if I was able to fully utilise it).
    public void Assign(Vector vector)
    {
        this.X = vector.X;
        this.Y = vector.Y;
        this.Z = vector.Z;
    }
}

//////////////////////////////////////////////////////////////////////////////
// VECTOR OPERATORS                                                         //
//////////////////////////////////////////////////////////////////////////////

stock Vector operator+(const Vector left, const Vector right)
{
    return Vector.Accumulator(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
}

stock Vector operator-(const Vector left, const Vector right)
{
    return Vector.Accumulator(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
}

stock Vector operator*(const Vector left, const Vector right)
{
    return Vector.Accumulator(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
}

stock Vector operator/(const Vector left, const Vector right)
{
    return Vector.Accumulator(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
}

stock Vector operator*(const Vector left, const float right)
{
    return Vector.Accumulator(left.X * right, left.Y * right, left.Z * right);
}

stock Vector operator/(const Vector left, const float right)
{
    return Vector.Accumulator(left.X / right, left.Y / right, left.Z / right);
}

stock Vector operator-(const Vector vector)
{
    return Vector.Accumulator(-vector.X, -vector.Y, -vector.Z);
}

//////////////////////////////////////////////////////////////////////////////
// VECTOR GLOBALS                                                           //
//////////////////////////////////////////////////////////////////////////////

void CrossProduct(const Vector a, const Vector b, Vector result)
{
    result.X = a.Y * b.Z - a.Z * b.Y;
    result.Y = a.Z * b.X - a.X * b.Z;
    result.Z = a.X * b.Y - a.Y * b.X;
}

void VectorVectors(const Vector forwardVector, Vector right, Vector up)
{
    if (forwardVector.X == 0.00 && forwardVector.Y == 0.00)
    {
        // pitch 90 degrees up/down from identity
        right.X = 0.00;
        right.Y = -1.00;
        right.Z = 0.00;
        up.X = -forwardVector.Z;
        up.Y = 0.00;
        up.Z = 0.00;
    }
    else
    {
        Vector temp = Vector.Malloc(0.00, 0.00, 1.00);
        CrossProduct(forwardVector, temp, right);
        right.NormalizeInPlace();
        CrossProduct(right, forwardVector, up);
        up.NormalizeInPlace();
        free(temp);
    }
}