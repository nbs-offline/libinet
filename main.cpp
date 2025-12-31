#include <httplib.h>
#include <cstring>
#include <iostream>
#include <android/log.h>
#include <atomic>

struct Result
{
  int status;
  char *body;
};

static std::atomic<int> g_progress{0};

extern "C"
{
  void set_download_progress(int percent)
  {
    g_progress.store(percent);
  }

  int get_download_progress()
  {
    return g_progress.load();
  }

  Result *get(char *path)
  {
    httplib::Client cli("https://brawlmods.com");
    cli.set_follow_location(true);
    cli.enable_server_certificate_verification(false);

    g_progress.store(0);

    std::string response_body;
    uint64_t received = 0;
    uint64_t total = 0;

    auto cppRes = cli.Get(
      path,
      [&](const httplib::Response &res) {
        total = res.body.size();
        return true;
      },
      [&](const char *data, size_t data_length) {
        response_body.append(data, data_length);
        received += data_length;

        if (total > 0)
        {
          int percent = static_cast<int>((received * 100) / total);
          set_download_progress(percent);
        }

        return true;
      }
    );

    Result *res = new Result();

    if (!cppRes)
    {
      httplib::Error err = cppRes.error();
      __android_log_print(ANDROID_LOG_ERROR, "inet", "HTTP Error Code: %d", static_cast<int>(err));
      res->status = -1;
      res->body = nullptr;
      return res;
    }

    set_download_progress(100);

    res->status = cppRes->status;
    res->body = new char[response_body.size() + 1];
    memcpy(res->body, response_body.data(), response_body.size());
    res->body[response_body.size()] = '\0';
    return res;
  }
}
