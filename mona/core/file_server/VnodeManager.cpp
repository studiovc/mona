#include "VnodeManager.h"
#include "sys/error.h"
#include <monapi.h>
#include <monapi/Buffer.h>

using namespace std;
using namespace io;
using namespace MonAPI;

extern string upperCase(const string& s);

VnodeManager::VnodeManager()
{
    cacher_ = new VnodeCacher;
}

VnodeManager::~VnodeManager()
{
    delete cacher_;
}

// remove first '/'. fix me
std::string VnodeManager::removeHead(const std::string& path)
{
  if (path.empty()) {
      return path;
    } else {
       return path.substr(1, path.size() - 1);
    }
}

int VnodeManager::delete_file(const std::string& name, bool isDirectory)
{
    // now fullpath only. fix me
    if (name.compare(0, 1, "/") != 0) return M_BAD_ARG;

    string filename = removeHead(name);
    Vnode* file;
    int ret = lookup(root_, filename, &file, isDirectory ? Vnode::DIRECTORY : Vnode::REGULAR);
    if (ret != M_OK) {
        return ret;
    }
    if (isDirectory) {
        ret = file->fs->delete_directory(file);
    } else {
        ret = file->fs->delete_file(file);
    }
    if (ret == M_OK) {
        Vnode* targetDirectory = NULL;
        uint32_t foundIndex = name.find_last_of('/');
        string filename = name;
        if (foundIndex == name.npos) {
            targetDirectory = root_;
        } else {
            string dirPath = name.substr(1, foundIndex - 1);
            int ret = lookup(root_, dirPath, &targetDirectory, Vnode::DIRECTORY);
            if (ret != M_OK) {
                return ret;
            }
            filename = name.substr(foundIndex + 1, name.size() - foundIndex);
        }
        cacher_->remove(targetDirectory, filename);
    }
    return ret;
}

int VnodeManager::delete_file(const std::string& name)
{
    const bool IS_DIRECTORY = false;
    return delete_file(name, IS_DIRECTORY);
}

int VnodeManager::delete_directory(const std::string& name)
{
    const bool IS_DIRECTORY = true;
    return delete_file(name, IS_DIRECTORY);
}

int VnodeManager::lookupOne(Vnode* directory, const string& file, Vnode** found, int type)
{
    Vnode* v = cacher_->lookup(directory, file);
    if (v != NULL && (v->type == type || type == Vnode::ANY)) {
        *found = v;
        return M_OK;
    }
    intptr_t ret =  directory->fs->lookup(directory, file, found, type);
    if (ret == M_OK) {
        cacher_->add(directory, file, *found);
    }
    return ret;
}

int VnodeManager::lookup(Vnode* directory, const string& file, Vnode** found, int type)
{
    if (directory->type != Vnode::DIRECTORY) {
        return M_BAD_ARG;
    }

    vector<string> directories;
    split(file, '/', directories);
    int ret = M_FILE_NOT_FOUND;
    Vnode* root = directory;
    for (uint32_t i = 0; i < directories.size(); i++) {
        string name = directories[i];
        int vtype = (i == directories.size() - 1) ? type : Vnode::DIRECTORY;
        ret = lookupOne(root, name, found, vtype);
        if (ret != M_OK) {
            return ret;
        }
        // link
        if ((*found)->mountedVnode != NULL) {
            *found = (*found)->mountedVnode;
        }
        if (i != directories.size() -1) {
            root = *found;
        }
    }
    return ret;
}

