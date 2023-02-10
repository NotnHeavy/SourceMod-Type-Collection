//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_QANGLE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_QANGLE

#pragma semicolon true 
#pragma newdecls required

#if !defined QANGLE_CACHE_SIZE
#define QANGLE_CACHE_SIZE 32
#endif

//////////////////////////////////////////////////////////////////////////////
// QANGLE DATA                                                              //
//////////////////////////////////////////////////////////////////////////////

enum
{
    QANGLE_OFFSET_X = 0,                   // float x;
    QANGLE_OFFSET_Y = QANGLE_OFFSET_X + 4, // float y;
    QANGLE_OFFSET_Z = QANGLE_OFFSET_Y + 4, // float z;
    
    SIZEOF_QAngle = QANGLE_OFFSET_Z + 4    // sizeof(QAngle);
}

// accumulator - user control
static float accumulator_array[3];
static QAngle accumulator;

// cache - up to a user-specified size
static float cache_array[QANGLE_CACHE_SIZE * 3 + 1]; // the + 1 is to compensate for VectorAligned objects.
static QAngle cache;
static int cache_index = 0;

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

    // Constructor - allocate a new vector in place within the current pointer to the cache buffer. 
    // The size of the cache is specified by QANGLE_CACHE_SIZE and when the end is reached, the
    // pointer loops around.
    public static QAngle Cache(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        if (!cache)
            cache = AddressOfArray(cache_array);
        QAngle obj = view_as<QAngle>(cache.Get(cache_index, SIZEOF_QAngle));
        cache_index = (cache_index + 1 < QANGLE_CACHE_SIZE) ? cache_index + 1 : 0;
        obj.ctor(x, y, z);
        return obj;
    }

    // Constructor - allocate a new QAngle in the accumulator, a temporary storage location.
    public static QAngle Accumulator(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        if (!accumulator)
            accumulator = AddressOfArray(accumulator_array);
        accumulator.ctor(x, y, z);
        return accumulator;
    }

    // Constructor - allocate a new QAngle onto the heap. Deallocation will be required when you are finished using this using free().
    public static QAngle Malloc(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        QAngle pointer = malloc(SIZEOF_QAngle);
        pointer.ctor(x, y, z);
        return pointer;
    }

    // Constructor - allocate a new QAngle onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(float x = 0.00, float y = 0.00, float z = 0.00)
    {
        char buffer[SIZEOF_QAngle];
        view_as<QAngle>(AddressOfString(buffer)).ctor(x, y, z);
        return buffer;
    }

    // Base address...
    public Pointer Base()
    {
        return view_as<Pointer>(this);
    }

    // Store the QAngle into a buffer.
    public void GetBuffer(float buffer[3])
    {
        memcpy(AddressOfArray(buffer), this, SIZEOF_QAngle);
    }

    // Copy from a buffer into this QAngle.
    public void SetFromBuffer(float buffer[3])
    {
        memcpy(this, AddressOfArray(buffer), SIZEOF_QAngle);
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

    // Generate random co-ordinates within a specific minValue/maxValue range.
    public void Random(float minValue, float maxValue)
    {
        this.X = GetRandomFloat(minValue, maxValue);
        this.Y = GetRandomFloat(minValue, maxValue);
        this.Z = GetRandomFloat(minValue, maxValue);
    }

    // Vector assignation.
    public void Assign(QAngle vector)
    {
        memcpy(this, vector, SIZEOF_QAngle);
    }
}

//////////////////////////////////////////////////////////////////////////////
// QANGLE OPERATORS                                                         //
//////////////////////////////////////////////////////////////////////////////

stock QAngle operator+(const QAngle left, const QAngle right)
{
    return QAngle.Cache(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
}

stock QAngle operator-(const QAngle left, const QAngle right)
{
    return QAngle.Cache(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
}

stock QAngle operator*(const QAngle left, const QAngle right)
{
    return QAngle.Cache(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
}

stock QAngle operator/(const QAngle left, const QAngle right)
{
    return QAngle.Cache(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
}

//////////////////////////////////////////////////////////////////////////////
// QANGLE GLOBALS                                                           //
//////////////////////////////////////////////////////////////////////////////

// Would be called VectorAdd, but there's no function overloading.
stock void QAngleAdd(const QAngle a, const QAngle b, QAngle result)
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
stock void AngleVectors(const QAngle angles, Vector& forwardVector = view_as<Vector>(NULL), Vector& right = view_as<Vector>(NULL), Vector& up = view_as<Vector>(NULL))
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

// Get a random QAngle. This QAngle is stored onto the accumulator and must be assigned to your desired QAngle, unless allocate is set to true, where it is malloc()'d instead.
stock QAngle RandomAngle(float minValue, float maxValue, bool allocate = false)
{
    QAngle random = allocate ? QAngle.Malloc() : QAngle.Accumulator();
    random.Random(minValue, maxValue);
    return random;
}

// Forward direction vector -> Euler angles
stock void VectorAngles(const Vector& forwardVector, QAngle& angles)
{
    float tmp, yaw, pitch;

    if (forwardVector.Y == 0.00 && forwardVector.Z == 0.00)
    {
        yaw = 0.00;
        if (forwardVector.Z > 0.00)
            pitch = 270.00;
        else
            pitch = 90.00;
    }
    else
    {
        yaw = (ArcTangent2(forwardVector.Y, forwardVector.Z) * 180.00 / M_PI);
        if (yaw < 0.00)
            yaw += 360.00;
        
        tmp = SquareRoot(forwardVector.X * forwardVector.X + forwardVector.Y * forwardVector.Y);
        pitch = (ArcTangent2(-forwardVector.Z, tmp) * 180.00 / M_PI);
        if (pitch < 0.00)
            pitch += 360.00;
    }

    angles.X = pitch;
    angles.Y = yaw;
    angles.Z = 0.00;
}