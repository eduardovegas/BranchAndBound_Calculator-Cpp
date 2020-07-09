#include "Data.h"
#include <fstream>

#define M 100000000.0

Data::Data()
{

    Min = false;

}

Data::Data(const char* file_name)
{

    lerArquivo(file_name);

}

void Data::lerUsuario(){

    int numc = 0;
    int numr = 0;
    int nVT = 0;
    int nVA = 0;
    double num2 = 0;
    double value = 0;
    std::string tipoR;
    std::vector <int> indexVA;

    std::vector <double> auxDouble;


    std::cout << "Problema de Maximizacao(0) ou Minimizacao(1)?" << std::endl;
    std::cin >> Min;

    std::cout << "Numero de variaveis da funcao objetiva: ";
    std::cin >> numc;
    nVT = numc;

    std::cout << "Numero de restricoes:";
    std::cin >> numr;

    for(int i = 0; i < numc + numr; i++){ //evitar usar resize
        auxDouble.push_back(0.0);
    }
    for(int i = 0; i < numr+1; i++){ //evitar usar resize
        auxCoefs.push_back(auxDouble);
    }
    //auxCoefs.resize(numr+1, std::vector<double>(numc + numr));
    
    for(int i = 0; i < numr+1; i++){ //evitar usar resize
        auxValores.push_back(0.0);
    }
    //auxValores.resize(numr+1);
    

    for(int i = 0; i < numr+1; i++){  // construir o problema

        if(i == 0){
            std::cout << "Coeficientes da funcao objetivo: ";
        }else{
            std::cout << "Coeficientes da restricao " << i << ": ";
        }

        for(int j = 0; j < numc; j++){

            std::cin >> num2;

            auxCoefs[i][j] = num2;
        }

    }

    std::cout << "Lado direito das restricoes: ";

    auxValores[0] = 0;
    for(int i = 1; i < numr+1; i++){

        std::cin >> value;
        auxValores[i] = value;
    }
    getchar();


    for(int i = 1; i < numr+1; i++){  // construir o problema


        std::cout << "Tipo da restricao numero " << i << "('<' para <= , '=' para =, '>' para >=) : ";

        std::getline(std::cin, tipoR);

        if(tipoR == "<"){
            
            nVT++;
            auxCoefs[i][nVT-1] = 1.0; // variavel de folga

        }

        if(tipoR == "="){ // Big M
            
            nVT++;
            VA.push_back(nVT);
            indexVA.push_back(i);

            auxCoefs[0][nVT-1] = -M; // penalidade
            auxCoefs[i][nVT-1] = 1.0; // variavel artificial

        }

        if(tipoR == ">"){ // Big M e variavel de folga
            
            nVT++;
            auxCoefs[i][nVT-1] = -1.0; // variavel de folga

            for(int n = 0; n < auxCoefs.size(); n++){
                
                auxCoefs[n].push_back(0.0);
            }
            nVT++;
            VA.push_back(nVT);
            indexVA.push_back(i);

            auxCoefs[0][nVT-1] = -M; // penalidade
            auxCoefs[i][nVT-1] = 1.0; // variavel artificial 

        }
        
    }


    for(int i = 0; i < numc; i++){

        VO.push_back(i+1); // adicionar as variaveis objetivo

        if(Min)
            auxCoefs[0][i] = -auxCoefs[0][i];


    }
    
    for(int j = 0; j < auxCoefs[0].size(); j++){
        
        if(auxCoefs[0][j] != 0.0)
            auxCoefs[0][j] = -auxCoefs[0][j]; // passar os valores da primeira linha para o outro lado
    }


    for(int i = 0; i < indexVA.size(); i++){ // Retirar as variaveis artificais da primeira linha

        int index = indexVA[i];
        
        for(int j = 0; j < auxCoefs[0].size(); j++){

            auxCoefs[0][j] = auxCoefs[0][j] - (double)(M*auxCoefs[index][j]);
        }

        auxValores[0] = auxValores[0] - (double)(M*auxValores[index]);

    }


    return;

}

