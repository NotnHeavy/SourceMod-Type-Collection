//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// I need to read more on x86. This CPU design genuinely horrifies me.

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_NEWCALL
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_NEWCALL

#pragma semicolon true 
#pragma newdecls required

#define NEWCALL_INT_COUNT 256
#define NEWCALL_BUFFER_SIZE 2048
#define NEWCALL_DEBUG_ADDRESS 0xFFFFFFFF

//////////////////////////////////////////////////////////////////////////////
// NEWCALL DATA                                                             //
//////////////////////////////////////////////////////////////////////////////

// General purpose registers.
enum register_t
{
    AL,
    CL,
    DL,
    BL,
    AH,
    CH,
    DH,
    BH,
    EAX,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI
};

// Opcodes.
enum opcode_t
{
    PUSH_IMM8 = 0x6A,
    PUSH_IMM32 = 0x68,         
    MOV_IMM = 0xB0,     // MOV_IMM8 (0xB0 - 0xB8), MOV_IMM16/32 (0xB8 - 0xBF),
    RET = 0xC3,     
    INT_IMM8 = 0xCD
};

// Return information.
enum return_t
{
    NEWCALL_VOID,   // void
    NEWCALL_AL,     // bool (reading from eax as a whole is not ideal due to partial register writes not clearing the whole register)
    NEWCALL_EAX,    // primitive C type
    NEWCALL_LONG,   // long long (a stack buffer will be returned, buffer[0] = low bits (eax), buffer[1] = high bits (edx))
    NEWCALL_STACK   // struct
};

static any stringBuilder = NULL;
static int stringBuilderIndex = 0;
static int stackSize = 0;

static Handle call = null;

//////////////////////////////////////////////////////////////////////////////
// NEWCALL METHODMAP                                                        //
//////////////////////////////////////////////////////////////////////////////

