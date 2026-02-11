#ifndef STD_SUPPORT_H
#define STD_SUPPORT_H

#include "../MyStringView.h"
#include "../MyString.h"
#include "../MySmallString.h"
#include "../MyStringID.h"


//For use in std::unordered_map
//http://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
namespace std
{
	template <>
	struct hash <mystrlib::StringId>
	{
		size_t operator()(const mystrlib::StringId& k) const
		{
			return k.GetHashId();
		};
	};

	template <>
	struct hash<mystrlib::String>
	{
		std::size_t operator()(const mystrlib::String& k) const
		{
			return static_cast<size_t>(k.GetHashCode());
		};
	};


	template <>
	struct hash<mystrlib::SmallString>
	{
		std::size_t operator()(const mystrlib::SmallString& k) const
		{
			return k.GetHashCode();
		};
	};

	template <>
	struct hash<mystrlib::StringView>
	{
		std::size_t operator()(const mystrlib::StringView& k) const
		{
			return static_cast<size_t>(k.GetHashCode());
		};
	};
};


#endif