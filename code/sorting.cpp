// Copyright Louis Dionne 2015
// Distributed under the Boost Software License, Version 1.0.

#include <boost/hana.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/less.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/partition.hpp>
#include <boost/mpl/protect.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/single_view.hpp>
#include <boost/mpl/vector.hpp>


namespace then {
using namespace boost::mpl;

// sample(sorting-then)
template <typename Sequence, typename Pred>
struct sort;

template <typename Pred, typename Pivot>
struct sort_pred {
  template <typename T>
  using apply = apply2<Pred, T, Pivot>;
};

template <typename Sequence, typename Pred>
struct sort_impl {
  using pivot = typename begin<Sequence>::type;
  using parts = typename partition<
    iterator_range<typename next<pivot>::type,
                   typename end<Sequence>::type>
    , protect<sort_pred<Pred, typename deref<pivot>::type>>
    , back_inserter<vector<>>
    , back_inserter<vector<>>
  >::type;

  using part1 = typename push_back<
    typename sort<typename parts::first, Pred>::type,
    typename deref<pivot>::type
  >::type;

  using part2 = typename sort<typename parts::second, Pred>::type;

  using type = typename insert_range<
    part1, typename end<part1>::type, part2
  >::type;
};

template <typename Sequence, typename Pred>
struct sort
  : eval_if<empty<Sequence>,
      identity<Sequence>,
      sort_impl<Sequence, Pred>
  >
{ };
// end-sample

static_assert(equal<
  sort<vector<>, quote2<less>>::type,
  vector<>
>::value, "");

static_assert(equal<
  sort<vector<int_<1>>, quote2<less>>::type,
  vector<int_<1>>
>::value, "");

static_assert(equal<
  sort<vector<int_<2>, int_<1>>, quote2<less>>::type,
  vector<int_<1>, int_<2>>
>::value, "");

static_assert(equal<
  sort<vector<int_<3>, int_<2>, int_<1>>, quote2<less>>::type,
  vector<int_<1>, int_<2>, int_<3>>
>::value, "");

static_assert(equal<
  sort<vector<int_<4>, int_<3>, int_<2>, int_<1>>, quote2<less>>::type,
  vector<int_<1>, int_<2>, int_<3>, int_<4>>
>::value, "");
}

namespace now {
using namespace boost::hana;

// sample(sorting-now)
auto sort = fix([](auto sort, auto xs, auto pred) {
  return eval_if(length(xs) < size_t<2>,
    lazy(xs),
    lazy([=](auto xs) {
      auto pivot = head(xs);
      auto rest = tail(xs);
      auto parts = partition(rest, partial(pred, pivot));
      return concat(
        sort(second(parts), pred),
        prepend(pivot, sort(first(parts), pivot))
      );
    })(xs)
  );
});
// end-sample

void test() {
  BOOST_HANA_CONSTANT_CHECK(
    sort(make_tuple(), less) == make_tuple()
  );

  BOOST_HANA_CONSTANT_CHECK(
    sort(make_tuple(int_<1>), less) == make_tuple(int_<1>)
  );

  BOOST_HANA_CONSTANT_CHECK(
    sort(make_tuple(int_<2>, int_<1>), less) == make_tuple(int_<1>, int_<2>)
  );

  BOOST_HANA_CONSTANT_CHECK(
    sort(make_tuple(int_<3>, int_<2>, int_<1>), less) ==
      make_tuple(int_<1>, int_<2>, int_<3>)
  );

  BOOST_HANA_CONSTANT_CHECK(
    sort(make_tuple(int_<4>, int_<3>, int_<2>, int_<1>), less) ==
      make_tuple(int_<1>, int_<2>, int_<3>, int_<4>)
  );
}
}

int main() {
  now::test();
}