// The value of this methodmap represents either an interrupt (0-255) or a function address.
// This uses SDKCall behind the scenes and was made to expand upon its existing functionality,
// such as passing parameters through registers or handle stack-allocated return values properly.
// I implemented INT support too because I was bored.
methodmap NewCall
{
    // Constructor - wrap around an interrupt number (0-255) or a function address.
    public NewCall(any address)
    {
        return view_as<NewCall>(address);
    }

    // Clear the string builder.
    public void Reset()
    {
        memset(stringBuilder, 0, NEWCALL_BUFFER_SIZE);
        stringBuilderIndex = 0;
        stackSize = 0;
    }

    // Move an immediate (constant value) to a register.
    public void MoveToRegister(any value, register_t register)
    {
        appendIntegerToStringBuilder(MOV_IMM + view_as<opcode_t>(register), 1);
        appendIntegerToStringBuilder(value);
    }

    // Move an immediate to the memory location specified at ebp + offset.
    // Remember: the function you are calling is very likely to be using the
    // cdecl/thiscall calling convention, meaning parameters will be passed right-to-left. 
    // In that case, the offset will be reversed.
    public void MoveToBasePointer(any value, int offset = 0)
    {
        if (signedIsByte(offset))
        {
            appendStringBuilder("\xC7\x45", 2); // mov dword ptr [ebp-offset], imm8
            appendIntegerToStringBuilder(offset, 1);
        }
        else
        {
            appendStringBuilder("\xC7\x85", 2); // mov dword ptr [ebp-offset], imm32
            appendIntegerToStringBuilder(offset);
        }
        appendIntegerToStringBuilder(value);
    }

    // Subtract an immediate from the stack register.
    public void SubFromStackRegister(any value)
    {
        if (signedIsByte(value))
        {
            appendStringBuilder("\x83\xEC", 2); // sub esp, imm8
            appendIntegerToStringBuilder(value, 1);
        }
        else
        {
            appendStringBuilder("\x81\xEC", 2); // sub esp, imm32
            appendIntegerToStringBuilder(value);
        }
        stackSize += value;
    }

    // Push a large (>4 bytes) object onto the stack.
    // This just simplifies the process rather than rely on MoveToBasePointer and
    // SubFromStackRegister. 
    // Remember: the function you are calling is very likely to be 
    // using the cdecl/thiscall calling convention, meaning parameters will be passed 
    // right-to-left. In that case, the last parameter would be pushed first with this 
    // function.
    public void PushObject(any pointer, int size)
    {
        this.SubFromStackRegister(size);
        for (int i = size; i > 0; i -= 4)
        {
            int offset = size - i;
            any value = LoadFromAddress(pointer + offset, NumberType_Int32);
            this.MoveToBasePointer(value, -stackSize + offset);
        }
    }

    // Allocate space for a return pointer. This must be the last stack push.
    public void PushReturnPointer(int resultSize)
    {
        // Make space for the return pointer.
        this.SubFromStackRegister(resultSize);
        if (signedIsByte(resultSize))
        {
            appendStringBuilder("\x8D\x45", 2); // lea eax, [ebp+disp8]
            appendIntegerToStringBuilder(-resultSize, 1);
        }
        else
        {
            appendStringBuilder("\x8D\x85", 2); // lea eax, [ebp+disp32]
            appendIntegerToStringBuilder(-resultSize);
        }

        // Push the eax register.
        appendIntegerToStringBuilder('\x50', 1);
    }

    // Push a float onto the stack.
    // Remember: the function you are calling is very likely to be using the
    // cdecl/thiscall calling convention, meaning parameters will be passed right-to-left. 
    // In that case, the last parameter would be pushed first with this function.
    public void PushFloat(float value)
    {
        this.Push(value);
        if (signedIsByte(-stackSize))
        {
            appendStringBuilder("\xD9\x45", 2); // fld dword ptr [ebp+disp8]
            appendIntegerToStringBuilder(-stackSize, 1);
        }
        else
        {
            appendStringBuilder("\xD9\x85", 2); // fld dword ptr [ebp+disp32]
            appendIntegerToStringBuilder(-stackSize);
        }
        appendStringBuilder("\xD9\x1C\x24", 3); // fstp dword ptr [esp]
    }

    // Push a parameter onto the stack.
    // Remember: the function you are calling is very likely to be using the
    // cdecl/thiscall calling convention, meaning parameters will be passed right-to-left. 
    // In that case, the last parameter would be pushed first with this function.
    public void Push(any value)
    {
        if (signedIsByte(value))
        {
            appendIntegerToStringBuilder(PUSH_IMM8, 1);
            appendIntegerToStringBuilder(value, 1);
        }
        else
        {
            appendIntegerToStringBuilder(PUSH_IMM32, 1);
            appendIntegerToStringBuilder(value);
        }
        stackSize += 4;
    }

    // Generate a software interrupt or call the specified function.
    // The string builder used behind the scenes will be reset. If NEWCALL_DEBUG_ADDRESS 
    // is passed, it will not be reset and no function will be called.
    // If returns is not NEWCALL_VOID, it will return a pointer. If NEWCALL_STACK is given,
    // resultSize must be set. You must free() this pointer manually. This is ignored if 
    // generating a software interrupt.
    public any Call(return_t returns = NEWCALL_VOID, int resultSize = 0)
    {
        // If the address is an interrupt number, generate a software interrupt.
        any address = this;
        if (address < NEWCALL_INT_COUNT && address != NEWCALL_DEBUG_ADDRESS)
        {
            appendIntegerToStringBuilder(INT_IMM8, 1);
            appendIntegerToStringBuilder(address, 1);
            appendIntegerToStringBuilder(RET, 1);
            SDKCall(call);
            this.Reset();
            return NULL;
        }

        // Insert the calling code.
        appendStringBuilder("\xFF\x15", 2); // call dword ptr [ds:address]
        if (address == NEWCALL_DEBUG_ADDRESS)
            appendStringBuilder("\xEF\xBE\xAD\xDE", 4); // 0xDEADBEEF
        else
            appendIntegerToStringBuilder(AddressOf(address));

        // Handle returning code by moving the returned value to our own buffer.
        any buffer = NULL;
        switch (returns)
        {
        case NEWCALL_AL:
        {
            if (address == NEWCALL_DEBUG_ADDRESS)
                buffer = 0xEFEBEADE;
            else
                buffer = malloc(1);
            appendIntegerToStringBuilder('\xA2', 1); // mov dword ptr [ds:address], al
            appendIntegerToStringBuilder(buffer);
        }
        case NEWCALL_EAX:
        {
            if (address == NEWCALL_DEBUG_ADDRESS)
                buffer = 0xEFEBEADE;
            else
                buffer = malloc(4);
            SetMemAccess(buffer, 4, SH_MEM_ALL);
            appendIntegerToStringBuilder('\xA3', 1); // mov dword ptr [ds:address], eax
            appendIntegerToStringBuilder(buffer);
        }
        case NEWCALL_LONG:
        {
            if (address == NEWCALL_DEBUG_ADDRESS)
                buffer = 0xEFEBEADE;
            else
                buffer = malloc(8);
            appendIntegerToStringBuilder('\xA3', 1); // mov dword ptr [ds:address], eax
            appendIntegerToStringBuilder(buffer);
            appendStringBuilder("\x89\x15", 2); // mov dword ptr [ds:address], edx
            appendIntegerToStringBuilder(buffer + 4);
        }
        case NEWCALL_STACK:
        {
            if (address == NEWCALL_DEBUG_ADDRESS)
                buffer = 0xEFEBEADE;
            else
                buffer = malloc(resultSize);
            for (int i = resultSize; i > 0; i -= 4)
            {
                // Move the member of the object + offset to the edx register.
                int offset = resultSize - i;
                if (offset == 0)
                    appendStringBuilder("\x8B\x10", 2); // mov edx, dword ptr [eax]
                else if ((offset & 0xFF) == offset)
                {
                    appendStringBuilder("\x8B\x50", 2); // mov edx, dword ptr [eax+disp8]
                    appendIntegerToStringBuilder(offset, 1);
                }
                else
                {
                    appendStringBuilder("\x8B\x90", 2); // mov edx, dword ptr[eax+disp32]
                    appendIntegerToStringBuilder(offset);
                }
                
                // Move it to our buffer + offset.
                appendStringBuilder("\x89\x15", 2); // mov dword ptr [ds:disp32], edx
                appendIntegerToStringBuilder(buffer + offset);
            }
        }
        }

        // Call the function. Insert the epilogue before doing so.
        appendStringBuilder("\x89\xEC\x5D\xC3", 4); // mov esp, ebp
                                                    // pop ebp
                                                    // ret
        if (address != NEWCALL_DEBUG_ADDRESS)
        {
            SDKCall(call);
            this.Reset();
        }
        return buffer;
    }

    // For debugging.
    public void DumpStringBuilder(char[] buffer, int maxlength, int& length)
    {
        memset(AddressOfString(buffer), 0, maxlength);
        memcpy(AddressOfString(buffer), stringBuilder, maxlength);
        length = stringBuilderIndex;
    }

    // Get the function allocated at a virtual pointer.
    // The index must be an array index (vtable[0], vtable[1], etc).
    public static any GetVirtualProc(any obj, any index)
    {
        return view_as<Pointer>(view_as<Pointer>(obj).Dereference()).Dereference(index * 4);
    }
}

