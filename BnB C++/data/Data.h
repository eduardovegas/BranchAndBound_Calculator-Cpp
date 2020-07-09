#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <string>
#include <vector>

class Data
{
    public:

        bool Min;
        std::string nome;
        std::vector <int> VO;
        std::vector <int> VA;
        std::vector <double> auxValores;
        std::vector <std::vector<double>> auxCoefs;

        Data();
        Data(const char*);
        void lerUsuario();
        void lerArquivo(const char*);

};


#endif