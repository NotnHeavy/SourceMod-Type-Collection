//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_SMTCHEADER
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_SMTCHEADER

#include <smmem> // https://github.com/Scags/SM-Memory
#include <sdktools>
#include <dhooks>

#pragma semicolon true 
#pragma newdecls required

#define NULL view_as<any>(0) // ((void*)0)
#define ENTITY_NULL -1

#define SIZEOF_Pointer 4

#define FLT_MAX view_as<float>(0x7f7fffff)

#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log2(e)
#define M_LOG10E   0.434294481903251827651  // log10(e)
#define M_LN2      0.693147180559945309417  // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define M_SQRT2    1.41421356237309504880   // sqrt(2)
#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)

//////////////////////////////////////////////////////////////////////////////
// FUNCTION MACROS AND STACK (terrorist acts here)                          //
//////////////////////////////////////////////////////////////////////////////

#define SIZEOF(%1) SIZEOF_%1
#define STACK_ALLOC(%1,%2,%3) char SMTC_%1[SIZEOF_%2]; %2 %1 = %2(AddressOfString(SMTC_%1)) // %1 = name, %2 = methodmap, %3 = size to allocate

#define ARRAY Pointer
#define STACK char[]
#define STACK_RETURNREF(%1) return SMTC_%1 // Return a variable allocated with STACK_ALLOC. %1 = variable
#define STACK_RETURNVALUE(%1, %2) char SMTC_%1[%2]; memcpy(AddressOfString(SMTC_%1), %1, %2); return SMTC_%1 // Return a stack-allocated object not used with STACK_ALLOC. %1 = variable, %2 = size to allocate
#define STACK_GETRETURN(%1) AddressOfString(%1) // %1 = variable
#define STACK_SIZEOF(%1) sizeof(SMTC_%1) // %1 = variable

#define SIZEOF_int 4
#define SIZEOF_float 4
#define SIZEOF_any 4
#define SIZEOF_bool 1
#define SIZEOF_char 1

#define SIZEOF_CHAR 1
#define SIZEOF_BYTE 1
#define SIZEOF_WCHAR 2
#define SIZEOF_SHORT 2
#define SIZEOF_USHORT 2
#define SIZEOF_BOOLEAN 1
#define SIZEOF_BOOL 4
#define SIZEOF_SIZE_T 4

#define SIZE_T int

//////////////////////////////////////////////////////////////////////////////
// GLOBALS                                                                  //
//////////////////////////////////////////////////////////////////////////////

enum OSType
{
    OSTYPE_WINDOWS,
    OSTYPE_LINUX,
    OSTYPE_MACOS
}

stock OSType OSTypeUsed;

stock Pointer g_pTFPlayerClassDataMgr;
stock CEngineTrace enginetrace;
stock CTFGameRules g_pGameRules;

stock Pointer CTraceFilterSimple_ShouldHitEntity;

stock DHookSetup SMTC_DHooks_CTFPlayer_OnTakeDamage_Alive;

stock Handle SMTC_SDKCall_IEngineTrace_ClipRayToEntity;
stock Handle SMTC_SDKCall_IEngineTrace_TraceRay;

stock Handle SMTC_SDKCall_CBaseEntity_TakeDamage;
stock Handle SMTC_SDKCall_CTFRadiusDamageInfo_CalculateFalloff;
stock Handle SMTC_SDKCall_CTFRadiusDamageInfo_ApplyToEntity;
stock Handle SMTC_SDKCall_CTFGameRules_RadiusDamage;

stock any m_RefEHandle_Offset;

//////////////////////////////////////////////////////////////////////////////
// CTFGAMERULES METHODMAP                                                   //
//////////////////////////////////////////////////////////////////////////////

// TF2 game rules. This is just used for organising a few function calls and does not re-construct CGameTrace.
methodmap CTFGameRules
{
    // Constructor - wrap around an existing address to make way for a CTFGameRules object.
    public CTFGameRules(Address address)
    {
        return view_as<CTFGameRules>(address);
    }

    // Apply damage within a specific radius using a CTFRadiusDamageInfo object.
    public void RadiusDamage(any& info)
    {
        SDKCall(SMTC_SDKCall_CTFGameRules_RadiusDamage, this, info);
    }
}

//////////////////////////////////////////////////////////////////////////////
// ENGINETRACE METHODMAP                                                    //
//////////////////////////////////////////////////////////////////////////////

// The engine trace object. This is just used for organising a few function calls.
methodmap CEngineTrace
{
    // Constructor - wrap around an existing address to make way for a CEngineTrace object.
    public CEngineTrace(Address address)
    {
        return view_as<CEngineTrace>(address);
    }

    // Traces a ray against a particular edict.
    public void ClipRayToEntity(const any ray, int fMask, any pEntity, any pTrace)
    {
        SDKCall(SMTC_SDKCall_IEngineTrace_ClipRayToEntity, this, ray, fMask, pEntity, pTrace);
    }

    // A version that simply accepts a ray (can work as a traceline or tracehull).
    public void TraceRay(const any ray, int fMask, any pTraceFilter, any pTrace)
    {
        SDKCall(SMTC_SDKCall_IEngineTrace_TraceRay, this, ray, fMask, pTraceFilter, pTrace);
    }
}

