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
	    : Ts<mutator<Ts, Ts...>>(
	          static_cast<const Ts<mutator<Ts, Ts...>>&>(orig))...
	{
	}
};

template <
    typename M, typename Requirements = TemplateTypes<>,
    typename Friends = TemplateTypes<>>
struct mixin : public M
{
	using requirements = Requirements;
	using friends = Friends;

	virtual ~mixin(){};
};

struct empty_mutator
{
	template <template <typename> class I>
	using friend_t = None;
	template <template <typename> class I>
	using requirement_t = None;

	template <template <typename> class I>
	void* mutate()
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
	using client_friends_t =
	    typename U<empty_mutator>::friends::template ToTypes<empty_mutator>;
	using client_req_t = typename U<
	    empty_mutator>::requirements::template ToTypes<empty_mutator>;

	// throw compilation error if not all requirements are met
	constexpr static bool is_valid =
	    mixin_list_t::template has_types<client_req_t>::value;
	using check = std::enable_if_t<is_valid>;

	template <template <typename> class I>
	using has_type_t = typename mixin_list_t::template has<I<empty_mutator>>;
	template <template <typename> class I>
	using is_friend_t =
	    typename client_friends_t::template has<I<empty_mutator>>;
	template <template <typename> class I>
	using does_require_t =
	    typename client_req_t::template has<I<empty_mutator>>;

	// generate mutator type for mixin
	template <template <typename> class I>
	using mutator_t = mutator<I, Ts...>;
	// generate full mixin type
	template <template <typename> class I>
	using mixin_t = I<mutator_t<I>>;

public:
	template <template <typename> class I>
	using friend_t = std::enable_if_t<
	    is_friend_t<I>::value,
	    std::conditional_t<has_type_t<I>::value, mixin_t<I>, None>>;

	template <template <typename> class I>
	using requirement_t =
	    std::enable_if_t<does_require_t<I>::value, mixin_t<I>>;

	template <template <typename> class I>
	requirement_t<I>* mutate()
	{
		auto most_derived = static_cast<composition_t*>(this);
		return static_cast<mixin_t<I>*>(most_derived);
	}
};

template <template <typename> class... Ts>
using requires = TemplateTypes<Ts...>;

template <template <typename> class... Ts>
using friends_with = TemplateTypes<Ts...>;

#endif