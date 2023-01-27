//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// ok seriously what even is the point of this BS methodmap name error?

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CSURFACE_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CSURFACE_T

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CSURFACE_T DATA                                                          //
//////////////////////////////////////////////////////////////////////////////

enum
{
    CSURFACE_T_OFFSET_NAME = 0,                                               // const char* name;
    CSURFACE_T_OFFSET_SURFACEPROPS = CSURFACE_T_OFFSET_NAME + SIZEOF_Pointer, // short surfaceProps;
    CSURFACE_T_OFFSET_FLAGS = CSURFACE_T_OFFSET_SURFACEPROPS + 2,             // unsigned short flags;

    SIZEOF_CSurface_t = CSURFACE_T_OFFSET_FLAGS + 2,                          // sizeof(csurface_t);
};

//////////////////////////////////////////////////////////////////////////////
// CSURFACE_T METHODMAP                                                     //
//////////////////////////////////////////////////////////////////////////////

// A struct containing information of a surface.
methodmap CSurface_t < Pointer
{
    // Constructor - wrap around an existing address to make way for a csurface_t object.
    public CSurface_t(Address address)
    {
        return view_as<CSurface_t>(address);
    }

    // Get the string pointer for the name.
    property Pointer name
    {
        public get() { return this.Dereference(CSURFACE_T_OFFSET_NAME); }
        public set(Pointer value) { this.Write(value, CSURFACE_T_OFFSET_NAME); }
    }

    // Get the surface properties.
    // This is meant to be a short, however SourcePawn uses 4-bit cells for every data type.
    property int surfaceProps
    {
        public get() { return this.Dereference(CSURFACE_T_OFFSET_SURFACEPROPS, NumberType_Int16); }
        public set(int value) { this.Write(value, CSURFACE_T_OFFSET_SURFACEPROPS, NumberType_Int16); }
    }

    // Get per surface flags.
    // This member is an unsigned short.
    property int flags
    {
        public get() { return this.Dereference(CSURFACE_T_OFFSET_FLAGS, NumberType_Int16); }
        public set(int value) { this.Write(value, CSURFACE_T_OFFSET_FLAGS, NumberType_Int16); }
    }

    // Call constructor on existing type.
    public void ctor(Pointer name = NULL, int surfaceProps = 0, int flags = 0)
    {
        this.name = name;
        this.surfaceProps = surfaceProps;
        this.flags = flags;
    }

    // Constructor - allocate a new csurface_t onto the heap. Deallocation will be required when you are finished using free().
    public static CSurface_t Malloc(Pointer name = NULL, int surfaceProps = 0, int flags = 0)
    {
        CSurface_t pointer = malloc(SIZEOF_CSurface_t);
        pointer.ctor(name, surfaceProps, flags);
        return pointer;
    }

    // Constructor - allocate a new csurface_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(Pointer name = NULL, int surfaceProps = 0, int flags = 0)
    {
        char buffer[SIZEOF_CSurface_t];
        view_as<CSurface_t>(AddressOfString(buffer)).ctor(name, surfaceProps, flags);
        return buffer;
    }
}