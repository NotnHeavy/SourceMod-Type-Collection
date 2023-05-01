//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CGLOBALVARSBASE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CGLOBALVARSBASE

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// MACROS                                                                   //
//////////////////////////////////////////////////////////////////////////////

// At the moment I don't have a methodmap for CUtlHash, nor do I have one 
// and its representing methodmaps for CSaveRestoreData, so I'll be making
// a bunch of macros representing this to fill the missing size.

#define MAX_MAP_NAME_SAVE 32
#define MAX_LEVEL_CONNECTIONS	16		// These are encoded in the lower 16bits of entitytable_t->flags

#define SIZEOF_CUtlHash (SIZEOF_CUtlVector + SIZEOF_Pointer + SIZEOF_Pointer + SIZEOF_bool * 4 + SIZEOF_int)

#define SIZEOF_CSaveRestoreSegment (SIZEOF_Pointer + SIZEOF_Pointer + SIZEOF_int + SIZEOF_int + SIZEOF_int + SIZEOF_Pointer)

#define SIZEOF_levellist_t (SIZEOF_char * MAX_MAP_NAME_SAVE + SIZEOF_char * 32 * SIZEOF_Pointer * SIZEOF_Vector)
#define SIZEOF_saverestorelevelinfo_t (SIZEOF_int + SIZEOF_levellist_t * MAX_LEVEL_CONNECTIONS + SIZEOF_int + SIZEOF_char * 20 + SIZEOF_Vector + SIZEOF_float + SIZEOF_char * MAX_MAP_NAME_SAVE + SIZEOF_int)
#define SIZEOF_CGameSaveRestoreInfo (SIZEOF_saverestorelevelinfo_t + SIZEOF_Vector + SIZEOF_int + SIZEOF_Pointer + SIZEOF_Pointer + SIZEOF_CUTlHash)

#define SIZEOF_CSaveRestoreData (SIZEOF_CSaveRestoreSegment + SIZEOF_CGameSaveRestoreInfo + SIZEOF_bool * 4)

//////////////////////////////////////////////////////////////////////////////
// CGLOBALVARSBASE DATA                                                     //
//////////////////////////////////////////////////////////////////////////////

enum
{
    CGLOBALVARSBASE_OFFSET_REALTIME = 0,                                                                             // float realtime;
    CGLOBALVARSBASE_OFFSET_FRAMECOUNT = CGLOBALVARSBASE_OFFSET_REALTIME + SIZEOF_float,                              // int framecount;
    CGLOBALVARSBASE_OFFSET_ABSOLUTEFRAMETIME = CGLOBALVARSBASE_OFFSET_FRAMECOUNT + SIZEOF_int,                       // float absoluteframetime;
    
    CGLOBALVARSBASE_OFFSET_CURTIME = CGLOBALVARSBASE_OFFSET_ABSOLUTEFRAMETIME + SIZEOF_float,                        // float curtime;
    
    CGLOBALVARSBASE_OFFSET_FRAMETIME = CGLOBALVARSBASE_OFFSET_CURTIME + SIZEOF_float,                                // float frametime;
    CGLOBALVARSBASE_OFFSET_MAXCLIENTS = CGLOBALVARSBASE_OFFSET_FRAMETIME + SIZEOF_float,                             // int maxClients;
    
    CGLOBALVARSBASE_OFFSET_TICKCOUNT = CGLOBALVARSBASE_OFFSET_MAXCLIENTS + SIZEOF_int,                               // int tickcount;
    
    CGLOBALVARSBASE_OFFSET_INTERVAL_PER_TICK = CGLOBALVARSBASE_OFFSET_TICKCOUNT + SIZEOF_int,                        // float interval_per_tick;
    
    CGLOBALVARSBASE_OFFSET_INTERPOLATION_AMOUNT = CGLOBALVARSBASE_OFFSET_INTERVAL_PER_TICK + SIZEOF_float,           // float interpolation_amount
    CGLOBALVARSBASE_OFFSET_SIMTICKSTHISFRAME = CGLOBALVARSBASE_OFFSET_INTERPOLATION_AMOUNT + SIZEOF_float,           // int simTicksThisFrame;

