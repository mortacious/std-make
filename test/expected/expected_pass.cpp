//! \file test_expected.cpp

// Copyright Pierre Talbot 2013.
// Copyright Vicente J. Botet Escriba 2013,2014.

// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
//(See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)


#include <experimental/expected.hpp>

#include <boost/detail/lightweight_test.hpp>

namespace stde = std::experimental;

struct NoDefaultConstructible
{
  NoDefaultConstructible() = delete;
  NoDefaultConstructible(int )  {}
};

struct NoCopyConstructible
{
  NoCopyConstructible()  {}
  NoCopyConstructible(NoCopyConstructible const&) = delete;
  NoCopyConstructible(NoCopyConstructible &&) noexcept = default;
};
struct NoMoveConstructible
{
  NoMoveConstructible()  {}
  NoMoveConstructible(NoMoveConstructible const&) noexcept = default;
  NoMoveConstructible(NoMoveConstructible &&) = delete;
  NoMoveConstructible& operator=(NoMoveConstructible const&) noexcept = default;
  NoMoveConstructible& operator=(NoMoveConstructible &&) = delete;
};

enum State
{
    sDefaultConstructed,
    sValueCopyConstructed,
    sValueMoveConstructed,
    sCopyConstructed,
    sMoveConstructed,
    sMoveAssigned,
    sCopyAssigned,
    sValueCopyAssigned,
    sValueMoveAssigned,
    sMovedFrom,
    sValueConstructed
};

struct OracleVal
{
    State s;
    int i;
    constexpr OracleVal(int i = 0) : s(sValueConstructed), i(i) {}

};

struct Oracle
{
    State s;
    OracleVal val;

    Oracle() : s(sDefaultConstructed) {}
    Oracle(const OracleVal& v) : s(sValueCopyConstructed), val(v) {}
    Oracle(OracleVal&& v)  noexcept : s(sValueMoveConstructed), val(std::move(v)) {v.s = sMovedFrom;}
    Oracle(const Oracle& o) : s(sCopyConstructed), val(o.val) {}
    Oracle(Oracle&& o) noexcept : s(sMoveConstructed), val(std::move(o.val)) {o.s = sMovedFrom;}

    Oracle& operator=(const OracleVal& v) { s = sValueCopyConstructed; val = v; return *this; }
    Oracle& operator=(OracleVal&& v)  noexcept { s = sValueMoveConstructed; val = std::move(v); v.s = sMovedFrom; return *this; }
    Oracle& operator=(const Oracle& o) { s = sCopyConstructed; val = o.val; return *this; }
    Oracle& operator=(Oracle&& o) noexcept { s = sMoveConstructed; val = std::move(o.val); o.s = sMovedFrom; return *this; }
};

struct Guard
{
    std::string val;
    Guard() {}
    explicit Guard(std::string s, int = 0) : val(s) {}
    Guard(const Guard&) = delete;
    Guard(Guard&&) = delete;
    void operator=(const Guard&) = delete;
    void operator=(Guard&&) = delete;
};

struct ExplicitStr
{
    std::string s;
    explicit ExplicitStr(const char* chp) : s(chp) {}
};

struct Date
{
    int i;
    Date() = delete;
    Date(int i) noexcept : i{i} {}
    Date(Date&& d) noexcept : i(d.i) { d.i = 0; }
    Date(const Date&) = delete;
    Date& operator=(const Date&) = delete;
    Date& operator=(Date&& d) noexcept { i = d.i; d.i = 0; return *this;}
};

struct TExcept
{
    int i;
    TExcept() = delete;
    TExcept(int i) : i{i} {}
    TExcept(TExcept&& d) : i(d.i) { d.i = 0; }
    TExcept(const TExcept&) = delete;
    TExcept& operator=(const TExcept&) = delete;
    TExcept& operator=(TExcept&& d) { i = d.i; d.i = 0; return *this;}
};

template <class T>
struct MoveAware
{
  T val;
  bool moved;
  MoveAware(T val) : val(val), moved(false) {}
  MoveAware(MoveAware const&) = delete;
  MoveAware(MoveAware&& rhs) : val(rhs.val), moved(rhs.moved) {
    rhs.moved = true;
  }
  MoveAware& operator=(MoveAware const&) = delete;
  MoveAware& operator=(MoveAware&& rhs) {
    val = (rhs.val);
    moved = (rhs.moved);
    rhs.moved = true;
    return *this;
  }
};

struct OverloadedAddressOf
{
  OverloadedAddressOf() {}
  OverloadedAddressOf* operator&() const { return nullptr; }
};

//using namespace boost;
//using namespace boost::functional;

class test_exception : public std::exception
{
};

int throwing_fun(){ throw test_exception(); }
int nothrowing_fun(){ return 4; }

void void_throwing_fun(){ throw test_exception(); }
void do_nothing_fun(){}

void except_default_constructor()
{
  // From value constructor.
  stde::expected<int> e {};
  try {
    int i = e.value();
    (void)i;
    BOOST_TEST(true);
  } catch(...) {
    BOOST_TEST(false);
  };
  BOOST_TEST( e.valid() );
  BOOST_TEST( e );
  BOOST_TEST( static_cast<bool>(e) );
}


void except_default_constructor_error_code()
{
  // From value constructor.
  stde::expected<int, std::error_code> e;
  BOOST_TEST(e.valid());
  BOOST_TEST( e);
  BOOST_TEST( static_cast<bool>(e));
}

void except_default_constructor_constexpr()
{
  // From value constructor.
  BOOST_CONSTEXPR stde::expected<int,int> e;
  BOOST_TEST(e.valid());
}

void expected_from_value()
{
  using T = int;
  using E = std::error_code;

  static_assert(noexcept( stde::adl::swap_impl( std::declval<T&>(), std::declval<T&>() ) ), "");
  static_assert(std::is_nothrow_copy_constructible<E>::value, "");
  static_assert(noexcept( stde::adl::swap_impl( std::declval<E&>(), std::declval<E&>() ) ), "");

  static_assert(std::is_nothrow_copy_constructible<stde::expected<int>>::value, "");
  static_assert(std::is_nothrow_copy_assignable<stde::expected<int>>::value, "");
  static_assert(std::is_nothrow_move_constructible<stde::expected<int>>::value, "");
  static_assert(std::is_nothrow_move_assignable<stde::expected<int>>::value, "");

  // From value constructor.
  stde::expected<int> e(5);
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), 5);
  BOOST_TEST_EQ(*e, 5);
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}

void expected_from_value2()
{
  // From value constructor.
  stde::expected<int> e(5);
  e = {};
  BOOST_TEST( e.valid() );
  BOOST_TEST_EQ(e.value(), 0);
}

