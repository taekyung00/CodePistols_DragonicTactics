/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

bool ASSERT_TRUE(bool condition)
{
  if (!(condition))
  {
	std::cout << " ASSERT_TRUE failed " << std::endl;
	return false;
  }
  else
  {
	std::cout << " ASSERT_TRUE successed! " << std::endl;
	return true;
  }
}

bool ASSERT_FALSE(bool condition)
{
  if ((condition))
  {
	std::cout << " ASSERT_FALSE failed" << std::endl;
	return false;
  }
  else
  {
	std::cout << " ASSERT_FALSE successed! " << std::endl;
	return true;
  }
}
