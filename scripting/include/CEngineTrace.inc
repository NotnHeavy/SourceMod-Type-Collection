//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CENGINETRACE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CENGINETRACE

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// ENGINETRACE METHODMAP                                                    //
//////////////////////////////////////////////////////////////////////////////

// The engine trace object. This is just used for organising a few function calls.
methodmap CEngineTrace < Pointer
{
    // Constructor - wrap around an existing address to make way for a CEngineTrace object.
    public CEngineTrace(Address address)
    {
        return view_as<CEngineTrace>(address);
    }

    // Traces a ray against a particular edict.
    public void ClipRayToEntity(const any ray, int fMask, any pEntity, any pTrace)
    {
        SDKCall(SMTC_SDKCall_IEngineTrace_ClipRayToEntity, this, ray, fMask, pEntity, pTrace);
    }

    // A version that simply accepts a ray (can work as a traceline or tracehull).
    public void TraceRay(const any ray, int fMask, any pTraceFilter, any pTrace)
    {
        SDKCall(SMTC_SDKCall_IEngineTrace_TraceRay, this, ray, fMask, pTraceFilter, pTrace);
    }
}

//////////////////////////////////////////////////////////////////////////////
// ENGINETRACE GLOBAL                                                       //
//////////////////////////////////////////////////////////////////////////////

stock CEngineTrace enginetrace;