//////////////////////////////////////////////////////////////////////////////
// POINTER METHODMAP                                                        //
//////////////////////////////////////////////////////////////////////////////

// A methodmap mimicking the design of a pointer, I.E. a variable that holds a value representing a location in memory.
methodmap Pointer
{
    // The value this pointer is holding.
    property any Address
    {
        public get() { return view_as<any>(this); }
    }

    // Constructor - wrap around an existing address.
    public Pointer(Address address)
    {
        return view_as<Pointer>(address);
    }

    // Locate the value that this pointer is pointing to and return it, with a given offset.
    public any Dereference(any offset = 0, NumberType bits = NumberType_Int32)
    {
        return LoadFromAddress(this.Address + offset, bits);
    }

    // Write to the address this pointer is holding, with a given offset.
    public void Write(any value, any offset = 0, NumberType bits = NumberType_Int32)
    {
        StoreToAddress(this.Address + offset, value, bits);
    }

    // Get the array offset from the pointer's current address. You must specify the size of the type used with this pointer using the typeSize parameter.
    public Pointer Get(int index, int typeSize = 4)
    {
        return Pointer(this.Address + view_as<Address>(index * typeSize));
    }
    
    // Essentially a strlen() replica for pointers.
    public int strlen()
    {
        int length;
        for (int i = 0; this.Dereference(i, NumberType_Int8) != 0; ++i)
            ++length;
        return length;
    }

    // Store pointer into a char buffer.
    public void ToCharBuffer(char[] buffer, int maxlen = -1)
    {
        for (int i = 0; i < maxlen; ++i)
        {
            char value = this.Dereference(i, NumberType_Int8);
            buffer[i] = value;
            if (value == '\0') // NUL
                break;
        }
    }

    // Store char buffer into a pointer.
    public void CharBufferToPointer(char[] buffer, int maxlen = -1)
    {
        for (int i = 0; i < maxlen; ++i)
        {
            char value = buffer[i];
            this.Write(value, i, NumberType_Int8);
            if (value == '\0') // NUL
                break;
        }
    }

    // Store into a buffer. You must specify the size of the type used with this pointer using the typeSize parameter.
    public void ToBuffer(char[] buffer, int typeSize = 4)
    {
        memcpy(AddressOfString(buffer), this, typeSize);
    }

    // Return the address of a cell.
    public static Pointer Reference(any& cell)
    {
        return AddressOf(cell);
    }

    // Return the address of an array of cells.
    public static Pointer ReferenceArray(any[] cell)
    {
        return AddressOfArray(cell);
    }

    // Return the address of a string.
    public static Pointer ReferenceString(char[] cell)
    {
        return AddressOfString(cell);
    }
}

//////////////////////////////////////////////////////////////////////////////
// POINTERS                                                                 //
//////////////////////////////////////////////////////////////////////////////

// :D
stock Address operator+(Address left, int right)
{
    return left + view_as<Address>(right);
}
stock Address operator+(int left, Address right)
{
    return view_as<Address>(left) + right;
}
stock Address operator-(Address left, int right)
{
    return left - view_as<Address>(right);
}
stock Address operator-(int left, Address right)
{
    return view_as<Address>(left) - right;
}

// Pointer arithmetic.
stock Pointer operator+(const Pointer left, const int right)
{
    return Pointer(left.Address + right);
}
stock Pointer operator-(const Pointer left, const int right)
{
    return Pointer(left.Address - right);
}
stock Pointer operator+(const Pointer left, const Address right)
{
    return Pointer(left.Address + right);
}
stock Pointer operator-(const Pointer left, const Address right)
{
    return Pointer(left.Address - right);
}

// Assignment overloading.
stock Address operator=(int value)
{
    return view_as<Address>(value);
}
stock Address operator=(Pointer value)
{
    return view_as<Address>(value);
}
stock Pointer operator=(int value)
{
    return view_as<Pointer>(value);
}
stock Pointer operator=(Address value)
{
    return view_as<Pointer>(value);
}

// Re-allocates a block of memory for an array of num elements. Each byte is initialized as 0.
stock any recalloc(Pointer memory, int num, int size, int old)
{
    Pointer buffer = calloc(num, size);
    memcpy(buffer, memory, old * size);
    free(memory);
    return buffer;
}

// Swap between two blocks of memory. You must specify the size of the type used with this global using the typeSize parameter.
stock void V_swap(Pointer x, Pointer y, int typeSize = 4)
{
    Pointer temp = malloc(typeSize);
    memcpy(temp, x, typeSize);
    memcpy(x, y, typeSize);
    memcpy(y, temp, typeSize);
    free(temp);
}

// abs(), but for floats.
// why did i make this it exists as a sm native
stock float FloatMakePositive(float f)
{
    return view_as<float>(view_as<int>(f) & 0x77777777); // evil floating point bit level hacking
}