void expected_from_cnv_value()
{
  OracleVal v;
  stde::expected<Oracle> e(v);
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST(! ! e) ;
  BOOST_TEST(e.valid());
  BOOST_TEST(bool(e));
  BOOST_TEST_EQ(e.value().s,  sMoveConstructed);
  BOOST_TEST_EQ(v.s, sValueConstructed);

  stde::expected<Oracle> e2(std::move(v));
  //BOOST_REQUIRE_NO_THROW(e2.value());
  BOOST_TEST(! ! e2) ;
  BOOST_TEST(e2.valid());
  BOOST_TEST(bool(e2));
  BOOST_TEST_EQ(e.value().s, sMoveConstructed);
  BOOST_TEST_EQ(v.s, sMovedFrom);

}

struct NDCE              // no default constructor
{                        // (no default date exists)
  explicit NDCE(int) {}
};

void except_constructor_NDCE()
{
  stde::expected<NDCE> e {NDCE{1}};
  BOOST_TEST( e.valid() );

}
struct NDC              // no default constructor
{                        // (no default date exists)
  NDC(int) {}
};

void except_constructor_NDC()
{
  static_assert(std::is_nothrow_copy_constructible<stde::expected<NDC>>::value, "");
  static_assert(std::is_nothrow_copy_assignable<stde::expected<NDC>>::value, "");
  static_assert(std::is_nothrow_move_constructible<stde::expected<NDC>>::value, "");
  static_assert(std::is_nothrow_move_assignable<stde::expected<NDC>>::value, "");
  stde::expected<NDC> e {1};
  BOOST_TEST( e.valid() );
}

void except_constructor_Date()
{
  static_assert(std::is_nothrow_move_constructible<stde::expected<Date>>::value, "");
  static_assert(std::is_nothrow_move_assignable<stde::expected<Date>>::value, "");
  stde::expected<Date> e {Date{1}};
  BOOST_TEST( e.valid() );
}

void except_constructor_TExcept()
{
  static_assert( ! std::is_nothrow_move_constructible<stde::expected<TExcept>>::value, "");
  static_assert( ! std::is_nothrow_move_assignable<stde::expected<TExcept>>::value, "");
  stde::expected<TExcept> e {TExcept{1}};
  BOOST_TEST( e.valid() );
}

void expected_from_in_place_value()
{
   OracleVal v;
   stde::expected<Oracle> e{stde::in_place , v};
   //BOOST_REQUIRE_NO_THROW(e.value());
   BOOST_TEST(! ! e) ;
   BOOST_TEST(e.valid());
   BOOST_TEST(bool(e));
   BOOST_TEST_EQ(e.value().s, sValueCopyConstructed);
   BOOST_TEST_EQ(v.s, sValueConstructed);

   stde::expected<Oracle> e2{stde::in_place , std::move(v)};
   //BOOST_REQUIRE_NO_THROW(e2.value());
   BOOST_TEST(! ! e2) ;
   BOOST_TEST(e2.valid());
   BOOST_TEST(bool(e2));
   BOOST_TEST_EQ(e2.value().s, sValueMoveConstructed);
   BOOST_TEST_EQ(v.s, sMovedFrom);

}

void expected_from_exception()
{
  // From stde::unexpected_type constructor.
  stde::exception_or<int> e(stde::make_unexpected(test_exception()));
  BOOST_TEST_THROWS(e.value(), test_exception);
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);
}

void expected_from_copy_value()
{
  // From copy constructor.
  stde::expected<int> ef(5);
  stde::expected<int> e(ef);
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), 5);
  BOOST_TEST_EQ(*e, 5);
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}

void expected_from_copy_exception()
{
  // From stde::unexpected_type constructor.
  stde::exception_or<int> ef(stde::make_unexpected(test_exception()));
  stde::exception_or<int> e(ef);
  BOOST_TEST_THROWS(e.value(), test_exception);
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);
}

void expected_from_in_place()
{
  // From stde::in_place constructor.
  stde::expected<std::string> e(stde::in_place, "stde::in_place");
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), "stde::in_place");
  BOOST_TEST_EQ(*e, "stde::in_place");
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}

void expected_from_exception_ptr()
{
  // From exception_ptr constructor.
  stde::exception_or<int> e(stde::make_unexpected(std::make_exception_ptr(test_exception())));
  BOOST_TEST_THROWS(e.value(), test_exception);
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);
}

void expected_from_moved_value()
{
  // From move value constructor.
  std::string value = "my value";
  stde::expected<std::string> e = std::move(value);
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), "my value");
  BOOST_TEST_EQ(*e, "my value");
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}

void expected_from_catch_block()
{
  // From catch block
  try
  {
    throw test_exception();
  }
  catch(...)
  {
    stde::exception_or<int> e(stde::make_unexpected(std::current_exception()));

    BOOST_TEST_THROWS(e.value(), std::exception);
    BOOST_TEST_EQ(e.valid(), false);
    BOOST_TEST_EQ(static_cast<bool>(e), false);
  }
}


void make_expected_E_from_value()
{
  //auto e = stde::make_expected<std::string>( 5 );
  //BOOST_TEST_EQ(e.valid(), false);
}
void make_expected_const_from_value()
{
#if defined __clang__ && __clang_major__ >= 4 && __cplusplus > 201402L
  const int i=0;
  auto e = stde::make_expected<const int>( i );
  static_assert(std::is_same<decltype(e), stde::expected<const int>>::value, "");
  BOOST_TEST_EQ(e.valid(), true);
#endif
}
void make_expected_from_U_value()
{
  stde::expected<int> e = stde::make_expected<int>( short(5) );
  static_assert(std::is_same<decltype(e), stde::expected<int>>{}, "");
  BOOST_TEST_EQ(e.valid(), true);
}
void make_expected_from_U_value2()
{
  stde::expected<std::string> e = stde::make_expected<std::string>( "aa" );
  static_assert(std::is_same<decltype(e), stde::expected<std::string>>{}, "");
  BOOST_TEST_EQ(e.valid(), true);
}

void expected_from_value_error_condition()
{
  // From value constructor.
  stde::expected<int,std::error_condition> e(5);
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), 5);
  BOOST_TEST_EQ(*e, 5);
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}

void expected_from_error_error_condition()
{
  // From stde::unexpected_type constructor.
  stde::expected<int, std::error_condition> e(stde::unexpected_type<std::error_condition>(std::make_error_condition(std::errc::invalid_argument)));
  auto error_from_except_check = [](const stde::bad_expected_access<std::error_condition>& except)
  {
    return std::errc(except.error().value()) == std::errc::invalid_argument;
  };
  try {
    (void)e.value();
  } catch (stde::bad_expected_access<std::error_condition>& ex) {
    BOOST_TEST(error_from_except_check(ex));
  }
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);
}


void expected_from_error_convertible()
{
  {
    stde::expected<int, short> e1 = stde::make_unexpected(1);
    stde::expected<int, int> e2(e1);
    BOOST_TEST_EQ(e2.valid(), false);
    BOOST_TEST_EQ(static_cast<bool>(e2), false);
    BOOST_TEST_EQ(e2.error(), 1);
  }
  {
    stde::expected<void, short> e1 = stde::make_unexpected(1);
    stde::expected<void, int> e2(e1);
    BOOST_TEST_EQ(e2.valid(), false);
    BOOST_TEST_EQ(static_cast<bool>(e2), false);
    BOOST_TEST_EQ(e2.error(), 1);
  }
}

