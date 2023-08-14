//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// this is suicide

// Internally, CUtlMemory requires templated arguments. However, because they aren't
// used to a large extent, I have managed to write all three necessary methodmaps
// for CUtlMemory::Iterator_t. You will only need to specify two parameters
// (depending on how the functions) work for the respective templated
// arguments:
//
// template< class T, class I = int >
// class CUtlMemory
//
// Where T is the type used, and I is the index (must be 1, 2 or 4 bytes).

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLMEMORY
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLMEMORY

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// ITERATOR_T BASE TYPES                                                    //
//////////////////////////////////////////////////////////////////////////////

methodmap SMTC_CUtlMemory_Iterator_t < Pointer { }

//////////////////////////////////////////////////////////////////////////////
// ITERATOR_T DATA                                                          //
//////////////////////////////////////////////////////////////////////////////

// NOTE: this methodmap may be renamed in the future!
#define SIZEOF_Iterator8_t 1
#define SIZEOF_Iterator16_t 2
#define SIZEOF_Iterator_t 4

//////////////////////////////////////////////////////////////////////////////
// ITERATOR_T METHOEDMAPS                                                   //
//////////////////////////////////////////////////////////////////////////////

// An iterator object used for enumerating over a CUtlMemory instance.
// This iterator uses 8-bit indexing.
methodmap Iterator8_t < SMTC_CUtlMemory_Iterator_t
{
    // Constructor - wrap around an existing address to make way for a Iterator8_t object.
    public Iterator8_t(Address address)
    {
        return view_as<Iterator8_t>(address);
    }

    // The index.
    property any index
    {
        public get() { return this.Dereference(0, NumberType_Int8); }
        public set(any value) { this.Write(value, 0, NumberType_Int8); }
    }

    // Constructor - allocate a new Iterator8_t onto the heap. Deallocation will be required when you are finished using this using free().
    public static Iterator8_t Malloc()
    {
        return malloc(SIZEOF_Iterator8_t);
    }

    // Constructor - allocate a new Iterator8_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_Iterator8_t] = { 0 };
        return buffer;
    }
}

// An iterator object used for enumerating over a CUtlMemory instance.
// This iterator uses 16-bit indexing.
methodmap Iterator16_t < SMTC_CUtlMemory_Iterator_t
{
    // Constructor - wrap around an existing address to make way for a Iterator16_t object.
    public Iterator16_t(Address address)
    {
        return view_as<Iterator16_t>(address);
    }

    // The index.
    property any index
    {
        public get() { return this.Dereference(0, NumberType_Int16); }
        public set(any value) { this.Write(value, 0, NumberType_Int16); }
    }

    // Constructor - allocate a new Iterator16_t onto the heap. Deallocation will be required when you are finished using this using free().
    public static Iterator16_t Malloc()
    {
        return malloc(SIZEOF_Iterator16_t);
    }

    // Constructor - allocate a new Iterator16_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_Iterator16_t] = { 0 };
        return buffer;
    }
}

// An iterator object used for enumerating over a CUtlMemory instance.
// This iterator uses 32-bit indexing.
methodmap Iterator_t < SMTC_CUtlMemory_Iterator_t
{
    // Constructor - wrap around an existing address to make way for a Iterator_t object.
    public Iterator_t(Address address)
    {
        return view_as<Iterator_t>(address);
    }

    // The index.
    property any index
    {
        public get() { return this.Dereference(); }
        public set(any value) { this.Write(value); }
    }

    // Constructor - allocate a new Iterator_t onto the heap. Deallocation will be required when you are finished using this using free().
    public static Iterator_t Malloc()
    {
        return malloc(SIZEOF_Iterator_t);
    }

    // Constructor - allocate a new Iterator_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_Iterator_t] = { 0 };
        return buffer;
    }
}

stock bool operator==(const Iterator8_t left, const Iterator8_t right)
{
    return left.index == right.index;
}

stock bool operator!=(const Iterator8_t left, const Iterator8_t right)
{
    return left.index != right.index;
}

stock bool operator==(const Iterator16_t left, const Iterator16_t right)
{
    return left.index == right.index;
}

stock bool operator!=(const Iterator16_t left, const Iterator16_t right)
{
    return left.index != right.index;
}

