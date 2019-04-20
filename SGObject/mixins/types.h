#ifndef TYPES_H
#define TYPES_H

#include <type_traits>

struct None
{
};

template <typename... Args>
struct Types
{
	using Head = None;
	using Tail = None;

	static constexpr int size = 0;

	template <typename U>
	struct has : std::false_type
	{
	};
};

template <typename T1, typename... Args>
struct Types<T1, Args...> : Types<Args...>
{
	using Tail = Types<Args...>;
	using Head = T1;
	static constexpr int size = sizeof...(Args) + 1;

	template <typename U>
	struct has : std::integral_constant<
	                 bool, std::is_same<U, None>::value ||
	                           std::conditional_t<
	                               std::is_same<Head, U>::value, std::true_type,
	                               typename Tail::template has<U>>::value>
	{
	};

	template <typename TypesT>
	struct has_types
	    : std::integral_constant<
	          bool,
	          has<typename TypesT::Head>::value &&
	              std::conditional_t<
	                  std::is_same<typename TypesT::Tail, None>::value,
	                  std::true_type, has_types<typename TypesT::Tail>>::value>
	{
	};
};

template <template <typename> class... Args>
struct TemplateTypes
{
	template <typename U>
	using Head = None;
	using Tail = None;

	static constexpr int size = 0;
	template <typename U>
	using ToTypes = Types<>;
};

template <template <typename> class T1, template <typename> class... Args>
struct TemplateTypes<T1, Args...> : TemplateTypes<Args...>
{
	template <typename U>
	using Head = T1<U>;
	using Tail = TemplateTypes<Args...>;
	template <typename U>
	using ToTypes = Types<T1<U>, Args<U>...>;

	static constexpr int size = sizeof...(Args) + 1;
};

#endif