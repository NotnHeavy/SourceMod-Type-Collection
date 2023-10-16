//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CLAGCOMPENSATIONMANAGER
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CLAGCOMPENSATIONMANAGER

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CLAGCOMPENSATIONMANAGER METHODMAP                                        //
//////////////////////////////////////////////////////////////////////////////

// The manager object for lag compensating entities. This just has a couple of methods.
methodmap CLagCompensationManager < Pointer
{
    // Constructor - wrap around an existing address to make way for a CLagCompensationManager object.
    public CLagCompensationManager(Address address)
    {
        return view_as<CLagCompensationManager>(address);
    }

    // Called during player movement to set up/restore after lag compensation
    public void StartLagCompensation(any player, any cmd)
    {
        SDKCall(SMTC_SDKCall_CLagCompensationManager_StartLagCompensation, this, player, cmd);
    }

    // Called during player movement to set up/restore after lag compensation
    public void FinishLagCompensation(any player)
    {
        SDKCall(SMTC_SDKCall_CLagCompensationManager_FinishLagCompensation, this, player);
    }
}

//////////////////////////////////////////////////////////////////////////////
// LAG COMPENSATION MANAGER GLOBAL                                          //
//////////////////////////////////////////////////////////////////////////////

stock CLagCompensationManager lagcompensation;