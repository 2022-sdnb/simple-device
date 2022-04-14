#include <Protocon/ClientToken.h>
#include <Protocon/Protocon.h>
#include <Protocon/Request.h>
#include <Protocon/Response.h>
#include <Protocon/SignInResponse.h>
#include <spdlog/spdlog.h>

#include <cstdio>
#include <ctime>
#include <thread>

int main() {
    bool trigger = false;

    auto gw = Protocon::GatewayBuilder(2)
                  .withSignInResponseHandler([&trigger](const Protocon::SignInResponse& r) {
                      if (!r.status)
                          trigger = true;
                  })
                  .withRequestHandler(0x3001, [](Protocon::ClientToken tk, const Protocon::Request& r) {
                      spdlog::info("接收到负控设备运行控制命令，data: {}", r.data);
                      return Protocon::Response{.time = static_cast<uint64_t>(std::time(nullptr)), .status = 0, .data = "{}"};
                  })
                  .withRequestHandler(0x3002, [](Protocon::ClientToken tk, const Protocon::Request& r) {
                      spdlog::info("接收到充电桩设备运行控制命令，data: {}", r.data);
                      return Protocon::Response{.time = static_cast<uint64_t>(std::time(nullptr)), .status = 0, .data = "{}"};
                  })
                  .withRequestHandler(0x3003, [](Protocon::ClientToken tk, const Protocon::Request& r) {
                      spdlog::info("接收到空调设备运行控制命令，data: {}", r.data);
                      return Protocon::Response{.time = static_cast<uint64_t>(std::time(nullptr)), .status = 0, .data = "{}"};
                  })
                  .build();

    auto tk = gw.createClientToken();

    if (!gw.run("127.0.0.1", 8082)) return 1;

    spdlog::info("连接服务器成功");

    while (gw.isOpen()) {
        gw.poll();

        if (trigger) {
            gw.send(tk, Protocon::Request{
                            static_cast<uint64_t>(time(nullptr)),
                            0x0004,
                            "{\"msg\": \"Hello world!\"}",
                        },
                    [](const Protocon::Response& r) {});

            spdlog::info("上报数据");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    return 0;
}
