CPPFLAGS += -isystem $(GTEST_DIR)/include -I ArduinoMock -I Quaternion -I QuadMotor -I ESC -DUNIT_TEST

CXXFLAGS += -g -Wall -Wextra -pthread

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

TESTS = QuaternionTest QuadMotorTest

all : $(TESTS)

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

Quaternion.o : Quaternion/Quaternion.cpp Quaternion/Quaternion.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c Quaternion/Quaternion.cpp

QuaternionTest.o : Quaternion/QuaternionTest.cpp Quaternion/Quaternion.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c Quaternion/QuaternionTest.cpp

QuaternionTest : Quaternion.o QuaternionTest.o gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

QuadMotor.o : QuadMotor/QuadMotor.cpp QuadMotor/QuadMotor.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c QuadMotor/QuadMotor.cpp

ESCMock.o : ESC/ESCMock.cpp ESC/ESC.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ESC/ESCMock.cpp

QuadMotorTest.o : QuadMotor/QuadMotorTest.cpp QuadMotor/QuadMotor.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c QuadMotor/QuadMotorTest.cpp

QuadMotorTest : QuadMotor.o QuadMotorTest.o ESCMock.o gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@
