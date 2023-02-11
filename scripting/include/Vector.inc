//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// Not really necessary, but you can include SMTC.inc and run SMTC_Initialize() to allocate vec3_origin properly.

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VECTOR
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VECTOR

#pragma semicolon true 
#pragma newdecls required

#if !defined VECTOR_CACHE_SIZE
#define VECTOR_CACHE_SIZE 32
#endif

//////////////////////////////////////////////////////////////////////////////
// VECTOR DATA                                                              //
//////////////////////////////////////////////////////////////////////////////

enum
{
    VECTOR_OFFSET_X = 0,                   // float x;
    VECTOR_OFFSET_Y = VECTOR_OFFSET_X + 4, // float y;
    VECTOR_OFFSET_Z = VECTOR_OFFSET_Y + 4, // float z;
    
    SIZEOF_Vector = VECTOR_OFFSET_Z + 4    // sizeof(Vector);
}

// accumulator - user control
static float accumulator_array[4]; // vector size + 1 to compensate for VectorAligned objects.
static Vector accumulator;

// cache - up to a user-specified size
static float cache_array[VECTOR_CACHE_SIZE * 3 + 1]; // the + 1 is to compensate for VectorAligned objects.
static Vector cache;
static int cache_index = 0;

stock Vector vec3_origin; // NOT INTENDED TO BE WRITTEN TO!

//////////////////////////////////////////////////////////////////////////////
// SMTC_VectorForward()                                                     //
//////////////////////////////////////////////////////////////////////////////

// todo: make vec3_origin a legitimate reference to internal vec3_origin
public void SMTC_VectorForward()
{
    vec3_origin = Vector(AddressOfArray({0.00, 0.00, 0.00}));
}

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

    // Constructor - allocate a new vector in place within the current pointer to the cache buffer. 
    // The size of the cache is specified by VECTOR_CACHE_SIZE and when the end is reached, the
    // pointer loops around.
    public static Vector Cache(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        if (!cache)
            cache = AddressOfArray(cache_array);
        Vector obj = view_as<Vector>(cache.Get(cache_index, SIZEOF_Vector));
        cache_index = (cache_index + 1 < VECTOR_CACHE_SIZE) ? cache_index + 1 : 0;
        obj.ctor(x, y, z);
        return obj;
    }

    // Constructor - allocate a new vector in the accumulator, a temporary storage location.
    public static Vector Accumulator(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        if (!accumulator)
            accumulator = AddressOfArray(accumulator_array);
        accumulator.ctor(x, y, z);
        return accumulator;
    }

    // Constructor - allocate a new vector onto the heap. Deallocation will be required when you are finished using this using free().
    public static Vector Malloc(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        Vector pointer = malloc(SIZEOF_Vector);
        pointer.ctor(x, y, z);
        return pointer;
    }

    // Constructor - allocate a new vector onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        char buffer[SIZEOF_Vector];
        view_as<Vector>(AddressOfString(buffer)).ctor(x, y, z);
        return buffer;
    }

    // Base address...
    public Pointer Base()
    {
        return view_as<Pointer>(this);
    }

    // Store the vector into a buffer.
    public void GetBuffer(float buffer[3])
    {
        memcpy(AddressOfArray(buffer), this, SIZEOF_Vector);
    }

    // Copy from a buffer into this vector.
    public void SetFromBuffer(float buffer[3])
    {
        memcpy(this, AddressOfArray(buffer), SIZEOF_Vector);
    }

    // Get the vector's magnitude squared.
    public float LengthSqr()
    {
        return this.X * this.X + this.Y * this.Y + this.Z * this.Z;
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
        if (magnitude > 0.00)
        {
            this.X /= magnitude;
            this.Y /= magnitude;
            this.Z /= magnitude;
        }
        else
        {
            this.X = 0.00;
            this.Y = 0.00;
            this.Z = 0.00;
        }
        return magnitude;
    }

    // Get the distance from this vector to the other one squared. The returned vector is stored onto the accumulator.
    public float DistToSqr(const Vector other)
    {
        return Vector.Cache(this.X - other.X, this.Y - other.Y, this.Z - other.Z).LengthSqr();
    }

    // Get the distance from this vector to the other one. The returned vector is stored onto the accumulator.
    public float DistTo(const Vector other)
    {
        return Vector.Cache(this.X - other.X, this.Y - other.Y, this.Z - other.Z).Length();
    }

    // Return the dot product between two vectors.
    public float Dot(const Vector other)
    {
        return this.X * other.X + this.Y * other.Y + this.Z * other.Z;
    }

    // Get the cross product between two vectors. The returned vector is stored onto the accumulator.
    public Vector Cross(const Vector other)
    {
        return Vector.Cache(this.Y * other.Z - this.Z * other.Y, this.Z * other.X - this.X * other.Z, this.X * other.Y - this.Y * other.X);
    }

    // Returns a vector with the min in X, Y and Z. The returned vector is stored onto the accumulator.
    public Vector Min(const Vector other)
    {
        return Vector.Cache(this.X < other.X ? this.X : other.X, this.Y < other.Y ? this.Y : other.Y, this.Z < other.Z ? this.Z : other.Z);
    }

    // Returns a vector with the max in X, Y and Z. The returned vector is stored onto the accumulator.
    public Vector Max(const Vector other)
    {
        return Vector.Cache(this.X > other.X ? this.X : other.X, this.Y > other.Y ? this.Y : other.Y, this.Z > other.Z ? this.Z : other.Z);
    }

    // Is this vector's length greater than the provided magnitude?
    public bool IsLengthGreaterThan(float val)
    {
        return this.LengthSqr() > val * val;
    }

    // Is this vector's length less than the provided magnitude?
    public bool IsLengthLessThan(float val)
    {
        return this.LengthSqr() < val * val;
    }

    // Generate random co-ordinates within a specific minValue/maxValue range.
    public void Random(float minValue, float maxValue)
    {
        this.X = GetRandomFloat(minValue, maxValue);
        this.Y = GetRandomFloat(minValue, maxValue);
        this.Z = GetRandomFloat(minValue, maxValue);
    }

    // Vector assignation.
    public void Assign(Vector vector)
    {
        memcpy(this, vector, SIZEOF_Vector);
    }

    // Get a vector from an array.
    public static Vector ArrayReference(float buffer[3])
    {
        return Vector(AddressOfArray(buffer));
    }

    // Get the vector from the accumulator.
    public static Vector FromAccumulator()
    {
        return accumulator;
    }
}

