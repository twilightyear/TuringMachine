#include "TuringMachine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace Turing;

Transition::Transition(const std::string& curr_s, char read_s, char write_s, Move m, const std::string& next_s)
    : curr_state{ curr_s }, read_symbol{ read_s }, write_symbol{ write_s }, move{ m }, next_state{ next_s }
{
}

void Transition::print(std::ostream& os) const
{
    os << curr_state << " ";
    os << read_symbol << " ";
    os << write_symbol << " ";
    os << (move == Move::LEFT ? 'l' : (move == Move::RIGHT ? 'r' : '*')) << " ";
    os << next_state;
}

Table::Table() {}

Table::~Table() {}

void Table::addTransition(const std::string& curr_s, char read_s, char write_s, Move move, const std::string& next_s)
{
    transitions.emplace_back(curr_s, read_s, write_s, move, next_s);
}

Transition* Table::findTransition(const std::string& curr_s, char read_s)
{
    for (auto& transition : transitions)
    {
        if (transition.getCurrState() == curr_s &&
            (transition.getReadSymbol() == read_s || transition.getReadSymbol() == '*'))
        {
            return &transition;
        }
    }
    return nullptr;
}

void Table::clear()
{
    transitions.clear();
}

std::string trim(const std::string& str)
{
    auto first = std::find_if_not(str.begin(), str.end(), [](int ch) {
        return std::isspace(ch);
        });
    auto last = std::find_if_not(str.rbegin(), str.rend(), [](int ch) {
        return std::isspace(ch);
        }).base();

        return (first >= last) ? std::string() : std::string(first, last);
}

void Table::initialize(const std::string& rule_script)
{
    std::istringstream iss(rule_script);
    std::string line;
    transitions.clear();

    while (std::getline(iss, line))
    {
        auto comment_pos = line.find(';');
        if (comment_pos != std::string::npos)
        {
            line = line.substr(0, comment_pos);
        }

        line = trim(line);

        if (line.empty())
        {
            continue;
        }

        std::istringstream line_stream(line);
        std::string curr_s, next_s, move_s;
        char read_s, write_s;

        line_stream >> curr_s >> read_s >> write_s >> move_s >> next_s;

        if (curr_s.empty() || next_s.empty() || move_s.empty() || read_s == '\0' || write_s == '\0')
        {
            std::cerr << "Error: 값을 분석하는데 실패했습니다. [ 줄: '" << line << " ]" << std::endl;
            continue;
        }

        Move move;
        if (move_s == "r")
        {
            move = Move::RIGHT;
        }
        else if (move_s == "l")
        {
            move = Move::LEFT;
        }
        else if (move_s == "*")
        {
            move = Move::NONE;
        }
        else
        {
            std::cerr << "Error: 움직일 방향이 올바르지 않습니다. [ 방향: " << move_s << ", 줄 :" << line << " ]" << std::endl;
            continue;
        }

        addTransition(curr_s, read_s, write_s, move, next_s);
    }
}

bool Table::load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: 파일을 여는데 실패했습니다. [ 경로: " << path << " ]" << std::endl;
        return false;
    }
    else {
        transitions.clear();
    }

    std::string line;
    while (std::getline(file, line))
    {
        auto comment_pos = line.find(';');
        if (comment_pos != std::string::npos)
        {
            line = line.substr(0, comment_pos);
        }

        line = trim(line);

        if (line.empty())
        {
            continue;
        }

        std::istringstream line_stream(line);
        std::string curr_s, next_s, move_s;
        char read_s, write_s;

        line_stream >> curr_s >> read_s >> write_s >> move_s >> next_s;

        if (line_stream.fail())
        {
            std::cerr << "Error: 값을 분석하는데 실패했습니다. [ 줄: '" << line << " ]" << std::endl;
            return false;
        }

        Move move;
        if (move_s == "r")
        {
            move = Move::RIGHT;
        }
        else if (move_s == "l")
        {
            move = Move::LEFT;
        }
        else if (move_s == "*")
        {
            move = Move::NONE;
        }
        else
        {
            std::cerr << "Error: 움직일 방향이 올바르지 않습니다. [ 방향: " << move_s << ", 줄 :" << line << " ]" << std::endl;
            return false;
        }

        addTransition(curr_s, read_s, write_s, move, next_s);
    }

    file.close();
    return true;
}

void Table::print(std::ostream& os) const
{
    int line_number = 0;
    for (const auto& trans : transitions)
    {
        os << "[" << line_number << "] ";
        trans.print(os);
        os << std::endl;
        ++line_number;
    }
}

