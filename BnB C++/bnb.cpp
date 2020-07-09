#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include <time.h>
#include <stdlib.h>
#include "data/Data.cpp"
#include "node/Node.cpp"
#include "simplex/Simplex.cpp"

#if defined(_WIN32) || defined(_WIN64)
    double divisao = 1000.0;
#else defined(__linux__) || defined(__unix__)
    double divisao = 1000000.0;
#endif

#define m 100000000.0

std::vector <double> val;
std::vector<std::vector<double>> matCoefs;

void aprofundamento(Data& data1, int select, int num);
void largura(Data& data1, int select, int num);
void printarNo(Node* &node, int k);
bool compare(double* num, double floor, double ceil);
void adicionarVariaveisObj(Simplex& copia, std::vector <double>& Var);
void atualizarPL(Simplex& copia, std::vector<Node*>& nos, int k);
void atualizarPL2(Simplex& copia);
bool checarLimite(double auxLong, Node* &best);
bool checarIntegralidade(int *candidate, std::vector <double>& auxVar);
int removerNos(Simplex& copia, Node* &best, std::vector<Node*>& nos, int* k);

int main(int argc, char* argv[]){

    int type = 0;
    int select = 0;
    int num = 1;

    Data data1 = Data();

    /*if(argc < 1)
        return 1;

    if(argc == 1){
        
        data1.lerUsuario();

    }else{

        data1.lerArquivo(argv[1]);

    }

    if(argc > 2){

        if(std::stoi(argv[2]) == 1)
            type = std::stoi(argv[2]);
    }

    if(argc > 3){

        if(std::stoi(argv[3]) == 1)
            select = 1;
    }

    if(argc > 4){
        
        if(std::stoi(argv[4]) >= 1)
            num = std::stoi(argv[4]);
    }*/

    switch(argc)
    {
        case 5:
            if(std::stoi(argv[4]) >= 1)
            num = std::stoi(argv[4]);

        case 4:
            if(std::stoi(argv[3]) == 1)
            select = 1;

        case 3:
            if(std::stoi(argv[2]) == 1)
            type = std::stoi(argv[2]);

        case 2:
            data1.lerArquivo(argv[1]);
            break;

        case 1:
            data1.lerUsuario();
            break;

        default:
            return 1;
    }       


    if(type)
        largura(data1, select, num);
    else
        aprofundamento(data1, select, num);


    return 0;

}

