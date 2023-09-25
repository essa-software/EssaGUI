#include <EssaUtil/Testing.hpp>

#include <EssaUtil/DelayedInit.hpp>

TEST_CASE(delayed_init) {
    // Construction
    {
        char const str[] = "{{foo bar baz etc}}";

        Util::DelayedInit<std::string> ref = str;
        EXPECT(ref.is_initialized());
        EXPECT_EQ(*ref, str);
    }

    // Delayed assignment
    {
        char const str[] = "{{foo bar baz etc}}";

        Util::DelayedInit<std::string> ref;
        EXPECT(!ref.is_initialized());

        ref = str;
        EXPECT(ref.is_initialized());
        EXPECT_EQ(*ref, str);
    }

    // Delayed construction and destruction
    {
        char const str[] = "{{foo bar baz etc}}";

        Util::DelayedInit<std::string> ref;
        EXPECT(!ref.is_initialized());

        ref.construct(str);
        EXPECT(ref.is_initialized());
        EXPECT_EQ(*ref, str);

        ref.destruct();
        EXPECT(!ref.is_initialized());
    }

    return {};
}

TEST_CASE(delayed_init_reference) {
    // Construction from rvalue
    // Util::DelayedInit<std::string&> ref2 = "shouldn't work";

    // Construction from lvalue
    {
        std::string str = "{{foo bar baz etc}}";

        Util::DelayedInit<std::string&> ref = str;
        EXPECT(ref.is_initialized());
        EXPECT_EQ(*ref, str);
    }

    // Delayed assignment
    {
        std::string str = "{{foo bar baz etc}}";

        Util::DelayedInit<std::string&> ref;
        EXPECT(!ref.is_initialized());

        ref = str;
        EXPECT(ref.is_initialized());
        EXPECT_EQ(*ref, str);
    }

    // Delayed construction and destruction
    {
        std::string str = "{{foo bar baz etc}}";

        Util::DelayedInit<std::string&> ref;
        EXPECT(!ref.is_initialized());

        ref.construct(str);
        EXPECT(ref.is_initialized());
        EXPECT_EQ(*ref, str);

        ref.destruct();
        EXPECT(!ref.is_initialized());
    }

    return {};
}
