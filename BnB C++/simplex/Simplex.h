#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <iostream>
#include <vector>
#include <cmath>

#include "../data/Data.h"

class Simplex
{

    public:

        bool min;
        bool ilimitada;
        bool degenerada;
        bool naoViavel;
        int linhas;
        int colunas;
        int auxPivo;
        int linhaPivo;
        int colunaPivo;
        std::vector <int> VO;
        std::vector <int> VA;
        std::vector <int> VB;
        std::vector <int> VNB;
        std::vector <int> VT;
        std::vector <int> varBase;
        std::vector <double> valores;
        std::vector <std::vector<double>> coefs;
        
        Simplex(Data dado);
        Simplex(bool Min, std::vector <std::vector<double>> cfs, std::vector <double> vls, std::vector<int> vo, std::vector<int> va);
        
        void calcularSimplex();
        void printarTabela();
        void printarResultado();
        void rhsAnalysis();

    private:

        bool checarOtimo();
        void acharColunaPivo();
        void acharLinhaPivo();
        void atualizarTabela();

        void obterMatBase(std::vector<std::vector<double> >& Matbasis);
        bool contains(std::vector<int>& array, int i);
        bool comparar(double* num, double floor, double ceil);
        
};


#endif