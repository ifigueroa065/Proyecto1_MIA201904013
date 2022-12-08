#ifndef SCANNER_H
#define SCANNER_H
#include <vector>
#include <string>
// #include "../lib/disco.h"

using namespace std;

class scanner
{
public:
    scanner();
    void start();
    void functions(string token, vector<string> tks); //Recibe el comando y lista de instrucciones
    string token(string text);                      
    vector<string> split(string text, string text_split);
    vector<string> split_tokens(string text);
    bool compare(string a, string b);
    string upper(string a);
    void errores(string operacion, string mensaje);
    void respuesta(string operacion, string mensaje);
    void funcion_excec(vector<string> tokens);
    void excec(string path);
    bool confirmar(string mensaje);
};

#endif