void except_valid_constexpr_int()
{
  // From value constructor.
  BOOST_CONSTEXPR stde::expected<int,int> e;
  BOOST_CONSTEXPR bool b = e.valid();
  BOOST_TEST(b);
}
void except_value_constexpr_int()
{
  // From value constructor.
  BOOST_CONSTEXPR stde::expected<int,int> e(1);
  BOOST_CONSTEXPR int x = e.value();
  BOOST_TEST_EQ(x, 1);
}

void expected_from_value3()
{
  stde::expected<int> e(5);
  BOOST_TEST_EQ(e.value(), 5);

  // From value assignment.
  e = 8;
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), 8);
  BOOST_TEST_EQ(*e, 8);
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}

void expected_from_copy_expected()
{
  stde::expected<int> e(5);
  stde::expected<int> e2(8);

  // From value assignment.
  e = e2;
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), 8);
  BOOST_TEST_EQ(*e, 8);
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}

void expected_from_moved_expected()
{
  stde::expected<std::string> e("e");
  stde::expected<std::string> e2("e2");

  // From value assignment.
  e = std::move(e2);
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), "e2");
  BOOST_TEST_EQ(*e, "e2");
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));

  //BOOST_REQUIRE_NO_THROW(e2.value());
  BOOST_TEST_EQ(e2.value(), "");
  BOOST_TEST_EQ(*e2, "");
  BOOST_TEST(e2.valid());
  BOOST_TEST(static_cast<bool>(e2));
}

void expected_from_in_place2()
{
  // From stde::in_place constructor.
  stde::expected<std::string> e(stde::in_place, "stde::in_place");
  BOOST_TEST_EQ(e.value(), "stde::in_place");

  // From emplace method.
  e.emplace("emplace method");
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), "emplace method");
  BOOST_TEST_EQ(*e, "emplace method");
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}

void expected_from_move_value()
{
  stde::expected<std::string> e("v");

  std::string value = "my value";
  // From assignment operator.
  e = std::move(value);
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), "my value");
  BOOST_TEST_EQ(*e, "my value");
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}


void expected_from_in_place3()
{
  // From stde::in_place factory.
  //auto e = stde::make_expected<std::string>("stde::in_place");
  auto e = stde::expected<std::string>("stde::in_place");
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), "stde::in_place");
  BOOST_TEST_EQ(*e, "stde::in_place");
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}
void expected_from_in_place_error()
{
  // From stde::in_place factory.
  auto e = stde::expected<std::string, std::error_condition>("stde::in_place");
  //BOOST_REQUIRE_NO_THROW(e.value());
  BOOST_TEST_EQ(e.value(), "stde::in_place");
  BOOST_TEST_EQ(*e, "stde::in_place");
  BOOST_TEST(e.valid());
  BOOST_TEST(static_cast<bool>(e));
}

void expected_from_exception_catch()
{
  // From catch block
  try
  {
    throw test_exception();
  }
  catch(...)
  {
    stde::exception_or<int> e = stde::make_unexpected(std::current_exception());

    BOOST_TEST_THROWS(e.value(), std::exception);
    BOOST_TEST_EQ(e.valid(), false);
    BOOST_TEST_EQ(static_cast<bool>(e), false);
  }
}

void expected_from_error()
{
  // From stde::unexpected_type constructor.
  auto e = stde::make_expected_from_error<int>(std::make_error_condition(std::errc::invalid_argument));
  auto error_from_except_check = [](const stde::bad_expected_access<std::error_condition>& except)
  {
    return std::errc(except.error().value()) == std::errc::invalid_argument;
  };
  try {
    (void)e.value();
  } catch (stde::bad_expected_access<std::error_condition>& ex) {
    BOOST_TEST(error_from_except_check(ex));
  }
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);
}

void expected_from_error_U()
{
  // From stde::unexpected_type constructor.
  auto e = stde::make_expected_from_error<int, short>(42);
  static_assert(std::is_same<decltype(e), stde::expected<int, short>>{}, "");
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);
}

void expected_from_exception2()
{
  // From stde::unexpected_type constructor.
  auto e = stde::make_expected_from_exception<int>(test_exception());
  //auto e = stde::expected<int>(stde::unexpected_type<>(test_exception()));
  BOOST_TEST_THROWS(e.value(), test_exception);
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);
}

void expected_from_exception_ptr2()
{
  // From exception_ptr constructor.
  auto e = stde::exception_or<int>(stde::make_unexpected(test_exception()));
  BOOST_TEST_THROWS(e.value(), test_exception);
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);
}

void make_expected_from_call_fun()
{
  try {
    stde::make_expected_from_call(throwing_fun);
    BOOST_TEST(true);
  } catch (...)
  {
    BOOST_TEST(false);
  }
  stde::exception_or<int> e = stde::make_expected_from_call(throwing_fun);
  BOOST_TEST_THROWS(e.value(), std::exception);
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);

  e = stde::make_expected_from_call(nothrowing_fun);
  try {
    (void)e.value();
    BOOST_TEST(true);
  } catch (...)
  {
    BOOST_TEST(false);
  }
  BOOST_TEST_EQ(e.value(), 4);
  BOOST_TEST_EQ(*e, 4);
  BOOST_TEST_EQ(e.valid(), true);
  BOOST_TEST_EQ(static_cast<bool>(e), true);

#if 0
  BOOST_TEST_THROWS(stde::make_expected_from_call<std::error_condition>(throwing_fun), test_exception);

  BOOST_TEST_NO_THROW(stde::make_expected_from_call<std::error_condition>(nothrowing_fun));
  stde::expected<int, std::error_condition> e2 = stde::make_expected_from_call<std::error_condition>(nothrowing_fun);
  BOOST_TEST_NO_THROW(e2.value());
  BOOST_TEST_EQ(e2.value(), 4);
  BOOST_TEST_EQ(*e2, 4);
  BOOST_TEST_EQ(e2.valid(), true);
  BOOST_TEST_EQ(static_cast<bool>(e2), true);
#endif
}

