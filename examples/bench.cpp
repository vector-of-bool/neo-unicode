#include <neo/unicode.hpp>

#define NONIUS_RUNNER
#include <nonius.h++>

/**
 * Explicitly instantiate std::string so that we are sure to get the same
 * optimization levels, since libstdc++ declares it as an extern template.
 */
template class std::basic_string<char>;

using namespace nonius;
using namespace std;

NONIUS_BENCHMARK("Create a small std::string", [] {
    string str = "Small";
    return str;
});

NONIUS_BENCHMARK("Create a large std::string", [] {
    string str
        = "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a "
          "story the Jedi would tell you. It’s a Sith legend. Darth Plagueis was a Dark Lord of "
          "the Sith, so powerful and so wise he could use the Force to influence the midichlorians "
          "to create life… He had such a knowledge of the dark side that he could even keep the "
          "ones he cared about from dying. The dark side of the Force is a pathway to many "
          "abilities some consider to be unnatural. He became so powerful… the only thing he was "
          "afraid of was losing his power, which eventually, of course, he did. Unfortunately, he "
          "taught his apprentice everything he knew, then his apprentice killed him in his sleep. "
          "Ironic. He could save others from death, but not himself.";
    return str;
});

NONIUS_BENCHMARK("Copy small std::string", [](chronometer meter) {
    vector<storage_for<string>> storage(meter.runs());
    string str = "Small";
    meter.measure([&](int i) { storage[i].construct(str); });
});

NONIUS_BENCHMARK("Copy large std::string", [](chronometer meter) {
    string str
        = "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a "
          "story the Jedi would tell you. It’s a Sith legend. Darth Plagueis was a Dark Lord of "
          "the Sith, so powerful and so wise he could use the Force to influence the midichlorians "
          "to create life… He had such a knowledge of the dark side that he could even keep the "
          "ones he cared about from dying. The dark side of the Force is a pathway to many "
          "abilities some consider to be unnatural. He became so powerful… the only thing he was "
          "afraid of was losing his power, which eventually, of course, he did. Unfortunately, he "
          "taught his apprentice everything he knew, then his apprentice killed him in his sleep. "
          "Ironic. He could save others from death, but not himself.";
    vector<storage_for<string>> storage(meter.runs());
    meter.measure([&](int i) { storage[i].construct(str); });
});

NONIUS_BENCHMARK("Create neo::unicode from large charptr", [] {
    const char* str
        = "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a "
          "story the Jedi would tell you. It’s a Sith legend. Darth Plagueis was a Dark Lord of "
          "the Sith, so powerful and so wise he could use the Force to influence the midichlorians "
          "to create life… He had such a knowledge of the dark side that he could even keep the "
          "ones he cared about from dying. The dark side of the Force is a pathway to many "
          "abilities some consider to be unnatural. He became so powerful… the only thing he was "
          "afraid of was losing his power, which eventually, of course, he did. Unfortunately, he "
          "taught his apprentice everything he knew, then his apprentice killed him in his sleep. "
          "Ironic. He could save others from death, but not himself.";
    neo::unicode u = str;
    return u;
})

NONIUS_BENCHMARK("Copy literal neo::unicode", [](chronometer meter) {
    neo::unicode str
        = "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a "
          "story the Jedi would tell you. It’s a Sith legend. Darth Plagueis was a Dark Lord of "
          "the Sith, so powerful and so wise he could use the Force to influence the midichlorians "
          "to create life… He had such a knowledge of the dark side that he could even keep the "
          "ones he cared about from dying. The dark side of the Force is a pathway to many "
          "abilities some consider to be unnatural. He became so powerful… the only thing he was "
          "afraid of was losing his power, which eventually, of course, he did. Unfortunately, he "
          "taught his apprentice everything he knew, then his apprentice killed him in his sleep. "
          "Ironic. He could save others from death, but not himself.";
    vector<storage_for<neo::unicode>> storage(meter.runs());
    meter.measure([&](int i) { storage[i].construct(str); });
});

NONIUS_BENCHMARK("Copy small neo::unicode", [](chronometer meter) {
    const char* charptr = "Small";
    neo::unicode str = charptr;
    vector<storage_for<neo::unicode>> storage(meter.runs());
    meter.measure([&](int i) { storage[i].construct(str); });
});

NONIUS_BENCHMARK("Copy large neo::unicode", [](chronometer meter) {
    const char* charptr
        = "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a "
          "story the Jedi would tell you. It’s a Sith legend. Darth Plagueis was a Dark Lord of "
          "the Sith, so powerful and so wise he could use the Force to influence the midichlorians "
          "to create life… He had such a knowledge of the dark side that he could even keep the "
          "ones he cared about from dying. The dark side of the Force is a pathway to many "
          "abilities some consider to be unnatural. He became so powerful… the only thing he was "
          "afraid of was losing his power, which eventually, of course, he did. Unfortunately, he "
          "taught his apprentice everything he knew, then his apprentice killed him in his sleep. "
          "Ironic. He could save others from death, but not himself.";
    neo::unicode str = charptr;
    vector<storage_for<neo::unicode>> storage(meter.runs());
    meter.measure([&](int i) { storage[i].construct(str); });
});