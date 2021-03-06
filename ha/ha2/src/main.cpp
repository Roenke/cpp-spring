#include <iostream>
#include <cassert>
#include <cstring>
#include "lazy_string.h"

using namespace std_utils;

void test_internal_typedefs() {
    assert(!lazy_string::traits_type::compare(
        "abc", "abc", sizeof("abc")));
    lazy_string::value_type chr = 'a';
    lazy_string::reference chr_ref = chr;
    lazy_string::const_reference cchr_ref = chr;
    lazy_string::pointer chr_ptr = &chr;
    lazy_string::const_pointer cchr_ptr = &cchr_ref;
    lazy_string::difference_type diff = chr_ptr - chr_ptr;
    lazy_string::size_type size = chr_ptr - chr_ptr;
    (void)chr;
    (void)chr_ref;
    (void)cchr_ref;
    (void)chr_ptr;
    (void)cchr_ptr;
    (void)diff;
    (void)size;
}

template<class STRING>
bool str_equal(const STRING& str1, const STRING& str2) {
    if (str1.size() != str2.size())
        return false;

    for (size_t i = 0; i < str1.size(); ++i) {
        if (STRING::traits_type::compare(&str1[i], &str2[i], 1))
            return false;
    }
    return true;
}

void test_empty_string() {
    lazy_string str_empty;
    assert(str_empty.empty());
    assert(str_empty.size() == 0);
    str_empty.clear();
    assert(str_empty.empty());
}

void test_constructors() {
    const char *c_str = "abcdefghijklmnop";
    lazy_string str1 = c_str;
    lazy_string str2 = str1;
    assert(str_equal<lazy_string>(c_str, str1));
    assert(str_equal<lazy_string>(str1, str2));

    lazy_string str3(10, 'a');
    assert(str3[0] == 'a');
    assert(str3.size() == 10);
    
    lazy_string str4(3, 'a');
    assert(3 == str4.size());
    assert(str4[2] == 'a');
}

void test_assignment_operator() {
    lazy_string str1("abcdefg");
    (void)str1;
    lazy_string str2("77712312ASD ASD sdasd");
    str1 = move(str2);
    (void)str1;
    assert(str2.empty()); // one of possible states after move
}

void test_plus_operator() {
    lazy_string str("abc");
    str += 'c';
    assert(str_equal<lazy_string>(str, "abcc"));

    str = "abc";
    assert(str_equal<lazy_string>('c' + str, "cabc"));
    assert(str_equal<lazy_string>(str + str, "abcabc"));
    assert(str_equal<lazy_string>("123" + str, "123abc"));
}

void test_index_operator() {
    const lazy_string cstr("1234567890");
    lazy_string str("1234567890");
    assert(cstr[0] == '1');
    auto str_0 = str[0];
    assert(str[0] == '1');
    assert(str_0 == '1');
}

void test_relational_operators() {
    assert(lazy_string("fgh") < lazy_string("fgh1"));
    assert(lazy_string("z") > lazy_string("fgh1"));
    assert(lazy_string("fgH") == lazy_string("fgH"));
    assert(lazy_string("fgH") <= lazy_string("fgH"));
    assert(lazy_string("fgH") >= lazy_string("fgH"));
}

void test_c_str() {
    const char* cstr = "I am cstr!";
    lazy_string str(cstr);
    assert(!strcmp(cstr, str.c_str()));
    cstr = str.c_str();
    assert(lazy_string("djghd") != cstr);
}

void test_swap() {
    lazy_string str1;
    lazy_string str2;
    str1.swap(str2);
    str1 = "123";
    str1.swap(str2);
    assert(str2 == "123");
}

void test_lazy_wstring() {
    lazy_wstring str1(L"Hell\xF6\x0A");
    lazy_wstring str2(L"Hell\xF6\x0A");
    lazy_wstring str3(L"Hell\xF7\x0A");
    assert(str1 == str2);
    assert(str1 != str3);
    assert(str2 != str3);

    str3[4] = L'\xF6';
    assert(str1 == str3);
}

void test_lazy() {
    lazy_string str1("abc");
    lazy_string str2(str1);
    lazy_string str3 = str2;
    assert(str1[0] == str3[0]);
    assert(str3.use_count() == 3);
    str3[0] = 'a';
    assert(str1.use_count() == str3.use_count());
    str3[0] = 'b';
    assert(str1[0] == 'a');
    assert(str3.use_count() == 1);
    assert(str1.use_count() == 2);
}

void test_comparison() {
    assert("a" < lazy_string("b"));
    assert(lazy_string("a") < lazy_string("b"));
    assert("a" == lazy_string("a"));
    assert(lazy_string("b") != "a");
    assert(lazy_string("b") >= "a");
    assert("b" > lazy_string("a"));
    assert(lazy_string("b") > "a");

    assert("c" > lazy_string(1, 'b'));
    assert("b" < lazy_string(2, 'b'));
    assert(lazy_string(5, 'c') == "ccccc");

    lazy_string str1('b', 5);
    assert("aa" < lazy_string(10, 'b'));
    assert("bbbb" < lazy_string(5, 'b'));
    assert("bbbb" <= lazy_string("bbbb"));
}

void test_icomparison() {
    assert(lazy_istring("abc") == lazy_istring("abc"));
    assert(lazy_istring("ABC") == lazy_istring("abc"));
    assert(lazy_istring("AbCd") == "abcd");
    assert(lazy_istring("ABC") <= lazy_istring("abc"));    
}

void test_concat() {
    lazy_string str1("a");
    assert(str1 + 'b' == "ab");
    assert('b' + str1 == "ba");
    assert("b" + str1 == "ba");
    assert(str1 + "b" == "ab");
    str1 += 'b';
    assert("a" != str1);
    assert("ab" == str1);
    assert(str1 == "ab");
    str1 += "cd";
    assert("abcd" == str1);
}

void test_bad_proxy() {
    lazy_string str("abc");
    auto good = [&str]() { return str[0]; };
    auto bad = []() { lazy_string str1("abc"); return str1[0]; };
    auto std_example = []() {std::string str1("abc"); return str1[0]; };

    auto good_proxy = good();
    auto bad_proxy = bad();
    auto std_proxy = std_example();

    (void)std_proxy;
    (void)bad_proxy;
    (void)good_proxy;

    assert(static_cast<char>(good_proxy) == 'a');
    assert(static_cast<char>(std_proxy) == 'a');
    //assert(static_cast<char>(bad_proxy) == 'a');
}

void my_tests() {
    test_lazy();

    test_comparison();
    test_icomparison();
    test_concat();
    test_bad_proxy();
}

int main() {
    test_internal_typedefs();
    test_empty_string();
    test_constructors();
    test_assignment_operator();
    test_plus_operator();
    test_index_operator();
    test_relational_operators();
    test_c_str();
    test_swap();
    test_lazy_wstring();

    my_tests();

    std::cout << "ok!" << std::endl;
    return 0;
}