void aprofundamento(Data& data1, int select, int num){

    Simplex copia(data1);

    matCoefs = copia.coefs; // passar a matriz inicial
    val = copia.valores; // passar a matriz inicial

    Node* bestNode;
    bestNode = new Node();
    std::vector<Node*> nos;
    nos.push_back(new Node());

    int numNos = 0;
    bool print = true;
    double auxLong = 0.0;
    std::vector<int> indexList;
    std::vector <double> auxVar;

    clock_t init, end;

    int indexCandidate = 0.0;

    init = clock();
    for(int k = 0; k < nos.size(); k++){ // Branch-And-Bound
            
        numNos++;

        print = (num == 0 || (numNos-1) % num == 0); 

        if(k != 0)
            atualizarPL(copia, nos, k);

        //copia.printarTabela();
        copia.calcularSimplex();
        
        nos[k]->done = true;
        nos[k]->min = copia.min;
        nos[k]->sol = copia.valores[0];

        if(print) //printar nos
            printf("\n~~~~~~~~~~ NODE %d ~~~~~~~~~~\n", numNos);

        if(copia.ilimitada || copia.naoViavel){
            
            if(print) //printar nos
                puts("\n  parada por inviabilidade");
            
            if(k == 0) //se for o primeiro simplex
                break;

            if(removerNos(copia, bestNode, nos, &k) < 0) //all nodes removed / done
                break;

            k--;
            continue;
        
        }

        adicionarVariaveisObj(copia, auxVar);
        nos[k]->variables = auxVar;

        if(print) //printar nos
            printarNo(nos[k], numNos);

        indexList.clear();
        if(checarIntegralidade(&indexCandidate, auxVar)){ //se todas as variaveis sao inteiras
            
            if(print) //printar nos
                puts("  parada por integralidade");

            if(k == 0){ //se for o primeiro simplex
                bestNode = nos[k];
                break;
            }

            if(nos[k]->sol >= bestNode->sol)
                bestNode = nos[k];
            
            
            if(removerNos(copia, bestNode, nos, &k) < 0) //all nodes removed / done
                break;

            k--;
            continue;
        }

        auxLong = nos[k]->sol;
        if(checarLimite(auxLong, bestNode)){

            if(print) //printar nos
                puts("     parada por limite"); 

            if(removerNos(copia, bestNode, nos, &k) < 0) //all nodes removed / done
                break;

            k--;
            continue;
        }

        //int r = rand() % indexList.size();
        //r = indexList[r];
        //auxLong = auxVar[r];
        auxLong = auxVar[indexCandidate];

        if(select){
            nos.push_back(new Node(true, floor(auxLong), indexCandidate)); //No menor igual que
            nos.push_back(new Node(false, ceil(auxLong), indexCandidate)); //No maior igual que
        }else{
            nos.push_back(new Node(false, ceil(auxLong), indexCandidate)); //No maior igual que
            nos.push_back(new Node(true, floor(auxLong), indexCandidate)); //No menor igual que
        }

        k++;

    }
    end = clock();


    if(bestNode->done){

        puts("\n\n/---------------------------/");

        puts("\n** Melhor No encontrado ** \n");

        for(int i = 0; i < bestNode->variables.size(); i++){

            //printf("Variavel X%d : %.12lf\n", i+1, bestNode->variables[i]);
            
            std::cout << "Variavel X" << i+1 << " : " << bestNode->variables[i] << std::endl;

        }
        if(bestNode->min)
            std::cout << "\nSolucao : " << -bestNode->sol << std::endl;
        else
            std::cout << "\nSolucao : " << bestNode->sol << std::endl;

        puts("\n** Melhor No encontrado **\n");

        printf("Numero de Nos: %d\n", numNos);
        std::cout << "Tempo Percorrido: " << (double)(end - init) / divisao << std::endl;

        puts("\n/---------------------------/");

    }

    printf("  BUSCA POR APROFUNDAMENTO\n\n");

}

