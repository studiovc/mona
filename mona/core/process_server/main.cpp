#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <monapi/Array.h>
#include <monalibc.h>
#include "ProcessServer.h"
#include "ProcessManager.h"
#include "ProcessServer.h"
#include "ProcessManager.h"

#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/Assert.h>
#include <vector>
#include "ProcessInfo.h"
using namespace std;
using namespace MonAPI;


class ProcessServer {
private:
    SharedMemory* commonParams_;
    vector<ProcessInfo> infos_;

public:
    ProcessServer()
    {
        initCommonParameters();
        if (monapi_notify_server_start("INIT") != M_OK) {
            monapi_fatal("monapi_notify_server_start to INIT failed");
        }

        if (monapi_name_add("/servers/process") != M_OK) {
            monapi_fatal("monapi_name_add failed");
        }
    }

private:
    void initCommonParameters()
    {
        commonParams_ = new SharedMemory(sizeof(CommonParameters));
        if (commonParams_->map(false) != M_OK) {
            monapi_fatal("initCommonParameters failed");
        }
    }

public:

ProcessInfo getProcessInfo(uint32_t tid)
{
    int size = infos_.size();
    for (int i = 0; i < size; i++)
    {
        if (infos_[i].tid == tid) {
            return infos_[i];
        }
    }
    // todo tid not required
    return ProcessInfo(tid);
}

intptr_t addProcessInfo(uint32_t parentTid, uint32_t subThreadTid)
{
    ProcessInfo parent = getProcessInfo(parentTid);
    if (parent.tid == THREAD_UNKNOWN) {
        return M_NOT_FOUND;
    }
    // todo copy constructor
    infos_.push_back(ProcessInfo(subThreadTid, parent.tid, parent.name, parent.path, parent.stdin_id, parent.stdin_id));
    return M_OK;
}


void addProcessInfo(uint32_t tid, uint32_t parent, const CString& name, const CString& path, uint32_t stdin_id, uint32_t stdout_id)
{
    ProcessInfo pi(tid, parent, name, path, stdin_id, stdout_id);
    infos_.push_back(pi);
}

 void removeProcessInfo(uint32_t tid, int status /* = -1 */)
{
    int size = infos_.size();
    for (int i = 0; i < size; i++)
    {
        if (infos_[i].tid != tid) continue;

        infos_.erase(&infos_[i]);
        return;
    }
}

 bool processHandler(MessageInfo* msg)
{
    switch (msg->header)
    {
        case MSG_PROCESS_GET_PROCESS_STDIO:
        {
            ProcessInfo pi = getProcessInfo(msg->arg1);
            Message::reply(msg, pi.stdin_id, pi.stdout_id);
            break;
        }
        case MSG_PROCESS_TERMINATED:
            removeProcessInfo(msg->arg1, msg->arg2);
            break;
        case MSG_PROCESS_GET_COMMON_PARAMS:
            Message::reply(msg, commonParams_->handle());
            break;
        case MSG_PROCESS_REGISTER_THREAD:
        {
            uint32_t parentTid = msg->from;
            uint32_t subThreadTid = msg->arg1;
            intptr_t ret = addProcessInfo(parentTid, subThreadTid);
            Message::reply(msg, ret);
            break;
        }
        default:
            return false;
    }
    return true;
}


 int ExecuteProcess(uint32_t parent, SharedMemory& shm, uint32_t entryPoint, const CString& path, const CString& name, CommandOption* option, bool prompt, uint32_t stdin_id, uint32_t stdout_id, uint32_t* tid, uint32_t observer)
{
    LoadProcessInfo info;
    info.image = shm.data();
    info.size  = shm.size();
    info.entrypoint = entryPoint;
    info.name = name;
    info.list = option;
    info.observer = observer;

    intptr_t ret = syscall_load_process_image(&info);
    if (ret != M_OK) {
        monapi_warn("failed to execute process %s", (const char*)path);
    }
    *tid = info.tid;
    _logprintf("[execute process name=%s tid=%x stdout_id =%x stdin_id = %x]", (const char*)name, *tid, stdout_id, stdin_id);
    addProcessInfo(*tid, parent, name, path, stdin_id, stdout_id);
    return ret;
}

