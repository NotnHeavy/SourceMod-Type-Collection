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

#define V_SWAP(%1,%2,%3) { %1 temp = %2; %2 = %3; %3 = temp; } // Not going to make 500 functions. Can't parse through methodmap properties either (which makes sense tbh). If you really need to use this, don't use an ending semi-colon. Will consider removing.

#include <smmem>

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
    public int DereferenceEntity()
    {
        static int offs_RefEHandle;
        if (!offs_RefEHandle) 
            offs_RefEHandle = getRefEHandleOffset();
        return this.DereferenceEHandle(offs_RefEHandle);
    }

    // Get the array offset from the pointer's current address. You must specify the size of the type used with this pointer using the typeSize parameter.
    public Pointer Get(int index, int typeSize = 4)
    {
        return Pointer(this.Address + view_as<Address>(index * typeSize));
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

// The tag mismatch warnings you get are complete BS, so there's these operator overloads.
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

// Re-allocate within a specific location in memory. You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
// Normally this would call the the constructor of the allocated memory, but that's not possible here.
void Construct(Pointer memory, int typeSize = 4)
{
    memset(memory, 0, typeSize);
}

// Re-allocate within a specific location in memory, from another source. You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
// Normally this would call the the constructor of the allocated memory, but that's not possible here.
void CopyConstruct(Pointer memory, Pointer source, int typeSize = 4)
{
    memcpy(memory, source, typeSize);
}

// Re-allocates a block of memory for an array of num elements. Each byte is initialized as 0.
any recalloc(Pointer memory, int num, int size)
{
    Pointer buffer = calloc(num, size);
    memcpy(buffer, memory, num * size);
    free(memory);
    return buffer;
}