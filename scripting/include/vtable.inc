//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// god

// vtable structure:
/*
struct obj
{
    virtual int a()
    {
        return 1;
    }
    virtual int b(int var)
    {
        return var + 1;
    }

    int var1;
    int var2;
    int var3;
};

virtual functions are all part of what's called a vtable, which is constructed for every class (not object). upon instantiating an object, a reference to the vtable is stored to a pointer declared at the beginning of the object's contents.

(obj + 0): uintptr_t vtable
(obj + 4): int var1;
(obj + 8): int var2;
(obj + 12): int var3;

upon dereferencing the vtable you'd find what are called vpointers. these are pointers that then point to the beginning of the actual subroutines.
*vtable:
0x12345678 (int (__thiscall **a)(obj* pThis))
0xDEADBEEF (int (__thiscall **b)(obj* pThis, int var))

when calling these vpointers, you must dereference them.
note that class methods on MSVC use the __thiscall calling convention, otherwise it's typically __cdecl.
__cdecl passes arguments onto the stack right-to-left. the caller cleans the stack up after the function call.
the this pointer is essentially the first argument.
with __thiscall, the this pointer is actually stored in the ecx register instead. the callee cleans the stack up.
*/

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VTABLE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VTABLE

#pragma semicolon true 
#pragma newdecls required

#define FROM_EXISTING_SOURCE 0

//////////////////////////////////////////////////////////////////////////////
// GLOBALS                                                                  //
//////////////////////////////////////////////////////////////////////////////

enum
{
    VTABLE_INFO_OFFSET_NAME = 0,                                            // char Name[32];
    VTABLE_INFO_OFFSET_VTABLE = VTABLE_INFO_OFFSET_NAME + SIZEOF_CHAR * 32, // void* VTable;
    VTABLE_INFO_OFFSET_MALLOC = VTABLE_INFO_OFFSET_VTABLE + SIZEOF_Pointer, // void* Malloc;
    VTABLE_INFO_OFFSET_SIZE = VTABLE_INFO_OFFSET_MALLOC + SIZEOF_Pointer,   // size_t Size;

    SIZEOF_VTable_Info = VTABLE_INFO_OFFSET_SIZE + SIZEOF_SIZE_T            // sizeof(VTable_Info);
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

    // An array depicting containing each vpointer that is malloc()'d.
    property Pointer Malloc
    {
        public get() { return this.Dereference(VTABLE_INFO_OFFSET_MALLOC); }
        public set(Pointer value) { this.Write(value, VTABLE_INFO_OFFSET_MALLOC); }
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
        memcpyf(this.Name, AddressOfString(name), 32);
        this.Name.Write('\0', 31, NumberType_Int8);
        this.Size = size;

        this.VTable = calloc(size, SIZEOF_Pointer);
        this.Malloc = calloc(size, SIZEOF_Pointer);
    }
}

//////////////////////////////////////////////////////////////////////////////
// VTABLE FUNCTIONALITY                                                     //
//////////////////////////////////////////////////////////////////////////////