void make_expected_from_call_void_fun()
{
#if 0
  BOOST_TEST_NO_THROW(stde::make_expected_from_call(void_throwing_fun));
  stde::expected<void> e = stde::make_expected_from_call(void_throwing_fun);
  BOOST_TEST_THROWS(e.value(), std::exception);
  BOOST_TEST_EQ(e.valid(), false);
  BOOST_TEST_EQ(static_cast<bool>(e), false);

  e = stde::make_expected_from_call(do_nothing_fun);
  BOOST_TEST_NO_THROW(e.value());
  BOOST_TEST_EQ(e.valid(), true);
  BOOST_TEST_EQ(static_cast<bool>(e), true);

  BOOST_TEST_THROWS(stde::make_expected_from_call<std::error_condition>(void_throwing_fun), test_exception);

  try {
    stde::make_expected_from_call<std::error_condition>(do_nothing_fun);
    BOOST_TEST(true);
  } catch (...)
  {
    BOOST_TEST(false);
  }
  stde::expected<std::error_condition, void> e2 = stde::make_expected_from_call<std::error_condition>(do_nothing_fun);
  try {
    (void)e2.value();
    BOOST_TEST(true);
  } catch (...)
  {
    BOOST_TEST(false);
  }
  //BOOST_TEST_NO_THROW(e2.value());
  BOOST_TEST_EQ(e2.valid(), true);
  BOOST_TEST_EQ(static_cast<bool>(e2), true);
#endif
}

void expected_swap_value()
{
  // From value constructor.
  stde::expected<int> e(5);
  stde::expected<int> e2(8);

  e.swap(e2);

  BOOST_TEST_EQ(e.value(), 8);
  BOOST_TEST_EQ(e2.value(), 5);

  e2.swap(e);

  BOOST_TEST_EQ(e.value(), 5);
  BOOST_TEST_EQ(e2.value(), 8);
}

void expected_swap_exception()
{
  // From value constructor.
  stde::exception_or<int> e = stde::make_unexpected(std::invalid_argument("e"));
  stde::exception_or<int> e2 = stde::make_unexpected(std::invalid_argument("e2"));

  e.swap(e2);

  auto equal_to_e = [](const std::invalid_argument& except) { return std::string(except.what()) == "e"; };
  auto equal_to_e2 = [](const std::invalid_argument& except) { return std::string(except.what()) == "e2"; };

  try {
    (void)e.value();
    BOOST_TEST(true);
  } catch (std::invalid_argument& ex)
  {
    BOOST_TEST(equal_to_e2(ex));
  }
  try {
    (void)e2.value();
    BOOST_TEST(true);
  } catch (std::invalid_argument& ex)
  {
    BOOST_TEST(equal_to_e(ex));
  }

  e2.swap(e);

  try {
    (void)e.value();
    BOOST_TEST(true);
  } catch (std::invalid_argument& ex)
  {
    BOOST_TEST(equal_to_e(ex));
  }
  try {
    (void)e2.value();
    BOOST_TEST(true);
  } catch (std::invalid_argument& ex)
  {
    BOOST_TEST(equal_to_e2(ex));
  }
}

void expected_swap_function_value()
{
  // From value constructor.
  stde::expected<int> e(5);
  stde::expected<int> e2(8);

  swap(e, e2);

  BOOST_TEST_EQ(e.value(), 8);
  BOOST_TEST_EQ(e2.value(), 5);

  swap(e, e2);

  BOOST_TEST_EQ(e.value(), 5);
  BOOST_TEST_EQ(e2.value(), 8);
}


int main()
{

  static_assert(! std::is_default_constructible<NoDefaultConstructible>::value, "");
  static_assert(! std::is_default_constructible<stde::expected<NoDefaultConstructible>>::value, "");

  static_assert(! std::is_copy_constructible<NoCopyConstructible>::value, "");
  static_assert(! std::is_constructible<stde::expected<NoCopyConstructible>, NoCopyConstructible const& >::value, "");
  static_assert(! std::is_constructible<stde::exception_or<NoCopyConstructible>, stde::exception_or<NoCopyConstructible> const& >::value, "");
  static_assert(! std::is_copy_constructible<stde::exception_or<NoCopyConstructible>>::value, "");

  //fixme
  {
    NoMoveConstructible nmc;
    //NoMoveConstructible nmc2 = std::move(nmc); // FAILS as expected

    stde::expected<NoMoveConstructible> x{ std::move(nmc) }; // DOESN'T FAIL as copy is selected instead
    (void)x;
  }
  //fixme
#if defined __clang__ && __clang_major__ >= 4 && __cplusplus > 201402L
  {
    NoMoveConstructible nmc;
    //NoMoveConstructible nmc2 = std::move(nmc); // FAILS as expected

    stde::expected<NoMoveConstructible> x = std::move(nmc); // DOESN'T FAIL as copy is selected instead
    (void)x;
  }
#endif

  static_assert(! std::is_move_constructible<NoMoveConstructible>::value, "");
  static_assert( std::is_constructible<stde::expected<NoMoveConstructible>, NoMoveConstructible && >::value, "");
  static_assert( std::is_move_constructible<stde::expected<NoMoveConstructible>>::value, "");

  except_default_constructor();
  except_default_constructor_error_code();
  except_default_constructor_constexpr();
  expected_from_value();
  expected_from_value2();
  expected_from_cnv_value();
  except_constructor_NDCE();
  except_constructor_NDC();
  except_constructor_Date();
  expected_from_in_place_value();
  expected_from_exception();
  expected_from_copy_value();
  expected_from_copy_exception();
  expected_from_in_place();
  expected_from_exception_ptr();
  expected_from_moved_value();
  expected_from_catch_block();
  make_expected_E_from_value();
  make_expected_const_from_value();
  make_expected_from_U_value2();
  expected_from_value_error_condition();
  expected_from_error_error_condition();
  expected_from_error_convertible();
  except_valid_constexpr_int();
  except_value_constexpr_int();
  expected_from_value3();
  expected_from_copy_expected();
  expected_from_moved_expected();
  expected_from_in_place2();
  expected_from_move_value();
  expected_from_in_place3();
  expected_from_in_place_error();
  expected_from_exception_catch();
  expected_from_error();
  expected_from_error_U();
  expected_from_exception2();
  expected_from_exception_ptr2();
  make_expected_from_call_fun();
  make_expected_from_call_void_fun();
  expected_swap_value();
  expected_swap_exception();
  expected_swap_function_value();

  return ::boost::report_errors();
}

#if 0
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


//void expected_from_error_catch_exception)
//{
//  // From catch block
//  try
//  {
//    throw test_exception();
//  }
//  catch(...)
//  {
//    auto throw_lambda = [](){ return stde::make_expected_from_error<int,std::error_condition>();};
//
//    //BOOST_TEST_THROWS(throw_lambda(), test_exception);
//  }
//}



////////////////////////////////////
BOOST_AUTO_TEST_SUITE(expected_map)

void expected_map()
{
  auto fun = [](bool b) -> stde::expected<int>
  {
    if(b)
      return stde::make_expected(5);
    else
      return stde::make_unexpected(test_exception());
  };

  auto add_five = [](int sum) -> int
  {
    return sum + 5;
  };

  auto launch_except = [](int sum) -> int
  {
    throw test_exception();
  };

  stde::expected<int> e = fun(true).map(add_five);
  BOOST_TEST_NO_THROW(e.value());
  BOOST_TEST_EQ(*e, 10);

  e = fun(true).map(add_five).map(add_five);
  BOOST_TEST_NO_THROW(e.value());
  BOOST_TEST_EQ(*e, 15);

  e = fun(false).map(add_five).map(add_five);
  BOOST_TEST_THROWS(e.value(), test_exception);

  BOOST_TEST_THROWS(fun(true).map(launch_except), test_exception);

}

