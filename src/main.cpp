#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::string read_file(const std::string & path)
{
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}
class Environment {};
void eval(const Environment& env, const std::string & source)
{
  std::cout << source << std::endl;
}


void repl(){
  std::string line;
  Environment environment;
  while (true)
  {

    std::cout << ">> ";
    std::getline(std::cin, line);
    eval(environment, line);
  }

}

int main(int argc, char const *argv[])
{
  if (argc == 1) {
    repl();
  }
  else if (argc == 2)
  {
    // run file given as an argument
    // read file
    std::string source = read_file(argv[1]);
    eval(Environment(), source);

  }

  return 0;
}