//////////////////////////////////////////////////////////////////////////////
// STRING CONSTRUCTION                                                      //
//////////////////////////////////////////////////////////////////////////////

static void verifyLength(int count)
{
    if (stringBuilder == NULL)
    {
        // Set up string builder.
        stringBuilder = malloc(NEWCALL_BUFFER_SIZE);
        SetMemAccess(stringBuilder, NEWCALL_BUFFER_SIZE, SH_MEM_ALL);

        // Set up SDKCall.
        StartPrepSDKCall(SDKCall_Static);
        PrepSDKCall_SetAddress(stringBuilder);
        call = EndPrepSDKCall();
    }
    else if (stringBuilderIndex + count >= NEWCALL_BUFFER_SIZE)
        ThrowError("Exceeded length of string builder for NewCall.");
    
    // Insert prologue.
    if (stringBuilderIndex == 0)
    {
        memcpy(stringBuilder, AddressOfString("\x55\x89\xE5"), 3);  // push ebp
                                                                    // mov ebp, esp
        stringBuilderIndex += 3;
    }
}

// length excludes NUL character (if C string provided)
static void appendStringBuilder(const char[] buffer, int length)
{
    verifyLength(length);
    memcpy(stringBuilder + stringBuilderIndex, AddressOfString(buffer), length);
    stringBuilderIndex += length;
}

// append integral value in binary format
static void appendIntegerToStringBuilder(any value, int bits = 4)
{
    verifyLength(bits);
    memcpy(stringBuilder + stringBuilderIndex, AddressOf(value), bits);
    stringBuilderIndex += bits;
}

//////////////////////////////////////////////////////////////////////////////
// MISCELLANEOUS                                                            //
//////////////////////////////////////////////////////////////////////////////

static bool signedIsByte(int value)
{
    if (((value >> 31) & 1) && ((value >> 8) & 0xFFFFFF) == 0xFFFFFF)
        return true;
    if ((value & 0b1111111) == value)
        return true;
    return false;
}