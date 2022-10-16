//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// god

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VTABLE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VTABLE

#pragma semicolon true 
#pragma newdecls required

#if !defined SIZE_T
#define SIZE_T        int
#define SIZEOF_SIZE_T 4
#endif

#include "Pointer.inc"

//////////////////////////////////////////////////////////////////////////////
// GLOBALS                                                                  //
//////////////////////////////////////////////////////////////////////////////

enum
{
    VTABLE_INFO_OFFSET_NAME = 0,                                             // char Name[32];
    VTABLE_INFO_OFFSET_VTABLE = VTABLE_INFO_OFFSET_NAME + SIZEOF_CHAR * 32,  // void* VTable;
    VTABLE_INFO_OFFSET_DERIVED = VTABLE_INFO_OFFSET_VTABLE + SIZEOF_Pointer, // bool* Derived;
    VTABLE_INFO_OFFSET_SIZE = VTABLE_INFO_OFFSET_DERIVED + SIZEOF_Pointer,   // size_t Size;

    SIZEOF_VTable_Info = VTABLE_INFO_OFFSET_SIZE + SIZEOF_SIZE_T             // sizeof(VTable_Info);
}

static Pointer vtables = NULL;
static SIZE_T vtablesLength = 0;

//////////////////////////////////////////////////////////////////////////////
// VTABLE_INFO METHODMAP                                                    //
//////////////////////////////////////////////////////////////////////////////

methodmap VTable_Info < Pointer
{
    // Constructor - wrap around an existing address to make way for a VTable_Info object.
    public VTable_Info(Address address)
    {
        return view_as<VTable_Info>(address);
    }

    // The name of the vtable info instance.
    property Pointer Name
    {
        public get() { return view_as<Pointer>(this.Address + VTABLE_INFO_OFFSET_NAME); }
    }

    // The pointer to the vtable.
    property Pointer VTable
    {
        public get() { return this.Dereference(VTABLE_INFO_OFFSET_VTABLE); }
        public set(Pointer value) { this.Write(value, VTABLE_INFO_OFFSET_VTABLE); }
    }

    // An array depicting whether each vpointer is derived.
    property Pointer Derived
    {
        public get() { return this.Dereference(VTABLE_INFO_OFFSET_DERIVED); }
        public set(Pointer value) { this.Write(value, VTABLE_INFO_OFFSET_DERIVED); }
    }

    // The length of the vtable.
    property SIZE_T Size
    {
        public get() { return this.Dereference(VTABLE_INFO_OFFSET_SIZE); }
        public set(SIZE_T value) { this.Write(value, VTABLE_INFO_OFFSET_SIZE); }
    }

    // Call constructor on existing type.
    public void ctor(char[] name, SIZE_T size)
    {
        memcpy(this.Name, AddressOfString(name), 32);
        this.Name.Write('\0', 31, NumberType_Int8);
        this.Size = size;

        this.VTable = calloc(size, SIZEOF_Pointer);
        this.Derived = calloc(size, SIZEOF_BOOL);
    }
}

//////////////////////////////////////////////////////////////////////////////
// VTABLE FUNCTIONALITY                                                     //
//////////////////////////////////////////////////////////////////////////////

static VTable_Info GetVTableInfo(char[] name)
{
    for (int i = 0; i < vtablesLength / SIZEOF_VTable_Info; ++i)
    {
        VTable_Info vtableInfo = view_as<VTable_Info>(vtables.Get(i, SIZEOF_VTable_Info));
        char buffer[32];
        PointerToCharBuffer(vtableInfo.Name, buffer, sizeof(buffer));
        if (StrEqual(name, buffer))
            return vtableInfo;
    }
    return view_as<VTable_Info>(NULL);
}