void largura(Data& data1, int select, int num){

    std::vector<Simplex> copia;
    Simplex inicial(data1);
    copia.push_back(inicial);
    
    Node* bestNode;
    bestNode = new Node();
    std::vector<Node*> nos;
    nos.push_back(new Node());

    int numNos = 0;
    bool print = true;
    double auxLong = 0.0;
    std::vector <double> auxVar;

    clock_t init, end;

    int indexCandidate = 0;

    init = clock();
    for(int k = 0; k < nos.size(); k++){ //Branch-And-Bound
            
        numNos++;

        print = (num == 0 || (numNos-1) % num == 0); 

        matCoefs = copia[k].coefs; // passar a matriz
        val = copia[k].valores; // passar a matriz

        if(numNos > 1)
            atualizarPL(copia[k], nos, k);

        Simplex auxS = copia[k];

        //copia[k].printarTabela();
        copia[k].calcularSimplex();
        
        nos[k]->done = true;
        nos[k]->min = copia[k].min;
        nos[k]->sol = copia[k].valores[0];

        if(print) //printar nos
            printf("\n~~~~~~~~~~ NODE %d ~~~~~~~~~~\n", numNos);

        if(copia[k].ilimitada || copia[k].naoViavel){
            
            if(print) //printar nos
                puts("\n  parada por inviabilidade");
            
            if(numNos == 1) //se for o primeiro simplex
                break;

            
            if(bestNode != nos[k]) //if the bestNode doesnt point to this node
                delete nos[k];
                
            nos.erase(nos.begin() + k);
            copia.erase(copia.begin() + k);

            if(copia.size() == 0) //all nodes removed / done
                break;

            k--;

            continue;
        
        }

        adicionarVariaveisObj(copia[k], auxVar);
        nos[k]->variables = auxVar;

        if(print) //printar nos
            printarNo(nos[k], numNos);

        if(checarIntegralidade(&indexCandidate, auxVar)){ //se todas as variaveis sao inteiras
            
            if(print) //printar nos
                puts("  parada por integralidade");

            if(numNos == 1){ //se for o primeiro simplex
                bestNode = nos[k];
                break;
            }

            if(nos[k]->sol >= bestNode->sol)
                bestNode = nos[k];
            
            
            if(bestNode != nos[k]) //if the bestNode doesnt point to this node
                delete nos[k];
                
            nos.erase(nos.begin() + k);
            copia.erase(copia.begin() + k);

            if(copia.size() == 0) //all nodes removed / done
                break;

            k--;

            continue;
        }

        auxLong = nos[k]->sol;
        if(checarLimite(auxLong, bestNode)){

            if(print) //printar nos
                puts("     parada por limite"); 


            if(bestNode != nos[k]) //if the bestNode doesnt point to this node
                delete nos[k];
                
            nos.erase(nos.begin() + k);
            copia.erase(copia.begin() + k);

            if(copia.size() == 0) //all nodes removed / done
                break;

            k--;

            continue;
        }

        auxLong = auxVar[indexCandidate];
        
        if(select)
        {
            nos.push_back(new Node(false, ceil(auxLong), indexCandidate)); //No maior igual que
            nos.push_back(new Node(true, floor(auxLong), indexCandidate)); //No menor igual que
        }
        else
        {
            nos.push_back(new Node(true, floor(auxLong), indexCandidate)); //No menor igual que
            nos.push_back(new Node(false, ceil(auxLong), indexCandidate)); //No maior igual que
        }

        nos.erase(nos.begin()+k);

        copia.push_back(auxS);
        copia.push_back(auxS);
        copia.erase(copia.begin() + k);
        
        k--;

    }
    end = clock();


    if(bestNode->done){

        puts("\n\n/---------------------------/");

        puts("\n** Melhor No encontrado ** \n");

        for(int i = 0; i < bestNode->variables.size(); i++){

            //printf("Variavel X%d : %.12lf\n", i+1, bestNode->variables[i]);
            
            std::cout << "Variavel X" << i+1 << " : " << bestNode->variables[i] << std::endl;

        }
        if(bestNode->min)
            std::cout << "\nSolucao : " << -bestNode->sol << std::endl;
        else
            std::cout << "\nSolucao : " << bestNode->sol << std::endl;

        puts("\n** Melhor No encontrado **\n");

        printf("Numero de Nos: %d\n", numNos);
        std::cout << "Tempo Percorrido: " << (double)(end - init) / divisao << std::endl;

        puts("\n/---------------------------/");

    }

    printf("      BUSCA POR LARGURA\n\n");

}

void printarNo(Node* &node, int numNos){

    if(numNos > 1){

        if(node->type)
            printf("\n\tX%d <= %d", node->indexVal + 1, node->val);
        else
            printf("\n\tX%d >= %d", node->indexVal + 1, node->val);
    }

    puts("");

    for(int i = 0; i < node->variables.size(); i++){
        
        std::cout << "\tX" << i+1 << " = " << node->variables[i] << std::endl;

    }
    if(node->min)
        std::cout << "\tZ = " << -node->sol << std::endl;
    else
        std::cout << "\tZ = " << node->sol << std::endl;

    return;
}

bool compare(double* num, double floor, double ceil){

    double cmp1 = *num - floor;
    double cmp2 = ceil - *num; 

    if((cmp1 < 0.000000001 && cmp1 > -0.000000001) || (cmp2 < 0.000000001 && cmp2 > -0.000000001))
        return true;

    return false;
        
}

void adicionarVariaveisObj(Simplex& copia, std::vector <double>& Var){

    int indexValor = 0;
    bool checarVO = false;

    Var.clear();

        for(int n = 0; n < copia.VO.size(); n++){ // Adicionar as variaveis ao No

            for(int i = 1; i < copia.linhas; i++){

                if(copia.VO[n] == copia.VB[i]){

                    indexValor = i;
                    checarVO = true;
                    break;
                }

            }

            if(checarVO)
                Var.push_back(copia.valores[indexValor]);
            else
                Var.push_back(0);

            checarVO = false;
        }


}

