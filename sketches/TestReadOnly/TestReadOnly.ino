// Available implementations
#define TEST_IMPLEMENTATION_PUBLIC_VARIABLE 1
#define TEST_IMPLEMENTATION_INLINE_ACCESSOR 2
#define TEST_IMPLEMENTATION_CONST_REFERENCE 3

// Available variable types
#define TEST_TYPE_INT 1    // int
#define TEST_TYPE_STRUCT 2 // struct { int, int, int }

// Change this and compare the compiled program size
#define TEST_IMPLEMENTATION TEST_IMPLEMENTATION_PUBLIC_VARIABLE
#define TEST_TYPE TEST_TYPE_INT

// 0: 1x test call
// 1: 4x test call
// 2: 8x test call
#define TEST_MULTI_CALL 0

class Test
{
  public:

    #if(TEST_TYPE == TEST_TYPE_STRUCT)
      typedef struct structType
      {
        int valueA;
        int valueB;
        int valueC;
      };
      #define TYPE Test::structType
    #elif(TEST_TYPE == TEST_TYPE_INT)
      #define TYPE int
    #endif

    static TYPE value;

    #if(TEST_IMPLEMENTATION == TEST_IMPLEMENTATION_INLINE_ACCESSOR)
      static inline TYPE getValue() { return value; }
    #endif

    #if(TEST_IMPLEMENTATION == TEST_IMPLEMENTATION_CONST_REFERENCE)
      static const TYPE& valueRef;
    #endif
};

TYPE Test::value;

#if(TEST_IMPLEMENTATION == TEST_IMPLEMENTATION_CONST_REFERENCE)
  const TYPE &Test::valueRef = value;
#endif

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  // Test the public variable implementation
  #if(TEST_IMPLEMENTATION == TEST_IMPLEMENTATION_PUBLIC_VARIABLE)
    #if(TEST_TYPE == TEST_TYPE_STRUCT)
      Serial.println(Test::value.valueA);
      #if(TEST_MULTI_CALL > 0)
        Serial.println(Test::value.valueA);
        Serial.println(Test::value.valueA);
        Serial.println(Test::value.valueA);
      #endif
      #if(TEST_MULTI_CALL > 1)
        Serial.println(Test::value.valueA);
        Serial.println(Test::value.valueA);
        Serial.println(Test::value.valueA);
        Serial.println(Test::value.valueA);
      #endif
    #elif(TEST_TYPE == TEST_TYPE_INT)
      Serial.println(Test::value);
      #if(TEST_MULTI_CALL > 0)
        Serial.println(Test::value);
        Serial.println(Test::value);
        Serial.println(Test::value);
      #endif
      #if(TEST_MULTI_CALL > 1)
        Serial.println(Test::value);
        Serial.println(Test::value);
        Serial.println(Test::value);
        Serial.println(Test::value);
      #endif
    #endif
  #endif

  // Test the inline accessor implementation
  #if(TEST_IMPLEMENTATION == TEST_IMPLEMENTATION_INLINE_ACCESSOR)
    #if(TEST_TYPE == TEST_TYPE_STRUCT)
      Serial.println(Test::getValue().valueA);
      #if(TEST_MULTI_CALL > 0)
        Serial.println(Test::getValue().valueA);
        Serial.println(Test::getValue().valueA);
        Serial.println(Test::getValue().valueA);
      #endif
      #if(TEST_MULTI_CALL > 1)
        Serial.println(Test::getValue().valueA);
        Serial.println(Test::getValue().valueA);
        Serial.println(Test::getValue().valueA);
        Serial.println(Test::getValue().valueA);
      #endif
    #elif(TEST_TYPE == TEST_TYPE_INT)
      Serial.println(Test::getValue());
      #if(TEST_MULTI_CALL > 0)
        Serial.println(Test::getValue());
        Serial.println(Test::getValue());
        Serial.println(Test::getValue());
      #endif
      #if(TEST_MULTI_CALL > 1)
        Serial.println(Test::getValue());
        Serial.println(Test::getValue());
        Serial.println(Test::getValue());
        Serial.println(Test::getValue());
      #endif
    #endif
  #endif

  // Test the const reference implementation
  #if(TEST_IMPLEMENTATION == TEST_IMPLEMENTATION_CONST_REFERENCE)
    #if(TEST_TYPE == TEST_TYPE_STRUCT)
      Serial.println(Test::valueRef.valueA);
      #if(TEST_MULTI_CALL > 0)
        Serial.println(Test::valueRef.valueA);
        Serial.println(Test::valueRef.valueA);
        Serial.println(Test::valueRef.valueA);
      #endif
      #if(TEST_MULTI_CALL > 1)
        Serial.println(Test::valueRef.valueA);
        Serial.println(Test::valueRef.valueA);
        Serial.println(Test::valueRef.valueA);
        Serial.println(Test::valueRef.valueA);
      #endif
    #elif(TEST_TYPE == TEST_TYPE_INT)
      Serial.println(Test::valueRef);
      #if(TEST_MULTI_CALL > 0)
        Serial.println(Test::valueRef);
        Serial.println(Test::valueRef);
        Serial.println(Test::valueRef);
      #endif
      #if(TEST_MULTI_CALL > 1)
        Serial.println(Test::valueRef);
        Serial.println(Test::valueRef);
        Serial.println(Test::valueRef);
        Serial.println(Test::valueRef);
      #endif
    #endif
  #endif
}
