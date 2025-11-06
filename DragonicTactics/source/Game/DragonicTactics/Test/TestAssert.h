bool ASSERT_TRUE(bool condition);

bool ASSERT_FALSE(bool condition);

template <typename T>
bool ASSERT_EQ(T actual, T expected);

template <typename T>
bool ASSERT_NE(T actual, T expected);

template <typename T>
bool ASSERT_GE(T actual, T minimum);

template <typename T>
bool ASSERT_LE(T actual, T maximum);

#include "TestAssert.ini"