 CString GetFileName(const CString& path)
{
    int p = path.lastIndexOf('/');
    if (p < 0) return path;

    p++;
    return path.substring(p, path.getLength() - p);
}

 int ExecuteFile(uint32_t parent, const CString& commandLine, bool prompt, uint32_t stdin_id, uint32_t stdout_id, uint32_t* tid, uint32_t observer)
{
    /* list initilize */
    CommandOption list;
    list.next = NULL;
    CommandOption* option = NULL;
    CString path;
    _A<CString> args = commandLine.split(' ');
    FOREACH (CString, arg, args)
    {
        if (arg == NULL) continue;

        if (path == NULL)
        {
            path = arg.toUpper();
            continue;
        }
        option = new CommandOption;
        strncpy(option->str, arg, sizeof(option->str));
        option->next = list.next;
        list.next = option;
    }
    END_FOREACH
    uint64_t s2 = MonAPI::Date::nowInMsec();
    SharedMemory* shm = NULL;
    uint32_t entryPoint = 0xa0000000;
    int result = 1;
    const char* svr_id = NULL;
    uint64_t s3, s4;
    if (path.endsWith(".ELF") || path.endsWith(".EL2") || path.endsWith(".EL5"))
    {
        svr_id = "/servers/elf";
    }
    else if (path.endsWith(".EXE") || path.endsWith(".EX2") || path.endsWith(".EX5"))
    {
        svr_id = "/servers/pe";
    }
    if (svr_id != NULL)
    {
        MessageInfo msg;
        uint32_t tid;
        if (monapi_name_whereis(svr_id, tid) != M_OK) {
            monapi_fatal("server not found");
        }
        if (Message::sendReceive(&msg, tid, MSG_PROCESS_CREATE_IMAGE, prompt ? MONAPI_TRUE : MONAPI_FALSE, 0, 0, path) != M_OK) {
            _printf("Error %s:%d\n", __FILE__, __LINE__);
            exit(-1);
        }
        if (msg.arg2 != 0) {
            result = 0;
            entryPoint = msg.arg3;
            shm = new SharedMemory(msg.arg2, atoi(msg.str));
            if (shm->map(true) != M_OK) {
                monapi_fatal("Error %s:%d\n", __FILE__, __LINE__);
                exit(-1);
            }
            // notify map is done.
            int status = Message::reply(&msg);
            if (status != M_OK) {
                monapi_warn("%s reply failed : %s\n", __func__, monapi_error_string(status));
            }

        }
        else
        {
            result = msg.arg3;
        }
    }
    else if (path.endsWith(".BN2"))
    {
        shm = monapi_call_file_decompress_bz2_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
    }
    else if (path.endsWith(".BN5"))
    {
        shm = monapi_call_file_decompress_st5_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
    }
    else
    {
        shm = monapi_file_read_all(path);
    }

    if (shm == NULL)
    {
        return result;
    }
    else
    {
        result = ExecuteProcess(parent, *shm, entryPoint, path, GetFileName(path), &list, prompt, stdin_id, stdout_id, tid, observer);
        delete shm;
    }
    CommandOption* next;
    for (option = list.next; option; option = next)
    {
        next = option->next;
        delete option;
    }
    return result;
}

 void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
            case MSG_PROCESS_EXECUTE_FILE:
            {
                uint32_t tid = 0;
                int result = ExecuteFile(msg.from, msg.str, msg.arg1 != 0, msg.arg2, msg.arg3, &tid, msg.from);
                Message::reply(&msg, result, tid);
                break;
            }
            default:
                if (processHandler(&msg)) break;
                break;
        }
    }
}
};

int main(int argc, char* argv[])
{
    ProcessServer server;
    server.MessageLoop();
    return 0;
}