//////////////////////////////////////////////////////////////////////////////
// VECTOR OPERATORS                                                         //
//////////////////////////////////////////////////////////////////////////////

stock Vector operator+(const Vector left, const Vector right)
{
    return Vector.Cache(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
}

stock Vector operator-(const Vector left, const Vector right)
{
    return Vector.Cache(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
}

stock Vector operator*(const Vector left, const Vector right)
{
    return Vector.Cache(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
}

stock Vector operator/(const Vector left, const Vector right)
{
    return Vector.Cache(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
}

stock Vector operator*(const Vector left, const float right)
{
    return Vector.Cache(left.X * right, left.Y * right, left.Z * right);
}

stock Vector operator/(const Vector left, const float right)
{
    return Vector.Cache(left.X / right, left.Y / right, left.Z / right);
}

stock Vector operator-(const Vector vector)
{
    return Vector.Cache(-vector.X, -vector.Y, -vector.Z);
}

//////////////////////////////////////////////////////////////////////////////
// VECTOR GLOBALS                                                           //
//////////////////////////////////////////////////////////////////////////////

stock void VectorClear(Vector vector)
{
    vector.X = 0.00;
    vector.Y = 0.00;
    vector.Z = 0.00;
}

stock void VectorCopy(Vector source, Vector destination)
{
    destination.X = source.X;
    destination.Y = source.Y;
    destination.Z = source.Z;
}

stock void VectorAdd(const Vector a, const Vector b, Vector result)
{
    result.X = a.X + b.X;
    result.Y = a.Y + b.Y;
    result.Z = a.Z + b.Z;
}

stock void VectorSubtract(const Vector a, const Vector b, Vector result)
{
    result.X = a.X - b.X;
    result.Y = a.Y - b.Y;
    result.Z = a.Z - b.Z;
}

stock void VectorMultiply(const Vector a, const Vector b, Vector result)
{
    result.X = a.X * b.X;
    result.Y = a.Y * b.Y;
    result.Z = a.Z * b.Z;
}

stock void VectorDivide(const Vector a, const Vector b, Vector result)
{
    result.X = a.X / b.X;
    result.Y = a.Y / b.Y;
    result.Z = a.Z / b.Z;
}

stock void VectorScale(const Vector vector, float scale, Vector result)
{
    result.X = vector.X * scale;
    result.Y = vector.Y * scale;
    result.Z = vector.Z * scale;
}

stock void CrossProduct(const Vector a, const Vector b, Vector result)
{
    result.X = a.Y * b.Z - a.Z * b.Y;
    result.Y = a.Z * b.X - a.X * b.Z;
    result.Z = a.X * b.Y - a.Y * b.X;
}

stock void VectorVectors(const Vector forwardVector, Vector& right, Vector& up)
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


// Vector equality with tolerance
stock bool VectorsAreEqual(const Vector left, const Vector right, float tolerance)
{
    if (FloatMakePositive(left.X - right.X) > tolerance)
        return false;
    if (FloatMakePositive(left.Y - right.Y) > tolerance)
        return false;
    return FloatMakePositive(left.Z - right.Z) <= tolerance;
}

// Get a random vector. This vector is stored onto the accumulator and must be assigned to your desired vector, unless allocate is set to true.
stock Vector RandomVector(float minValue, float maxValue, bool allocate = false)
{
    Vector random = allocate ? Vector.Malloc() : Vector.Accumulator();
    random.Random(minValue, maxValue);
    return random;
}