stock bool operator==(const Iterator_t left, const Iterator_t right)
{
    return left.index == right.index;
}

stock bool operator!=(const Iterator_t left, const Iterator_t right)
{
    return left.index != right.index;
}

//////////////////////////////////////////////////////////////////////////////
// CUTLMEMORY DATA                                                          //
//////////////////////////////////////////////////////////////////////////////

#define SMTC_CUTLMEMORY8_SIZEOF_I 1
#define SMTC_CUTLMEMORY16_SIZEOF_I 2
#define SMTC_CUTLMEMORY_SIZEOF_I 4

enum
{
    CUTLMEMORY_OFFSET_M_PMEMORY = 0,                                                     // T* m_pMemory; (where T = CUtlMemory<T>)
    CUTLMEMORY_OFFSET_M_NALLOCATIONCOUNT = CUTLMEMORY_OFFSET_M_PMEMORY + SIZEOF_Pointer, // int m_nAllocationCount;
    CUTLMEMORY_OFFEST_M_NGROWSIZE = CUTLMEMORY_OFFSET_M_NALLOCATIONCOUNT + 4,            // int m_nGrowSize;

    SIZEOF_CUtlMemory = CUTLMEMORY_OFFEST_M_NGROWSIZE + 4                                // sizeof(CUtlMemory);
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
    
    // Call deconstructor on existing type.
    public void dtor()
    {
        this.Purge();
    }

    // Constructor - allocate a new CUtlMemory onto the heap. Deallocation will be required when you are finished using CUtlMemory.Dispose().
    // You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public static CUtlMemory Malloc(int growSize = 0, int initialAllocationCount = 0, int typeSize = 4)
    {
        CUtlMemory pointer = malloc(SIZEOF_CUtlMemory);
        pointer.ctor(growSize, initialAllocationCount, typeSize);
        return pointer;
    }

    // Constructor - allocate a new CUtlMemory onto the stack. When using the return value, you must use STACK_GETRETURN().
    // You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public static STACK StackAlloc(int growSize = 0, int initialAllocationCount = 0, int typeSize = 4)
    {
        char buffer[SIZEOF_CUtlMemory];
        view_as<CUtlMemory>(AddressOfString(buffer)).ctor(growSize, initialAllocationCount, typeSize);
        return buffer;
    }

    // Specify the invalid ('null') index that we'll only return on failure.
    // You must specify the type size of the index used with this CUtlMemory using the indexSize parameter.
    public static int InvalidIndex(int indexSize = 4)
    {
        // BECAUSE BIT SHIFTS OVERFLOW FOR SOME REASON???????????????????????????
        // WHY IS 1 << 32 EQUAL TO 1?
        if (indexSize == 4)
            return -1;
        return (1 << (indexSize * 8)) - 1;
    }

    // Is element index valid?
    public bool IsIdxValid(int index)
    {
        return index < this.m_nAllocationCount;
    }

    // Get the first index as an iterator. When using the return value, you must use STACK_GETRETURN().
    // You must specify the type size of the index used with this CUtlMemory using the indexSize parameter.
    public STACK First(int indexSize = 4)
    {
        return GetIterator_t(this.IsIdxValid(0) ? 0 : CUtlMemory.InvalidIndex(indexSize), indexSize);
    }

    // Get the next index as an iterator. When using the return value, you must use STACK_GETRETURN().
    // You must specify the type size of the index used with this CUtlMemory using the indexSize parameter.
    public STACK Next(const SMTC_CUtlMemory_Iterator_t it, int indexSize = 4)
    {
        int index = GetIterator_tValue(it, indexSize) + 1;
        return GetIterator_t(this.IsIdxValid(index) ? index : CUtlMemory.InvalidIndex(indexSize), indexSize);
    }

    // Get the index of an iterator.
    // You must specify the type size of the index used with this CUtlMemory using the indexSize parameter.
    public any GetIndex(const SMTC_CUtlMemory_Iterator_t it, int indexSize = 4)
    {
        return GetIterator_tValue(it, indexSize);
    }

    // Check if the index is after the iterator's index.
    // You must specify the type size of the index used with this CUtlMemory using the indexSize parameter.
    public bool IsIdxAfter(any i, const SMTC_CUtlMemory_Iterator_t it, int indexSize = 4)
    {
        return i > GetIterator_tValue(it, indexSize);
    }

