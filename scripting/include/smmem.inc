// I'm insane enough to modify someone else's header.
// This is slight tweaks based upon Scag's SM-Memory header, 
// specialised for SMTC.

#if defined _smmem_included_
  #endinput
#endif
#define _smmem_included_

#define SH_MEM_READ		(1 << 0)
#define SH_MEM_WRITE	(1 << 1)
#define SH_MEM_EXEC 	(1 << 2)

#define SMTC_SMMEM_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT true

native any Calloc(int num, int size);
native any Malloc(int size);
native any Realloc(any p, int size);
native any ReallocF(any p, int size);
native void Free(any p);
native void FreeF(any p);
native void MemMove(any dest, any src, int size);
native void MemMoveF(any dest, any src, int size);
native any MemCopy(any dest, any src, int size);
native any MemCopyF(any dest, any src, int size);
native int MemCmp(any p1, any p2, int size);
native int MemCmpF(any p1, any p2, int size);
native void MemSet(any p, int val, int size);
native void MemSetF(any p, int val, int size);

/**
 *  Emit a sequence of asm.
 *  Note: You're better off creating a static SDKCall with the address being your own pointer
 *  to assembly code. 
 *
 *  @param bytes            Array of assembly to emit.
 *  @param len              Size of the array.
 *  @param customcleanup    If true, you are responsible for cleaning up the function call.
 *                          Otherwise, the native will do it for you. If you're passing params,
 *                          you must clean them up yourself (Set this to true).
 *  @param ...              Optional parameters to add in case this is a function call.
 *                          Note that you're most likely better off creating your own SDKCall.
 *                          This exists solely for the pedantic and crazy.
 *
 *  @return                 Whatever you tell it to return.
*/
#pragma deprecated Use a static SDKCall with a pointer to your assembly instead
native any Emit(any[] bytes, int len, bool customcleanup = false, any ...);

/**
 *	Get Address of a variable.
 *
 *	@param cell 			Variable to get the address of.
 *
 *	@return 				Address of the variable.
*/
native any AddressOf(any &cell);
stock any AddressOfArray(any[] array)
{
	return AddressOf(array[0]);
}
// For some reason str[0] gets a mismatch. Probably because it's too small to be a cell
native any AddressOfString(const char[] str);

/**
 *  Store memory to an address
 *  NOTE: This does not set memory access. You must use SetMemAccess first.
 *  This is same as StoreToAddress but exists purely for optimization purposes.
 *
 *  @param p                Address to store to.
 *  @param val              Value to store.
 *  @param numt             Size to store.
*/
#pragma deprecated Use StoreToAddress with updateMemAccess (param 4) set to false
native void StoreToAddressFast(any p, any val, NumberType numt = NumberType_Int32);

// Set the memory access of allocated memory using the SH_MEM_* flags.
native bool SetMemAccess(any p, int size, int flags);

/**
 * 	Format a string with a va_list.
 * 	For use with hooks for functions with va_list arguments.
 * 	NOTE: LINUX ONLY. Will not work with Windows.
 * 
 * 	@param buffer 			Buffer to write to.
 * 	@param size 			Size of the buffer.
 * 	@param format 			String format.
 * 	@param valist 			va_list ptr.
 * 
 * 	@error 					If calling on Windows.
 * 	@return 				Length of buffer.
*/
native int VAFormat(char[] buffer, int size, char[] format, any valist);

native any calloc(int num, int size) = Calloc;
native any malloc(int size) = Malloc
native any reallocf(any p, int size) = ReallocF;
native void freef(any p) = FreeF;
native void memmovef(any dest, any src, int size) = MemMoveF;
native any memcpyf(any dest, any src, int size) = MemCopyF;
native int memcmpf(any p1, any p2, int size) = MemCmpF;
native void memsetf(any p, int val, int size) = MemSetF;

stock any realloc(any p, int size, bool write = SMTC_SMMEM_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT)
{
	if (write)
		return Realloc(p, size);
	return ReallocF(p, size);
}
stock void free(any p, bool write = SMTC_SMMEM_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT)
{
	if (write)
	{
		Free(p);
		return;
	}
	FreeF(p);
}
stock void memmove(any dest, any src, int size, bool write = SMTC_SMMEM_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT)
{
	if (write)
	{
		MemMove(dest, src, size);
		return;
	}
	MemMoveF(dest, src, size);
}
stock any memcpy(any dest, any src, int size, bool write = SMTC_SMMEM_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT)
{
	if (write)
		return MemCopy(dest, src, size);
	return MemCopyF(dest, src, size);
}
stock int memcmp(any p1, any p2, int size, bool write = SMTC_SMMEM_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT)
{
	if (write)
		return MemCmp(p1, p2, size);
	return MemCmpF(p1, p2, size);
}
stock void memset(any p, int val, int size, bool write = SMTC_SMMEM_UPDATEMEMACCESS_WHILEWRITING_BYDEFAULT)
{
	if (write)
	{
		MemSet(p, val, size);
		return;
	}
	MemSetF(p, val, size);
}

public Extension __ext_smmem =
{
	name = "SM-Mem",
	file = "sm-mem.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public void __ext_smmem_SetNTVOptional()
{
	MarkNativeAsOptional("Calloc");
	MarkNativeAsOptional("Malloc");
	MarkNativeAsOptional("Realloc");
	MarkNativeAsOptional("Free");
	MarkNativeAsOptional("MemMove");
	MarkNativeAsOptional("MemCopy");
	MarkNativeAsOptional("MemCmp");
	MarkNativeAsOptional("MemSet");
	MarkNativeAsOptional("Emit");
	MarkNativeAsOptional("AddressOf");
	MarkNativeAsOptional("AddressOfString");
	MarkNativeAsOptional("StoreToAddressFast");
	MarkNativeAsOptional("SetMemAccess");
	MarkNativeAsOptional("VAFormat");

	MarkNativeAsOptional("ReallocF");
	MarkNativeAsOptional("FreeF");
	MarkNativeAsOptional("MemMoveF");
	MarkNativeAsOptional("MemCopyF");
	MarkNativeAsOptional("MemCmpF");
	MarkNativeAsOptional("MemSetF");

	MarkNativeAsOptional("DynLib.DynLib");
	MarkNativeAsOptional("DynLib.BaseAddr.get");
	MarkNativeAsOptional("DynLib.GetName");
	MarkNativeAsOptional("DynLib.FindPattern");
	MarkNativeAsOptional("DynLib.ResolveSymbol");
}
#endif