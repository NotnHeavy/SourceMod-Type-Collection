//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// TODO: define CUserCmd::GetChecksum()?

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUSERCMD
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CUSERCMD

#pragma semicolon true 
#pragma newdecls required

// Requires "QAngle.inc".
#include "QAngle.inc"

//////////////////////////////////////////////////////////////////////////////
// CUSERCMD DATA                                                            //
//////////////////////////////////////////////////////////////////////////////

// TF2 specific, need enough space for OBJ_LAST items from tf_shareddefs.h
#define WEAPON_SUBTYPE_BITS	6

enum
{
    CUSERCMD_OFFSET_VTABLE = 0,                                                     // void* vtable;

    CUSERCMD_OFFSET_COMMAND_NUMBER = CUSERCMD_OFFSET_VTABLE + SIZEOF_Pointer,       // int command_number;
    CUSERCMD_OFFSET_TICK_COUNT = CUSERCMD_OFFSET_COMMAND_NUMBER + SIZEOF_int,       // int tick_count;

    CUSERCMD_OFFSET_VIEWANGLES = CUSERCMD_OFFSET_TICK_COUNT + SIZEOF_int,           // QAngle viewangles;
    CUSERCMD_OFFSET_FORWARDMOVE = CUSERCMD_OFFSET_VIEWANGLES + SIZEOF_QAngle,       // float forwardmove;
    CUSERCMD_OFFSET_SIDEMOVE = CUSERCMD_OFFSET_FORWARDMOVE + SIZEOF_float,          // float sidemove;
    CUSERCMD_OFFSET_UPMOVE = CUSERCMD_OFFSET_SIDEMOVE + SIZEOF_float,               // float upmove;
    CUSERCMD_OFFSET_BUTTONS = CUSERCMD_OFFSET_UPMOVE + SIZEOF_float,                // int buttons;
    CUSERCMD_OFFSET_IMPULSE = CUSERCMD_OFFSET_BUTTONS + SIZEOF_int,                 // byte impulse;
    CUSERCMD_OFFSET_WEAPONSELECT = CUSERCMD_OFFSET_IMPULSE + 4,                     // int weaponselect;
    CUSERCMD_OFFSET_WEAPONSUBTYPE = CUSERCMD_OFFSET_WEAPONSELECT + SIZEOF_int,      // int weaponsubtype;
    
    CUSERCMD_OFFSET_RANDOM_SEED = CUSERCMD_OFFSET_WEAPONSUBTYPE + SIZEOF_int,       // int random_seed;
    CUSERCMD_OFFSET_SERVER_RANDOM_SEED = CUSERCMD_OFFSET_RANDOM_SEED + SIZEOF_int,  // int server_random_seed;

    CUSERCMD_OFFSET_MOUSEDX = CUSERCMD_OFFSET_SERVER_RANDOM_SEED + SIZEOF_int,      // short mousedx;
    CUSERCMD_OFFSET_MOUSEDY = CUSERCMD_OFFSET_MOUSEDX + SIZEOF_SHORT,               // short mousedy;

    CUSERCMD_OFFSET_HASBEENPREDICTED = CUSERCMD_OFFSET_MOUSEDY + SIZEOF_SHORT,      // bool hasbeenpredicted;

    SIZEOF_CUserCmd = CUSERCMD_OFFSET_HASBEENPREDICTED + 4                          // sizeof(CUserCmd);
};

//////////////////////////////////////////////////////////////////////////////
// CUSERCMD METHODMAP                                                       //
//////////////////////////////////////////////////////////////////////////////

