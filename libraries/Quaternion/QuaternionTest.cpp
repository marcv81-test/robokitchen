#include "gtest/gtest.h"
#include "Quaternion.h"

TEST(VectorTest, Accessors)
{
  Vector v = Vector(1.0, 2.0, 3.0);
  EXPECT_FLOAT_EQ(v.getX(), 1.0);
  EXPECT_FLOAT_EQ(v.getY(), 2.0);
  EXPECT_FLOAT_EQ(v.getZ(), 3.0);
}

TEST(VectorTest, Norm)
{
  Vector v = Vector(0.0, 3.0, 4.0);
  EXPECT_FLOAT_EQ(v.norm(), 5.0);
}

TEST(VectorTest, Normalise)
{
  for(int i=0; i<10 ; i++)
  {
    Vector v = Vector(1.0, 2.0, 3.0);
    v.normalise();
    ASSERT_FLOAT_EQ(v.norm(), 1.0);
  }
}
