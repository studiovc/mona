#include "vnode.h"

using namespace std;

VnodeManager::VnodeManager()
{
}

VnodeManager::~VnodeManager()
{

}

int VnodeManager::lookup(Vnode* diretory, const string& file, Vnode** found)
{
    return diretory->fs->lookup(diretory, file, found);
}

int VnodeManager::open(const string& name, int mode, bool create, Vnode** entry)
{
    if (create)
    {
        // do something
    }

    // process current path
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    // now fullpath only
    if (name.compare(0, 1, "/") != 0) return MONA_ERROR_INVALID_ARGUMENTS;

    Vnode* found;
    if (lookup(root_, name, &found) != MONA_OK)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        return MONA_ERROR_ENTRY_NOT_FOUND;
    }

    return found->fs->open(found, mode);
}


Vnode* VnodeManager::alloc()
{
    Vnode* v = new Vnode;
    assert(v != NULL);
    return v;
}

int VnodeManager::read(Vnode* file, io::Context* context)
{
   return file->fs->read(file, context);
}