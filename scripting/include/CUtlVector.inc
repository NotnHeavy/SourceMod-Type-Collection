//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// this is suicide

// Requires "CUtlMemory.inc".

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLVECTOR
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUTLVECTOR

#include "CUtlMemory.inc"

#pragma semicolon true
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CUTLVECTOR DATA                                                          //
//////////////////////////////////////////////////////////////////////////////

enum
{
    CUTLVECTOR_OFFSET_M_MEMORY = 0,                                            // CAllocator m_Memory; (where CAllocator = CUtlMemory<T>, where T = CUtlVector<T>)
    CUTLVECTOR_OFFSET_M_SIZE = CUTLVECTOR_OFFSET_M_MEMORY + SIZEOF_CUtlMemory, // int m_Size;
    CUTLVECTOR_OFFSET_M_PELEMENTS = CUTLVECTOR_OFFSET_M_SIZE + SIZEOF_Pointer, // T* m_pElements; (where T = CUtlVector<T>)
    
    SIZEOF_CUtlVector = CUTLVECTOR_OFFSET_M_PELEMENTS + 4                      // sizeof(CUtlVector);
}


//////////////////////////////////////////////////////////////////////////////
// CUTLVECTOR METHODMAP                                                     //
//////////////////////////////////////////////////////////////////////////////

