#ifndef MIXINS_H
#define MIXINS_H

#include "types.h"
#include <utility>

template <template <typename> class U, template <typename> class... Ts>
struct mutator;

template <template <typename> class... Ts>
class composition : public Ts<mutator<Ts, Ts...>>...
{
public:
	composition() : Ts<mutator<Ts, Ts...>>()...
	{
	}

	composition(const composition<Ts...>& orig)
	    : Ts<mutator<Ts, Ts...>>(orig)...
	{
	}

	composition(composition<Ts...>&& orig)
	    : Ts<mutator<Ts, Ts...>>(std::move(orig))...
	{
	}

	virtual ~composition()
	{
	}
};

template <typename M, typename Requirements = TemplateTypes<>,
    typename Friends = TemplateTypes<>>
struct mixin : public M
{
	virtual ~mixin(){};
};

struct empty_mutator
{
	template <template <typename> class I>
	using friend_t = None;
	template <template <typename> class I>
	using requirement_t = None;

	template <template <typename> class I>
	None* mutate()
	{
		return nullptr;
	}
};

template <template <typename> class U, template <typename> class... Ts>
struct mutator
{
private:
	using mixin_list_t = Types<Ts<empty_mutator>...>;
	using composition_t = composition<Ts...>;
	using friends_t =
	    typename U<empty_mutator>::friends::template ToTypes<empty_mutator>;
	using requirements_t = typename U<
	    empty_mutator>::requirements::template ToTypes<empty_mutator>;

	// throw compilation error if not all requirements are met
	static constexpr bool is_valid =
	    mixin_list_t::template has_types<requirements_t>::value;
	using check = std::enable_if_t<is_valid>;

	template <template <typename> class I>
	static constexpr bool has_type =
	    mixin_list_t::template has_v<I<empty_mutator>>;
	template <template <typename> class I>
	static constexpr bool is_friend =
	    friends_t::template has_v<I<empty_mutator>>;
	template <template <typename> class I>
	static constexpr bool does_require =
	    requirements_t::template has_v<I<empty_mutator>>;

	// generate mutator type for mixin
	template <template <typename> class I>
	using mutator_t = mutator<I, Ts...>;
	// generate full mixin type
	template <template <typename> class I>
	using mixin_t = I<mutator_t<I>>;

public:
	template <template <typename> class I>
	using friend_t = std::enable_if_t<
	    is_friend<I>, std::conditional_t<has_type<I>, mixin_t<I>, None>>;

	template <template <typename> class I>
	using requirement_t = std::enable_if_t<does_require<I>, mixin_t<I>>;

	template <template <typename> class I>
	requirement_t<I>& mutate()
	{
		auto most_derived = static_cast<composition_t*>(this);
		return static_cast<mixin_t<I>&>(*most_derived);
	}

	virtual ~mutator()
	{
	}
};

template <typename Requirements, typename Friends>
struct mixin<empty_mutator, Requirements, Friends>
{
	using requirements = Requirements;
	using friends = Friends;
};

template <template <typename> class... Ts>
using requires = TemplateTypes<Ts...>;

template <template <typename> class... Ts>
using friends_with = TemplateTypes<Ts...>;

#endif