int VnodeManager::read_directory(const std::string&name, SharedMemory** mem)
{
    // now fullpath only. fix me
    if (name.compare(0, 1, "/") != 0) return M_FILE_NOT_FOUND;

    // remove first '/'. fix me
    string filename;
    if (name.size() == 1)
    {
        filename = name;
    }
    else
    {
        filename = removeHead(name);
    }

    Vnode* dir;
    if (filename == "/") {
        dir = root_;
    }
    else
    {
        int ret = lookup(root_, filename, &dir, Vnode::DIRECTORY);
        if (ret != M_OK)
        {
            return ret;
        }
    }
    int ret = dir->fs->read_directory(dir, mem);

    if (ret != M_OK) {
        return ret;
    }

    // check mounted directories on caches.
    typedef std::vector<std::string> strings;
    strings caches;
    cacher_->enumCaches(dir, caches);
    if (!caches.empty()) {
        std::map<std::string, bool> seen;
        for (size_t i = sizeof(int); i < (*mem)->size(); i += sizeof(monapi_directoryinfo)) {
            monapi_directoryinfo* p = (monapi_directoryinfo*)(&((*mem)->data()[i]));
            seen.insert(std::pair<std::string, bool>(upperCase(p->name), true));
        }

        strings diff;
        for (strings::const_iterator it = caches.begin(); it != caches.end(); ++it) {
            if (seen.find(*it) == seen.end()) {
                diff.push_back(*it);
            }
        }

        if (!diff.empty()) {
            int size = (*mem)->size() + diff.size() * sizeof(monapi_directoryinfo);
            SharedMemory* ret = new SharedMemory(size);
            if (ret->map() != M_OK) {
                return M_NO_MEMORY;
            }
            MonAPI::Buffer dest(ret->data(), ret->size());
            MonAPI::Buffer src((*mem)->data(), (*mem)->size());
            bool isOK = MonAPI::Buffer::copy(dest, src, (*mem)->size());
            MONA_ASSERT(isOK);
            int entriesNum = *((int*)(*mem)->data()) + diff.size();
            MonAPI::Buffer src2(&entriesNum, sizeof(int));
            isOK = MonAPI::Buffer::copy(dest, src2, sizeof(int));
            MONA_ASSERT(isOK);
            for (size_t i = 0; i < diff.size(); i++) {
                monapi_directoryinfo di;
                di.size = 0;
                strcpy(di.name, diff[i].c_str());
                di.attr = ATTRIBUTE_DIRECTORY;
                MonAPI::Buffer dirBuf(&di, sizeof(monapi_directoryinfo));
                bool isOK = MonAPI::Buffer::copy(dest, (*mem)->size() + i * sizeof(monapi_directoryinfo), dirBuf, 0, sizeof(monapi_directoryinfo));
                MONA_ASSERT(isOK);
            }
            *mem = ret;
        }
    }
    return M_OK;
}

int VnodeManager::create_file(const std::string& name, bool isDirectory)
{
    Vnode* targetDirectory = NULL;
    uint32_t foundIndex = name.find_last_of('/');
    string filename = name;
    if (foundIndex == name.npos) {
        targetDirectory = root_;
    } else {
        string dirPath = name.substr(1, foundIndex - 1);
        int ret = lookup(root_, dirPath, &targetDirectory, Vnode::DIRECTORY);
        if (ret != M_OK) {
            return ret;
        }
        filename = name.substr(foundIndex + 1, name.size() - foundIndex);
    }
    if (isDirectory) {
        return targetDirectory->fs->create_directory(targetDirectory, filename);
    } else {
        return targetDirectory->fs->create(targetDirectory, filename);
    }
}

int VnodeManager::create(const std::string& name)
{
    const bool IS_DIRECTORY = false;
    return create_file(name, IS_DIRECTORY);
}

int VnodeManager::create_directory(const std::string& name)
{
    const bool IS_DIRECTORY = true;
    return create_file(name, IS_DIRECTORY);
}

int VnodeManager::open(const std::string& name, intptr_t mode, uint32_t tid, uint32_t* fileID)
{
    static bool tooLongWarnShown = false;
    // Joliet spec restriction.
    if (name.size() > 64 && !tooLongWarnShown) {
        monapi_warn("too long filename <%s>\n", name.c_str());
        tooLongWarnShown = true;
    }
    // now fullpath only. fix me
    if (name.compare(0, 1, "/") != 0) {
        return M_UNKNOWN;
    }
    // remove first '/'. fix me
    string filename = removeHead(name);
    Vnode* file;
    if (lookup(root_, filename, &file) != M_OK) {
        if (mode & FILE_CREATE) {
            int ret = create(name);
            if (M_OK != ret) {
                return ret;
            }
            if (lookup(root_, filename, &file) != M_OK) {
                return M_FILE_NOT_FOUND;
            }
        } else {
            return M_FILE_NOT_FOUND;
        }
    } else {
        // found case
        if (mode & FILE_TRUNCATE) {
            int ret = file->fs->truncate(file);
            if (ret != M_OK) {
                return ret;
            }
        }
    }
    int ret = file->fs->open(file, mode);
    if (M_OK != ret) {
        return ret;
    }
    *fileID = this->fileID(file, tid);
    FileInfo* fileInfo = new FileInfo;
    fileInfo->vnode = file;
    fileInfo->context.tid = tid;
    fileInfoMap_.insert(pair< uint32_t, FileInfo* >(*fileID, fileInfo));
    return M_OK;
}