// A growable array which doubles in size by default.
methodmap CUtlVector < Pointer
{
    // Constructor - wrap around an existing address to make way for a CUtlVector object.
    public CUtlVector(Address address)
    {
        return view_as<CUtlVector>(address);
    }

    // The allocator.
    property CUtlMemory m_Memory
    {
        public get() { return view_as<CUtlMemory>(this.Address + CUTLVECTOR_OFFSET_M_MEMORY); }
        public set(CUtlMemory allocator) { this.Write(allocator, CUTLVECTOR_OFFSET_M_MEMORY); }
    }

    // The CUtlVector's size.
    property int m_Size
    {
        public get() { return this.Dereference(CUTLVECTOR_OFFSET_M_SIZE); }
        public set(int value) { this.Write(value, CUTLVECTOR_OFFSET_M_SIZE); }
    }

    // For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
    property Pointer m_pElements
    {
        public get() { return this.Dereference(CUTLVECTOR_OFFSET_M_PELEMENTS); }
        public set(Pointer pointer) { this.Write(pointer, CUTLVECTOR_OFFSET_M_PELEMENTS); }
    }

    // Gets the base address (can change when adding elements!)
    public Pointer Base()
    {
        return this.m_Memory.Base();
    }

    // Set this.m_pElements to starting point.
    public void ResetDbgInfo()
    {
        this.m_pElements = this.Base();
    }

    // Call constructor on existing type. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void ctor(int growSize = 0, int initialCapacity = 0, int typeSize = 4)
    {
        this.m_Memory.ctor(growSize, initialCapacity, typeSize);
        this.m_Size = 0;
        this.ResetDbgInfo();
    }

    // Call deconstructor on existing type.
    public void dtor()
    {
        this.Purge();
    }

    // Constructor - allocate a new CUtlVector onto the heap. Deallocation will be required when you are finished using CUtlVector.Dispose().
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public static CUtlVector Malloc(int growSize = 0, int initialCapacity = 0, int typeSize = 4)
    {
        CUtlVector pointer = malloc(SIZEOF_CUtlVector);
        pointer.ctor(growSize, initialCapacity, typeSize);
        return pointer;
    }

    // Constructor - allocate a new CUtlVector onto the stack. When using the return value, you must use STACK_GETRETURN().
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public static STACK StackAlloc(int growSize = 0, int initialCapacity = 0, int typeSize = 4)
    {
        char buffer[SIZEOF_CUtlVector];
        view_as<CUtlVector>(AddressOfString(buffer)).ctor(growSize, initialCapacity, typeSize);
        return buffer;
    }

    // Element access - get from specified index. Returns a pointer on success, returns NULL on failure.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // You can use the pointer methodmap in order to use read/write operations.
    public Pointer Get(int index, int typeSize = 4)
    {
        return this.m_Memory.Get(index, typeSize);
    }

    // Same as CUtlVector.Get().
    public Pointer Element(int index, int typeSize = 4)
    {
        return this.m_Memory.Get(index, typeSize);
    }

    // Element access - get from the start. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // You can use the pointer methodmap in order to use read/write operations.
    public Pointer Head(int typeSize = 4)
    {
        return this.m_Memory.Get(0, typeSize);
    }

    // Element access - get from the end. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // You can use the pointer methodmap in order to use read/write operations.
    public Pointer Tail(int typeSize = 4)
    {
        return this.m_Memory.Get(this.m_Size - 1, typeSize);
    }

    // Returns the number of elements in the vector.
    public int Count()
    {
        return this.m_Size;
    }

    // Are there no elements?
    public bool IsEmpty()
    {
        return this.Count() == 0;
    }

    // Invalid index.
    public static int InvalidIndex()
    {
        return -1;
    }
    
    // Is element index valid?
    public bool IsValidIndex(int i)
    {
        return i >= 0 && i < this.m_Size;
    }

    // Grows the vector. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void GrowVector(int num = 1, int typeSize = 4)
    {
        if (this.m_Size + num > this.m_Memory.NumAllocated())
            this.m_Memory.Grow(this.m_Size + num - this.m_Memory.NumAllocated(), typeSize);
        
        this.m_Size += num;
        this.ResetDbgInfo();
    }

    // Shifts elements to the left. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void ShiftElementsLeft(int elem, int num = 1, int typeSize = 4)
    {
        if (!(this.IsValidIndex(num) || this.m_Size == 0 || num == 0))
            return;

        int numToMove = this.m_Size - elem - num;
        if (numToMove > 0 && num > 0)
        {
            memmove(this.Element(elem, typeSize), this.Element(elem + num, typeSize), numToMove * typeSize);
        }
    }

    // Shifts elements to the right. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void ShiftElementsRight(int elem, int num = 1, int typeSize = 4)
    {
        if (!(this.IsValidIndex(num) || this.m_Size == 0 || num == 0))
            return;

        int numToMove = this.m_Size - elem - num;
        if (numToMove > 0 && num > 0)
        {
            memmove(this.Element(elem + num, typeSize), this.Element(elem, typeSize), numToMove * typeSize);
        }
    }

    // Adds an element. Returns this.InvalidIndex() (-1) on failure, returns index provided on success.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public int InsertBefore(int index, int typeSize = 4)
    {
        // Can insert at the end.
        if (!(index == this.Count() || this.IsValidIndex(index)))
            return CUtlVector.InvalidIndex();
        
        this.GrowVector();
        this.ShiftElementsRight(index, .typeSize = typeSize);
        Construct(this.Element(index), typeSize);
        
        return index;
    }

    // Adds an element. Returns this.InvalidIndex() (-1) on failure, returns index provided on success.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public int InsertAfter(int index, int typeSize = 4)
    {
        return this.InsertBefore(index + 1, typeSize);
    }

    // Adds an element. Returns this.InvalidIndex() (-1) on failure, returns index provided on success.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public int AddToHead(int typeSize = 4)
    {
        return this.InsertBefore(0, typeSize);
    }

    // Adds an element. Returns this.InvalidIndex() (-1) on failure, returns index provided on success.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public int AddToTail(int typeSize = 4)
    {
        return this.InsertBefore(this.m_Size, typeSize);
    }

    // Adds an element. Returns NULL on failure, returns pointer to new element on success.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public Pointer AddToTailGetPtr(int typeSize = 4)
    {
        return this.Element(this.AddToTail(typeSize), typeSize);
    }

    // Initializes elements before a specified index. Returns this.InvalidIndex(-1) on failure, returns index provided on success or if num == 0.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public int InsertMultipleBefore(int elem, int num, int typeSize = 4)
    {
        if (num == 0)
            return elem;

        // Can insert at the end.
        if (!(elem == this.Count() || this.IsValidIndex(elem)))
            return CUtlVector.InvalidIndex();

        this.GrowVector(num);
        this.ShiftElementsRight(elem, num, typeSize);

        // Invoke default constructors.
        for (int i = 0; i < num; ++i)
            Construct(this.Element(elem + i));

        return elem;
    }

    // Copies from one source to the other, before a specified index. Returns this.InvalidIndex(-1) on failure, returns index provided on success or if num == 0.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // Meant to be a function overload of InsertMultipleBefore, but that's not possible in SourceMod.
    public int CopyFromBefore(int elem, int num, const Pointer toInsert, int typeSize = 4)
    {
        if (num == 0)
            return elem;

        // Can insert at the end.
        if (!(elem == this.Count() || this.IsValidIndex(elem)))
            return INVALID_INDEX;

        this.GrowVector(num);
        this.ShiftElementsRight(elem, num, typeSize);

        // Invoke default constructors.
        if (toInsert == NULL)
        {
            for (int i = 0; i < num; ++i)
                Construct(this.Element(elem + i));
        }
        else
        {
            for (int i = 0; i < num; ++i)
                CopyConstruct(this.Element(elem + i, typeSize), toInsert.Get(i, typeSize), typeSize);
        }

        return elem;
    }

    // Initializes elements before a specified index. Returns this.InvalidIndex(-1) on failure, returns index provided on success or if num == 0.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public int AddMultipleToHead(int num, int typeSize = 4)
    {
        return this.InsertMultipleBefore(0, num, typeSize);
    }

    // Initializes elements before a specified index. Returns this.InvalidIndex(-1) on failure, returns index provided on success or if num == 0.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public int AddMultipleToTail(int num, int typeSize = 4)
    {
        return this.InsertMultipleBefore(this.m_Size, num, typeSize);
    }

    // Initializes elements before a specified index. Returns this.InvalidIndex(-1) on failure, returns index provided on success or if num == 0.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public int InsertMultipleAfter(int elem, int num, int typeSize = 4)
    {
        return this.InsertMultipleBefore(elem + 1, num, typeSize);
    }

    // Finds an element. Returns the index if found. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public int Find(const Pointer source, int typeSize = 4)
    {
        for (int i = 0; i < this.Count(); ++i)
        {
            if (memcmp(this.Element(i, typeSize), source, typeSize) == 0)
                return i;
        }
        return CUtlVector.InvalidIndex();
    }

    // Element removal. Doesn't preserve order.
    // Internally this code would call the deconstructor on allocated memory, but that's not possible here.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void FastRemove(int elem, int typeSize = 4)
    {
        if (!this.IsValidIndex(elem))
            return;

        if (this.m_Size > 0)
        {
            if (elem != this.m_Size - 1)
            {
                memcpy(this.Element(elem), this.Element(this.m_Size - 1), typeSize);
            }
            --this.m_Size;
        }
    }

    // Element removal. Preserves order, shifts elements. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // Internally this code would call the deconstructor on allocated memory, but that's not possible here.
    public void Remove(int elem, int typeSize = 4)
    {
        this.ShiftElementsLeft(elem, .typeSize = typeSize);
        --this.m_Size;
    }

    // Element removal. Removes first occurence of source, preserves order, shifts elements.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // Internally this code would call the deconstructor on allocated memory, but that's not possible here.
    public bool FindAndRemove(const Pointer source, int typeSize = 4)
    {
        int elem = this.Find(source, typeSize);
        if (elem != -1)
        {
            this.Remove(elem);
            return true;
        }
        return false;
    }

    // Element removal. Removes first occurence of source, doesn't preserve order.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // Internally this code would call the deconstructor on allocated memory, but that's not possible here.
    public bool FindAndFastRemove(const Pointer source, int typeSize = 4)
    {
        int elem = this.Find(source, typeSize);
        if (elem != -1)
        {
            this.FastRemove(elem);
            return true;
        }
        return false;
    }

    // Element removal. Preserves order, shifts elements. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // Internally this code would call the deconstructor on allocated memory, but that's not possible here.
    public void RemoveMultiple(int elem, int num, int typeSize = 4)
    {
        if (!(elem >= 0 || elem + num <= this.Count()))
            return;

        this.ShiftElementsLeft(elem, num, typeSize);
        this.m_Size -= num;
    }

    // Element removal. Removes num elements from head. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // Internally this code would call the deconstructor on allocated memory, but that's not possible here.
    public void RemoveMultipleFromHead(int num, int typeSize = 4)
    {
        if (!(num <= this.Count()))
            return;
        
        this.ShiftElementsLeft(0, num, typeSize);
        this.m_Size -= num;
    }

    // Element removal. Removes num elements from tail.
    // Internally this code would call the deconstructor on allocated memory, but that's not possible here.
    public void RemoveMultipleFromTail(int num)
    {
        if (!(num <= this.Count()))
            return;
        this.m_Size -= num;
    }

    // Element removal. Doesn't de-allocate memory.
    // Internally this code would call the deconstructor on allocated memory, but that's not possible here.
    public void RemoveAll()
    {
        // Normal code is the following, but you can't call deconstructors on the fly in SourceMod.
        /*
        for (int i = m_Size; --i >= 0; )
        {
            // Global scope to resolve conflict with Scaleform 4.0
            ::Destruct(&Element(i));
        }
        */

        this.m_Size = 0;
    }

    // SetCount deletes the previous contents of the container and sets the container to have this many elements.
	// Use GetCount to retrieve the current count.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void SetCount(int count, int typeSize = 4)
    {
        this.RemoveAll();
        this.AddMultipleToTail(count, typeSize);
    }

    // Sets count by adding or removing elements to tail. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void SetCountNonDestructively(int count, int typeSize = 4)
    {
        int delta = count - this.m_Size;
        if (delta > 0)
            this.AddMultipleToTail(delta, typeSize);
        else if (delta < 0)
            this.RemoveMultipleFromTail(-delta);
    }

    // Calls RemoveAll() then AddMultipleToTail.
    // SetSize is a synonym for SetCount.
    // You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void SetSize(int size, int typeSize = 4)
    {
        this.SetCount(size, typeSize);
    }

    // Calls SetSize and copies each element. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void CopyArray(const Pointer array, int size, int typeSize = 4)
    {
        // Can't insert something that's in the list... reallocation may hose us.
        if (!(this.Base() == NULL || array == NULL || this.Base() >= array + size || array >= this.Base() + this.Count()))
            return;

        this.SetSize(size);
        memcpy(this, array, size * typeSize);
    }

    // Fast swap.
    public void Swap(CUtlVector vector)
    {
        // nightmare
        this.m_Memory.Swap(vector.m_Memory);
        V_swap(view_as<Pointer>(this) + CUTLVECTOR_OFFSET_M_SIZE, view_as<Pointer>(vector) + CUTLVECTOR_OFFSET_M_SIZE);
        V_swap(view_as<Pointer>(this) + CUTLVECTOR_OFFSET_M_PELEMENTS, view_as<Pointer>(vector) + CUTLVECTOR_OFFSET_M_PELEMENTS);
    }

    // Makes sure we have enough memory allocated to store a requested # of elements.
	// Use NumAllocated() to retrieve the current capacity.
    public void EnsureCapacity(int num)
    {
        this.m_Memory.EnsureCapacity(num);
        this.ResetDbgInfo();
    }

    // Makes sure we have at least this many elements.
	// Use GetCount to retrieve the current count.
    public void EnsureCount(int num)
    {
        if (this.Count() < num)
            this.AddMultipleToTail(num - this.Count());
    }

    // Add the specified array to the tail. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    // Returns previous count on success, returns -1 on failure.
    public int AddVectorToTail(const CUtlVector vector, int typeSize = 4)
    {
        if (vector == this)
            return CUtlVector.InvalidIndex();

        int base = this.Count();

        // Make space.
        int sourceCount = vector.Count();
        this.EnsureCapacity(base + sourceCount);

        // Copy the elements.
        this.m_Size += sourceCount;
        for (int i = 0; i < sourceCount; ++i)
            CopyConstruct(this.Element(base + i, typeSize), vector.Element(base + i, typeSize), typeSize);
        return base;
    }

    // Fill up this CUtlVector with a specific value. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void FillWithValue(const Pointer value, int typeSize = 4)
    {
        for (int i = 0; i < this.Count(); ++i)
        {
            memcpy(this.Element(i, typeSize), value, typeSize);
        }
    }

    // Does this CUtlVector have the following element? You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public bool HasElement(const Pointer source, int typeSize = 4)
    {
        return this.Find(source, typeSize) >= 0;
    }

    // Memory deallocation.
    public void Purge()
    {
        this.RemoveAll();
        this.m_Memory.Purge();
        this.ResetDbgInfo();
    }

    // Purges the list and calls delete on each element in it. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void PurgeAndDeleteElements(int typeSize = 4)
    {
        for (int i = 0; i < this.m_Size; ++i)
        {
            free(this.Element(i, typeSize));
        }
        this.Purge();
    }

    // Compacts the vector to the number of elements actually in use.
    public void Compact()
    {
        this.m_Memory.PurgeButGiven(this.m_Size);
    }

    // Only use this if you really know what you're doing!
    public int NumAllocated()
    {
        return this.m_Memory.NumAllocated();
    }

    // Reverse the order of elements. You must specify the size of the type used with this CUtlVector using the typeSize parameter.
    public void Reverse(int typeSize = 4)
    {
        for (int i = 0; i < this.m_Size / 2; ++i)
            V_swap(this.m_Memory.Get(i, typeSize), this.m_Memory.Get(this.m_Size - 1 - i, typeSize), typeSize);
    }

    // Dispose this object.
    public void Dispose()
    {
        this.dtor();
        free(this);
    }
}