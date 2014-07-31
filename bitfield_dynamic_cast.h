// Ŭnicode please

#include "loki/Typelist.h"
#include <type_traits>

namespace sora
{
	struct IObject {
		virtual unsigned int GetInheritanceBitField() = 0;
	};

	template<int Mask>
	struct IBaseInterface {
		enum {
			Mask = Mask,
		};
		static bool IsDerived(IObject *child)
		{
			int childFlag = child->GetInheritanceBitField();
			return (Mask & childFlag) ? true : false;
		}
	};

	template<class TList> struct InheritanceBitField;
	template<>
	struct InheritanceBitField<Loki::NullType> {
		enum { value = 0 };
	};
	template<class T, class U>
	struct InheritanceBitField< Loki::Typelist<T, U> > {
		static_assert((T::Mask & InheritanceBitField<U>::value) == 0, "BitMask Conflict!");
		enum { value = T::Mask | InheritanceBitField<U>::value };
	};


	template<class TList> struct InheritanceGenerator;
	template<> struct InheritanceGenerator<Loki::NullType> {
		typedef Loki::NullType type;
	};
	template<class T, class U>
	struct InheritanceGenerator< Loki::Typelist<T, U> > {
		struct type : public T, public InheritanceGenerator<U>::type { };
	};


	template<typename TList>
	struct BitFieldInheritance : public IObject, public InheritanceGenerator<TList>::type {
		virtual unsigned int GetInheritanceBitField()
		{
			return InheritanceBitField<TList>::value;
		}
	};

	template<typename T>
	T bitfield_dynamic_cast(IObject* obj)
	{	
		if (std::remove_pointer<T>::type::IsDerived(obj))
		{
			return dynamic_cast<T>(obj);
		}
		return nullptr;
	}
}