void expected_void_map()
{
  auto fun = [](bool b)
  {
    if(b)
      return stde::make_expected();
    else
      return stde::expected<void>(stde::make_unexpected(test_exception()));
  };

  auto launch_except = []() -> void
  {
    throw test_exception();
  };

  auto do_nothing = [](){};

  stde::expected<void> e = fun(true).map(do_nothing);
  BOOST_TEST_NO_THROW(e.value());

  e = fun(false).map(do_nothing);
  BOOST_TEST_THROWS(e.value(), test_exception);

  BOOST_TEST_THROWS(fun(true).map(launch_except), test_exception);

}

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////
BOOST_AUTO_TEST_SUITE(expected_bind)

void expected_bind()
{
  auto fun = [](bool b) -> stde::expected<int>
  {
    if(b)
      return stde::make_expected(5);
    else
      return stde::make_unexpected(test_exception());
  };

  auto add_five = [](int sum) -> stde::expected<int>
  {
    return stde::make_expected(sum + 5);
  };

  auto launch_except = [](int sum) -> stde::expected<int>
  {
    throw test_exception();
  };

  stde::expected<int> e = fun(true).bind(add_five);
  BOOST_TEST_NO_THROW(e.value());
  BOOST_TEST_EQ(*e, 10);

  e = fun(true).bind(add_five).bind(add_five);
  BOOST_TEST_NO_THROW(e.value());
  BOOST_TEST_EQ(*e, 15);

  e = fun(false).bind(add_five).bind(add_five);
  BOOST_TEST_THROWS(e.value(), test_exception);

  BOOST_TEST_THROWS(fun(true).bind(launch_except), test_exception);

}

void expected_void_bind()
{
  auto fun = [](bool b)
  {
    if(b)
      return stde::make_expected();
    else
      return stde::expected<void>(stde::make_unexpected(test_exception()));
  };

  auto launch_except = []() -> stde::expected<void>
  {
    throw test_exception();
  };

  auto do_nothing = [](){
    return stde::make_expected();
  };

  stde::expected<void> e = fun(true).bind(do_nothing);
  BOOST_TEST_NO_THROW(e.value());

  e = fun(false).bind(do_nothing);
  BOOST_TEST_THROWS(e.value(), test_exception);

  BOOST_TEST_THROWS(fun(true).bind(launch_except), test_exception);

}

BOOST_AUTO_TEST_SUITE_END()


////////////////////////////////////
BOOST_AUTO_TEST_SUITE(expected_then)

void expected_non_void_then()
{
  auto fun = [](bool b) -> stde::expected<int>
  {
    if(b)
      return stde::make_expected(5);
    else
      return stde::make_unexpected(test_exception());
  };


  auto add_five = [](int sum) -> int
  {
    return sum + 5;
  };
  auto six = []() -> int
  {
    return 6;
  };

  auto pair = [](int a) -> bool
  {
    return (a%2)==0;
  };

  auto launch_except = [](int sum) -> int
  {
    throw test_exception();
  };

  auto then_launch_except = [](expected<int>) -> int
  {
    throw test_exception();
  };

  stde::expected<int> e = fun(true).then(if_valued(add_five));
  BOOST_TEST_NO_THROW(e.value());
  BOOST_TEST_EQ(*e, 10);

  e = fun(true).then(if_valued(ident(six)));
  BOOST_TEST_NO_THROW(e.value());
  BOOST_TEST_EQ(*e, 6);

  e = fun(false).then(if_unexpected(ident(six)));
  BOOST_TEST_NO_THROW(e.value());
  BOOST_TEST_EQ(*e, 6);

  stde::expected<bool> e1 = fun(true).then(if_valued(pair));
  BOOST_TEST_NO_THROW(e1.value());
  BOOST_TEST_EQ(*e1, false);



  e = fun(true).then(if_valued(add_five)).then(if_valued(add_five));
  BOOST_TEST_NO_THROW(e.value());
  BOOST_TEST_EQ(*e, 15);

  e = fun(false).then(if_valued(add_five)).then(if_valued(add_five));
  BOOST_TEST_THROWS(e.value(), test_exception);

  BOOST_TEST_THROWS(fun(true).then(if_valued(launch_except)), test_exception);

  e = fun(false).then(catch_all(then_launch_except));
  BOOST_TEST_THROWS(e.value(), test_exception);

}

