#include <httplib.h>
#include <cstring>
#include <iostream>
#include <android/log.h>

struct Result
{
    int status;
    char *body;
};

extern "C"
{
    Result *get(char *path)
    {
        httplib::Client cli("https://brawlmods.com");
        cli.set_follow_location(true);
        cli.enable_server_certificate_verification(false);

        auto cppRes = cli.Get(path);
        Result *res = new Result();

        if (!cppRes)
        {
            httplib::Error err = cppRes.error();
            __android_log_print(ANDROID_LOG_ERROR, "inet", "HTTP Error Code: %d", static_cast<int>(err));
            res->status = -1;
            res->body = nullptr;
            return res;
        }

        res->status = cppRes->status;
        res->body = new char[cppRes->body.size() + 1];
        memcpy(res->body, cppRes->body.data(), cppRes->body.size());
        res->body[cppRes->body.size()] = '\0';
        return res;
    }

    Result *getScript()
    {
        char path[] = "/shadowbrawl/script.js";
        return get(path);
    }

    Result *getVersion()
    {
        char path[] = "/shadowbrawl/version.txt";
        return get(path);
    }
}