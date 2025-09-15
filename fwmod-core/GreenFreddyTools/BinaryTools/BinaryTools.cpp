#include "BinaryWriter.h"
#include "BinaryReader.h"
#include "Binary.h"
#include "BinaryIO.h"
#include <array>

struct TestPod
{
    float x;
    float y;
    float z;
    uint32_t cash;
    int32_t score;
};

int main()
{
    printf("**** Test 1 - Write + Read a few values ****\n");
    //Test writing a few values and reading them back
    {
        {
            printf("Writing some values to file... ");
            BinaryWriter writer("./TestBin1.bin");
            writer.WriteFloat(1232.3f);
            writer.WriteFloat(300.7f);
            writer.WriteFloat(1680.0f);
            writer.WriteUint32(8000);
            writer.WriteInt32(-2003443);
            printf("Done!\n");
        }
        {
            printf("Reading those values back...\n");
            BinaryReader reader("./TestBin1.bin");
            printf("Float: %f\n", reader.ReadFloat());
            printf("Float: %f\n", reader.ReadFloat());
            printf("Float: %f\n", reader.ReadFloat());
            printf("Uint32: %d\n", reader.ReadUint32());
            printf("Int32: %d\n", reader.ReadInt32());
            printf("Done!\n");
        }

    }

    printf("\n\n**** Test 2 - Write + Read a POD struct directly to/from memory ****\n");
    //Test writing a struct from memory, reading it back and casting the data onto it
    {
        //Write data
        {
            TestPod writeData = {};
            writeData.x = 1234.44f;
            writeData.y = 1734.44f;
            writeData.z = 22334.44f;
            writeData.cash = 1003;
            writeData.score = -64230;
            printf("sizeof(TestPod) = %zd\n", sizeof(TestPod));

            printf("Writing POD struct from memory... ");
            BinaryWriter writer("./TestBin2.bin");
            writer.WriteFromMemory(&writeData, sizeof(TestPod));
            printf("Done!\n\n");
        }
        //Read it back
        {
            TestPod readData = {};
            readData.x = 0.00000000f;
            readData.y = 0.00000000f;
            readData.z = 0.00000000f;
            readData.cash = 0;
            readData.score = 0;

            printf("Reading back data directly into POD struct location in memory... ");
            BinaryReader reader("./TestBin2.bin");
            reader.ReadToMemory(&readData, sizeof(TestPod));
            printf("Done!\n");
            printf("Printing values...\n");
            printf("Float: %f\n", readData.x);
            printf("Float: %f\n", readData.y);
            printf("Float: %f\n", readData.z);
            printf("Uint32: %d\n", readData.cash);
            printf("Int32: %d\n", readData.score);
        }
    }

    printf("\n\n**** Test 3 - Read a POD struct directly to/from memory from handmade binary file ****\n");
    //Test reading data from handmade binary file straight into POD struct memory location
    {
        {
            TestPod readData = {};
            readData.x = 0.00000000f;
            readData.y = 0.00000000f;
            readData.z = 0.00000000f;
            readData.cash = 0;
            readData.score = 0;

            printf("Reading data directly into POD struct location in memory... ");
            BinaryReader reader("./TestBin3.bin");
            reader.ReadToMemory(&readData, sizeof(TestPod));
            printf("Done!\n");
            printf("Printing values...\n");
            printf("Float: %f\n", readData.x);
            printf("Float: %f\n", readData.y);
            printf("Float: %f\n", readData.z);
            printf("Uint32: %d\n", readData.cash);
            printf("Int32: %d\n", readData.score);
        }
    }

    printf("\n\n**** Test 4 - Read a POD struct from a file to memory and read data from that memory area with BinaryReader ****\n");
    //Test reading data from handmade binary file straight into POD struct memory location
    {
        {
            TestPod readData = {};
            readData.x = 0.00000000f;
            readData.y = 0.00000000f;
            readData.z = 0.00000000f;
            readData.cash = 0;
            readData.score = 0;

            printf("Reading data directly into memory... ");
            printf("Done!\n");
            auto span = ReadAllBytes("./TestBin3.bin");
            printf("Reading values of memory buffer with BinaryReader... ");
            printf("Done!\n");

            BinaryReader reader(span.Data(), (uint32_t)span.Size());
            printf("Printing values...\n");
            printf("Float: %f\n", reader.ReadFloat());
            printf("Float: %f\n", reader.ReadFloat());
            printf("Float: %f\n", reader.ReadFloat());
            printf("Uint32: %d\n", reader.ReadUint32());
            printf("Int32: %d\n", reader.ReadInt32());
            delete span.Data();

            struct test
            {
                int a;
                int b;
            };

            std::array<test, 3> testArray;
            testArray[0] = test{ 2, 3 };
            testArray[1] = test{ 4, 5 };
            testArray[2] = test{ 6, 7 };
            //Intentionally specifying size of 2 here to see if end() actually points to the end of last element of the span
            Span<test> testSpan(testArray.data(), 2);

            test* begin = testSpan.begin();
            test* end = testSpan.end();
            auto& front = testSpan.front();
            auto& back = testSpan.back();

            auto& zero = testSpan[0];
            auto& one = testSpan[1];
            auto& two = testSpan[2];

            printf("Testing use of range based for loops with Span<T>...\n");
            for (auto& val : testSpan)
                printf("value: {a: %d, b: %d}\n", val.a, val.b);

            auto a = 2;
        }
    }

    printf("\n\n**** Test 5 - BinaryIO: Write and Read from memory buffer (like Python BytesIO) ****\n");
    //Test BinaryIO functionality - write to memory buffer and read back
    {
        printf("Creating empty BinaryIO... ");
        BinaryIO io;
        printf("Done!\n");

        printf("Writing data to BinaryIO... ");
        io.WriteFloat(3.14159f); // 4
        io.WriteUint32(0x12345678); // 4
        io.WriteNullTerminatedString("Hello BinaryIO!"); // 16
        io.WriteInt32(-42); // 4
        printf("Done!\n");

        printf("BinaryIO size: %zu bytes\n", io.Length()); // 28 total
        printf("BinaryIO position: %zu\n", io.Position()); // cur pos

        printf("Seeking to beginning... ");
        io.SeekBeg(0);
        printf("Done!\n");

        printf("Reading data back from BinaryIO...\n");
        printf("Float: %f\n", io.ReadFloat());
        printf("Uint32: 0x%08X\n", io.ReadUint32());
        printf("String: %s\n", io.ReadNullTerminatedString().c_str());
        printf("Int32: %d\n", io.ReadInt32());
        printf("Done!\n");
    }

    printf("\n\n**** Test 6 - BinaryIO: Initialize with existing data and modify ****\n");
    //Test BinaryIO with initial data
    {
        printf("Creating BinaryIO with initial data... ");
        std::vector<uint8_t> initialData = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
        BinaryIO io(initialData);
        printf("Done!\n");

        printf("Reading initial data: %s\n", io.ReadFixedLengthString(5).c_str());
        
        printf("Appending new data... ");
        io.WriteNullTerminatedString(" World!");
        printf("Done!\n");

        printf("Seeking to beginning and reading all data... ");
        io.SeekBeg(0);
        printf("Full string: %s\n", io.ReadNullTerminatedString().c_str());
    }

    printf("\n\n**** Test 7 - BinaryIO: Raw data access and manipulation ****\n");
    //Test raw data access
    {
        printf("Creating BinaryIO and writing data... ");
        BinaryIO io;
        io.WriteUint32(0xDEADBEEF);
        io.WriteUint32(0xCAFEBABE);
        printf("Done!\n");

        printf("Accessing raw data... ");
        uint8_t* rawData = io.Data();
        printf("First 4 bytes: 0x%02X%02X%02X%02X\n", 
               rawData[0], rawData[1], rawData[2], rawData[3]);
        printf("Next 4 bytes: 0x%02X%02X%02X%02X\n", 
               rawData[4], rawData[5], rawData[6], rawData[7]);

        printf("Using span access... ");
        auto span = io.GetSpan();
        printf("Span size: %zu\n", span.size());
        printf("First byte via span: 0x%02X\n", span[0]);
    }

    printf("\n\n**** Test 8 - BinaryIO: Seek and partial read operations ****\n");
    //Test seeking and partial reads
    {
        printf("Creating BinaryIO with test data... ");
        BinaryIO io;
        for (int i = 0; i < 10; i++) {
            io.WriteUint32(i * 1000);
        }
        printf("Done!\n");

        printf("Seeking to position 8 (2nd uint32)... ");
        io.SeekBeg(8);
        printf("Position: %zu\n", io.Position());

        printf("Reading uint32 at position 8: %u\n", io.ReadUint32());
        
        printf("Seeking back 4 bytes... ");
        io.SeekReverse(4);
        printf("Position: %zu\n", io.Position());
        printf("Reading same uint32 again: %u\n", io.ReadUint32());
    }

}
