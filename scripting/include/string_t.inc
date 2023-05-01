//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// Slight differences to Source SDK 2013 string_t:
// 1) Because SourcePawn does not work similarly to C and there are no concepts of things like
//    unions, castable_string_t has been merged into string_t.
// 2) NULL_STRING is called NULL_STRING_T due to it being previously defined.

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_STRING_T
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_STRING_T

#pragma semicolon true 
#pragma newdecls required

//////////////////////////////////////////////////////////////////////////////
// STRING_T DATA                                                            //
//////////////////////////////////////////////////////////////////////////////

enum
{
    STRING_T_OFFSET_PSZVALUE = 0,                               // const char* pszValue;

    SIZEOF_String_t = STRING_T_OFFSET_PSZVALUE + SIZEOF_Pointer // sizeof(string_t);
};

//////////////////////////////////////////////////////////////////////////////
// STRING_T METHODMAP                                                       //
//////////////////////////////////////////////////////////////////////////////

// seriously for fuck's sake just let me type "string_t"
methodmap String_t < Pointer
{
    // Constructor - wrap around an existing address to make way for a string_t object.
    public String_t(Address address)
    {
        return view_as<String_t>(address);
    }

    // The string object. (const char*)
    property Pointer pszValue
    {
        public get() { return this.Dereference(STRING_T_OFFSET_PSZVALUE); }
        public set(Pointer value) { this.Write(value, STRING_T_OFFSET_PSZVALUE); }
    }

    // Convert to a C string.
    public Pointer TOCStr()
    {
        return (this.pszValue) ? this.pszValue : aEmptyString;
    }

    // Convert to a SourcePawn string. maxlen includes NULL character.
    public void ToSourcePawnStr(char[] buffer, int maxlen)
    {
        if (!this.pszValue)
        {
            strcopy(buffer, maxlen, empty_string);
            return;
        }
        this.pszValue.ToCharBuffer(buffer, maxlen);
    }

    // Call constructor on existing type.
    public void ctor(Pointer string = NULL)
    {
        this.pszValue = (string && string.Dereference(.bits = NumberType_Int8)) ? string : NULL;
    }

    // Constructor - allocate a new string_t onto the heap. Deallocation will be required when you are finished using free().
    public static String_t Malloc(Pointer string = NULL)
    {
        String_t pointer = malloc(SIZEOF_String_t);
        pointer.ctor(string);
        return pointer;
    }

    // Constructor - allocate a new string_t onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(Pointer string = NULL)
    {
        char buffer[SIZEOF_String_t];
        view_as<String_t>(AddressOfString(buffer)).ctor(string);
        return buffer;
    }
}

//////////////////////////////////////////////////////////////////////////////
// STRING_T OPERATORS                                                       //
//////////////////////////////////////////////////////////////////////////////

stock bool operator!(String_t string)
{
    return string.pszValue == NULL;
}

stock bool operator==(String_t left, String_t right)
{
    return left.pszValue == right.pszValue;
}

stock bool operator!=(String_t left, String_t right)
{
    return left.pszValue != right.pszValue;
}

stock bool operator<(String_t left, String_t right)
{
    return left.pszValue < right.pszValue;
}

//////////////////////////////////////////////////////////////////////////////
// MACROS                                                                   //
//////////////////////////////////////////////////////////////////////////////

// actual name would be NULL_STRING, which uses sourcepawn char buffers; do not confuse it with this
#define NULL_STRING_T STACK_GETRETURN(String_t.StackAlloc())

#define STRING(%1) (%1).ToCStr()
#define MAKE_STRING(%1) STACK_GETRETURN(String_t.StackAlloc(%1))