void Data::lerArquivo(const char* file_name){

    std::ifstream file(file_name);

    int nVT = 0;
    int numVar = 0;
    int numRes = 0;
    std::string auxS;
    std::vector <int> indexVA;
    std::vector <std::string> tipoRes;

    std::vector <double> auxDouble;

    
    if (!file.is_open())
        exit(1);

    if(!(file >> auxS) || auxS != "NAME:")
        exit(1);
    
    file.ignore(1, ' ');
    std::getline(file, nome);
    file.ignore();

    if(!(file >> auxS) || auxS != "p_t:")
        exit(1);

    if(!(file >> Min) || (Min != 1 && Min != 0))
        exit(1);

    if(!(file >> auxS) || auxS != "n_v:")
        exit(1);

    file >> numVar;
    
    if(!(file >> auxS) || auxS != "n_c:")
        exit(1);

    file >> numRes;

    file.ignore();

    if (!(file >> auxS) || auxS != "Z" ||
        !(file >> auxS) || auxS != "="  )
        exit(1);

    for(int i = 0; i < numVar + numRes; i++){ //evitar usar resize
        auxDouble.push_back(0.0);
    }
    for(int i = 0; i < numRes+1; i++){ //evitar usar resize
        auxCoefs.push_back(auxDouble);
    }
    //auxCoefs.resize(numRes+1, std::vector<double>(numVar + numRes));
    
    for(int i = 0; i < numRes+1; i++){ //evitar usar resize
        auxValores.push_back(0.0);
    }
    //auxValores.resize(numRes+1);
    tipoRes.resize(numRes);


    nVT = numVar;
    auxValores[0] = 0.0;

    for(int i = 0; i < numRes+1; i++){ // CONSTRUIR O PROBLEMA

        for(int j = 0; j < numVar; j++){

            file >> auxCoefs[i][j];

        }

        if(i == 0)
            continue;

        file.ignore(1, ' ');
        file >> tipoRes[i-1];

        file >> auxValores[i];

    }

    for(int i = 0; i < tipoRes.size(); i++){  // CONSTRUIR O PROBLEMA


        if(tipoRes[i] == "<"){
            
            nVT++;
            auxCoefs[i+1][nVT-1] = 1.0; // variavel de folga

        }

        if(tipoRes[i] == "="){ // Big M
            
            nVT++;
            VA.push_back(nVT);
            indexVA.push_back(i+1);

            auxCoefs[0][nVT-1] = -M; // penalidade
            auxCoefs[i+1][nVT-1] = 1.0; // variavel artificial

        }

        if(tipoRes[i] == ">"){ // Big M e variavel de folga
            
            nVT++;
            auxCoefs[i+1][nVT-1] = -1.0; // variavel de folga

            for(int n = 0; n < auxCoefs.size(); n++){

                auxCoefs[n].push_back(0.0);
            }
            nVT++;
            VA.push_back(nVT);
            indexVA.push_back(i+1);

            auxCoefs[0][nVT-1] = -M; // penalidade
            auxCoefs[i+1][nVT-1] = 1.0; // variavel artificial 

        }
        
    }

    for(int i = 0; i < numVar; i++){

        VO.push_back(i+1); // adicionar as variaveis objetivo

        if(Min)
            auxCoefs[0][i] = -auxCoefs[0][i];

    }
    
    for(int j = 0; j < auxCoefs[0].size(); j++){
        
        if(auxCoefs[0][j] != 0.0)
            auxCoefs[0][j] = -auxCoefs[0][j]; // passar os valores da primeira linha para o outro lado
    }


    for(int i = 0; i < indexVA.size(); i++){ // Retirar as variaveis artificais da primeira linha

        int index = indexVA[i];
        
        for(int j = 0; j < auxCoefs[0].size(); j++){

            auxCoefs[0][j] = auxCoefs[0][j] - (double)(M*auxCoefs[index][j]);
        }

        auxValores[0] = auxValores[0] - (double)(M*auxValores[index]);

    }


    file.close();

    return;

}