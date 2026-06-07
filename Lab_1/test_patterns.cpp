#include <gtest/gtest.h>
#include "patterns/singleton_logger.hpp"
#include "patterns/message_factory.hpp"
#include "patterns/message_decorator.hpp"
#include "patterns/message_builder.hpp"
#include "patterns/command.hpp"
#include "patterns/observer.hpp"

TEST(SingletonTest, SameInstance) {
    Logger& a = Logger::instance();
    Logger& b = Logger::instance();
    EXPECT_EQ(&a, &b);
}

TEST(SingletonTest, LogStored) {
    Logger::instance().log("test entry");
    const auto& h = Logger::instance().history();
    EXPECT_FALSE(h.empty());
    EXPECT_NE(h.back().find("test entry"), std::string::npos);
}

TEST(FactoryTest, TextMessage) {
    TextMessageFactory f("Alice");
    auto msg = f.create("Hello");
    EXPECT_EQ(msg->render(), "Alice: Hello");
    EXPECT_EQ(msg->type(),   "text");
}

TEST(FactoryTest, SystemMessage) {
    SystemMessageFactory f;
    auto msg = f.create("Connected");
    EXPECT_EQ(msg->render(), "[System] Connected");
}

TEST(FactoryTest, ErrorMessage) {
    ErrorMessageFactory f;
    auto msg = f.create("Timeout");
    EXPECT_EQ(msg->render(), "[Error] Timeout");
}

TEST(DecoratorTest, EncryptedWraps) {
    TextMessageFactory f("Bob");
    auto msg = f.create("Hi");
    auto enc = std::make_unique<EncryptedDecorator>(std::move(msg));
    EXPECT_NE(enc->render().find("Bob: Hi"), std::string::npos);
}

TEST(DecoratorTest, TimestampAdded) {
    SystemMessageFactory f;
    auto msg = f.create("ok");
    auto ts  = std::make_unique<TimestampDecorator>(std::move(msg));
    EXPECT_NE(ts->render().find("["), std::string::npos);
}

TEST(DecoratorTest, ChainedDecorators) {
    TextMessageFactory f("Eve");
    auto msg = f.create("secret");
    auto enc = std::make_unique<EncryptedDecorator>(std::move(msg));
    auto ts  = std::make_unique<TimestampDecorator>(std::move(enc));
    EXPECT_NE(ts->render().find("Eve: secret"), std::string::npos);
}

TEST(BuilderTest, BasicMessage) {
    auto out = OutgoingMessageBuilder()
        .setNick("Alice").setBody("Hello").build();
    EXPECT_EQ(out.serialize(), "Alice: Hello");
}

TEST(BuilderTest, WithTag) {
    auto out = OutgoingMessageBuilder()
        .setNick("Alice").setBody("urgent").withTag("#alert").build();
    EXPECT_EQ(out.serialize(), "#alert Alice: urgent");
}

TEST(BuilderTest, WithEncryption) {
    auto out = OutgoingMessageBuilder()
        .setNick("Bob").setBody("secret").withEncryption().build();
    EXPECT_NE(out.serialize().find("[ENC]"), std::string::npos);
}

TEST(CommandTest, ExecuteAppends) {
    std::vector<std::string> log;
    CommandHistory history;
    history.execute(std::make_unique<AppendMessageCommand>(
        [&](const std::string& s){ log.push_back(s); },
        [&](){ if (!log.empty()) log.pop_back(); },
        "hello"
    ));
    EXPECT_EQ(log.size(), 1u);
    EXPECT_EQ(log[0], "hello");
}

TEST(CommandTest, UndoRemoves) {
    std::vector<std::string> log;
    CommandHistory history;
    history.execute(std::make_unique<AppendMessageCommand>(
        [&](const std::string& s){ log.push_back(s); },
        [&](){ if (!log.empty()) log.pop_back(); },
        "hello"
    ));
    history.undo();
    EXPECT_TRUE(log.empty());
}

TEST(CommandTest, CanUndoFlag) {
    std::vector<std::string> log;
    CommandHistory history;
    EXPECT_FALSE(history.canUndo());
    history.execute(std::make_unique<AppendMessageCommand>(
        [&](const std::string& s){ log.push_back(s); },
        [&](){ if (!log.empty()) log.pop_back(); },
        "x"
    ));
    EXPECT_TRUE(history.canUndo());
}

class TestObserver : public IChatObserver {
public:
    std::vector<ChatEventData> received;
    void onEvent(const ChatEventData& e) override { received.push_back(e); }
};

TEST(ObserverTest, NotifiesSubscribers) {
    ChatEventBus bus;
    TestObserver obs;
    bus.subscribe(&obs);
    bus.notify({ ChatEvent::MessageSent, "Hello" });
    ASSERT_EQ(obs.received.size(), 1u);
    EXPECT_EQ(obs.received[0].payload, "Hello");
}

TEST(ObserverTest, UnsubscribeStopsNotifications) {
    ChatEventBus bus;
    TestObserver obs;
    bus.subscribe(&obs);
    bus.unsubscribe(&obs);
    bus.notify({ ChatEvent::MessageSent, "Hello" });
    EXPECT_TRUE(obs.received.empty());
}

TEST(ObserverTest, MultipleObservers) {
    ChatEventBus bus;
    TestObserver obs1, obs2;
    bus.subscribe(&obs1);
    bus.subscribe(&obs2);
    bus.notify({ ChatEvent::PeerConnected, "127.0.0.1" });
    EXPECT_EQ(obs1.received.size(), 1u);
    EXPECT_EQ(obs2.received.size(), 1u);
}