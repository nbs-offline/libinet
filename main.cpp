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
    Result *res = new Result();
    res->status = -1;
    res->body = nullptr;

    httplib::Client cli("https://brawlmods.com");

    cli.set_follow_location(true);
    cli.enable_server_certificate_verification(false);

    cli.set_connection_timeout(1, 0);
    cli.set_read_timeout(1, 0);
    cli.set_write_timeout(1, 0);

    if (!cli.is_valid())
    {
      return res;
    }

    g_progress.store(0);

    std::string response_body;
    uint64_t received = 0;
    uint64_t total = 0;

    auto cppRes = cli.Get(
      path,
      [&](const httplib::Response &r) {
        auto it = r.headers.find("Content-Length");
        if (it != r.headers.end())
        {
          total = std::stoull(it->second);
        }
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

    if (!cppRes)
    {
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
