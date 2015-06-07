#include "function_traits.hh"

#include <algorithm>
#include <cassert>
#include <limits>
#include <type_traits>
#include <iostream>

#include <random>

#include <cmath>

template<typename Domain>
struct RealEqual {
    constexpr RealEqual(Domain delta) : delta(delta) {}
    constexpr bool operator()(Domain lhs, Domain rhs) const
    { return std::abs(lhs - rhs) < delta; }
    const Domain delta;
};

template<typename container_type, size_t count>
container_type ranf(typename container_type::value_type a,
                    typename container_type::value_type b)
{
    std::random_device r_dev;
    std::mt19937 engine(r_dev());
    std::uniform_real_distribution<> dist(a, b);
//    container_type c(count);
//    std::generate_n(std::begin(c), std::end(c), dist(engine));
    container_type c;
    c.reserve(count);
    for(size_t i = 0; i < count; i++)
        c.push_back(dist(engine));

    return c;
}


template<typename F> using domain0
= typename types::remove_cvr<typename types::function_traits<F>::
                             template arg<0>::type>::type;

template<typename F, typename P>
domain0<F> collison_point(const domain0<F> &x, F f, P p)
{
    static_assert(types::is_transformation<F>::value, "");
    static_assert(types::is_unary_predicate<P>::value, "");
    static_assert(std::is_same<domain0<P>, domain0<F>>::value, "");

    if(!p(x))
        return x;
    domain0<F> slow = x; // f^0(x)
    domain0<F> fast = f(x); // f^1(x)
    while(fast != slow) {
        slow = f(slow);
        if(!p(fast))
            return fast;
        fast = f(fast);
        if(!p(fast))
            return fast;
        fast = f(fast);
    }
    return fast;
}

template<typename F, typename P>
bool terminating_orbit(const domain0<F> &x, F f, P p)
{
    static_assert(types::is_transformation<F>::value, "");
    static_assert(types::is_unary_predicate<P>::value, "");
    static_assert(std::is_same<domain0<P>, domain0<F> >::value, "");

    return !p(collison_point(x, f, p));
}

template<typename F, typename P>
bool circular_orbit(const domain0<F> &x, F f, P p)
{
    static_assert(types::is_transformation<F>::value, "");
    static_assert(types::is_unary_predicate<P>::value, "");
    static_assert(std::is_same<domain0<P>, domain0<F> >::value, "");

    domain0<F>  y = collison_point(x, f, p);
    return p(y) && x == f(y);
}

template<typename T>
bool definition_space_addition_runtime(T lhs, T rhs)
{
// https://www.securecoding.cert.org/confluence/display/c/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow

    static_assert(std::is_signed<T>::value, "!is_signed");
    if(((rhs > 0) && (lhs > (std::numeric_limits<T>::max() - rhs)))
        || ((rhs < 0) && (lhs < (std::numeric_limits<T>::min() - rhs))))
        return false;
    return true;
}

template<typename I, typename Op>
domain0<Op> reduce_nonempty(I first, I last, Op op)
{
    static_assert(types::is_operation<Op>::value, "");

    domain0<Op> r = *first;
    first++;
    while(first != last) {
        r = op(r, *first);
        first++;
    }
    return r;
}

template<typename I, typename Op>
domain0<Op> reduce(I first, I last, Op op, domain0<Op> z)
{
    static_assert(types::is_operation<Op>::value, "");

    if(first == last)
        return z;
    return reduce_nonempty(first, last, op);
}

void eop_examples()
{
    struct defspace {
        bool operator()(int x) { return definition_space_addition_runtime(x, 1); }
    } p;
    const int x = 0x7fffffff - 133;
    const auto c = collison_point(x, [](int x) { return x + 1; }, p);
    (void)c;
    std::cout << "collison_point(" << x << ") -> " << c
              << " for f(x) -> x + 1"
              << ", is terminating: "
              << terminating_orbit(x, [](int x) { return x + 1; }, p)
              << ", circular: "
              << circular_orbit(x, [](int x) { return x + 1; }, p)
              << '\n';

        const auto L = 0.13f, R = 199.17f;
        const auto v = ranf<std::vector<float>, 1024u>(L, R);
        const auto op = [](const float lhs, const float rhs) {
            return lhs + rhs; };

        const auto r1 = reduce_nonempty(std::begin(v), std::end(v), op);
        const auto r2 = reduce(std::begin(v), std::end(v), op, 0);
        const auto r = std::accumulate(std::begin(v), std::end(v), 0.f, op);

        const RealEqual<float> req(0.000001f);
        if(!req(r, r1)) {
            std::cout << "reduce: r != r1 " << r << " / " << r1 << "\n";
            assert(true);
        }
        if(!req(r, r2)) {
            std::cout << "reduce: r != r2 " << r << " / " << r1 << "\n";
            assert(true);
        }
}

int main()
{
    // test with code from Elements of Programming
    eop_examples();
}