Vnode* VnodeManager::alloc()
{
    Vnode* v = new Vnode;
    v->mountedVnode = NULL;
    return v;
}

int VnodeManager::read(uint32_t fileID, uint32_t size, SharedMemory** mem)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end()) {
        return M_FILE_NOT_FOUND;
    }
    io::FileInfo* fileInfo = (*it).second;
    io::Context* context = &(fileInfo->context);
    context->size = size;
    int result = fileInfo->vnode->fs->read(fileInfo->vnode, context);
    *mem = context->memory;
    context->memory = NULL;
    return result;
}

int VnodeManager::write(uint32_t fileID, uint32_t size, SharedMemory* mem)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end()) {
        return M_BAD_FILE_ID;
    }
    io::FileInfo* fileInfo = (*it).second;
    io::Context* context = &(fileInfo->context);
    context->size = size;
    context->memory = mem;
    return fileInfo->vnode->fs->write(fileInfo->vnode, context);
}

int VnodeManager::close(uint32_t fileID)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end()) {
        return M_FILE_NOT_FOUND;
    }

    FileInfo* fileInfo = (*it).second;
    fileInfo->context.memory = NULL;
    Vnode* file = fileInfo->vnode;
    int ret = file->fs->close(file);
    if (M_OK != ret) {
        return ret;
    }
    fileInfoMap_.erase(fileID);
    delete fileInfo;
    return M_OK;
}

int VnodeManager::stat(uint32_t fileID, Stat* st)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end())
    {
        return M_FILE_NOT_FOUND;
    }

    FileInfo* fileInfo = (*it).second;
    Vnode* file = fileInfo->vnode;
    return file->fs->stat(file, st);
}

bool VnodeManager::exists(const std::string& path)
{
    string filename = removeHead(path);
    Vnode* file;
    return lookup(root_, path, &file, Vnode::ANY) == M_OK;
}

int VnodeManager::stat(const string& path, Stat* st)
{
    // remove first '/'. fix me
    string filename = removeHead(path);
    Vnode* file;
    intptr_t ret = lookup(root_, filename, &file);
    if (ret != M_OK) {
        intptr_t ret2 = lookup(root_, filename, &file, Vnode::DIRECTORY);
        if (ret2 != M_OK) {
            return ret;
        }
        return file->fs->stat(file, st);
    }
    return file->fs->stat(file, st);
}


int VnodeManager::seek(uint32_t fileID, int32_t offset, uint32_t origin)
{
    FileInfoMap::iterator it = fileInfoMap_.find(fileID);
    if (it == fileInfoMap_.end()) {
        return M_FILE_NOT_FOUND;
    }

    FileInfo* fileInfo = (*it).second;
    Vnode* file = fileInfo->vnode;
    int32_t newOffset = 0;
    switch (origin)
    {
    case SEEK_SET:
    {
        newOffset = offset;
        break;
    }
    case SEEK_CUR:
    {
        newOffset = fileInfo->context.offset + offset;
        break;
    }
    case SEEK_END:
    {
        Stat st;
        intptr_t ret = file->fs->stat(file, &st);
        if (M_OK != ret) {
            return ret;
        }
        newOffset = st.size-offset;
        break;
    }
    default:
        return M_UNKNOWN;
    }
    if (newOffset < 0) {
        return M_BAD_OFFSET;
    } else {
        fileInfo->context.offset = newOffset;
        fileInfo->context.origin = origin;
        return newOffset;
    }
}

int VnodeManager::mount(Vnode* a, const std::string& path, Vnode* b)
{
    cacher_->add(a, path, b);
    return M_OK;
}

void VnodeManager::split(string str, char ch, vector<string>& v)
{
    uint32_t index = 0;
    uint32_t next = 0;
    while ((index = str.find_first_of(ch, next)) != string::npos)
    {
        string t = string(str.begin() + next, str.begin() + index);
        if (!t.empty()) {
            v.push_back(t);
        }
        next = index + 1;
    }
    v.push_back(string(str.begin() + next, str.end()));
}
