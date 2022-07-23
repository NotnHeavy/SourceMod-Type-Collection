//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_QANGLE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_QANGLE

#include "Pointer.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// QANGLE DATA                                                              //
//////////////////////////////////////////////////////////////////////////////

enum
{
    QANGLE_OFFSET_X = 0,                   // float x;
    QANGLE_OFFSET_Y = QANGLE_OFFSET_X + 4, // float y;
    QANGLE_OFFSET_Z = QANGLE_OFFSET_Y + 4, // float z;
    
    QANGLE_SIZE = QANGLE_OFFSET_Z + 4      // sizeof(QAngle);
}

static float accumulator[3];
static Address accumulatorAddress;

//////////////////////////////////////////////////////////////////////////////
// QANGLE METHODMAP                                                         //
//////////////////////////////////////////////////////////////////////////////

// A 3 co-ordinate vector representing pitch, yaw and roll. 
// When assigning a result QAngle of an arithmetic operation to another, please do not directly use the result QAngle, as it is stored onto an accumulator buffer that is overwrited on next operation. Assign the result vector to another QAngle stored elsewhere.
methodmap QAngle < Pointer
{
    // Constructor - wrap around an existing address to make way for a QAngle object.
    public QAngle(Address address)
    {
        return view_as<QAngle>(address);
    }

    // Constructor - allocate a new QAngle in the accumulator, a temporary storage location.
    public static QAngle Accumulator(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        if (!accumulatorAddress)
            accumulatorAddress = AddressOfArray(accumulator);
        accumulator[0] = x;
        accumulator[1] = y;
        accumulator[2] = z;
        return QAngle(accumulatorAddress);
    }

    // X co-ordinate.
    property float X
    {
        public get() { return this.Dereference(QANGLE_OFFSET_X); }
        public set(float x) { this.Write(x, QANGLE_OFFSET_X); }
    }

    // Y co-ordinate.
    property float Y
    {
        public get() { return this.Dereference(QANGLE_OFFSET_Y); }
        public set(float y) { this.Write(y, QANGLE_OFFSET_Y); }
    }
    
    // Z co-ordinate.
    property float Z
    {
        public get() { return this.Dereference(QANGLE_OFFSET_Z); }
        public set(float z) { this.Write(z, QANGLE_OFFSET_Z); }
    }

    // Call constructor on existing type.
    public void ctor(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        this.X = x;
        this.Y = y;
        this.Z = z;
    }

    // Constructor - allocate a new QAngle onto the heap. Deallocation will be required when you are finished using this using free().
    public static QAngle Malloc(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        QAngle pointer = malloc(QANGLE_SIZE);
        pointer.ctor(x, y, z);
        return pointer;
    }

    // Base address...
    public Pointer Base()
    {
        return view_as<Pointer>(this);
    }

    // Store the QAngle into a buffer.
    public void GetBuffer(float buffer[3])
    {
        memcpy(AddressOfArray(buffer), this, QANGLE_SIZE);
    }

    // Copy from a buffer into this QAngle.
    public void SetFromBuffer(float buffer[3])
    {
        memcpy(this, AddressOfArray(buffer), QANGLE_SIZE);
    }

    // Get the QAngle's magnitude squared.
    public float LengthSqr()
    {
        return this.X * this.X + this.Y * this.Y + this.Z * this.Z;
    }

    // Get the QAngle's magnitude.
    public float Length()
    {
        return SquareRoot(this.LengthSqr());
    }

    // Vector assignation.
    public void Assign(QAngle vector)
    {
        memcpy(this, vector, VECTOR_SIZE);
    }
}

//////////////////////////////////////////////////////////////////////////////
// QANGLE OPERATORS                                                         //
//////////////////////////////////////////////////////////////////////////////

stock QAngle operator+(const QAngle left, const QAngle right)
{
    return QAngle.Accumulator(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
}

stock QAngle operator-(const QAngle left, const QAngle right)
{
    return QAngle.Accumulator(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
}

stock QAngle operator*(const QAngle left, const QAngle right)
{
    return QAngle.Accumulator(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
}

stock QAngle operator/(const QAngle left, const QAngle right)
{
    return QAngle.Accumulator(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
}

//////////////////////////////////////////////////////////////////////////////
// QANGLE GLOBALS                                                           //
//////////////////////////////////////////////////////////////////////////////

stock void VectorAdd(const QAngle a, const QAngle b, QAngle result)
{
    result.X = a.X + b.X;
    result.Y = a.Y + b.Y;
    result.Z = a.Z + b.Z;
}

stock void VectorMA(const QAngle start, float scale, const QAngle direction, QAngle dest)
{
    dest.X = start.X + scale * direction.X;
    dest.Y = start.Y + scale * direction.Y;
    dest.Z = start.Z + scale * direction.Z;
}

// Vector equality with tolerance
stock bool QAnglesAreEqual(const QAngle left, const QAngle right, float tolerance = 0.00)
{
    if (FloatMakePositive(left.X - right.X) > tolerance)
        return false;
    if (FloatMakePositive(left.Y - right.Y) > tolerance)
        return false;
    return FloatMakePositive(left.Z - right.Z) <= tolerance;
}

// Euler QAngle -> Basis Vectors. Each vector is optional.
stock void AngleVectors(const QAngle angles, Vector forwardVector = view_as<Vector>(NULL), Vector right = view_as<Vector>(NULL), Vector up = view_as<Vector>(NULL))
{
    // bit of a cheap implementation but i'm not really a maths wizard (yet).
    float anglesBuffer[3];
    float forwardBuffer[3];
    float rightBuffer[3];
    float upBuffer[3];

    angles.GetBuffer(anglesBuffer);
    if (forwardVector)
        forwardVector.GetBuffer(forwardBuffer);
    if (right)
        right.GetBuffer(rightBuffer);
    if (up)
        up.GetBuffer(upBuffer);

    GetAngleVectors(anglesBuffer, forwardVector ? forwardBuffer : NULL_VECTOR, right ? rightBuffer : NULL_VECTOR, up ? upBuffer : NULL_VECTOR);

    if (forwardVector)
        forwardVector.SetFromBuffer(forwardBuffer);
    if (right)
        right.SetFromBuffer(rightBuffer);
    if (up)
        up.SetFromBuffer(upBuffer);
}

// The QAngle returned is stored onto the accumulator and must be assigned to your desired vector, unless allocate is set to true.
stock QAngle GetEntQAngle(int entity, PropType type, const char[] property, int index = 0, bool allocate = false)
{
    float buffer[3];
    GetEntPropVector(entity, type, property, buffer, index);
    return allocate ? QAngle.Malloc(buffer[0], buffer[1], buffer[2]) : QAngle.Accumulator(buffer[0], buffer[1], buffer[2]);
}