void expected_void_then()
{
  auto fun = [](bool b) -> stde::expected<void>
  {
    if(b)
      return stde::make_expected();
    else
      return stde::make_unexpected(test_exception());
  };

  auto launch_except = []()
  {
    throw test_exception();
  };

  auto do_nothing = [](){};

  BOOST_TEST(true);
  stde::expected<void> e = fun(true).then(if_valued(do_nothing));
  BOOST_TEST_NO_THROW(e.value());

  e = fun(false).then(if_valued(do_nothing));
  BOOST_TEST_THROWS(e.value(), test_exception);

  BOOST_TEST_THROWS(fun(true).then(if_valued(launch_except)), test_exception);

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(expected_recover)

void expected_recover()
{
  auto fun = [](bool b)
  {
    if(b)
      return stde::expected<int>(5);
    else
      return stde::expected<int>(stde::make_unexpected(test_exception()));
  };

  auto add_five = [](int sum) -> stde::expected<int>
  {
    return stde::make_expected(sum + 5);
  };

  auto recover_error = [](std::exception_ptr p)
  {
    return stde::make_expected(0);
  };

  auto recover_error_silent_failure = [](std::exception_ptr p)
  {
    return stde::expected<int>(stde::make_unexpected(p));
  };

  auto recover_error_failure = [](std::exception_ptr p) -> stde::expected<int>
  {
    return stde::expected<int>(stde::make_unexpected(test_exception()));
  };

  auto recover_error_throws = [](std::exception_ptr p) -> stde::expected<int>
  {
    throw test_exception();
  };

  BOOST_TEST_EQ(fun(false).catch_error(recover_error).valid(), true);
  BOOST_TEST_EQ(fun(false).catch_error(recover_error).value(), 0);
  BOOST_TEST_EQ(fun(true).catch_error(recover_error).value(), 5);
  BOOST_TEST_EQ(fun(false).catch_error(recover_error_silent_failure).valid(), false);
  BOOST_TEST_EQ(fun(false).catch_error(recover_error_failure).valid(), false);

  BOOST_TEST_EQ(fun(true).bind(add_five).value(), 10);
  BOOST_TEST_EQ(fun(true).bind(add_five).catch_error(recover_error).value(), 10);
  BOOST_TEST_EQ(fun(true).bind(add_five).catch_error(recover_error_silent_failure).value(), 10);
  BOOST_TEST_EQ(fun(true).bind(add_five).catch_error(recover_error_failure).value(), 10);

  BOOST_TEST_EQ(fun(false).catch_error(recover_error).bind(add_five).value(), 5);
  BOOST_TEST_EQ(fun(false).catch_error(recover_error).bind(add_five).bind(add_five).value(), 10);
  BOOST_TEST_EQ(fun(false).catch_error(recover_error_failure).bind(add_five).valid(), false);
  BOOST_TEST_EQ(fun(false).bind(add_five).catch_error(recover_error_failure).bind(add_five).valid(), false);
  BOOST_TEST_EQ(fun(false).bind(add_five).catch_error(recover_error_silent_failure).bind(add_five).valid(), false);

  BOOST_TEST_THROWS(fun(false).catch_error(recover_error_throws), test_exception);

}

void expected_void_recover()
{
  auto fun = [](bool b)
  {
    if(b)
      return stde::make_expected();
    else
      return stde::expected<void>(boost::stde::make_unexpected(test_exception()));
  };

  auto do_nothing = [](){
    return stde::make_expected();
  };

  auto recover_error = [](std::exception_ptr p)
  {
    return stde::make_expected();
  };

  auto recover_error_silent_failure = [](std::exception_ptr p)
  {
    return stde::expected<void>(boost::stde::make_unexpected(p));
  };

  auto recover_error_failure = [](std::exception_ptr p) -> stde::expected<void>
  {
    throw test_exception();
  };

  // The catch_error doesn't alter the stde::expected if it's valid.
  BOOST_TEST_EQ(fun(true).catch_error(recover_error_failure).valid(), true);

  // Simple catch_error tests.
  BOOST_TEST_EQ(fun(false).catch_error(recover_error).valid(), true);
  BOOST_TEST_THROWS(fun(false).catch_error(recover_error_failure), test_exception);
  BOOST_TEST_EQ(fun(false).catch_error(recover_error_silent_failure).valid(), false);

  // With a bind between.
  BOOST_TEST_THROWS(fun(false).bind(do_nothing).catch_error(recover_error_failure), test_exception);

}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(proposal)

void proposal_concept()
{
  using namespace std;

  {
  stde::expected<int, string> ei = 0;
  stde::expected<int, string> ej = 1;
  stde::expected<int, string> ek = stde::make_unexpected(string());

  ei = 1;
  ej = stde::make_unexpected(string());;
  ek = 0;

  ei = stde::make_unexpected(string());;
  ej = 0;
  ek = 1;
  }
}
void proposal_init()
{
  using namespace std;
  {
    string s{"STR"};

    stde::expected<string, int> ep{stde::make_unexpected(-1)};              // unexpected value, requires Movable<E>
    stde::expected<string, int> eq = {stde::make_unexpected(-1)};           // unexpected value, requires Movable<E>

    stde::expected<string> es{s};                    // requires Copyable<T>
    stde::expected<string> et = s;                   // requires Copyable<T>
    stde::expected<string> ev = string{"STR"};       // requires Movable<T>

    stde::expected<string> ew;                       // unexpected value
    stde::expected<string> ex{};                     // unexpected value
    stde::expected<string> ey = {};                  // unexpected value
    stde::expected<string> ez = stde::expected<string>{};  // unexpected value
  }

  {
    stde::expected<Guard, int> eg;                        // unexpected value
    stde::expected<Guard, int> eh{};                      // unexpected value
    stde::expected<Guard, int> ei{stde::in_place};               // calls Guard{} in place
    stde::expected<Guard, int> ej{stde::in_place, "arg"};        // calls Guard{"arg"} in place
  }

  {
    stde::expected<int, string> ei{unexpect};               // unexpected value, calls string{} in place
    stde::expected<int, string> ej{unexpect, "arg"};        // unexpected value, calls string{"arg"} in place
  }
}
void proposal_make_unexpected_fact()
{
  using namespace std;
  {
    stde::expected<string, int> opt1 = stde::make_unexpected(1);
    stde::expected<string, int> opt2 = {unexpect, 1};

    opt1 = stde::make_unexpected(1);
    opt2 = {unexpect, 1};
  }
}
void proposal_error_exception_ts()
{
  using namespace std;
  {
    stde::expected<int, error_exception<std::error_code, std::system_error> > e =
        stde::make_unexpected(make_error_code(errc::invalid_argument));
#if !defined BOOST_MSVC || BOOST_MSVC >= 1900
    BOOST_TEST(e.error()==make_error_code(errc::invalid_argument));
#else
    // VS2013 doesn't match operator==(boost::error_exception<std::error_code,std::system_error>, std::error_code)
    BOOST_TEST(e.error()==(error_exception<std::error_code, std::system_error>(make_error_code(errc::invalid_argument))));
#endif
    try {
      e.value();
      BOOST_TEST (false);
    } catch (std::system_error const& ex) {

    } catch (...) {
      BOOST_TEST (false);
    }
    stde::expected<int, error_exception<std::error_code, std::system_error> > e2 = e.get_unexpected();
#if !defined BOOST_MSVC || BOOST_MSVC >= 1900
    BOOST_TEST(e2.error()==make_error_code(errc::invalid_argument));
#else
    // VS2013 doesn't match operator==(boost::error_exception<std::error_code,std::system_error>, std::error_code)
    BOOST_TEST(e2.error()==(error_exception<std::error_code, std::system_error>(make_error_code(errc::invalid_argument))));
#endif
    try {
      e2.value();
      BOOST_TEST (false);
    } catch (std::system_error const& ex) {

    } catch (...) {
      BOOST_TEST (false);
    }

  }
}
void proposal_ensured_read_ts()
{
  using namespace std;
  {
    ensured_read<int> e = make_ensured_read(1);
    BOOST_TEST(e==1);
  }
  {
    ensured_read<int> e = make_ensured_read(1);
    stde::unexpected_type<ensured_read<int>> ue1 = stde::make_unexpected(std::move(e));
    BOOST_TEST(ue1.value()==1);
  }
//  {
//    stde::make_unexpected(make_ensured_read(1));
//    // calls to terminate.
//  }
//  {
//    stde::expected<int, ensured_read<int> > e = stde::make_unexpected(make_ensured_read(1));
//    // calls to terminate.
//  }
  {
    stde::expected<int, ensured_read<int> > e{1};
    BOOST_TEST(e.value()==1);
  }
  {
    stde::expected<int, ensured_read<int> > e = stde::make_unexpected(make_ensured_read(1));
    BOOST_TEST(e.error()==1);
  }
  {
    stde::expected<int, ensured_read<int> > e {unexpect, 1};
    BOOST_TEST(e.error()==1);
  }
  {
    ensured_read<std::exception_ptr> e = make_ensured_read(std::make_exception_ptr(1));
    BOOST_TEST_THROWS(std::rethrow_exception(e.value()), int);
  }
  {
    stde::expected<int, ensured_read<std::exception_ptr> > e = stde::make_unexpected(make_ensured_read(std::make_exception_ptr(1)));
    BOOST_TEST_THROWS(std::rethrow_exception(e.error().value()), int);
  }
}
void proposal_relational_operators()
{
  using namespace std;
  {
    stde::expected<unsigned, int> e0{0};
    stde::expected<unsigned, int> e1{1};
    stde::expected<unsigned, int> eN{unexpect, -1};

    BOOST_TEST (eN < e0);
    BOOST_TEST (e0 < e1);
    BOOST_TEST (! (e0 < eN));
    BOOST_TEST (eN <= e0);
    BOOST_TEST (e0 <= e1);

    BOOST_TEST (e0 > eN);
    BOOST_TEST (e1 > e0);
    BOOST_TEST (e0 >= eN);
    BOOST_TEST (e1 >= e0);

    BOOST_TEST (!(eN  < eN));
    BOOST_TEST (!(e1 < e1));
    BOOST_TEST (eN <= eN);
    BOOST_TEST (e1 <= e1);

    BOOST_TEST (eN != e0);
    BOOST_TEST (e0 != e1);
    BOOST_TEST (eN == eN);
    BOOST_TEST (e0 == e0);

    //////

    BOOST_TEST (eN == stde::make_unexpected(-1));
    BOOST_TEST (e0 != stde::make_unexpected(1));
    BOOST_TEST (eN != 1u);
    BOOST_TEST (e1 == 1u);

    BOOST_TEST (eN < 1u);
    BOOST_TEST (eN <= 1u);
    BOOST_TEST (1u > eN);
    BOOST_TEST (!(1u < eN));
    BOOST_TEST (1u >= eN);
    BOOST_TEST (stde::make_unexpected(1) < e0);
    BOOST_TEST (stde::make_unexpected(1) <= e0);
    BOOST_TEST (! (stde::make_unexpected(1) > e0));
    BOOST_TEST (! (stde::make_unexpected(1) >= e0));


    BOOST_TEST (!(e0 < stde::make_unexpected(1)));
    BOOST_TEST (!(e0 <= stde::make_unexpected(1)));
    BOOST_TEST (e0 > stde::make_unexpected(1));
    BOOST_TEST (e0 >= stde::make_unexpected(1));
  }
  {
    stde::expected<void, int> e0{boost::expect};
    stde::expected<void, int> eN{unexpect, -1};

    BOOST_TEST (!(e0 < e0));
    BOOST_TEST (eN < e0);
    BOOST_TEST (!(e0 < eN));
    BOOST_TEST (!(eN < eN));
    BOOST_TEST (e0 <= e0);
    BOOST_TEST (eN <= e0);
    BOOST_TEST (!(e0 <= eN));
    BOOST_TEST (eN <= eN);

    BOOST_TEST (!(e0 > e0));
    BOOST_TEST (e0 > eN);
    BOOST_TEST (!(eN > e0));
    BOOST_TEST (!(eN > eN));
    BOOST_TEST (e0 >= e0);
    BOOST_TEST (e0 >= eN);
    BOOST_TEST (!(eN >= e0));
    BOOST_TEST (eN >= eN);

    BOOST_TEST (!(e0 != e0));
    BOOST_TEST (eN != e0);
    BOOST_TEST (e0 != eN);
    BOOST_TEST (!(eN != eN));
    BOOST_TEST (e0 == e0);
    BOOST_TEST (!(eN == e0));
    BOOST_TEST (!(e0 == eN));
    BOOST_TEST (eN == eN);

    //////

    BOOST_TEST (eN == stde::make_unexpected(-1));
    BOOST_TEST (e0 != stde::make_unexpected(1));

    BOOST_TEST (stde::make_unexpected(1) < e0);
    BOOST_TEST (stde::make_unexpected(1) <= e0);
    BOOST_TEST (!(stde::make_unexpected(1) > e0));
    BOOST_TEST (!(stde::make_unexpected(1) >= e0));

    BOOST_TEST (!(stde::make_unexpected(1) < eN));
    BOOST_TEST (!(stde::make_unexpected(1) <= eN));
    BOOST_TEST (stde::make_unexpected(1) > eN);
    BOOST_TEST (stde::make_unexpected(1) >= eN);

    BOOST_TEST (!(eN < stde::make_unexpected(-1)));
    BOOST_TEST (eN <= stde::make_unexpected(-1));
    BOOST_TEST (!(eN > stde::make_unexpected(-1)));
    BOOST_TEST (eN >= stde::make_unexpected(-1));
  }
}
void proposal_dereference_operators()
{
  using namespace std;
  {
    const string s{"STR"};

    stde::expected<string> e0{s};
    const stde::expected<string> e1{s};
    BOOST_TEST(*e0.operator->() == s);
    BOOST_TEST(*e1.operator->() == s);

    // Test with class which has operator&() overloaded
    const OverloadedAddressOf o{};
    BOOST_TEST(&o == nullptr);

    stde::expected<OverloadedAddressOf> e2{o};
    const stde::expected<OverloadedAddressOf> e3{o};
    BOOST_TEST(e2.operator->() != nullptr);
    BOOST_TEST(e3.operator->() != nullptr);
  }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(movesem)
//////////////////////////////
void movesem_moved_from_state()
{
  // first, test mock:
  MoveAware<int> i{1}, j{2};
  BOOST_TEST (i.val == 1);
  BOOST_TEST (!i.moved);
  BOOST_TEST (j.val == 2);
  BOOST_TEST (!j.moved);

  MoveAware<int> k = std::move(i);
  BOOST_TEST (k.val == 1);
  BOOST_TEST (!k.moved);
  BOOST_TEST (i.val == 1);
  BOOST_TEST (i.moved);

  k = std::move(j);
  BOOST_TEST (k.val == 2);
  BOOST_TEST (!k.moved);
  BOOST_TEST (j.val == 2);
  BOOST_TEST (j.moved);

  // now, test stde::expected
  stde::expected<MoveAware<int>> oi{1}, oj{2};
  BOOST_TEST (oi);
  BOOST_TEST (!oi->moved);
  BOOST_TEST (oj);
  BOOST_TEST (!oj->moved);

  stde::expected<MoveAware<int>> ok{std::move(oi)};
  BOOST_TEST (ok);
  BOOST_TEST (!ok->moved);
  BOOST_TEST (oi);
  BOOST_TEST (oi->moved);

  ok = std::move(oj);
  BOOST_TEST (ok);
  BOOST_TEST (!ok->moved);
  BOOST_TEST (oj);
  BOOST_TEST (oj->moved);

}
void movesem_move_only_value()
{
  const auto make_int = []() {
    std::unique_ptr<int> value{new int};
    *value = 100;
    return value;
  };
  const auto return_void = [](std::unique_ptr<int> value) {
    BOOST_TEST(value != nullptr);
    BOOST_TEST(*value == 100);
  };
  const auto return_expected = [](std::unique_ptr<int> value) {
    BOOST_TEST(value != nullptr);
    BOOST_TEST(*value == 100);
    return stde::expected<void>{boost::expect};
  };
  const auto return_int = [](std::unique_ptr<int> value) {
    BOOST_TEST(value != nullptr);
    BOOST_TEST(*value == 100);
    return 200;
  };

  BOOST_TEST(expected<std::unique_ptr<int>>{make_int()}.map(return_void));
  BOOST_TEST(expected<std::unique_ptr<int>>{make_int()}.map(return_expected));
  BOOST_TEST(expected<std::unique_ptr<int>>{make_int()}.map(return_int));
}
void movesem_move_only_value2()
{
  const auto make_int = []() {
    std::unique_ptr<int> value{new int};
    *value = 100;
    return value;
  };
  const auto return_expected_void = [](std::unique_ptr<int> value) {
    BOOST_TEST(value != nullptr);
    BOOST_TEST(*value == 100);
    return stde::make_expected();
  };
  const auto return_expected = [](std::unique_ptr<int> value) {
    BOOST_TEST(value != nullptr);
    BOOST_TEST(*value == 100);
    return stde::expected<void>{boost::expect};
  };
  BOOST_TEST(expected<std::unique_ptr<int>>{make_int()}.bind(return_expected_void));
  BOOST_TEST(expected<std::unique_ptr<int>>{make_int()}.bind(return_expected));
}
void movesem_copy_move_ctor_optional_int()
{
  stde::expected<int> oi;
  stde::expected<int> oj = oi;

  BOOST_TEST (oj);
  BOOST_TEST (oj == oi);
  BOOST_TEST (bool(oj));

  oi = 1;
  stde::expected<int> ok = oi;
  BOOST_TEST (!!ok);
  BOOST_TEST (bool(ok));
  BOOST_TEST (ok == oi);
  BOOST_TEST (ok != oj);
  BOOST_TEST (*ok == 1);

  stde::expected<int> ol = std::move(oi);
  BOOST_TEST (!!ol);
  BOOST_TEST (bool(ol));
  BOOST_TEST (ol == oi);
  BOOST_TEST (ol != oj);
  BOOST_TEST (*ol == 1);
}
void movesem_expected_expected()
{
  stde::expected<expected<int, int>> oi1 = stde::make_unexpected(-1);
  BOOST_TEST (!oi1);

  {
  stde::expected<expected<int>> oi2 {expect};
  BOOST_TEST (bool(oi2));
  BOOST_TEST ((*oi2));
  //std::cout << typeid(**oi2).name() << std::endl;
  }

  {
  stde::expected<expected<int,int>> oi2 {expect, stde::make_unexpected(-1)};
  BOOST_TEST (bool(oi2));
  BOOST_TEST (!*oi2);
  }

  {
  stde::expected<expected<int>> oi2 {expected<int>{}};
  BOOST_TEST (bool(oi2));
  BOOST_TEST (*oi2);
  }

  stde::expected<int> oi;
  auto ooi = stde::make_expected(oi);
  static_assert( std::is_same<expected<expected<int>>, decltype(ooi)>::value, "");

}
BOOST_AUTO_TEST_SUITE_END()

void process(){}
void process(int ){}
void processNil(){}

BOOST_AUTO_TEST_SUITE(Examples)
//////////////////////////////

void example1()
{
  stde::expected<int> oi; // create disengaged object
  stde::expected<int> oj = {unexpect}; // alternative syntax
  oi = oj; // assign disengaged object
  stde::expected<int> ok = oj; // ok is disengaged

  if (oi) BOOST_TEST(false); // 'if oi is engaged...'
  if (!oi) BOOST_TEST(true); // 'if oi is disengaged...'

  BOOST_TEST(oi == ok); // two disengaged optionals compare equal

  ///////////////////////////////////////////////////////////////////////////
  stde::expected<int> ol{1}; // ol is engaged; its contained value is 1
  ok = 2; // ok becomes engaged; its contained value is 2
  oj = ol; // oj becomes engaged; its contained value is 1

  BOOST_TEST(oi != ol); // disengaged != engaged
  BOOST_TEST(ok != ol); // different contained values
  BOOST_TEST(oj == ol); // same contained value
  //BOOST_TEST(oi < ol); // disengaged < engaged
  //BOOST_TEST(ol < ok); // less by contained value

  /////////////////////////////////////////////////////////////////////////////
  stde::expected<int> om{1}; // om is engaged; its contained value is 1
  stde::expected<int> on = om; // on is engaged; its contained value is 1
  om = 2; // om is engaged; its contained value is 2
  BOOST_TEST (on != om); // on still contains 3. They are not pointers

  /////////////////////////////////////////////////////////////////////////////
  int i = *ol; // i obtains the value contained in ol
  BOOST_TEST (i == 1);
  *ol = 9; // the object contained in ol becomes 9
  BOOST_TEST(*ol == 9);
  BOOST_TEST(ol == stde::make_expected(9));

  ///////////////////////////////////
  int p = 1;
  stde::expected<int> op = p;
  BOOST_TEST(*op == 1);
  p = 2;
  BOOST_TEST(*op == 1); // value contained in op is separated from p

  ////////////////////////////////
  if (ol)
    process(*ol); // use contained value if present
  else
    process(); // proceed without contained value

  if (!om)
    processNil();
  else
    process(*om);

  /////////////////////////////////////////
  process(ol.value_or(0)); // use 0 if ol is disengaged

  ////////////////////////////////////////////
  ok = {unexpect}; // if ok was engaged calls T's dtor
  oj = {}; // assigns a temporary disengaged stde::expected
}
//////////////////////////////////////////////////
void ValueOr()
{
  stde::expected<int> oi = 1;
  int i = oi.value_or(0);
  BOOST_TEST (i == 1);

  oi = {unexpect};
  BOOST_TEST (oi.value_or(3) == 3);

  stde::expected<std::string> os{"AAA"};
  BOOST_TEST (os.value_or("BBB") == "AAA");
  os = {};
  BOOST_TEST (os);
  BOOST_TEST (os.value()=="");

  BOOST_TEST (os.value_or(std::string("BBB")) == "");
  {
    constexpr stde::expected<int> e = 1;
    static_assert(e.has_value(), "");
    static_assert(*e==1, "");
    static_assert(e.value()==1, "");
  }
  {
    constexpr std::error_code ec = std::make_error_code(std::errc(1));
    constexpr stde::expected<int> e = stde::make_unexpected(ec);
    static_assert(!e.has_value(), "");
    static_assert(e.error()==ec, "");
  }
  {
    constexpr stde::expected<OracleVal> e = 1;
    static_assert(e.has_value(), "");
    static_assert(*e==1, "");
    static_assert(e.value()==1, "");
  }
  {
    constexpr std::error_code ec = std::make_error_code(std::errc(1));
    constexpr stde::expected<OracleVal> e = stde::make_unexpected(ec);
    static_assert(!e.has_value(), "");
    static_assert(e.error()==ec, "");
  }
}

//////////////////////////////////////////////////
BOOST_AUTO_TEST_SUITE_END()

#endif
