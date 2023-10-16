//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CSPATIALPARTITION
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CSPATIALPARTITION

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// CLAGCOMPENSATIONMANAGER DATA                                             //
//////////////////////////////////////////////////////////////////////////////

enum
{
	PARTITION_ENGINE_SOLID_EDICTS		= (1 << 0),		// every edict_t that isn't SOLID_TRIGGER or SOLID_NOT (and static props)
	PARTITION_ENGINE_TRIGGER_EDICTS		= (1 << 1),		// every edict_t that IS SOLID_TRIGGER
	PARTITION_CLIENT_SOLID_EDICTS		= (1 << 2),
	PARTITION_CLIENT_RESPONSIVE_EDICTS	= (1 << 3),		// these are client-side only objects that respond to being forces, etc.
	PARTITION_ENGINE_NON_STATIC_EDICTS	= (1 << 4),		// everything in solid & trigger except the static props, includes SOLID_NOTs
	PARTITION_CLIENT_STATIC_PROPS		= (1 << 5),
	PARTITION_ENGINE_STATIC_PROPS		= (1 << 6),
	PARTITION_CLIENT_NON_STATIC_EDICTS	= (1 << 7),		// everything except the static props
};

//////////////////////////////////////////////////////////////////////////////
// CLAGCOMPENSATIONMANAGER METHODMAP                                        //
//////////////////////////////////////////////////////////////////////////////

// This is the spatial partition manager, groups objects into buckets.
// This just has a few methods.
methodmap CSpatialPartition < Pointer
{
    // Constructor - wrap around an existing address to make way for a CSpatialPartition object.
    public CSpatialPartition(Address address)
    {
        return view_as<CSpatialPartition>(address);
    }

    // Enumerate all elements within a box.
    public void EnumerateElementsInABox(int listMask, const any mins, const any maxs, bool coarseTest, any pIterator) // void CSpatialPartition::EnumerateElementsInBox( SpatialPartitionListMask_t listMask, const Vector& mins, const Vector& maxs, bool coarseTest, IPartitionEnumerator* pIterator )
    {
        SDKCall(SMTC_SDKCall_CSpatialPartition_EnumerateElementsInBox, this, listMask, mins, maxs, coarseTest, pIterator);
    }
}

//////////////////////////////////////////////////////////////////////////////
// LAG COMPENSATION MANAGER GLOBAL                                          //
//////////////////////////////////////////////////////////////////////////////

stock CSpatialPartition partition;