static VTable_Info GetVTableInfo(char[] name, bool countLast = true)
{
    int length = vtablesLength / SIZEOF_VTable_Info - (countLast ? 0 : SIZEOF_VTable_Info);
    for (int i = 0; i < length; ++i)
    {
        VTable_Info vtableInfo = view_as<VTable_Info>(vtables.Get(i, SIZEOF_VTable_Info));
        char buffer[32];
        vtableInfo.Name.ToCharBuffer(buffer, sizeof(buffer));
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

        VTable_Info baseVTableInfo = GetVTableInfo(base);
        if (baseVTableInfo != NULL && length < baseVTableInfo.Size)
            length = baseVTableInfo.Size;

        if (vtables == NULL)
        {
            vtables = calloc(1, SIZEOF_VTable_Info);
            vtablesLength = SIZEOF_VTable_Info;
        }
        else
        {
            vtablesLength += SIZEOF_VTable_Info;
            vtables = recalloc(vtables, vtablesLength, SIZEOF_VTable_Info, vtablesLength - SIZEOF_VTable_Info);
        }

        VTable_Info vtableInfo = view_as<VTable_Info>(vtables.Get(vtablesLength - SIZEOF_VTable_Info, 1));
        vtableInfo.ctor(name, length);

        if (baseVTableInfo != NULL)
        {
            memcpyf(vtableInfo.VTable, baseVTableInfo.VTable, baseVTableInfo.Size * SIZEOF_Pointer);
            for (int i = 0; i < baseVTableInfo.Size; ++i)
            {
                if (vtableInfo.VTable.Dereference(i * SIZEOF_Pointer) != NULL)
                    vtableInfo.Malloc.Write(NULL, i * SIZEOF_Pointer);
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
    // If asmLength is set to FROM_EXISTING_SOURCE (0), the subroutine will not be copied over.
    public static Pointer RegisterVPointer(char[] name, int index, Pointer asm, SIZE_T asmLength = FROM_EXISTING_SOURCE)
    {
        VTable_Info info = GetVTableInfo(name);
        if (info == NULL || index < 0 || index >= info.Size)
            return NULL;
        if (asmLength)
        {
            Pointer vpointer = malloc(asmLength);
            memcpyf(vpointer, asm, asmLength);
            info.VTable.Write(vpointer, index * SIZEOF_Pointer);
            info.Malloc.Write(vpointer, index * SIZEOF_Pointer);
            SetMemAccess(vpointer, asmLength, SH_MEM_READ | SH_MEM_WRITE | SH_MEM_EXEC);
            return vpointer;
        }
        else
        {
            info.VTable.Write(asm, index * SIZEOF_Pointer);
            info.Malloc.Write(NULL, index * SIZEOF_Pointer);
            return NULL;
        }
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

        if (info.Malloc.Dereference(index * SIZEOF_Pointer) == vpointer)
            free(vpointer);
        info.VTable.Write(NULL, index * SIZEOF_Pointer);
        info.Malloc.Write(NULL, index * SIZEOF_Pointer);
    }

    // Hook a registered vtable onto an existing object.
    // There will be overhead if updateMemAccess is toggled.
    public static void HookOntoObject(char[] name, Pointer obj, bool updateMemAccess = SMTC_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT)
    {
        Pointer vtable = VTable.GetVTable(name);
        if (vtable == NULL)
            return;
        obj.Write(vtable, .updateMemAccess = updateMemAccess);
    }

    // Grab the pointer to an object's vtable.
    public static Pointer GetObjectVTable(Pointer obj)
    {
        return obj.Dereference();
    }

    // Grab an object's vpointer.
    public static Pointer GetObjectVPointer(Pointer obj, int index)
    {
        return view_as<Pointer>(obj.Dereference()).Get(index);
    }

    // Override an object's vtable.
    // There will be overhead if updateMemAccess is toggled.
    public static void ReplaceObjectVTable(Pointer obj, Pointer vtable, bool updateMemAccess = SMTC_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT)
    {
        obj.Write(vtable, .updateMemAccess = updateMemAccess);
    }

    // Override a vpointer on an existing object with another vpointer.
    // There will be overhead if updateMemAccess is toggled.
    public static void OverrideVPointer(Pointer vtable, Pointer vpointer, int index, bool updateMemAccess = SMTC_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT)
    {
        if (vtable == NULL || vpointer == NULL)
            return;
        vtable.Write(vpointer, index * SIZEOF_Pointer, .updateMemAccess = updateMemAccess);
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
                memcpyf(AddressOfString(name), vtableInfo.Name, sizeof(name));
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
                // the vpointer will only be free()'d if it has been instantiated with this include.
                Pointer vpointer = vtableInfo.VTable.Dereference(v * SIZEOF_Pointer);
                if (vpointer != NULL && vtableInfo.Malloc.Dereference(v * SIZEOF_Pointer) == vpointer)
                    free(vpointer);
            }
            free(vtableInfo.VTable);
            free(vtableInfo.Malloc);
        }
        if (vtables != NULL)
        {
            free(vtables);
            vtables = NULL;
            vtablesLength = 0;
        }
    }
}