    CGLOBALVARSBASE_OFFSET_NETWORK_PROTOCOL = CGLOBALVARSBASE_OFFSET_SIMTICKSTHISFRAME + SIZEOF_int,                 // int network_protocol;

    // There's a gap here:
    // Here would be "CSaveRestoreData* pSaveData", however I'm not planning to work on this for the time being.
    CGLOBALVARSBASE_OFFSET_PSAVEDATA = CGLOBALVARSBASE_OFFSET_NETWORK_PROTOCOL + SIZEOF_int,                         // CSaveRestoreData* pSaveData;

    CGLOBALVARSBASE_OFFSET_M_BCLIENT = CGLOBALVARSBASE_OFFSET_PSAVEDATA + SIZEOF_Pointer,                            // bool m_bClient;
    
    CGLOBALVARSBASE_OFFSET_NTIMESTAMPNETWORKINGBASE = CGLOBALVARSBASE_OFFSET_M_BCLIENT + SIZEOF_bool * 4,            // int nTimestampNetworkingBase;
    CGLOBALVARSBASE_OFFSET_NTIMESTAMPRANDOMIZEWINDOW = CGLOBALVARSBASE_OFFSET_NTIMESTAMPNETWORKINGBASE + SIZEOF_int, // int nTimestampRandomizeWindow;

    SIZEOF_CGlobalVarsBase = CGLOBALVARSBASE_OFFSET_NTIMESTAMPRANDOMIZEWINDOW + SIZEOF_int                           // sizeof(CGlobalVarsBase);
};

//////////////////////////////////////////////////////////////////////////////
// CGLOBALVARSBASE METHODMAP                                                //
//////////////////////////////////////////////////////////////////////////////