void atualizarPL(Simplex& copia, std::vector<Node*>& nos, int k){
    
    std::vector<double> a;

    for(int i = 0 ; i < matCoefs[0].size(); i++){
        a.push_back(0.0);
    }
    matCoefs.push_back(a); // evitar usar resize

    int linhas = matCoefs.size();

    for(int i = 0; i < linhas; i++){
        matCoefs[i].push_back(0.0);
    }

    int colunas = matCoefs[0].size();

    val.push_back(nos[k]->val);

    if(nos[k]->type){ // se eh uma restricao <=


        matCoefs[linhas - 1][nos[k]->indexVal] = 1.0;
        matCoefs[linhas - 1][colunas - 1] = 1.0;


    }else{ // se eh uma restricao >=

        matCoefs[linhas - 1][nos[k]->indexVal] = 1.0;
        matCoefs[linhas - 1][colunas - 1] = -1.0;
        for(int i = 0; i < linhas; i++){
            matCoefs[i].push_back(0.0);
        }
        colunas++;
        matCoefs[linhas - 1][colunas - 1] = 1.0;

        for(int j = 0; j < colunas-1; j++){
            
            /*if(matCoefs[linhas-1][j] == 0.0000000000)
                continue;*/

            if(matCoefs[linhas-1][j] < 0.000000000001 && matCoefs[linhas-1][j] > -0.000000000001)
                continue;
            
            matCoefs[0][j] = matCoefs[0][j] - (double)(m * matCoefs[linhas-1][j]);

        }

        val[0] = val[0] - (double)(m * val[linhas-1]); 

        copia.VA.push_back(colunas);
    }

    atualizarPL2(copia);

    return;
}

void atualizarPL2(Simplex& copia){

    copia.VB.clear();
    copia.VT.clear();
    copia.coefs = matCoefs;
    copia.valores = val;
    copia.linhas = matCoefs.size();
    copia.colunas = matCoefs[0].size();
    copia.degenerada = false;
    copia.ilimitada = false;
    copia.naoViavel = false;

    int numZeros = 0;
    int numUns = 0;
    
    copia.VB.push_back(0); // variavel basica Z

    for(int j = 0; j < copia.colunas; j++){

        for(int i = 0; i < copia.linhas; i++){

            if(copia.coefs[i][j] == 0.0)
                numZeros++;
            if(copia.coefs[i][j] == 1.0)
                numUns++;
        }

        if(numZeros == copia.linhas - 1 && numUns == 1)
            copia.VB.push_back(j+1); // adicionar as varaveis basicas  

        numZeros = 0;
        numUns = 0;

        copia.VT.push_back(j+1); // adicionar todas as variaveis

    }

    copia.varBase = copia.VB;
    copia.varBase.erase(copia.varBase.begin());

}

bool checarLimite(double auxLong, Node* &best){

    //se as variaveis forem fracionarias e a solucao for menor igual a melhor solucao
    if(auxLong <= best->sol)
        return true;

    return false;
}

bool checarIntegralidade(int* candidate, std::vector <double>& auxVar){

    int auxInt = 0;
    double decimal  = 0.0;

    for(int i = 0; i < auxVar.size(); i++){ //checar variaveis

        if(compare(&auxVar[i], floor(auxVar[i]), ceil(auxVar[i]))){ //its an integer
            auxInt++;
            continue;
        }
        
        if(auxVar[i] - floor(auxVar[i]) > decimal)
        {
            decimal = auxVar[i] - floor(auxVar[i]);
            *candidate = i;
        }
        //indexList.push_back(i);
    }
    
    if(auxInt == auxVar.size())
        return true;

    return false;
}

int removerNos(Simplex& copia, Node* &best, std::vector<Node*>& nos, int* k){

    int i = 0;

    for(i = *k; i >= 0; i--){


        if(!(nos[i]->done))
            break;


        if(nos[i]->type){ //se eh uma restricao <=

            matCoefs.pop_back();
            
            for(int j = 0; j < matCoefs.size(); j++){

                matCoefs[j].pop_back();
            
            }

            val.pop_back();

        }else{ //se eh uma restricao >=

            matCoefs.pop_back();
            
            for(int j = 0; j < matCoefs.size(); j++){

                matCoefs[j].pop_back();
                matCoefs[j].pop_back();
            
            }

            matCoefs[0][nos[i]->indexVal] = matCoefs[0][nos[i]->indexVal] + m;

            val[0] = val[0] + (double)(m * val[val.size()-1]);
            val.pop_back();

            copia.VA.pop_back();
        }

        if(best != nos[i]) //if the bestNode doesnt point to this node
            delete nos[i];

        nos.pop_back();
        *k = *k - 1;

    }

    if(*k < 0)
        return -1;

    return 1;
}
