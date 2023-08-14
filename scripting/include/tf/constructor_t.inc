//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CONSTRUCTOR_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CONSTRUCTOR_T

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR_T DATA                                                       //
//////////////////////////////////////////////////////////////////////////////

enum
{
    CONSTRUCTOR_T_OFFSET_FLHITTIME = 0,                                           // float flHitTime;
    CONSTRUCTOR_T_OFFSET_FLVALUE = CONSTRUCTOR_T_OFFSET_FLHITTIME + SIZEOF_float, // flaot flValue;

    SIZEOF_Constructor_t = CONSTRUCTOR_T_OFFSET_FLVALUE + SIZEOF_float,           // sizeof(constructor_t);
}

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR_T METOHDMAP                                                  //
//////////////////////////////////////////////////////////////////////////////

// ?????????????????
// IT IS 22:53.
// I WILL DO A RAF AND SAM FROM X86-SECRET IF DVANDER WILL NOT CHANGE
// THIS METHODMAP ERROR! I CANNOT TOLERATE THIS! I WOULD LIEK TO NAME
// MY METYHODMAP "constructor_t" BUT I CANNOT DUE TO SOURCEMOD ACTIVITIES...
methodmap Constructor_t < Pointer
{
    // Constructor - wrap around an existing address to make way for a Constructor_t object.
    public Constructor_t(Address address)
    {
        return view_as<Constructor_t>(address);
    }

    // Time this constructor last hit me
    property float flHitTime
    {
        public get() { return this.Dereference(CONSTRUCTOR_T_OFFSET_FLHITTIME); }
        public set(float value) { this.Write(value, CONSTRUCTOR_T_OFFSET_FLHITTIME); }
    }

    // Speed value of this constructor. Defaults to 1.0, but some constructors are worth more.
    property float flValue
    {
        public get() { return this.Dereference(CONSTRUCTOR_T_OFFSET_FLVALUE); }
        public set(float value) { this.Write(value, CONSTRUCTOR_T_OFFSET_FLVALUE); }
    }

    // Constructor - allocate a new Constructor_t onto the heap. Deallocation will be required when you are finished using this using free().
    public static Constructor_t Malloc()
    {
        return malloc(SIZEOF_Constructor_t);
    }

    // Constructor - allocate a new Constructor_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_Constructor_t] = { 0 };
        return buffer;
    }
}