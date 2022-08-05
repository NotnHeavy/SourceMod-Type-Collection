//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_POINTER
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_POINTER

#pragma semicolon true 
#pragma newdecls required

#define NULL view_as<Pointer>(0) // ((void*)0)
#define ENTITY_NULL -1

#define STACK_ALLOC(%1,%2,%3) int SMTC_%1[%3 / 4]; %2 %1 = %2(AddressOfArray(SMTC_%1)) // %1 = name, %2 = methodmap, %3 = size in bytes (buffer size will be %3 / 4)

#include <smmem> // https://github.com/Scags/SM-Memory

//////////////////////////////////////////////////////////////////////////////
// POINTER METHODMAP                                                        //
//////////////////////////////////////////////////////////////////////////////

// A methodmap mimicking the design of a pointer, I.E. a variable that holds a value representing a location in memory.
methodmap Pointer
{
    // The value this pointer is holding.
    property Address Address
    {
        public get() { return view_as<Address>(this); }
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

    // Get the entity index from an EHANDLE. Taken from nosoop's stocksoup repository.
    public int DereferenceEHandle(any offset = 0)
    {
        return EntRefToEntIndex(this.Dereference(offset) | (1 << 31));
    }

    // Write entity handle to this pointer's address. Taken from nosoop's stocksoup repository.
    public void WriteEHandle(any index, any offset = 0)
    {
        this.Write(IsValidEntity(index) ? EntIndexToEntRef(index) & ~(1 << 31) : 0, offset);
    }

    // Get the entity index from an address. Taken from nosoop's stocksoup repository.
    public int DereferenceEntity(any offset = 0)
    {
        static int offs_RefEHandle;
        if (!offs_RefEHandle) 
            offs_RefEHandle = getRefEHandleOffset();
        return this.DereferenceEHandle(offset + offs_RefEHandle);
    }

    // Write the entity address to this pointer.
    public void WriteEntity(int entity, any offset = 0)
    {
        this.Write(IsValidEntity(entity) ? GetEntityAddress(entity) : Address_Null, offset);
    }

    // Get the array offset from the pointer's current address. You must specify the size of the type used with this pointer using the typeSize parameter.
    public Pointer Get(int index, int typeSize = 4)
    {
        return Pointer(this.Address + view_as<Address>(index * typeSize));
    }

    // Store into a buffer. You must specify the size of the type used with this pointer using the typeSize parameter.
    // If the size specified is based upon the size of your buffer, you must multiply it by 4 for the typeSize parameter.
    public void ToBuffer(any[] buffer, int typeSize = 4)
    {
        memcpy(AddressOfArray(buffer), this, typeSize);
    }
}

//////////////////////////////////////////////////////////////////////////////
// MISCELLANEOUS                                                            //
//////////////////////////////////////////////////////////////////////////////

// Taken from nosoop's stocksoup repository.
static int getRefEHandleOffset()
{
    // if we don't have it already, attempt to lookup offset based on SDK information
    // CWorld is derived from CBaseEntity so it should have both offsets
    int offs_angRotation = FindDataMapInfo(0, "m_angRotation"), offs_vecViewOffset = FindDataMapInfo(0, "m_vecViewOffset");
    if (offs_angRotation == -1) 
    {
        ThrowError("Could not find offset for ((CBaseEntity) CWorld)::m_angRotation");
    }
    else if (offs_vecViewOffset == -1) 
    {
        ThrowError("Could not find offset for ((CBaseEntity) CWorld)::m_vecViewOffset");
    } 
    else if ((offs_angRotation + 0x0C) != (offs_vecViewOffset - 0x04)) 
    {
        char game[32];
        GetGameFolderName(game, sizeof(game));
        ThrowError("Could not confirm offset of CBaseEntity::m_RefEHandle "
                ... "(incorrect assumption for game '%s'?)", game);
    }

    // offset seems right, cache it for the next call
    return offs_angRotation + 0x0C;
}

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

// Re-allocate within a specific location in memory. You must specify the size of the type used with this global using the typeSize parameter.
// Normally this would call the the constructor of the allocated memory, but that's not possible here.
stock void Construct(Pointer memory, int typeSize = 4)
{
    memset(memory, 0, typeSize);
}

// Re-allocate within a specific location in memory, from another source. You must specify the size of the type used with this global using the typeSize parameter.
// Normally this would call the the constructor of the allocated memory, but that's not possible here.
stock void CopyConstruct(Pointer memory, Pointer source, int typeSize = 4)
{
    memcpy(memory, source, typeSize);
}

// Re-allocates a block of memory for an array of num elements. Each byte is initialized as 0.
stock any recalloc(Pointer memory, int num, int size)
{
    Pointer buffer = calloc(num, size);
    memcpy(buffer, memory, num * size);
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
stock float FloatMakePositive(float f)
{
    return view_as<float>(view_as<int>(f) & 0x77777777); // evil floating point bit level hacking
}

// strlen, but on a char pointer, rather than a char array.
stock int PointerStrLen(Pointer pointer)
{
    int length;
    for (int i = 0; pointer.Dereference(i, NumberType_Int8) != 0; ++i)
        ++length;
    return length;
}