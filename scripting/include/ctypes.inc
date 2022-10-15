//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// NOTE!
// Until SourceMod gets updated to use the most recent version of SourcePawn and is then compiled as a new release, you will have to build your own copy, using a SourcePawn commit after "Fix regression in assignment overloading".
// This is due to operator= being broken.

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CTYPES
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CTYPES

#pragma semicolon true 
#pragma newdecls required

// Type macros.
#define CHAR Int8_t
#define BYTE UInt8_t
#define WCHAR Int16_t
#define SHORT Int16_t
#define USHORT UInt16_t
#define BOOLEAN BYTE
#define BOOL int
#define SIZE_T int

#define TRUE 1
#define FALSE 0

// Used for masking.
#define USHRT_MAX 0xFFFF
#define UCHAR_MAX 0xFF

// Cosmetic purposes only.
#define UINT_MAX 0xFFFFFFFFF
#define INT_MIN 0x80000000
#define INT_MAX 0x7FFFFFFF
#define SHRT_MIN 0x8000
#define SHRT_MAX 0x7FFF
#define CHAR_MIN 0x80
#define CHAR_MAX 0x7F

// Sizeof.
#define SIZEOF_UInt8_t 1
#define SIZEOF_Int8_t 1
#define SIZEOF_UInt16_t 2
#define SIZEOF_Int16_t 2

// The parser hates this.
#define SIZEOF_CHAR 1
#define SIZEOF_BYTE 1
#define SIZEOF_WCHAR 2
#define SIZEOF_SHORT 2
#define SIZEOF_USHORT 2
#define SIZEOF_BOOLEAN 1
#define SIZEOF_BOOL 4
#define SIZEOF_SIZE_T 4

// Just last few things for SIZEOF().
#define SIZEOF_int 4
#define SIZEOF_float 4
#define SIZEOF_any 4
#define SIZEOF_bool 1
#define SIZEOF_char 1

//////////////////////////////////////////////////////////////////////////////
// INSTANTIATE TAGS                                                         //
//////////////////////////////////////////////////////////////////////////////

methodmap UInt8_t
{
    // Conversion.
    public static UInt8_t ToUInt8(int value)
    {
        return view_as<UInt8_t>(value & UCHAR_MAX);
    }
    public int ToCell()
    {
        return view_as<int>(this);
    }

    // Constructor.
    public UInt8_t(int value)
    {
        return UInt8_t.ToUInt8(value);
    }

    // Bitwise operators.
    public int And(int value)
    {
        return this.ToCell() & value;
    }
    public int Or(int value)
    {
        return this.ToCell() | value;
    }
    public int Xor(int value)
    {
        return this.ToCell() ^ value;
    }
    public int Not()
    {
        return ~this.ToCell();
    }
}
methodmap Int8_t
{
    // Conversion.
    public static Int8_t ToInt8(int value)
    {
        return view_as<Int8_t>(value & UCHAR_MAX);
    }
    public int ToCell()
    {
        int value = view_as<int>(this);
        return (value >> 7) & 1 ? value | 0xFFFFFF00 : value;
    }

    // Constructor.
    public Int8_t(int value)
    {
        return Int8_t.ToInt8(value);
    }

    // Bitwise operators.
    public int And(int value)
    {
        return this.ToCell() & value;
    }
    public int Or(int value)
    {
        return this.ToCell() | value;
    }
    public int Xor(int value)
    {
        return this.ToCell() ^ value;
    }
    public int Not()
    {
        return ~this.ToCell();
    }
}
methodmap UInt16_t
{
    // Conversion.
    public static UInt16_t ToUInt16(int value)
    {
        return view_as<UInt16_t>(value & USHRT_MAX);
    }
    public int ToCell()
    {
        return view_as<int>(this);
    }

    // Constructor.
    public UInt16_t(int value)
    {
        return UInt16_t.ToUInt16(value);
    }

    // Bitwise operators.
    public int And(int value)
    {
        return this.ToCell() & value;
    }
    public int Or(int value)
    {
        return this.ToCell() | value;
    }
    public int Xor(int value)
    {
        return this.ToCell() ^ value;
    }
    public int Not()
    {
        return ~this.ToCell();
    }
}
methodmap Int16_t
{
    // Conversion.
    public static Int16_t ToInt16(int value)
    {
        return view_as<Int16_t>(value & USHRT_MAX);
    }
    public int ToCell()
    {
        int value = view_as<int>(this);
        return (value >> 15) & 1 ? value | 0xFFFF0000 : value;
    }

    // Constructor.
    public Int16_t(int value)
    {
        return Int16_t.ToInt16(value);
    }

    // Bitwise operators.
    public int And(int value)
    {
        return this.ToCell() & value;
    }
    public int Or(int value)
    {
        return this.ToCell() | value;
    }
    public int Xor(int value)
    {
        return this.ToCell() ^ value;
    }
    public int Not()
    {
        return ~this.ToCell();
    }
}