// Global variables used by shared code.
methodmap CGlobalVarsBase < Pointer
{
    // Constructor - wrap around an existing address to make way for a CGlobalVarsBase object.
    public CGlobalVarsBase(Address address)
    {
        return view_as<CGlobalVarsBase>(address);
    }
    
    // Absolute time (per frame still - Use Plat_FloatTime() for a high precision real time 
	//  perf clock, but not that it doesn't obey host_timescale/host_framerate)
    property float realtime
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_REALTIME); }
        public set(float value) { this.Write(value, CGLOBALVARSBASE_OFFSET_REALTIME); }
    }

    // Absolute frame counter
    property int framecount
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_FRAMECOUNT); }
        public set(int value) { this.Write(value, CGLOBALVARSBASE_OFFSET_FRAMECOUNT); }
    }

    // Non-paused frametime
    property float absoluteframetime
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_ABSOLUTEFRAMETIME); }
        public set(float value) { this.Write(value, CGLOBALVARSBASE_OFFSET_ABSOLUTEFRAMETIME); }
    }

    // Current time 
	//
	// On the client, this (along with tickcount) takes a different meaning based on what
	// piece of code you're in:
	// 
	//   - While receiving network packets (like in PreDataUpdate/PostDataUpdate and proxies),
	//     this is set to the SERVER TICKCOUNT for that packet. There is no interval between
	//     the server ticks.
	//     [server_current_Tick * tick_interval]
	//
	//   - While rendering, this is the exact client clock 
	//     [client_current_tick * tick_interval + interpolation_amount]
	//
	//   - During prediction, this is based on the client's current tick:
	//     [client_current_tick * tick_interval]
    property float curtime
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_CURTIME); }
        public set(float value) { this.Write(value, CGLOBALVARSBASE_OFFSET_CURTIME); }
    }

    // Time spent on last server or client frame (has nothing to do with think intervals)
    property float frametime
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_FRAMETIME); }
        public set(float value) { this.Write(value, CGLOBALVARSBASE_OFFSET_FRAMETIME); }
    }

    // current maxplayers setting
    property int maxClients
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_MAXCLIENTS); }
        public set(int value) { this.Write(value, CGLOBALVARSBASE_OFFSET_MAXCLIENTS); }
    }

    // Simulation ticks
    property int tickcount
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_TICKCOUNT); }
        public set(int value) { this.Write(value, CGLOBALVARSBASE_OFFSET_TICKCOUNT); }
    }

    // Simulation tick interval
    property float interval_per_tick
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_INTERVAL_PER_TICK); }
        public set(float value) { this.Write(value, CGLOBALVARSBASE_OFFSET_INTERVAL_PER_TICK); }
    }

    // interpolation amount ( client-only ) based on fraction of next tick which has elapsed
    property float interpolation_amount
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_INTERPOLATION_AMOUNT); }
        public set(float value) { this.Write(value, CGLOBALVARSBASE_OFFSET_INTERPOLATION_AMOUNT); }
    }

    property int simTicksThisFrame
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_SIMTICKSTHISFRAME); }
        public set(int value) { this.Write(value, CGLOBALVARSBASE_OFFSET_SIMTICKSTHISFRAME); }
    }

    property int network_protocol
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_NETWORK_PROTOCOL); }
        public set(int value) { this.Write(value, CGLOBALVARSBASE_OFFSET_NETWORK_PROTOCOL); } 
    }

    // current saverestore data
    property Pointer pSaveData
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_PSAVEDATA); }
        public set(Pointer value) { this.Write(value, CGLOBALVARSBASE_OFFSET_NETWORK_PROTOCOL); } 
    }

    // Set to true in client code.
    property bool m_bClient
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_M_BCLIENT, NumberType_Int8); }
        public set(bool value) { this.Write(value, CGLOBALVARSBASE_OFFSET_M_BCLIENT, NumberType_Int8); } 
    }

    // 100 (i.e., tickcount is rounded down to this base and then the "delta" from this base is networked
    property int nTimestampNetworkingBase
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_NTIMESTAMPNETWORKINGBASE); }
        public set(int value) { this.Write(value, CGLOBALVARSBASE_OFFSET_NTIMESTAMPNETWORKINGBASE); } 
    }

    // 32 (entindex() % nTimestampRandomizeWindow ) is subtracted from gpGlobals->tickcount to set the networking basis, prevents
	//  all of the entities from forcing a new PackedEntity on the same tick (i.e., prevents them from getting lockstepped on this)
    property int nTimestampRandomizeWindow
    {
        public get() { return this.Dereference(CGLOBALVARSBASE_OFFSET_NTIMESTAMPRANDOMIZEWINDOW); }
        public set(int value) { this.Write(value, CGLOBALVARSBASE_OFFSET_NTIMESTAMPRANDOMIZEWINDOW); } 
    }

    public int GetNetworkBase(int nTick, int nEntity)
    {
        int nEntityMod = nEntity & this.nTimestampRandomizeWindow;
        int nBaseTick = this.nTimestampNetworkingBase * ((nTick - nEntityMod) / this.nTimestampNetworkingBase);
        return nBaseTick;
    }

    public bool IsClient()
    {
        return this.m_bClient;
    }

    // Call constructor on existing type.
    public void ctor(bool bIsClient)
    {
        this.m_bClient = bIsClient;
        this.nTimestampNetworkingBase = 100;
        this.nTimestampRandomizeWindow = 32;
    }

    // Constructor - allocate a new CGlobalVarsBase onto the heap. Deallocation will be required when you are finished using free().
    public static CGlobalVarsBase Malloc(bool bIsClient)
    {
        CGlobalVarsBase pointer = malloc(SIZEOF_CGlobalVarsBase);
        pointer.ctor(bIsClient);
        return pointer;
    }

    // Constructor - allocate a new CGlobalVarsBase onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(bool bIsClient)
    {
        char buffer[SIZEOF_CGlobalVarsBase];
        view_as<CGlobalVarsBase>(AddressOfString(buffer)).ctor(bIsClient);
        return buffer;
    }
}