Tape::Tape() : sz(0), space(0), elem(nullptr) {}

Tape::Tape(const Tape& t) : sz(t.sz), space(t.space), elem(new char[t.sz])
{
    std::copy(t.elem, t.elem + sz, elem);
}

Tape::Tape(Tape&& t) noexcept : sz(t.sz), space(t.space), elem(t.elem)
{
    t.sz = 0;
    t.space = 0;
    t.elem = nullptr;
}

Tape::~Tape()
{
    delete[] elem;
}

Tape& Tape::operator=(const Tape& t)
{
    if (this == &t) return *this;
    if (space < t.sz)
    {
        delete[] elem;
        elem = new char[t.sz];
        space = t.sz;
    }
    sz = t.sz;
    std::copy(t.elem, t.elem + sz, elem);
    return *this;
}

Tape& Tape::operator=(Tape&& t) noexcept
{
    delete[] elem;
    elem = t.elem;
    sz = t.sz;
    space = t.space;
    t.elem = nullptr;
    t.sz = 0;
    t.space = 0;
    return *this;
}

bool Tape::read(int i, char& c) const
{
    if (i >= 0 && i < sz)
    {
        c = elem[i];
        return true;
    }
    return false;
}

bool Tape::write(int i, char c)
{
    if (i >= 0 && i < sz)
    {
        elem[i] = c;
        return true;
    }
    return false;
}

void Tape::push_back(char c)
{
    if (sz == space)
    {
        reserve(space == 0 ? 8 : 2 * space);
    }
    elem[sz++] = c;
}

void Tape::push_front(char c)
{
    if (sz == space)
    {
        reserve(space == 0 ? 8 : 2 * space);
    }
    for (int i = sz; i > 0; --i)
    {
        elem[i] = elem[i - 1];
    }
    elem[0] = c;
    ++sz;
}

void Tape::reserve(int newalloc)
{
    if (newalloc <= space) return;
    char* p = new char[newalloc];
    for (int i = 0; i < sz; ++i)
    {
        p[i] = elem[i];
    }
    delete[] elem;
    elem = p;
    space = newalloc;
}

void Tape::resize(int newsize)
{
    reserve(newsize);
    for (int i = sz; i < newsize; ++i)
    {
        elem[i] = 0;
    }
    sz = newsize;
}

int Tape::size() const
{
    return sz;
}

int Tape::capacity() const
{
    return space;
}

void Tape::initialize(const std::string& s)
{
    delete[] elem;
    sz = s.size();
    space = sz;
    elem = new char[sz];
    for (int i = 0; i < sz; ++i)
    {
        elem[i] = s[i];
    }
}

void Tape::clear()
{
    delete[] elem;
    sz = 0;
    space = 0;
    elem = nullptr;
}

void Tape::print(std::ostream& os) const
{
    for (int i = 0; i < sz; ++i)
    {
        os << elem[i];
    }
}

void Machine::initTape(const std::string& initial_symbols)
{
    tape.initialize(initial_symbols);
}



void Machine::initTable(const std::string& rule_script)
{
    table.initialize(rule_script);
}

bool Machine::loadTable(const std::string& path)
{
    return table.load(path);
}

void Machine::start(const std::string& start_state, const std::string& accept_state, const std::string& reject_state)
{
    current_state = start_state;
    current_mode = Mode::NORMAL;
    current_pos = 0;
    this->accept_state = accept_state;
    this->reject_state = reject_state;
}

bool Machine::step()
{
    if (current_mode != Mode::NORMAL)
    {
        return false;
    }

    char current_symbol;
    if (!tape.read(current_pos, current_symbol))
    {
        tape.push_back('_');
        current_symbol = '_';
    }

    Transition* transition = table.findTransition(current_state, current_symbol);
    if (!transition)
    {
        current_mode = Mode::ERROR;
        std::cerr << "Error: transition을 찿을 수 없습니다." << std::endl;
        return false;
    }

    current_state = transition->getNextState();
    tape.write(current_pos, transition->getWriteSymbol());

    switch (transition->getMove())
    {
    case Move::LEFT:
        --current_pos;
        if (current_pos < 0)
        {
            tape.push_front('_');
            current_pos = 0;
        }
        break;
    case Move::RIGHT:
        ++current_pos;
        if (current_pos >= tape.size())
        {
            tape.push_back('_');
        }
        break;
    case Move::NONE:
        break;
    }

    if (current_state == accept_state)
    {
        current_mode = Mode::ACCEPT;
    }
    else if (current_state == reject_state)
    {
        current_mode = Mode::REJECT;
    }

    return true;
}
