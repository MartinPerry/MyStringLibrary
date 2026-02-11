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
	struct hash <mystrlib::MyStringId>
	{
		size_t operator()(const mystrlib::MyStringId& k) const
		{
			return k.GetHashId();
		};
	};

	template <>
	struct hash<mystrlib::MyString>
	{
		std::size_t operator()(const mystrlib::MyString& k) const
		{
			return static_cast<size_t>(k.GetHashCode());
		};
	};


	template <>
	struct hash<mystrlib::MySmallString>
	{
		std::size_t operator()(const mystrlib::MySmallString& k) const
		{
			return k.GetHashCode();
		};
	};

	template <>
	struct hash<mystrlib::MyStringView>
	{
		std::size_t operator()(const mystrlib::MyStringView& k) const
		{
			return static_cast<size_t>(k.GetHashCode());
		};
	};
};


#endif