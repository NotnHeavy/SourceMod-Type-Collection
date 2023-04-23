//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CTFGAMERULES
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CTFGAMERULES

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CTFGAMERULES METHODMAP                                                   //
//////////////////////////////////////////////////////////////////////////////

// TF2 game rules. This is just used for organising a few function calls and does not re-construct CGameTrace.
methodmap CTFGameRules < Pointer
{
    // Constructor - wrap around an existing address to make way for a CTFGameRules object.
    public CTFGameRules(Address address)
    {
        return view_as<CTFGameRules>(address);
    }

    // Apply damage within a specific radius using a CTFRadiusDamageInfo object.
    public void RadiusDamage(any& info)
    {
        SDKCall(SMTC_SDKCall_CTFGameRules_RadiusDamage, this, info);
    }
}

//////////////////////////////////////////////////////////////////////////////
// CTFGAMERULES GLOBAL                                                      //
//////////////////////////////////////////////////////////////////////////////

stock CTFGameRules g_pGameRules;