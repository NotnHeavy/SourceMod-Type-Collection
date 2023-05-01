//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// Requires "CGlobalVarsBase.inc".

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CGLOBALVARS
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CGLOBALVARS

#pragma semicolon true 
#pragma newdecls required

#include "CGlobalVarsBase.inc"

//////////////////////////////////////////////////////////////////////////////
// CGLOBALVARS DATA                                                         //
//////////////////////////////////////////////////////////////////////////////

enum MapLoadType_t
{
	MapLoad_NewGame = 0,
	MapLoad_LoadGame,
	MapLoad_Transition,
	MapLoad_Background,
};

enum
{
    CGLOBALVARS_OFFSET_MAPNAME = SIZEOF_CGlobalVarsBase,                                 // string_t mapname;
    CGLOBALVARS_OFFSET_MAPVERSION = CGLOBALVARS_OFFSET_MAPNAME + SIZEOF_String_t,        // int mapversion;
    CGLOBALVARS_OFFSET_STARTSPOT = CGLOBALVARS_OFFSET_MAPVERSION + SIZEOF_int,           // string_t startspot;
    CGLOBALVARS_OFFSET_ELOADTYPE = CGLOBALVARS_OFFSET_STARTSPOT + SIZEOF_String_t,       // MapLoadType_t eLoadType;
    CGLOBALVARS_OFFSET_BMAPLOADFAILED = CGLOBALVARS_OFFSET_ELOADTYPE + SIZEOF_int,       // bool bMapLoadFailed;

    CGLOBALVARS_OFFSET_DEATHMATCH = CGLOBALVARS_OFFSET_BMAPLOADFAILED + SIZEOF_bool,     // bool deathmatch;
    CGLOBALVARS_OFFSET_COOP = CGLOBALVARS_OFFSET_DEATHMATCH + SIZEOF_bool,               // bool coop;
    CGLOBALVARS_OFFSET_TEAMPLAY = CGLOBALVARS_OFFSET_COOP + SIZEOF_bool,                 // bool teamplay;
    CGLOBALVARS_OFFSET_MAXENTITIES = CGLOBALVARS_OFFSET_TEAMPLAY + SIZEOF_bool,          // int maxEntities;
    
    CGLOBALVARS_OFFSET_SERVERCOUNT = CGLOBALVARS_OFFSET_MAXENTITIES + SIZEOF_int,        // int serverCount;

    SIZEOF_CGlobalVars = CGLOBALVARS_OFFSET_SERVERCOUNT + SIZEOF_int,                    // sizeof(CGlobalVars);
};

stock CGlobalVars gpGlobals = NULL;

//////////////////////////////////////////////////////////////////////////////
// CGLOBALVARSBASE METHODMAP                                                //
//////////////////////////////////////////////////////////////////////////////

// Global variables shared between the engine and the game .dll
methodmap CGlobalVars < CGlobalVarsBase
{
    // Constructor - wrap around an existing address to make way for a CGlobalVars object.
    public CGlobalVars(Address address)
    {
        return view_as<CGlobalVars>(address);
    }

    // Current map
    property String_t mapname
    {
        public get() { return view_as<String_t>(this.Address + CGLOBALVARS_OFFSET_MAPNAME); }
        public set(String_t value) { memcpy(this.Address + CGLOBALVARS_OFFSET_MAPNAME, value, SIZEOF_String_t); }
    }

    property int mapversion
    {
        public get() { return this.Dereference(CGLOBALVARS_OFFSET_MAPVERSION); }
        public set(int value) { this.Write(value, CGLOBALVARS_OFFSET_MAPVERSION); }
    }

    property String_t startspot
    {
        public get() { return view_as<String_t>(this.Address + CGLOBALVARS_OFFSET_STARTSPOT); }
        public set(String_t value) { memcpy(this.Address + CGLOBALVARS_OFFSET_STARTSPOT, value, SIZEOF_String_t); }
    }

    // How the current map was loaded
    property MapLoadType_t eLoadType
    {
        public get() { return this.Dereference(CGLOBALVARS_OFFSET_ELOADTYPE); }
        public set(MapLoadType_t value) { this.Write(value, CGLOBALVARS_OFFSET_ELOADTYPE); }
    }

    // Map has failed to load, we need to kick back to the main menu
    property bool bMapLoadFailed
    {
        public get() { return this.Dereference(CGLOBALVARS_OFFSET_BMAPLOADFAILED, NumberType_Int8); }
        public set(bool value) { this.Write(value, CGLOBALVARS_OFFSET_MAPVERSION, NumberType_Int8); }
    }

    // game specific flags
    property bool deathmatch
    {
        public get() { return this.Dereference(CGLOBALVARS_OFFSET_DEATHMATCH, NumberType_Int8); }
        public set(bool value) { this.Write(value, CGLOBALVARS_OFFSET_DEATHMATCH, NumberType_Int8); }
    }

    property bool coop
    {
        public get() { return this.Dereference(CGLOBALVARS_OFFSET_COOP, NumberType_Int8); }
        public set(bool value) { this.Write(value, CGLOBALVARS_OFFSET_COOP, NumberType_Int8); }
    }

    property bool teamplay
    {
        public get() { return this.Dereference(CGLOBALVARS_OFFSET_TEAMPLAY, NumberType_Int8); }
        public set(bool value) { this.Write(value, CGLOBALVARS_OFFSET_TEAMPLAY, NumberType_Int8); }
    }

    // current maxentities
    property int maxEntities
    {
        public get() { return this.Dereference(CGLOBALVARS_OFFSET_MAXENTITIES); }
        public set(int value) { this.Write(value, CGLOBALVARS_OFFSET_MAXENTITIES); }
    }

    property int serverCount
    {
        public get() { return this.Dereference(CGLOBALVARS_OFFSET_SERVERCOUNT); }
        public set(int value) { this.Write(value, CGLOBALVARS_OFFSET_SERVERCOUNT); }
    }

    // Call constructor on existing type.
    public void ctor(bool bIsClient)
    {
        view_as<CGlobalVarsBase>(this).ctor(bIsClient);
        this.serverCount = 0;
    }

    // Constructor - allocate a new CGlobalVars onto the heap. Deallocation will be required when you are finished using free().
    public static CGlobalVars Malloc(bool bIsClient)
    {
        CGlobalVars pointer = malloc(SIZEOF_CGlobalVars);
        pointer.ctor(bIsClient);
        return pointer;
    }

    // Constructor - allocate a new CGlobalVars onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(bool bIsClient)
    {
        char buffer[SIZEOF_CGlobalVars];
        view_as<CGlobalVars>(AddressOfString(buffer)).ctor(bIsClient);
        return buffer;
    }
}