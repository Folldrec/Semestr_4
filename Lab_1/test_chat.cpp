#include <gtest/gtest.h>
#include "chat_controller.hpp"
#include "message_formatter.hpp"

class MockNetwork : public INetwork {
public:
    std::string lastNick, lastMsg;
    unsigned short startedPort = 0;
    MessageCallback callback;

    void startServer(unsigned short port) override { startedPort = port; }
    void connectToPeer(const std::string&, const std::string&) override {}
    void sendMessage(const std::string& nick, const std::string& msg) override {
        lastNick = nick;
        lastMsg  = msg;
    }
    void setMessageCallback(MessageCallback cb) override {
        callback = std::move(cb);
    }
};

TEST(MessageFormatterTest, FormatBasic) {
    MessageFormatter fmt;
    EXPECT_EQ(fmt.format("Alice", "Hello"), "Alice: Hello");
}

TEST(MessageFormatterTest, FormatSystem) {
    MessageFormatter fmt;
    EXPECT_EQ(fmt.formatSystem("Connected"), "[System] Connected");
}

TEST(MessageFormatterTest, FormatEmptyMessage) {
    MessageFormatter fmt;
    EXPECT_EQ(fmt.format("Bob", ""), "Bob: ");
}

TEST(ChatControllerTest, SendMessageCallsNetwork) {
    auto mock = std::make_unique<MockNetwork>();
    MockNetwork* raw = mock.get();
    ChatController ctrl(std::move(mock));

    ctrl.sendMessage("Bob", "Hi!");
    EXPECT_EQ(raw->lastNick, "Bob");
    EXPECT_EQ(raw->lastMsg,  "Hi!");
}

TEST(ChatControllerTest, SendMessageReturnsFormatted) {
    auto mock = std::make_unique<MockNetwork>();
    ChatController ctrl(std::move(mock));

    std::string result = ctrl.sendMessage("Alice", "Hey");
    EXPECT_EQ(result, "Alice: Hey");
}

TEST(ChatControllerTest, StartServerDelegatesToNetwork) {
    auto mock = std::make_unique<MockNetwork>();
    MockNetwork* raw = mock.get();
    ChatController ctrl(std::move(mock));

    ctrl.startServer(8080);
    EXPECT_EQ(raw->startedPort, 8080);
}

TEST(ChatControllerTest, ReceivedMessagesDeliveredToCallback) {
    auto mock = std::make_unique<MockNetwork>();
    MockNetwork* raw = mock.get();
    ChatController ctrl(std::move(mock));

    std::string received;
    ctrl.setOnMessage([&](const std::string& m) { received = m; });

    raw->callback("Alice: Test message");
    EXPECT_EQ(received, "Alice: Test message");
}