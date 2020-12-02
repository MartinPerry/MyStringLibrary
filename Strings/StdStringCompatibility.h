#ifndef STD_STRING_COMPATIBILITY_H
#define STD_STRING_COMPATIBILITY_H

/// <summary>
/// Class with std::string compatible methods
/// </summary>
template <typename Type>
class StdStringCompatibility 
{
public:

	void append(const char * str, size_t len = 0)
	{
		static_cast<Type *>(this)->Append(str, len);
	}

	void reserve(size_t s)
	{
		static_cast<Type *>(this)->ResizeBuffer(s);
	}

	size_t find(const char c) const
	{
		return static_cast<const Type *>(this)->Find(c);
	}

	size_t find(const char * str, size_t offset = 0) const
	{		
		return static_cast<const Type *>(this)->Find(str, offset);
	}

	Type substr(const size_t start = 0, const size_t length = Type::npos) const
	{
		if (length == Type::npos)
		{
			return static_cast<const Type *>(this)->SubString(static_cast<int>(start));
		}
		return static_cast<const Type *>(this)->SubString(static_cast<int>(start), length);
	}

	void pop_back() noexcept
	{		
		static_cast<Type*>(this)->PopBack();
	}

	void push_back(char c)
	{
		static_cast<Type*>(this)->operator+=(c);
	}

	char & back() noexcept
	{
		static_cast<Type*>(this)->GetLastChar();
	}
};

#endif