methodmap CUserCmd < Pointer
{
    // Constructor - wrap around an existing address to make way for a CUserCmd object.
    public CUserCmd(Address address)
    {
        return view_as<CUserCmd>(address);
    }

    // CUserCmd vtable.
    property Pointer vtable
    {
        public get() { return this.Dereference(TF_POINT_T_OFFSET_VTABLE); }
        public set(Pointer value) { this.Write(value, TF_POINT_T_OFFSET_VTABLE); }
    }

    // For matching server and client commands for debugging
    property int command_number
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_COMMAND_NUMBER); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_COMMAND_NUMBER); }
    }

    // the tick the client created this command
    property int tick_count
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_TICK_COUNT); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_TICK_COUNT); }
    }

    // Player instantaneous view angles.
    property QAngle viewangles
    {
        public get() { return view_as<any>(this) + CUSERCMD_OFFSET_VIEWANGLES; }
        public set(QAngle value) { memcpy(view_as<any>(this) + CUSERCMD_OFFSET_VIEWANGLES, value, SIZEOF_Vector); }
    }

    // forward velocity.
    property float forwardmove
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_FORWARDMOVE); }
        public set(float value) { this.Write(value, CUSERCMD_OFFSET_FORWARDMOVE); }
    }

    // sideways velocity.
    property float sidemove
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_SIDEMOVE); }
        public set(float value) { this.Write(value, CUSERCMD_OFFSET_SIDEMOVE); }
    }

    // upward velocity.
    property float upmove
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_UPMOVE); }
        public set(float value) { this.Write(value, CUSERCMD_OFFSET_UPMOVE); }
    }

    // Attack button states
    property int buttons
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_BUTTONS); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_BUTTONS); }
    }

    // Impulse command issued.
    property int impulse
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_IMPULSE, NumberType_Int8); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_IMPULSE, NumberType_Int8); }
    }

    // Current weapon id
    property int weaponselect
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_WEAPONSELECT); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_WEAPONSELECT); }
    }

    property int weaponsubtype
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_WEAPONSUBTYPE); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_WEAPONSUBTYPE); }
    }

    // For shared random functions
    property int random_seed
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_RANDOM_SEED); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_RANDOM_SEED); }
    }

    // Only the server populates this seed
    property int server_random_seed
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_SERVER_RANDOM_SEED); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_SERVER_RANDOM_SEED); }
    }

    // mouse accum in x from create move
    property int mousedx
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_MOUSEDX, NumberType_Int16); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_MOUSEDX, NumberType_Int16); }
    }

    // mouse accum in y from create move
    property int mousedy
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_MOUSEDY, NumberType_Int16); }
        public set(int value) { this.Write(value, CUSERCMD_OFFSET_MOUSEDY, NumberType_Int16); }
    }

    // Client only, tracks whether we've predicted this command at least once
    property bool hasbeenpredicted
    {
        public get() { return this.Dereference(CUSERCMD_OFFSET_HASBEENPREDICTED, NumberType_Int8); }
        public set(bool value) { this.Write(value, CUSERCMD_OFFSET_HASBEENPREDICTED, NumberType_Int8); }
    }

    // Reset the members of this function.
    public void Reset()
    {
        this.command_number = 0;
        this.tick_count = 0;
        this.viewangles.X = 0.00;
        this.viewangles.Y = 0.00;
        this.viewangles.Z = 0.00;
        this.forwardmove = 0.00;
        this.sidemove = 0.00;
        this.upmove = 0.00;
        this.buttons = 0;
        this.impulse = 0;
        this.weaponselect = 0;
        this.weaponsubtype = 0;
        this.random_seed = 0;
        this.server_random_seed = 0;
        this.mousedx = 0;
        this.mousedy = 0;
        this.hasbeenpredicted = false;
    }

    // Allow command, but negate gameplay-affecting values
    public void MakeInert()
    {
        this.viewangles = vec3_angle;
        this.forwardmove = 0.00;
        this.sidemove = 0.00;
        this.upmove = 0.00;
        this.buttons = 0;
        this.impulse = 0;
    }

    // Call constructor on existing type.
    public void ctor(const CUserCmd src = NULL)
    {
        if (src)
            memcpy(this, src, SIZEOF_CUserCmd);
        else
        {
            this.vtable = SMTC_CUserCmd_vtable;
            this.Reset();
        }
    }

    // Constructor - allocate a new CUserCmd onto the heap. Deallocation will be required when you are finished using free().
    public static CUserCmd Malloc()
    {
        CUserCmd pointer = malloc(SIZEOF_CUserCmd);
        pointer.ctor();
        return pointer;
    }

    // Constructor - allocate a new CUserCmd onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc()
    {
        char buffer[SIZEOF_CUserCmd];
        view_as<CUserCmd>(AddressOfString(buffer)).ctor();
        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////////
// MISCELLANEOUS                                                            //
//////////////////////////////////////////////////////////////////////////////

// TODO (maybe):
/*
void ReadUsercmd( bf_read *buf, CUserCmd *move, CUserCmd *from );
void WriteUsercmd( bf_write *buf, const CUserCmd *to, const CUserCmd *from );
*/