methodmap VTable
{
    // The name must be up to 32 characters, including the NUL character. The length is the number of vpointers to be used.
    // If base is found in the registered vtables list, the default length will be set to the registered vtable length and the vtable's vpointers will be copied over to this vtable.
    public static Pointer CreateVTable(char[] name, SIZE_T length, char[] base = "")
    {
        if (StrEqual(name, ""))
            return NULL;

        if (vtables == NULL)
        {
            vtables = calloc(1, SIZEOF_VTable_Info);
            vtablesLength = SIZEOF_VTable_Info;
        }
        else
        {
            vtablesLength += SIZEOF_VTable_Info;
            vtables = recalloc(vtables, 1, vtablesLength);
        }

        VTable_Info baseVTableInfo = GetVTableInfo(base);
        if (baseVTableInfo != NULL && length < baseVTableInfo.Size)
            length = baseVTableInfo.Size;

        VTable_Info vtableInfo = view_as<VTable_Info>(vtables.Get(vtablesLength - SIZEOF_VTable_Info, 1));
        vtableInfo.ctor(name, length);

        if (baseVTableInfo != NULL)
        {
            memcpy(vtableInfo.VTable, baseVTableInfo.VTable, baseVTableInfo.Size * SIZEOF_Pointer);
            for (int i = 0; i < baseVTableInfo.Size; ++i)
            {
                if (vtableInfo.VTable.Dereference(i * SIZEOF_Pointer) != NULL)
                    vtableInfo.Derived.Write(true, i, NumberType_Int8);
            }
        }
        return vtableInfo.VTable;
    }

    // Get the vtable from the registered name.
    public static Pointer GetVTable(char[] name)
    {
        VTable_Info info = GetVTableInfo(name);
        return info != NULL ? info.VTable : NULL;
    }

    // Register a new vpointer to a registered vtable.
    public static Pointer RegisterVPointer(char[] name, Pointer asm, int asmLength, int index)
    {
        VTable_Info info = GetVTableInfo(name);
        if (info == NULL || index < 0 || index >= info.Size)
            return NULL;
        Pointer vpointer = malloc(asmLength);
        memcpy(vpointer, asm, asmLength);
        info.VTable.Write(vpointer, index * SIZEOF_Pointer);
        info.Derived.Write(false, index, NumberType_Int8);
        return vpointer;
    }

    // Remove a vpointer from a registered vtable.
    public static void RemoveVPointer(char[] name, int index)
    {
        VTable_Info info = GetVTableInfo(name);
        if (info == NULL || index < 0 || index >= info.Size)
            return;

        Pointer vpointer = info.VTable.Dereference(index * SIZEOF_Pointer);
        if (vpointer == NULL)
            return;

        if (!info.Derived.Dereference(index, NumberType_Int8))
            free(vpointer);
        info.VTable.Write(NULL, index * SIZEOF_Pointer);
        info.Derived.Write(false, index, NumberType_Int8);
    }

    // Hook a registered vtable onto an existing object.
    public static void HookOntoObject(char[] name, Pointer obj)
    {
        Pointer vtable = VTable.GetVTable(name);
        if (vtable == NULL)
            return;
        obj.Write(vtable);
    }

    // Grab the pointer to an object's vtable.
    public static Pointer GetObjectVTable(Pointer obj)
    {
        return obj.Dereference();
    }

    // Override a vpointer on an existing object with another vpointer.
    // This should not be used on vtables registered with this include, otherwise unwanted behaviour will occur. Use RemoveVPointer() and RegisterVPointer() instead.
    public static void OverrideVPointer(Pointer vtable, Pointer vpointer, int index)
    {
        if (vtable == NULL || vpointer == NULL)
            return;
        vtable.Write(vpointer, index * SIZEOF_Pointer);
    }

    // Check to see if an object is using a vtable that is registered here.
    public static bool IsObjectUsingRegisteredVTable(Pointer obj, char name[32])
    {
        if (obj == NULL)
        {
            name = "";
            return false;
        }

        Pointer vtable = VTable.GetObjectVTable(obj);
        for (int i = 0; i < vtablesLength / SIZEOF_VTable_Info; ++i)
        {
            VTable_Info vtableInfo = view_as<VTable_Info>(vtables.Get(i, SIZEOF_VTable_Info));
            if (vtableInfo.VTable == vtable)
            {
                memcpy(AddressOfString(name), vtableInfo.Name, sizeof(name));
                return true;
            }
        }

        name = "";
        return false;
    }

    // This should be called on plugin end.
    public static void ClearVTables()
    {
        // free() all registered vtable definitions.
        for (int i = 0; i < vtablesLength / SIZEOF_VTable_Info; ++i)
        {
            VTable_Info vtableInfo = view_as<VTable_Info>(vtables.Get(i, SIZEOF_VTable_Info));
            for (int v = 0; v < vtableInfo.Size; ++v)
            {
                // the vpointer will only be free()'d if it is coming from an original source.
                Pointer vpointer = vtableInfo.VTable.Dereference(v * SIZEOF_Pointer);
                if (vpointer != NULL && !vtableInfo.Derived.Dereference(v, NumberType_Int8))
                    free(vpointer);
            }
            free(vtableInfo.VTable);
            free(vtableInfo.Derived);
        }
        if (vtables != NULL)
        {
            free(vtables);
            vtables = NULL;
            vtablesLength = 0;
        }
    }
}