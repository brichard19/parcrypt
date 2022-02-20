#ifndef _GET_OPT_H
#define _GET_OPT_H

#include <string>
#include <vector>
#include <map>

namespace platform {

class Option {

public:
  std::string name;
  std::string arg;
};

class ArgParserException : public std::exception
{

private:
  std::string _msg;

public:
  ArgParserException(const std::string& msg) : _msg(msg)
  {
  }

  const char* what()
  {
    return _msg.c_str();
  }
};

class ArgParser {

private:
  int _argc;
  char** _argv;

  class OptArgPair {
  public:
    std::string opt;
    bool has_arg;

    OptArgPair()
    {
      opt = "";
      has_arg = false;
    }

    OptArgPair(const std::string& o, bool a) : opt(o), has_arg(a)
    {
    }

  };

  std::map<std::string, OptArgPair> _opt_args;
  std::vector<std::string> _operands;

public:
  ArgParser(int argc, char** argv) : _argc(argc), _argv(argv) {}

  std::vector<Option> parse()
  {
    std::vector<Option> opts;

    for(int i = 1; i < _argc; i++) {

      std::string s(_argv[i]);
      if(s.length() > 2 && s[0] == '-' && s[1] == '-') {
        // Possibly an option
        std::string opt = s.substr(2);

        if(_opt_args.find(opt) != _opt_args.end()) {
          // It's an option
          std::string arg = "";
          if(_opt_args[opt].has_arg) {
            if(i == _argc - 1) {
              throw ArgParserException(s + " requires an argument");
            }
            arg = std::string(_argv[i + 1]);
          }

          Option o;
          o.name = opt;
          o.arg = arg;

          opts.push_back(o);

          // Skip the next element since it is an option argument
          i++;

        } else {
          throw ArgParserException("Unknown option " + s);
        }
      } else {
        // Definitely not an option
        _operands.push_back(s);
      }
    }

    return opts;
  }

  std::vector<std::string> operands()
  {
    return _operands;
  }

  void add(const std::string& option, bool has_arg)
  {
    _opt_args[option] = OptArgPair(option, has_arg);
  }
};

}

#endif