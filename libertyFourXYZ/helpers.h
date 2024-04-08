#pragma once
#include <type_traits>

namespace rage { class datResource; }
namespace libertyFourXYZ { class rsc85_layout; }

namespace helpers {
	namespace __ {
		template<typename T, typename = void>
		struct _hasPlace : std::false_type {};
		template<typename T>
		struct _hasPlace<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().place(NULL)), void>::value>> : std::true_type {};

		template<typename T, typename = void>
		struct _has_destructor : std::false_type {};
		template<typename T>
		struct _has_destructor<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().~T()), void>::value>> : std::true_type {};

		template<typename T, typename = void>
		struct _has_addToLayout : std::false_type {};
		template<typename T>
		struct _has_addToLayout<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().addToLayout(NULL, 0)), void>::value>> : std::true_type {};

		template<typename T, typename = void>
		struct _has_replacePtrs : std::false_type {};
		template<typename T>
		struct _has_replacePtrs<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().replacePtrs(NULL, NULL, 0)), void>::value>> : std::true_type {};

		template<typename T, typename = void>
		struct _has_clearRefCount : std::false_type {};
		template<typename T>
		struct _has_clearRefCount<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().clearRefCount()), void>::value>> : std::true_type {};

		template<typename T, typename = void>
		struct _hasSetRefCount : std::false_type {};
		template<typename T>
		struct _hasSetRefCount<T, std::enable_if_t<std::is_same<decltype(std::declval<T>().setRefCount()), void>::value>> : std::true_type {};

		template<typename T, typename = void>
		struct _has_usage_count : std::false_type {};
		template <typename T>
		struct _has_usage_count<T, std::enable_if_t<std::is_arithmetic<decltype(std::declval<T>().m_usageCount)>::value>> : std::true_type {};

		int _addObjectWithCounterToLayout(libertyFourXYZ::rsc85_layout* pLayout, size_t size, char blockType, void* pObj);
		int _replacePtrsInObjWithCounter(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, size_t depth, void* pObj);
		void* _replacePtrsInObjWithCounter2(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, size_t size, void** pObj);
		void* _replacePtrsInObjWithCounter3(libertyFourXYZ::rsc85_layout* pLayout, rage::datResource* pRsc, size_t size, void** pObj);
	}

	template<typename T> constexpr bool hasPlace = __::_hasPlace<T>::value;
	template<typename T> constexpr bool hasDestructor = __::_has_destructor<T>::value;
	template<typename T> constexpr bool hasAddToLayout = __::_has_addToLayout<T>::value;
	template<typename T> constexpr bool hasReplacePtrs = __::_has_replacePtrs<T>::value;
	template<typename T> constexpr bool hasClearUsageCount = __::_has_clearRefCount<T>::value;
	template<typename T> constexpr bool hasSetUsageCount = __::_hasSetRefCount<T>::value;
	template<typename T> constexpr bool hasUsageCount = __::_has_usage_count<T>::value;

	template<typename _Type> void addObjectWithCounterToLayout(void* _pLayout, char blockType,size_t depth,_Type *pObj) {
		int result = __::_addObjectWithCounterToLayout((libertyFourXYZ::rsc85_layout*)_pLayout, sizeof(_Type), blockType, pObj);
		if (result != 0)
			return;

		if constexpr (helpers::hasAddToLayout<_Type>)
			pObj->addToLayout((libertyFourXYZ::rsc85_layout*)_pLayout, depth);
	}

	template<typename _Type> void replacePtrsInObjWithCounter(void* _pLayout, void* _pRsc, size_t depth, _Type *&pObj) {
		int result = __::_replacePtrsInObjWithCounter((libertyFourXYZ::rsc85_layout*)_pLayout, (rage::datResource*)_pRsc, depth, pObj);
		if (result != 0) {
			__::_replacePtrsInObjWithCounter3((libertyFourXYZ::rsc85_layout*)_pLayout, (rage::datResource*)_pRsc, sizeof(_Type), (void**) & pObj);
			return;
		}

		if (helpers::hasReplacePtrs<_Type>)
			pObj->replacePtrs((libertyFourXYZ::rsc85_layout*)_pLayout, (rage::datResource*)_pRsc, depth);

		//pObj = (_Type*)__::_replacePtrsInObjWithCounter2((libertyFourXYZ::rsc85_layout*)_pLayout, (rage::datResource*)_pRsc, sizeof(_Type), pObj);
		__::_replacePtrsInObjWithCounter2((libertyFourXYZ::rsc85_layout*)_pLayout, (rage::datResource*)_pRsc, sizeof(_Type), (void**)&pObj);
	}


}