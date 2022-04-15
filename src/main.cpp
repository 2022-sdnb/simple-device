#include <Protocon/ClientToken.h>
#include <Protocon/Protocon.h>
#include <Protocon/Request.h>
#include <Protocon/Response.h>
#include <Protocon/SignInResponse.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <thread>

int main() {
    std::srand(time(nullptr));
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

    // 确保登录成功
    while (!trigger) {
        gw.poll();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 负控静态数据
    gw.send(tk, Protocon::Request{
                    static_cast<uint64_t>(time(nullptr)),
                    0x1001,
                    nlohmann::json{
                        {"kg_type", "kg"},
                        {"phase_cnt", 1},
                    }
                        .dump(),
                },
            [](const Protocon::Response& r) {});

    // 充电桩静态数据
    gw.send(tk, Protocon::Request{
                    static_cast<uint64_t>(time(nullptr)),
                    0x1002,
                    "{}",
                },
            [](const Protocon::Response& r) {});

    // 空调静态数据
    gw.send(tk, Protocon::Request{
                    static_cast<uint64_t>(time(nullptr)),
                    0x1003,
                    "{}",
                },
            [](const Protocon::Response& r) {});

    // 网关静态数据
    gw.send(tk, Protocon::Request{
                    static_cast<uint64_t>(time(nullptr)),
                    0x1004,
                    "{}",
                },
            [](const Protocon::Response& r) {});

    uint64_t prev_time = time(nullptr);
    uint64_t charger_timer = 0;

    while (gw.isOpen()) {
        gw.poll();

        uint64_t curr = time(nullptr);
        charger_timer += curr - prev_time;
        prev_time = curr;

        // 负控运行数据
        gw.send(tk, Protocon::Request{
                        static_cast<uint64_t>(time(nullptr)),
                        0x2001,
                        nlohmann::json{
                            {"p_rt", std::rand()},
                            {"q_rt", std::rand()},
                        }
                            .dump(),
                    },
                [](const Protocon::Response& r) {});

        // 充电桩运行数据
        gw.send(tk, Protocon::Request{
                        static_cast<uint64_t>(time(nullptr)),
                        0x2002,
                        "{}",
                    },
                [](const Protocon::Response& r) {});

        // 中央空调运行数据
        gw.send(tk, Protocon::Request{
                        static_cast<uint64_t>(time(nullptr)),
                        0x2003,
                        "{}",
                    },
                [](const Protocon::Response& r) {});

        // 每个十秒上传一次
        if (charger_timer >= 8) {
            charger_timer -= 8;
            // 充电桩交易数据
            gw.send(tk, Protocon::Request{
                            static_cast<uint64_t>(time(nullptr)),
                            0x2004,
                            "{}",
                        },
                    [](const Protocon::Response& r) {});
        }

        spdlog::info("上报数据");

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    return 0;
}
