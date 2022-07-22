//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// this is suicide

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLMEMORY
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLMEMORY

#include "Pointer.inc"

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CUTLMEMORY DATA                                                          //
//////////////////////////////////////////////////////////////////////////////

enum
{
    CUTLMEMORY_OFFSET_M_PMEMORY = 0,                                          // T* m_pMemory; (where T = CUtlMemory<T>)
    CUTLMEMORY_OFFSET_M_NALLOCATIONCOUNT = CUTLMEMORY_OFFSET_M_PMEMORY + 4,   // int m_nAllocationCount;
    CUTLMEMORY_OFFEST_M_NGROWSIZE = CUTLMEMORY_OFFSET_M_NALLOCATIONCOUNT + 4, // int m_nGrowSize;

    CUTLMEMORY_SIZE = CUTLMEMORY_OFFEST_M_NGROWSIZE + 4                       // sizeof(CUtlMemory);
};

enum
{
    EXTERNAL_BUFFER_MARKER = -1,
    EXTERNAL_CONST_BUFFER_MARKER = -2,
};

const int INVALID_INDEX = -1;

//////////////////////////////////////////////////////////////////////////////
// CUTLMEMORY METHODMAP                                                     //
//////////////////////////////////////////////////////////////////////////////

// A growable memory class which doubles in size by default.
// I hope this is all correct.
methodmap CUtlMemory < Pointer
{
    // Constructor - wrap around an existing address to make way for a CUtlMemory object.
    public CUtlMemory(Address address)
    {
        return view_as<CUtlMemory>(address);
    }

    // The memory pointer itself.
    property Pointer m_pMemory
    {
        public get() { return this.Dereference(CUTLMEMORY_OFFSET_M_PMEMORY); }
        public set(Pointer pointer) { this.Write(pointer, CUTLMEMORY_OFFSET_M_PMEMORY); }
    }

    // Current length of the memory block.
    property int m_nAllocationCount
    {
        public get() { return this.Dereference(CUTLMEMORY_OFFSET_M_NALLOCATIONCOUNT); }
        public set(int value) { this.Write(value, CUTLMEMORY_OFFSET_M_NALLOCATIONCOUNT); }
    }

    // The grow size
    property int m_nGrowSize
    {
        public get() { return this.Dereference(CUTLMEMORY_OFFEST_M_NGROWSIZE); }
        public set(int value) { this.Write(value, CUTLMEMORY_OFFEST_M_NGROWSIZE); }
    }

    // Call constructor on existing type. You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public void ctor(int growSize = 0, int initialAllocationCount = 0, int typeSize = 4)
    {
        if (growSize < 0)
            return;
        this.m_nAllocationCount = initialAllocationCount;
        this.m_nGrowSize = growSize;
        if (this.m_nAllocationCount)
            this.m_pMemory = calloc(this.m_nAllocationCount, typeSize);
        else
            this.m_pMemory = NULL;
    }

    // Constructor - allocate a new CUtlMemory onto the heap. Deallocation will be required when you are finished using CUtlMemory.Dispose().
    // You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public static CUtlMemory Malloc(int growSize = 0, int initialAllocationCount = 0, int typeSize = 4)
    {
        CUtlMemory pointer = malloc(CUTLMEMORY_SIZE);
        pointer.ctor(growSize, initialAllocationCount, typeSize);
        return pointer;
    }

    // Specify the invalid ('null') index that we'll only return on failure
    public static int InvalidIndex()
    {
        return INVALID_INDEX;
    }

    // Is element index valid?
    public bool IsIdxValid(int index)
    {
        return index < this.m_nAllocationCount;
    }

    // Element access - get from specified index. Returns a pointer on success, returns NULL on failure.
    // You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    // You can use the pointer methodmap in order to use read/write operations.
    public Pointer Get(int index, int typeSize = 4)
    {
        if (!this.IsIdxValid(index))
            return NULL;
        return Pointer(this.m_pMemory.Address + index * typeSize);
    }

    // Gets the base address (can change when adding elements!)
    public Pointer Base()
    {
        return this.m_pMemory;
    }

    // Is the memory externally allocated?
    public bool IsExternallyAllocated()
    {
        return this.m_nGrowSize < 0;
    }

    // Is the memory read only?
    public bool IsReadOnly()
    {
        return this.m_nGrowSize == EXTERNAL_CONST_BUFFER_MARKER;
    }

    // Memory deallocation.
    public void Purge()
    {
        if (!this.IsExternallyAllocated())
        {
            if (this.m_pMemory != NULL)
            {
                free(this.m_pMemory);
                this.m_pMemory = NULL;
            }
            this.m_nAllocationCount = 0;
        }
    }

    // Purge all but the given number of elements. Would also be called Purge(), however function overloading is not a thing in SourcePawn.
    // You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public void PurgeButGiven(int elements, int typeSize = 4)
    {
        if (elements < 0 || elements < this.m_nAllocationCount || this.IsExternallyAllocated() || this.m_pMemory == NULL)
            return;

        // If we have zero elements, simply do a purge.
        if (elements == 0)
        {
            this.Purge();
            return;
        }
        
        this.m_nAllocationCount = elements;

        // Allocation count > 0, shrink it down.
        this.m_pMemory = recalloc(this.m_pMemory, this.m_nAllocationCount, typeSize);
    }

    // Set the size by which the memory grows. You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public void Init(int growSize = 0, int initSize = 0, int typeSize = 4)
    {
        if (growSize < 0)
            return;
        this.Purge();

        this.m_nGrowSize = growSize;
        this.m_nAllocationCount = initSize;
        if (this.m_nAllocationCount)
            this.m_pMemory = calloc(this.m_nAllocationCount, typeSize);
    }

    // Attaches the buffer to external memory...
    public void SetExternalBuffer(Pointer memory, int elements)
    {
        // Blow away any existing allocated memory.
        this.Purge();

        this.m_pMemory = memory;
        this.m_nAllocationCount = elements;

        // Indicate that we don't own the memory.
        this.m_nGrowSize = EXTERNAL_BUFFER_MARKER;
    }

    // Takes ownership of the passed memory, including freeing it when this buffer is destroyed.
    public void AssumeMemory(Pointer memory, int elements)
    {
        // Blow away any existing allocated memory.
        this.Purge();

        // Simply take the pointer but don't mark us as external.
        this.m_pMemory = memory;
        this.m_nAllocationCount = elements;
    }

    // Fast swap.
    public void Swap(CUtlMemory memory)
    {
        // this is weird, but whatever.
        V_swap(view_as<Pointer>(this) + CUTLMEMORY_OFFEST_M_NGROWSIZE, view_as<Pointer>(memory) + CUTLMEMORY_OFFEST_M_NGROWSIZE);
        V_swap(this.m_pMemory, memory.m_pMemory);
        V_swap(view_as<Pointer>(this) + CUTLMEMORY_OFFSET_M_NALLOCATIONCOUNT, view_as<Pointer>(memory) + CUTLMEMORY_OFFSET_M_NALLOCATIONCOUNT);
    }

    // Switches the buffer from an external memory buffer to a reallocatable buffer.
	// Will copy the current contents of the external buffer to the reallocatable buffer.
    // You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public void ConvertToGrowableMemory(int growSize, int typeSize = 4)
    {
        if (!this.IsExternallyAllocated())
            return;

        this.m_nGrowSize = growSize;
        if (this.m_nAllocationCount)
        {
            int numBytes = this.m_nAllocationCount * typeSize;
            Pointer pMemory = malloc(numBytes);
            memcpy(pMemory, this.m_pMemory, numBytes);
            this.m_pMemory = pMemory;
        }
        else
            this.m_pMemory = NULL;
    }

    // Size.
    public int NumAllocated()
    {
        return this.m_nAllocationCount;
    }

    // Size.
    public int Count()
    {
        return this.m_nAllocationCount;
    }

    // Grows the memory, so that at least allocated + num elements are allocated. You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public void Grow(int num = 1, int typeSize = 4)
    {
        if (num < 1 || this.IsExternallyAllocated())
            return;
        
        // Make sure we have at least numallocated + num allocations.
        // Use the grow rules specified for this memory (in m_nGrowSize).
        int allocationRequested = this.m_nAllocationCount + num;
        int newAllocationCount = UtlMemory_CalcNewAllocationCount(this.m_nAllocationCount, this.m_nGrowSize, allocationRequested, typeSize);
        this.m_nAllocationCount = newAllocationCount;

        if (this.m_pMemory)
            this.m_pMemory = recalloc(this.m_pMemory, this.m_nAllocationCount, typeSize);
        else
            this.m_pMemory = calloc(this.m_nAllocationCount, typeSize);
    }

    // Makes sure we've got at least this much memory. You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public void EnsureCapacity(int num, int typeSize = 4)
    {
        if (this.m_nAllocationCount >= num || this.IsExternallyAllocated())
            return;
        this.m_nAllocationCount = num;

        if (this.m_pMemory)
            this.m_pMemory = recalloc(this.m_pMemory, this.m_nAllocationCount, typeSize);
        else
            this.m_pMemory = calloc(this.m_nAllocationCount, typeSize);
    }

    // Set the size by which the memory grows.
    public void SetGrowSize(int size)
    {
        if (this.IsExternallyAllocated() || size < 0)
            return;
        this.m_nGrowSize = size;
    }

    // Deconstructor.
    public void Dispose()
    {
        this.Purge();
        free(this);
    }
}

//////////////////////////////////////////////////////////////////////////////
// MISCELLANEOUS                                                            //
//////////////////////////////////////////////////////////////////////////////

stock int UtlMemory_CalcNewAllocationCount(int allocationCount, int growSize, int newSize, int bytesItem)
{
    if (growSize)
        allocationCount = (1 + ((newSize - 1) / growSize)) * growSize;
    else
    {
         // Compute an allocation which is at least as big as a cache line...
        if (!allocationCount)
            allocationCount = (31 + bytesItem) / bytesItem;

        while (allocationCount < newSize)
            allocationCount *= 2;
    }
    return allocationCount;
}