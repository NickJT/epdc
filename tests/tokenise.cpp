
#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <algorithm>
/**
 * @brief Demonstrating that tokenizing words is not a good way of 
 * wordwrapping text
 * 1) Ends with a vector of vector of string_views that needs be be iterated over
 * 2) Can't deal with hard linebreaks without additional tests
 * 3) Can't remove the last space from a word-wrapped line without additional tests
 * 
 * Tokenise function works well if "finish-1" is changed to "finish". Then it produces a 
 * vector of string_view tokens with no leading or trailing spaces
 */

using namespace std;

constexpr int MAX_COMP{1024};
constexpr int MAX_LEN{16};
constexpr char CR{'\r'};
constexpr char LF{'\n'};
constexpr char SPACE{' '};
constexpr char EOT{0};
constexpr char wspace[4]{CR, LF, SPACE, EOT};
constexpr std::string_view WHITESPACE(wspace, std::size(wspace));
constexpr std::string_view space(&SPACE, 1);

string_view tst1{"A line to wrap"};
string_view tst2{"A line that is longer with  spaces. sfasdf sfwe W erth JKFGHKJR DS SD dgfag adgh\
dfgadfg jdg sdfg  kjhjkfhjdg fgdf j hgjfdf dfgdfg jghjf dgdf jk ghjfgd edfsdf d hfgjfgj dfgd\
sdfsadf sdfasdf hsdgfsdg sdf fsshjsdgfs ash hf dfgss hjssd gh end."};
string_view tst3{"A line with linebreaks\nSecond line with breaks\nThird line\nFinal Line."};
string_view tst4{"              Prefix spaces."};
string_view tst5{"Postfix spaces.    "};

vector<string_view> tests;

void wrapTestData()
{
    tests.push_back("A line to wrap");
    tests.push_back("A line to wrap with a few extraordinarily long words");
    tests.push_back("isninechr");
    tests.push_back("A");
    tests.push_back("ismorethantenchars");
    tests.push_back(tst1);
    tests.push_back(tst2);
    tests.push_back(tst3);
    tests.push_back(tst4);
    tests.push_back(tst5);
}

bool isDelimiter(char const c)
{
    return (c == SPACE) || (c == EOT);
}

bool isLineBreak(char const c)
{
    return ((c == CR) || (c == LF));
}

bool isEndOfText(char const c)
{
    return (c == EOT);
}

size_t widthOf(char c)
{
    return 1;
}

size_t widthOf(std::string_view s)
{
    size_t width{0};
    for (auto const c : s)
    {
        width += widthOf(c);
    }
    return width;
}

std::vector<std::pair<std::string_view, size_t>> tokenize(std::string_view s)
{
    std::vector<std::pair<std::string_view, size_t>> words;
    size_t start{0};
    size_t finish{0};

    while (finish != std::string_view::npos)
    {
        start = s.find_first_not_of(SPACE, finish);
        // no letters left - we're done here
        if (start == string_view::npos)
        {
            return words;
        }
        finish = s.find_first_of(SPACE, start);
        // No spaces so use the rest of the string
        if (finish == string_view::npos)
        {
            finish = s.size();
        }
        auto word{s.substr(start, finish + 1 - start)};

        size_t width{widthOf(word)};
        words.push_back({word, width});
        start = finish;
    }
    return words;
}

// A line to wrap
int main()
{
    vector<string_view> tests{"  A line to wrap  ",
                              "A line with lots of small words that make up a long sentence ending in a dot.",
                              "multi    space",
                              {" last "},
                              {"   A"},
                              {"AAA"},
                              {"   "}};
    const char old[]{'o', 'l', 'd', 0};
    string_view conv{old};
    tests.push_back(conv);

    for (auto test : tests)
    {
        cout << "Test: " << test << endl;
        auto res{tokenize(test)};
        for (auto word : res)
        {
            cout << "|" << word.first << "| = " << word.second << endl;
        }

        std::vector<std::vector<std::string_view>> wrapped;

        std::vector<std::string_view> line;
        size_t lineLength{0};
        size_t maxLine{9};
        for (auto word : res)
        {
            lineLength += word.second;
            if (lineLength <= maxLine)
            {
                line.push_back(word.first);
            }
            else
            {
                wrapped.push_back(line);
                lineLength = 0;
                line.resize(0);
                line.push_back(word.first);
                lineLength = word.second;
            }
        }
        wrapped.push_back(line);
        for (auto line : wrapped)
        {
            for (auto word : line)
            {
                cout << word;
            }
            cout << endl;
        }
    }

    return EXIT_SUCCESS;
}