    // Check if the iterator has a valid index.
    // You must specify the type size of the index used with this CUtlMemory using the indexSize parameter.
    public bool IsValidIterator(const SMTC_CUtlMemory_Iterator_t it, int indexSize = 4)
    {
        return this.IsIdxValid(GetIterator_tValue(it, indexSize));
    }

    // Invalid iterator. When using the return value, you must use STACK_GETRETURN().
    // You must specify the type size of the index used with this CUtlMemory using the indexSize parameter.
    public STACK InvalidIterator(int indexSize = 4)
    {
        return GetIterator_t(CUtlMemory.InvalidIndex(indexSize), indexSize);
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

        // Allocation count > 0, shrink it down.
        this.m_pMemory = recalloc(this.m_pMemory, elements, typeSize, this.m_nAllocationCount);
        this.m_nAllocationCount = elements;
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

    // Grows the memory, so that at least allocated + num elements are allocated. 
    // You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    // You must specify the type size of the index used with this CUtlMemory using the indexSize parameter.
    public void Grow(int num = 1, int typeSize = 4, int indexSize = 4)
    {
        if (num < 1 || this.IsExternallyAllocated())
            return;
        
        // Make sure we have at least numallocated + num allocations.
        // Use the grow rules specified for this memory (in m_nGrowSize).
        int allocationRequested = this.m_nAllocationCount + num;
        int newAllocationCount = UtlMemory_CalcNewAllocationCount(this.m_nAllocationCount, this.m_nGrowSize, allocationRequested, typeSize);

        // if m_nAllocationRequested wraps index type I, recalculate
        int bitmask = CUtlMemory.InvalidIndex(indexSize);
        int bitmaskedCount = newAllocationCount & bitmask;
        if (bitmaskedCount < allocationRequested)
        {
            if (bitmaskedCount == 0 && ((newAllocationCount - 1) & bitmask) >= allocationRequested)
                --newAllocationCount; // deal w/ the common case of m_nAllocationCount == MAX_USHORT + 1
            else
            {
                if ((allocationRequested & bitmaskedCount) != allocationRequested)
                {
                    // we've been asked to grow memory to a size s.t. the index type can't address the requested amount of memory
                    return;
                }
                while ((newAllocationCount & bitmask) < allocationRequested)
                    newAllocationCount = (newAllocationCount / allocationRequested) / 2;
            }
        }

        if (this.m_pMemory)
            this.m_pMemory = recalloc(this.m_pMemory, newAllocationCount, typeSize, this.m_nAllocationCount);
        else
            this.m_pMemory = calloc(newAllocationCount, typeSize);
        this.m_nAllocationCount = newAllocationCount;
    }

    // Makes sure we've got at least this much memory. You must specify the size of the type used with this CUtlMemory using the typeSize parameter.
    public void EnsureCapacity(int num, int typeSize = 4)
    {
        if (this.m_nAllocationCount >= num || this.IsExternallyAllocated())
            return;

        if (this.m_pMemory)
            this.m_pMemory = recalloc(this.m_pMemory, num, typeSize, this.m_nAllocationCount);
        else
            this.m_pMemory = calloc(num, typeSize);
        this.m_nAllocationCount = num;
    }

    // Set the size by which the memory grows.
    public void SetGrowSize(int size)
    {
        if (this.IsExternallyAllocated() || size < 0)
            return;
        this.m_nGrowSize = size;
    }

    // Dispose this object.
    public void Dispose()
    {
        this.dtor();
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

// Allocated on the stack.
static STACK GetIterator_t(int index, int indexSize)
{
    STACK_ALLOC(iterator, Iterator_t, SIZEOF_Iterator_t);
    switch (indexSize)
    {
    case 1:
        view_as<Iterator8_t>(iterator).index = index;
    case 2:
        view_as<Iterator16_t>(iterator).index = index;
    default:
        iterator.index = index;    
    }
    STACK_RETURNREF(iterator);
}

static any GetIterator_tValue(const SMTC_CUtlMemory_Iterator_t it, int indexSize)
{
    switch (indexSize)
    {
    case 1:
        return view_as<Iterator8_t>(it).index;
    case 2:
        return view_as<Iterator16_t>(it).index;
    default:
        return view_as<Iterator_t>(it).index;
    }
}