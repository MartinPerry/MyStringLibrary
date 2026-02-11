#ifndef STD_STRING_COMPATIBILITY_H
#define STD_STRING_COMPATIBILITY_H

namespace mystrlib
{

	/// <summary>
	/// Class with std::string compatible methods
	/// </summary>
	template <typename Type>
	class StdStringCompatibility
	{
	public:

		void append(const char* str, size_t len = 0)
		{
			static_cast<Type*>(this)->Append(str, len);
		}

		void reserve(size_t s)
		{
			static_cast<Type*>(this)->ResizeBuffer(s + 1);
		}

		void resize(size_t s, char t = 0)
		{
			size_t strLength = static_cast<const Type*>(this)->length();
			if (s > strLength)
			{
				size_t dif = s - strLength;
				static_cast<Type*>(this)->ResizeBuffer(s + 1);
				static_cast<Type*>(this)->AppendMultiple(t, dif);
			}
			else if (s < strLength)
			{
				size_t dif = strLength - s;
				for (size_t i = 0; i < dif; i++)
				{
					static_cast<Type*>(this)->PopBack();
				}
			}
		}

		size_t find(const char c, size_t offset = 0) const
		{
			return static_cast<const Type*>(this)->Find(c, offset);
		}

		size_t find(const char* str, size_t offset = 0) const
		{
			return static_cast<const Type*>(this)->Find(str, offset);
		}

		size_t find(const Type& str, size_t offset = 0) const
		{
			return static_cast<const Type*>(this)->Find(str.c_str(), offset);
		}

		bool empty() const
		{
			return static_cast<const Type*>(this)->length() == 0;
		}

		Type substr(const size_t start = 0, const size_t length = Type::npos) const
		{
			if (length == Type::npos)
			{
				return static_cast<const Type*>(this)->SubString(static_cast<int>(start));
			}
			return static_cast<const Type*>(this)->SubString(static_cast<int>(start), length);
		}

		void pop_back() noexcept
		{
			static_cast<Type*>(this)->PopBack();
		}

		void push_back(char c)
		{
			static_cast<Type*>(this)->operator+=(c);
		}

		char& back() noexcept
		{
			return static_cast<Type*>(this)->GetLastChar();
		}
	};

}

#endif
