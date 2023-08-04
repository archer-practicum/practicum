#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

string GetFileContents(string file) {
    ifstream stream(file);

    // конструируем string по двум итераторам
    return {(istreambuf_iterator<char>(stream)), istreambuf_iterator<char>()};
}

// напишите эту функцию
bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) {
    ifstream ifile(in_file);
    if (!ifile.good()) return false;

    static std::regex include_1(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
    static std::regex include_2(R"/(\s*#\s*include\s*<([^>]*)>\s*)/");
    std::smatch m;

    ofstream ofile(out_file);
    for (std::string line; std::getline(ifile, line);) {
        if (std::regex_match(line, m, include_1)) {
            std::filesystem::path path_file = in_file.parent_path() / std::filesystem::path(m[1]);
            ifstream file(path_file);
            if (file.good()) {
                std::string content{(istreambuf_iterator<char>(file)), istreambuf_iterator<char>()};
                ofile.write(content.data(), content.size());
            } else {
                std::cout << "error not file"sv << std::endl;
                return false;
            }            
        } else if (std::regex_match(line, m, include_2)) {
            bool has = false;
            for (const auto &path : include_directories) {
                std::filesystem::path path_file = path / std::filesystem::path(m[1]);
                ifstream file(path_file);
                if (file.good()) {
                    std::string content{(istreambuf_iterator<char>(file)), istreambuf_iterator<char>()};
                    ofile.write(content.data(), content.size());
                    has = true;
                    break;
                }
            }
            if (!has) {
                std::cout << "error not file"sv << std::endl;
                return false;
            }
        } else {
            ofile.write(line.data(), line.size());
        }
    }
    return true;
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
                "#include \"dir1/b.h\"\n"
                "// text between b.h and c.h\n"
                "#include \"dir1/d.h\"\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"
                "#   include<dummy.txt>\n"
                "}\n"sv;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
                "#include \"subdir/c.h\"\n"
                "// text from b.h after include"sv;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
                "#include <std1.h>\n"
                "// text from c.h after include\n"sv;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
                "#include \"lib/std2.h\"\n"
                "// text from d.h after include\n"sv;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"sv;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"sv;
    }

    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
                                  {"sources"_p / "include1"_p,"sources"_p / "include2"_p})));

    ostringstream test_out;
    test_out << "// this comment before include\n"
                "// text from b.h before include\n"
                "// text from c.h before include\n"
                "// std1\n"
                "// text from c.h after include\n"
                "// text from b.h after include\n"
                "// text between b.h and c.h\n"
                "// text from d.h before include\n"
                "// std2\n"
                "// text from d.h after include\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"sv;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}