//////////////////////////////////////////////////////////////////////////////
// OPERATORS (this is horrendous)                                           //
//////////////////////////////////////////////////////////////////////////////

stock UInt8_t operator=(int value)
{
    return UInt8_t(value);
}
stock UInt8_t operator=(char value)
{
    return UInt8_t(value);
}
stock UInt8_t operator++(UInt8_t value)
{
    return UInt8_t(value + 1);
}
stock UInt8_t operator--(UInt8_t value)
{
    return UInt8_t(value - 1);
}
stock bool operator!(UInt8_t value)
{
    return !value.ToCell();
}
stock int operator-(UInt8_t value)
{
    return -value.ToCell();
}
stock int operator+(UInt8_t left, UInt8_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(UInt8_t left, Int8_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(UInt8_t left, UInt16_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(UInt8_t left, Int16_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(UInt8_t left, int right)
{
    return left.ToCell() + right;
}
stock int operator+(UInt8_t left, char right)
{
    return left.ToCell() + right;
}
stock float operator+(UInt8_t left, float right)
{
    return left.ToCell() + right;
}
stock int operator-(UInt8_t left, UInt8_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(UInt8_t left, Int8_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(UInt8_t left, UInt16_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(UInt8_t left, Int16_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(UInt8_t left, int right)
{
    return left.ToCell() - right;
}
stock int operator-(UInt8_t left, char right)
{
    return left.ToCell() - right;
}
stock float operator-(UInt8_t left, float right)
{
    return left.ToCell() - right;
}
stock int operator*(UInt8_t left, UInt8_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(UInt8_t left, Int8_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(UInt8_t left, UInt16_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(UInt8_t left, Int16_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(UInt8_t left, int right)
{
    return left.ToCell() * right;
}
stock int operator*(UInt8_t left, char right)
{
    return left.ToCell() * right;
}
stock float operator*(UInt8_t left, float right)
{
    return left.ToCell() * right;
}
stock int operator/(UInt8_t left, UInt8_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(UInt8_t left, Int8_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(UInt8_t left, UInt16_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(UInt8_t left, Int16_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(UInt8_t left, int right)
{
    return left.ToCell() / right;
}
stock int operator/(UInt8_t left, char right)
{
    return left.ToCell() / right;
}
stock float operator/(UInt8_t left, float right)
{
    return left.ToCell() / right;
}
stock bool operator>(UInt8_t left, UInt8_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(UInt8_t left, Int8_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(UInt8_t left, UInt16_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(UInt8_t left, Int16_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(UInt8_t left, int right)
{
    return left.ToCell() > right;
}
stock bool operator>(UInt8_t left, char right)
{
    return left.ToCell() > right;
}
stock bool operator>(UInt8_t left, float right)
{
    return left.ToCell() > right;
}
stock bool operator>=(UInt8_t left, UInt8_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(UInt8_t left, Int8_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(UInt8_t left, UInt16_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(UInt8_t left, Int16_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(UInt8_t left, int right)
{
    return left.ToCell() >= right;
}
stock bool operator>=(UInt8_t left, char right)
{
    return left.ToCell() >= right;
}
stock bool operator>=(UInt8_t left, float right)
{
    return left.ToCell() >= right;
}
stock bool operator<(UInt8_t left, UInt8_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(UInt8_t left, Int8_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(UInt8_t left, UInt16_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(UInt8_t left, Int16_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(UInt8_t left, int right)
{
    return left.ToCell() < right;
}
stock bool operator<(UInt8_t left, char right)
{
    return left.ToCell() < right;
}
stock bool operator<(UInt8_t left, float right)
{
    return left.ToCell() < right;
}
stock bool operator<=(UInt8_t left, UInt8_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(UInt8_t left, Int8_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(UInt8_t left, UInt16_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(UInt8_t left, Int16_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(UInt8_t left, int right)
{
    return left.ToCell() <= right;
}
stock bool operator<=(UInt8_t left, char right)
{
    return left.ToCell() <= right;
}
stock bool operator<=(UInt8_t left, float right)
{
    return left.ToCell() <= right;
}
stock bool operator!=(UInt8_t left, UInt8_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(UInt8_t left, Int8_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(UInt8_t left, UInt16_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(UInt8_t left, Int16_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(UInt8_t left, int right)
{
    return left.ToCell() != right;
}
stock bool operator!=(UInt8_t left, char right)
{
    return left.ToCell() != right;
}
stock bool operator!=(UInt8_t left, float right)
{
    return left.ToCell() != right;
}
stock bool operator==(UInt8_t left, UInt8_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(UInt8_t left, Int8_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(UInt8_t left, UInt16_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(UInt8_t left, Int16_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(UInt8_t left, int right)
{
    return left.ToCell() == right;
}
stock bool operator==(UInt8_t left, char right)
{
    return left.ToCell() == right;
}
stock bool operator==(UInt8_t left, float right)
{
    return left.ToCell() == right;
}
stock int operator%(UInt8_t left, UInt8_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(UInt8_t left, Int8_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(UInt8_t left, UInt16_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(UInt8_t left, Int16_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(UInt8_t left, int right)
{
    return left.ToCell() % right;
}
stock int operator%(UInt8_t left, char right)
{
    return left.ToCell() % right;
}



stock Int8_t operator=(int value)
{
    return Int8_t(value);
}
stock Int8_t operator=(char value)
{
    return Int8_t(value);
}
stock Int8_t operator++(Int8_t value)
{
    return Int8_t(value + 1);
}
stock Int8_t operator--(Int8_t value)
{
    return Int8_t(value - 1);
}
stock bool operator!(Int8_t value)
{
    return !value.ToCell();
}
stock int operator-(Int8_t value)
{
    return -value.ToCell();
}
stock int operator+(Int8_t left, UInt8_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(Int8_t left, Int8_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(Int8_t left, UInt16_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(Int8_t left, Int16_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(Int8_t left, int right)
{
    return left.ToCell() + right;
}
stock int operator+(Int8_t left, char right)
{
    return left.ToCell() + right;
}
stock float operator+(Int8_t left, float right)
{
    return left.ToCell() + right;
}
stock int operator-(Int8_t left, UInt8_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(Int8_t left, Int8_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(Int8_t left, UInt16_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(Int8_t left, Int16_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(Int8_t left, int right)
{
    return left.ToCell() - right;
}
stock int operator-(Int8_t left, char right)
{
    return left.ToCell() - right;
}
stock float operator-(Int8_t left, float right)
{
    return left.ToCell() - right;
}
stock int operator*(Int8_t left, UInt8_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(Int8_t left, Int8_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(Int8_t left, UInt16_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(Int8_t left, Int16_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(Int8_t left, int right)
{
    return left.ToCell() * right;
}
stock int operator*(Int8_t left, char right)
{
    return left.ToCell() * right;
}
stock float operator*(Int8_t left, float right)
{
    return left.ToCell() * right;
}
stock int operator/(Int8_t left, UInt8_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(Int8_t left, Int8_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(Int8_t left, UInt16_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(Int8_t left, Int16_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(Int8_t left, int right)
{
    return left.ToCell() / right;
}
stock int operator/(Int8_t left, char right)
{
    return left.ToCell() / right;
}
stock float operator/(Int8_t left, float right)
{
    return left.ToCell() / right;
}
stock bool operator>(Int8_t left, UInt8_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(Int8_t left, Int8_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(Int8_t left, UInt16_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(Int8_t left, Int16_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(Int8_t left, int right)
{
    return left.ToCell() > right;
}
stock bool operator>(Int8_t left, char right)
{
    return left.ToCell() > right;
}
stock bool operator>(Int8_t left, float right)
{
    return left.ToCell() > right;
}
stock bool operator>=(Int8_t left, UInt8_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(Int8_t left, Int8_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(Int8_t left, UInt16_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(Int8_t left, Int16_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(Int8_t left, int right)
{
    return left.ToCell() >= right;
}
stock bool operator>=(Int8_t left, char right)
{
    return left.ToCell() >= right;
}
stock bool operator>=(Int8_t left, float right)
{
    return left.ToCell() >= right;
}
stock bool operator<(Int8_t left, UInt8_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(Int8_t left, Int8_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(Int8_t left, UInt16_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(Int8_t left, Int16_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(Int8_t left, int right)
{
    return left.ToCell() < right;
}
stock bool operator<(Int8_t left, char right)
{
    return left.ToCell() < right;
}
stock bool operator<(Int8_t left, float right)
{
    return left.ToCell() < right;
}
stock bool operator<=(Int8_t left, UInt8_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(Int8_t left, Int8_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(Int8_t left, UInt16_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(Int8_t left, Int16_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(Int8_t left, int right)
{
    return left.ToCell() <= right;
}
stock bool operator<=(Int8_t left, char right)
{
    return left.ToCell() <= right;
}
stock bool operator<=(Int8_t left, float right)
{
    return left.ToCell() <= right;
}
stock bool operator!=(Int8_t left, UInt8_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(Int8_t left, Int8_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(Int8_t left, UInt16_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(Int8_t left, Int16_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(Int8_t left, int right)
{
    return left.ToCell() != right;
}
stock bool operator!=(Int8_t left, char right)
{
    return left.ToCell() != right;
}
stock bool operator!=(Int8_t left, float right)
{
    return left.ToCell() != right;
}
stock bool operator==(Int8_t left, UInt8_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(Int8_t left, Int8_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(Int8_t left, UInt16_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(Int8_t left, Int16_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(Int8_t left, int right)
{
    return left.ToCell() == right;
}
stock bool operator==(Int8_t left, char right)
{
    return left.ToCell() == right;
}
stock bool operator==(Int8_t left, float right)
{
    return left.ToCell() == right;
}
stock int operator%(Int8_t left, UInt8_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(Int8_t left, Int8_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(Int8_t left, UInt16_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(Int8_t left, Int16_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(Int8_t left, int right)
{
    return left.ToCell() % right;
}
stock int operator%(Int8_t left, char right)
{
    return left.ToCell() % right;
}



stock UInt16_t operator=(int value)
{
    return UInt16_t(value);
}
stock UInt16_t operator=(char value)
{
    return UInt16_t(value);
}
stock UInt16_t operator++(UInt16_t value)
{
    return UInt16_t(value + 1);
}
stock UInt16_t operator--(UInt16_t value)
{
    return UInt16_t(value - 1);
}
stock bool operator!(UInt16_t value)
{
    return !value.ToCell();
}
stock int operator-(UInt16_t value)
{
    return -value.ToCell();
}
stock int operator+(UInt16_t left, UInt8_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(UInt16_t left, Int8_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(UInt16_t left, UInt16_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(UInt16_t left, Int16_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(UInt16_t left, int right)
{
    return left.ToCell() + right;
}
stock int operator+(UInt16_t left, char right)
{
    return left.ToCell() + right;
}
stock float operator+(UInt16_t left, float right)
{
    return left.ToCell() + right;
}
stock int operator-(UInt16_t left, UInt8_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(UInt16_t left, Int8_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(UInt16_t left, UInt16_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(UInt16_t left, Int16_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(UInt16_t left, int right)
{
    return left.ToCell() - right;
}
stock int operator-(UInt16_t left, char right)
{
    return left.ToCell() - right;
}
stock float operator-(UInt16_t left, float right)
{
    return left.ToCell() - right;
}
stock int operator*(UInt16_t left, UInt8_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(UInt16_t left, Int8_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(UInt16_t left, UInt16_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(UInt16_t left, Int16_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(UInt16_t left, int right)
{
    return left.ToCell() * right;
}
stock int operator*(UInt16_t left, char right)
{
    return left.ToCell() * right;
}
stock float operator*(UInt16_t left, float right)
{
    return left.ToCell() * right;
}
stock int operator/(UInt16_t left, UInt8_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(UInt16_t left, Int8_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(UInt16_t left, UInt16_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(UInt16_t left, Int16_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(UInt16_t left, int right)
{
    return left.ToCell() / right;
}
stock int operator/(UInt16_t left, char right)
{
    return left.ToCell() / right;
}
stock float operator/(UInt16_t left, float right)
{
    return left.ToCell() / right;
}
stock bool operator>(UInt16_t left, UInt8_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(UInt16_t left, Int8_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(UInt16_t left, UInt16_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(UInt16_t left, Int16_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(UInt16_t left, int right)
{
    return left.ToCell() > right;
}
stock bool operator>(UInt16_t left, char right)
{
    return left.ToCell() > right;
}
stock bool operator>(UInt16_t left, float right)
{
    return left.ToCell() > right;
}
stock bool operator>=(UInt16_t left, UInt8_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(UInt16_t left, Int8_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(UInt16_t left, UInt16_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(UInt16_t left, Int16_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(UInt16_t left, int right)
{
    return left.ToCell() >= right;
}
stock bool operator>=(UInt16_t left, char right)
{
    return left.ToCell() >= right;
}
stock bool operator>=(UInt16_t left, float right)
{
    return left.ToCell() >= right;
}
stock bool operator<(UInt16_t left, UInt8_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(UInt16_t left, Int8_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(UInt16_t left, UInt16_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(UInt16_t left, Int16_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(UInt16_t left, int right)
{
    return left.ToCell() < right;
}
stock bool operator<(UInt16_t left, char right)
{
    return left.ToCell() < right;
}
stock bool operator<(UInt16_t left, float right)
{
    return left.ToCell() < right;
}
stock bool operator<=(UInt16_t left, UInt8_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(UInt16_t left, Int8_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(UInt16_t left, UInt16_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(UInt16_t left, Int16_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(UInt16_t left, int right)
{
    return left.ToCell() <= right;
}
stock bool operator<=(UInt16_t left, char right)
{
    return left.ToCell() <= right;
}
stock bool operator<=(UInt16_t left, float right)
{
    return left.ToCell() <= right;
}
stock bool operator!=(UInt16_t left, UInt8_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(UInt16_t left, Int8_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(UInt16_t left, UInt16_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(UInt16_t left, Int16_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(UInt16_t left, int right)
{
    return left.ToCell() != right;
}
stock bool operator!=(UInt16_t left, char right)
{
    return left.ToCell() != right;
}
stock bool operator!=(UInt16_t left, float right)
{
    return left.ToCell() != right;
}
stock bool operator==(UInt16_t left, UInt8_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(UInt16_t left, Int8_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(UInt16_t left, UInt16_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(UInt16_t left, Int16_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(UInt16_t left, int right)
{
    return left.ToCell() == right;
}
stock bool operator==(UInt16_t left, char right)
{
    return left.ToCell() == right;
}
stock bool operator==(UInt16_t left, float right)
{
    return left.ToCell() == right;
}
stock int operator%(UInt16_t left, UInt8_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(UInt16_t left, Int8_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(UInt16_t left, UInt16_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(UInt16_t left, Int16_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(UInt16_t left, int right)
{
    return left.ToCell() % right;
}
stock int operator%(UInt16_t left, char right)
{
    return left.ToCell() % right;
}


stock Int16_t operator=(int value)
{
    return Int16_t(value);
}
stock Int16_t operator=(char value)
{
    return Int16_t(value);
}
stock Int16_t operator++(Int16_t value)
{
    return Int16_t(value + 1);
}
stock Int16_t operator--(Int16_t value)
{
    return Int16_t(value - 1);
}
stock bool operator!(Int16_t value)
{
    return !value.ToCell();
}
stock int operator-(Int16_t value)
{
    return -value.ToCell();
}
stock int operator+(Int16_t left, UInt8_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(Int16_t left, Int8_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(Int16_t left, UInt16_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(Int16_t left, Int16_t right)
{
    return left.ToCell() + right.ToCell();
}
stock int operator+(Int16_t left, int right)
{
    return left.ToCell() + right;
}
stock int operator+(Int16_t left, char right)
{
    return left.ToCell() + right;
}
stock float operator+(Int16_t left, float right)
{
    return left.ToCell() + right;
}
stock int operator-(Int16_t left, UInt8_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(Int16_t left, Int8_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(Int16_t left, UInt16_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(Int16_t left, Int16_t right)
{
    return left.ToCell() - right.ToCell();
}
stock int operator-(Int16_t left, int right)
{
    return left.ToCell() - right;
}
stock int operator-(Int16_t left, char right)
{
    return left.ToCell() - right;
}
stock float operator-(Int16_t left, float right)
{
    return left.ToCell() - right;
}
stock int operator*(Int16_t left, UInt8_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(Int16_t left, Int8_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(Int16_t left, UInt16_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(Int16_t left, Int16_t right)
{
    return left.ToCell() * right.ToCell();
}
stock int operator*(Int16_t left, int right)
{
    return left.ToCell() * right;
}
stock int operator*(Int16_t left, char right)
{
    return left.ToCell() * right;
}
stock float operator*(Int16_t left, float right)
{
    return left.ToCell() * right;
}
stock int operator/(Int16_t left, UInt8_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(Int16_t left, Int8_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(Int16_t left, UInt16_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(Int16_t left, Int16_t right)
{
    return left.ToCell() / right.ToCell();
}
stock int operator/(Int16_t left, int right)
{
    return left.ToCell() / right;
}
stock int operator/(Int16_t left, char right)
{
    return left.ToCell() / right;
}
stock float operator/(Int16_t left, float right)
{
    return left.ToCell() / right;
}
stock bool operator>(Int16_t left, UInt8_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(Int16_t left, Int8_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(Int16_t left, UInt16_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(Int16_t left, Int16_t right)
{
    return left.ToCell() > right.ToCell();
}
stock bool operator>(Int16_t left, int right)
{
    return left.ToCell() > right;
}
stock bool operator>(Int16_t left, char right)
{
    return left.ToCell() > right;
}
stock bool operator>(Int16_t left, float right)
{
    return left.ToCell() > right;
}
stock bool operator>=(Int16_t left, UInt8_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(Int16_t left, Int8_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(Int16_t left, UInt16_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(Int16_t left, Int16_t right)
{
    return left.ToCell() >= right.ToCell();
}
stock bool operator>=(Int16_t left, int right)
{
    return left.ToCell() >= right;
}
stock bool operator>=(Int16_t left, char right)
{
    return left.ToCell() >= right;
}
stock bool operator>=(Int16_t left, float right)
{
    return left.ToCell() >= right;
}
stock bool operator<(Int16_t left, UInt8_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(Int16_t left, Int8_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(Int16_t left, UInt16_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(Int16_t left, Int16_t right)
{
    return left.ToCell() < right.ToCell();
}
stock bool operator<(Int16_t left, int right)
{
    return left.ToCell() < right;
}
stock bool operator<(Int16_t left, char right)
{
    return left.ToCell() < right;
}
stock bool operator<(Int16_t left, float right)
{
    return left.ToCell() < right;
}
stock bool operator<=(Int16_t left, UInt8_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(Int16_t left, Int8_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(Int16_t left, UInt16_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(Int16_t left, Int16_t right)
{
    return left.ToCell() <= right.ToCell();
}
stock bool operator<=(Int16_t left, int right)
{
    return left.ToCell() <= right;
}
stock bool operator<=(Int16_t left, char right)
{
    return left.ToCell() <= right;
}
stock bool operator<=(Int16_t left, float right)
{
    return left.ToCell() <= right;
}
stock bool operator!=(Int16_t left, UInt8_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(Int16_t left, Int8_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(Int16_t left, UInt16_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(Int16_t left, Int16_t right)
{
    return left.ToCell() != right.ToCell();
}
stock bool operator!=(Int16_t left, int right)
{
    return left.ToCell() != right;
}
stock bool operator!=(Int16_t left, char right)
{
    return left.ToCell() != right;
}
stock bool operator!=(Int16_t left, float right)
{
    return left.ToCell() != right;
}
stock bool operator==(Int16_t left, UInt8_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(Int16_t left, Int8_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(Int16_t left, UInt16_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(Int16_t left, Int16_t right)
{
    return left.ToCell() == right.ToCell();
}
stock bool operator==(Int16_t left, int right)
{
    return left.ToCell() == right;
}
stock bool operator==(Int16_t left, char right)
{
    return left.ToCell() == right;
}
stock bool operator==(Int16_t left, float right)
{
    return left.ToCell() == right;
}
stock int operator%(Int16_t left, UInt8_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(Int16_t left, Int8_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(Int16_t left, UInt16_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(Int16_t left, Int16_t right)
{
    return left.ToCell() % right.ToCell();
}
stock int operator%(Int16_t left, int right)
{
    return left.ToCell() % right;
}
stock int operator%(Int16_t left, char right)
{
    return left.ToCell() % right;
}



stock int operator=(UInt8_t value)
{
    return value.ToCell();
}
stock int operator=(Int8_t value)
{
    return value.ToCell();
}
stock int operator=(UInt16_t value)
{
    return value.ToCell();
}
stock int operator=(Int16_t value)
{
    return value.ToCell();
}
stock int operator+(int left, UInt8_t right)
{
    return left + right.ToCell();
}
stock int operator+(int left, Int8_t right)
{
    return left + right.ToCell();
}
stock int operator+(int left, UInt16_t right)
{
    return left + right.ToCell();
}
stock int operator+(int left, Int16_t right)
{
    return left + right.ToCell();
}
stock int operator-(int left, UInt8_t right)
{
    return left - right.ToCell();
}
stock int operator-(int left, Int8_t right)
{
    return left - right.ToCell();
}
stock int operator-(int left, UInt16_t right)
{
    return left - right.ToCell();
}
stock int operator-(int left, Int16_t right)
{
    return left - right.ToCell();
}
stock int operator*(int left, UInt8_t right)
{
    return left * right.ToCell();
}
stock int operator*(int left, Int8_t right)
{
    return left * right.ToCell();
}
stock int operator*(int left, UInt16_t right)
{
    return left * right.ToCell();
}
stock int operator*(int left, Int16_t right)
{
    return left * right.ToCell();
}
stock int operator/(int left, UInt8_t right)
{
    return left / right.ToCell();
}
stock int operator/(int left, Int8_t right)
{
    return left / right.ToCell();
}
stock int operator/(int left, UInt16_t right)
{
    return left / right.ToCell();
}
stock int operator/(int left, Int16_t right)
{
    return left / right.ToCell();
}
stock bool operator>(int left, UInt8_t right)
{
    return left > right.ToCell();
}
stock bool operator>(int left, Int8_t right)
{
    return left > right.ToCell();
}
stock bool operator>(int left, UInt16_t right)
{
    return left > right.ToCell();
}
stock bool operator>(int left, Int16_t right)
{
    return left > right.ToCell();
}
stock bool operator>=(int left, UInt8_t right)
{
    return left >= right.ToCell();
}
stock bool operator>=(int left, Int8_t right)
{
    return left >= right.ToCell();
}
stock bool operator>=(int left, UInt16_t right)
{
    return left >= right.ToCell();
}
stock bool operator>=(int left, Int16_t right)
{
    return left >= right.ToCell();
}
stock bool operator<(int left, UInt8_t right)
{
    return left < right.ToCell();
}
stock bool operator<(int left, Int8_t right)
{
    return left < right.ToCell();
}
stock bool operator<(int left, UInt16_t right)
{
    return left < right.ToCell();
}
stock bool operator<(int left, Int16_t right)
{
    return left < right.ToCell();
}
stock bool operator<=(int left, UInt8_t right)
{
    return left <= right.ToCell();
}
stock bool operator<=(int left, Int8_t right)
{
    return left <= right.ToCell();
}
stock bool operator<=(int left, UInt16_t right)
{
    return left <= right.ToCell();
}
stock bool operator<=(int left, Int16_t right)
{
    return left <= right.ToCell();
}
stock bool operator!=(int left, UInt8_t right)
{
    return left != right.ToCell();
}
stock bool operator!=(int left, Int8_t right)
{
    return left != right.ToCell();
}
stock bool operator!=(int left, UInt16_t right)
{
    return left != right.ToCell();
}
stock bool operator!=(int left, Int16_t right)
{
    return left != right.ToCell();
}
stock bool operator==(int left, UInt8_t right)
{
    return left == right.ToCell();
}
stock bool operator==(int left, Int8_t right)
{
    return left == right.ToCell();
}
stock bool operator==(int left, UInt16_t right)
{
    return left == right.ToCell();
}
stock bool operator==(int left, Int16_t right)
{
    return left == right.ToCell();
}
stock int operator%(int left, UInt8_t right)
{
    return left % right.ToCell();
}
stock int operator%(int left, Int8_t right)
{
    return left % right.ToCell();
}
stock int operator%(int left, UInt16_t right)
{
    return left % right.ToCell();
}
stock int operator%(int left, Int16_t right)
{
    return left % right.ToCell();
}



stock float operator=(UInt8_t value)
{
    return float(value.ToCell());
}
stock float operator=(Int8_t value)
{
    return float(value.ToCell());
}
stock float operator=(UInt16_t value)
{
    return float(value.ToCell());
}
stock float operator=(Int16_t value)
{
    return float(value.ToCell());
}
stock float operator+(float left, UInt8_t right)
{
    return left + right.ToCell();
}
stock float operator+(float left, Int8_t right)
{
    return left + right.ToCell();
}
stock float operator+(float left, UInt16_t right)
{
    return left + right.ToCell();
}
stock float operator+(float left, Int16_t right)
{
    return left + right.ToCell();
}
stock float operator-(float left, UInt8_t right)
{
    return left - right.ToCell();
}
stock float operator-(float left, Int8_t right)
{
    return left - right.ToCell();
}
stock float operator-(float left, UInt16_t right)
{
    return left - right.ToCell();
}
stock float operator-(float left, Int16_t right)
{
    return left - right.ToCell();
}
stock float operator*(float left, UInt8_t right)
{
    return left * right.ToCell();
}
stock float operator*(float left, Int8_t right)
{
    return left * right.ToCell();
}
stock float operator*(float left, UInt16_t right)
{
    return left * right.ToCell();
}
stock float operator*(float left, Int16_t right)
{
    return left * right.ToCell();
}
stock float operator/(float left, UInt8_t right)
{
    return left / right.ToCell();
}
stock float operator/(float left, Int8_t right)
{
    return left / right.ToCell();
}
stock float operator/(float left, UInt16_t right)
{
    return left / right.ToCell();
}
stock float operator/(float left, Int16_t right)
{
    return left / right.ToCell();
}
stock bool operator>(float left, UInt8_t right)
{
    return left > right.ToCell();
}
stock bool operator>(float left, Int8_t right)
{
    return left > right.ToCell();
}
stock bool operator>(float left, UInt16_t right)
{
    return left > right.ToCell();
}
stock bool operator>(float left, Int16_t right)
{
    return left > right.ToCell();
}
stock bool operator>=(float left, UInt8_t right)
{
    return left >= right.ToCell();
}
stock bool operator>=(float left, Int8_t right)
{
    return left >= right.ToCell();
}
stock bool operator>=(float left, UInt16_t right)
{
    return left >= right.ToCell();
}
stock bool operator>=(float left, Int16_t right)
{
    return left >= right.ToCell();
}
stock bool operator<(float left, UInt8_t right)
{
    return left < right.ToCell();
}
stock bool operator<(float left, Int8_t right)
{
    return left < right.ToCell();
}
stock bool operator<(float left, UInt16_t right)
{
    return left < right.ToCell();
}
stock bool operator<(float left, Int16_t right)
{
    return left < right.ToCell();
}
stock bool operator<=(float left, UInt8_t right)
{
    return left <= right.ToCell();
}
stock bool operator<=(float left, Int8_t right)
{
    return left <= right.ToCell();
}
stock bool operator<=(float left, UInt16_t right)
{
    return left <= right.ToCell();
}
stock bool operator<=(float left, Int16_t right)
{
    return left <= right.ToCell();
}
stock bool operator!=(float left, UInt8_t right)
{
    return left != right.ToCell();
}
stock bool operator!=(float left, Int8_t right)
{
    return left != right.ToCell();
}
stock bool operator!=(float left, UInt16_t right)
{
    return left != right.ToCell();
}
stock bool operator!=(float left, Int16_t right)
{
    return left != right.ToCell();
}
stock bool operator==(float left, UInt8_t right)
{
    return left == right.ToCell();
}
stock bool operator==(float left, Int8_t right)
{
    return left == right.ToCell();
}
stock bool operator==(float left, UInt16_t right)
{
    return left == right.ToCell();
}
stock bool operator==(float left, Int16_t right)
{
    return left == right.ToCell();
}



stock char operator=(UInt8_t value)
{
    return value.ToCell();
}
stock char operator=(Int8_t value)
{
    return value.ToCell();
}
stock char operator=(UInt16_t value)
{
    return value.ToCell();
}
stock char operator=(Int16_t value)
{
    return value.ToCell();
}
stock int operator+(char left, UInt8_t right)
{
    return left + right.ToCell();
}
stock int operator+(char left, Int8_t right)
{
    return left + right.ToCell();
}
stock int operator+(char left, UInt16_t right)
{
    return left + right.ToCell();
}
stock int operator+(char left, Int16_t right)
{
    return left + right.ToCell();
}
stock int operator-(char left, UInt8_t right)
{
    return left - right.ToCell();
}
stock int operator-(char left, Int8_t right)
{
    return left - right.ToCell();
}
stock int operator-(char left, UInt16_t right)
{
    return left - right.ToCell();
}
stock int operator-(char left, Int16_t right)
{
    return left - right.ToCell();
}
stock int operator*(char left, UInt8_t right)
{
    return left * right.ToCell();
}
stock int operator*(char left, Int8_t right)
{
    return left * right.ToCell();
}
stock int operator*(char left, UInt16_t right)
{
    return left * right.ToCell();
}
stock int operator*(char left, Int16_t right)
{
    return left * right.ToCell();
}
stock int operator/(char left, UInt8_t right)
{
    return left / right.ToCell();
}
stock int operator/(char left, Int8_t right)
{
    return left / right.ToCell();
}
stock int operator/(char left, UInt16_t right)
{
    return left / right.ToCell();
}
stock int operator/(char left, Int16_t right)
{
    return left / right.ToCell();
}
stock bool operator>(char left, UInt8_t right)
{
    return left > right.ToCell();
}
stock bool operator>(char left, Int8_t right)
{
    return left > right.ToCell();
}
stock bool operator>(char left, UInt16_t right)
{
    return left > right.ToCell();
}
stock bool operator>(char left, Int16_t right)
{
    return left > right.ToCell();
}
stock bool operator>=(char left, UInt8_t right)
{
    return left >= right.ToCell();
}
stock bool operator>=(char left, Int8_t right)
{
    return left >= right.ToCell();
}
stock bool operator>=(char left, UInt16_t right)
{
    return left >= right.ToCell();
}
stock bool operator>=(char left, Int16_t right)
{
    return left >= right.ToCell();
}
stock bool operator<(char left, UInt8_t right)
{
    return left < right.ToCell();
}
stock bool operator<(char left, Int8_t right)
{
    return left < right.ToCell();
}
stock bool operator<(char left, UInt16_t right)
{
    return left < right.ToCell();
}
stock bool operator<(char left, Int16_t right)
{
    return left < right.ToCell();
}
stock bool operator<=(char left, UInt8_t right)
{
    return left <= right.ToCell();
}
stock bool operator<=(char left, Int8_t right)
{
    return left <= right.ToCell();
}
stock bool operator<=(char left, UInt16_t right)
{
    return left <= right.ToCell();
}
stock bool operator<=(char left, Int16_t right)
{
    return left <= right.ToCell();
}
stock bool operator!=(char left, UInt8_t right)
{
    return left != right.ToCell();
}
stock bool operator!=(char left, Int8_t right)
{
    return left != right.ToCell();
}
stock bool operator!=(char left, UInt16_t right)
{
    return left != right.ToCell();
}
stock bool operator!=(char left, Int16_t right)
{
    return left != right.ToCell();
}
stock bool operator==(char left, UInt8_t right)
{
    return left == right.ToCell();
}
stock bool operator==(char left, Int8_t right)
{
    return left == right.ToCell();
}
stock bool operator==(char left, UInt16_t right)
{
    return left == right.ToCell();
}
stock bool operator==(char left, Int16_t right)
{
    return left == right.ToCell();
}
stock int operator%(char left, UInt8_t right)
{
    return left % right.ToCell();
}
stock int operator%(char left, Int8_t right)
{
    return left % right.ToCell();
}
stock int operator%(char left, UInt16_t right)
{
    return left % right.ToCell();
}
stock int operator%(char left, Int16_t right)
{
    return left % right.ToCell();
}