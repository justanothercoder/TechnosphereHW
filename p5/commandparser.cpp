#include "commandparser.hpp"
#include <map>

std::unique_ptr<ProcessTree> CommandParser::parse(const std::string& line)    
{
    tokenize(line);

    std::map<std::string, std::function<Tree(Tree, Tree)>> factory = {
        {"&&", [](Tree a, Tree b) { return std::make_unique<AndProcess>(std::move(a), std::move(b)); }},
        {"||", [](Tree a, Tree b) { return std::make_unique<OrProcess>(std::move(a), std::move(b)); }},
        {"|" , [](Tree a, Tree b) { return std::make_unique<PipeProcess>(std::move(a), std::move(b)); }}
    };

    auto tree = parse_command();
    while (pos < tokens.size() && (tokens[pos] == "&&" || tokens[pos] == "||" || tokens[pos] == "|")) {
        auto op = tokens[pos];
        ++pos;
       
        auto command = parse_command();
        tree = factory[op](std::move(tree), std::move(command));
    }

    if (pos < tokens.size() && tokens[pos] == "&") {
        ++pos;
        tree->is_daemon = true;
    }

    if (pos != tokens.size()) {
        return nullptr;
    }

    return std::move(tree);
}

void CommandParser::tokenize(const std::string& line)
{
    pos = 0;

    while (pos < line.length()) {
        while (pos < line.length() && std::isspace(line[pos])) {
            ++pos;           
        }
        
        if (pos == line.length()) {
            break;
        }

        if (line[pos] == '&') {
            ++pos;
            if (pos < line.length() && line[pos] == '&') {
                ++pos;
                tokens.push_back("&&");            
            } else {
                tokens.push_back("&");
            }
        } else if (line[pos] == '|') {
            ++pos;
            if (pos < line.length() && line[pos] == '|') {
                ++pos;
                tokens.push_back("||");            
            } else {
                tokens.push_back("|");
            }    
        } else if (line[pos] == '<') {
            ++pos;
            tokens.push_back("<");        
        } else if (line[pos] == '>') {
            ++pos;
            tokens.push_back(">");
        } else if (line[pos] == '"') {
            ++pos;
            std::string buf = "";
            while (pos < line.length() && line[pos] != '"') {
                buf += line[pos];
                ++pos;            
            }
            ++pos;
            tokens.emplace_back(std::move(buf));
        } else {
            std::string buf = "";
            while (pos < line.length() 
                && line[pos] != '&' 
                && line[pos] != '|' 
                && line[pos] != '<'
                && line[pos] != '>'
                && !std::isspace(line[pos])) {
                buf += line[pos];
                ++pos;
            }
            tokens.emplace_back(std::move(buf));
        }
    }

    pos = 0;
}

bool CommandParser::isNotOperation(const std::string& line)
{
    return line != "&&" && line != "||" && line != "|" && line != "<" && line != ">";
}

Tree CommandParser::parse_command()
{
    Command command;
    command.program = tokens[pos];
    ++pos;

    while (pos < tokens.size() && isNotOperation(tokens[pos])) {               
        command.args.emplace_back(tokens[pos]);
        ++pos;
    }

    while (pos < tokens.size() && (tokens[pos] == ">" || tokens[pos] == "<")) {
        auto op = tokens[pos];
        ++pos;

        if (op == ">") {
            command.out_file = tokens[pos];
            ++pos;
        } else {
            command.in_file = tokens[pos];
            ++pos;
        }
    }

    return std::make_unique<LeafProcess>(command);
}
