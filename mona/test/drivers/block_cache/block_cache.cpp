#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <vector>

using namespace std;

static const int MAX_CACHE_SIZE = 1 * 1024 * 1024;

static void testEmptyCacheHasNoCacheOf0thSector()
{
    BlockCache bc(MAX_CACHE_SIZE);
    CacheList cacheList;
    const int startSector = 0;
    const int numSectors = 1;
    EXPECT_EQ(false, bc.get(startSector, numSectors, cacheList));
    EXPECT_EQ(0, cacheList.size());
}

int main(int argc, char *argv[])
{
    testEmptyCacheHasNoCacheOf0thSector();
    TEST_RESULTS();
    return 0;
}

