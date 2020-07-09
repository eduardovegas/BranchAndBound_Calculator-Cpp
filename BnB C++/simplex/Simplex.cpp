#include "Simplex.h"

Simplex::Simplex(Data dado){

    linhaPivo = 0;
    colunaPivo = 0;
    ilimitada = false;
    degenerada = false;
    naoViavel = false;

    min = dado.Min;

    this->VO = dado.VO;
    this->VA = dado.VA;

    coefs = dado.auxCoefs;
    valores = dado.auxValores;

    linhas = dado.auxCoefs.size();
    colunas = dado.auxCoefs[0].size();


    int numZeros = 0;
    int numUns = 0;
    
    VB.push_back(0); // variavel basica Z

    for(int j = 0; j < colunas; j++){

        for(int i = 0; i < linhas; i++){

            if(coefs[i][j] == 0.0)
                numZeros++;
            if(coefs[i][j] == 1.0)
                numUns++;
        }

        if(numZeros == linhas - 1 && numUns == 1){
            
            VB.push_back(j+1); // adicionar as varaveis basicas  
        }else{

            VNB.push_back(j+1); // adicionar as variaveis nao basicas
        }

        numZeros = 0;
        numUns = 0;

        VT.push_back(j+1); // adicionar todas as variaveis
    }

    varBase = VB;
    varBase.erase(varBase.begin());


}

Simplex::Simplex(bool Min, std::vector <std::vector<double>> cfs, std::vector <double> vls, std::vector<int> vo, std::vector<int> va)
{

    linhas = cfs.size();
    colunas = cfs[0].size();
    linhaPivo = 0;
    colunaPivo = 0;

    min = Min;
    ilimitada = false;
    degenerada = false;
    naoViavel = false;

    coefs = cfs;
    valores = vls;

    VO = vo;
    VA = va;

    int numZeros = 0;
    int numUns = 0;
    
    VB.push_back(0); // variavel basica Z

    for(int j = 0; j < colunas; j++){

        for(int i = 0; i < linhas; i++){

            if(coefs[i][j] == 0.0)
                numZeros++;
            if(coefs[i][j] == 1.0)
                numUns++;
        }

        if(numZeros == linhas - 1 && numUns == 1){
            
            VB.push_back(j+1); // adicionar as varaveis basicas  
        }else{

            VNB.push_back(j+1); // adicionar as variaveis nao basicas
        }

        numZeros = 0;
        numUns = 0;

        VT.push_back(j+1); // adicionar todas as variaveis
    }

    varBase = VB;
    varBase.erase(varBase.begin());

}

void Simplex::calcularSimplex(){
    
    /*int n = 1;

    printf("Variaveis artificiais: ");

    for(int i = 0; i < VA.size(); i++){
        
        printf("X%d ", VA[i]);

    }
    puts("\n");*/
    
    while(!checarOtimo()){
        
        acharColunaPivo();
        acharLinhaPivo();

        if(ilimitada){
            break;
        }

        atualizarTabela();

        /*std::cout << "\nITERACAO " << n << std::endl;
        printarTabela();
        std::cout << "Variavel que entrou: X" << VB[linhaPivo] << std::endl;
        std::cout << "Variavel que saiu: X" << auxPivo << std::endl;
        //getchar();

        n++;*/

    }

    for(int i = 1; i < VB.size(); i++){

        if(valores[i] == 0.0)
            degenerada = true;

        if(contains(VA, VB[i])){ // Checar se existe uma variavel artificial na base
            
            //puts("\n");
            //printarTabela();
            //printf("existe\n");
            naoViavel = true;
            break;
            
        }
    }

    for(int i = 1; i < valores.size(); i++){

        comparar(&valores[i], floor(valores[i]), ceil(valores[i]));

    }

    //printarResultado();

    return;
}

bool Simplex::checarOtimo(){

    bool Otimo = true;

    for(int i = 0; i < 1; i++){ // checar na primeira linha, da função Objetivo

        for(int j = 0; j < colunas; j++){

            if(coefs[i][j] < 0.0){

                Otimo = false;
                break;
            }
        }
    }

    return Otimo;

}

void Simplex::acharColunaPivo(){

    int best_j = 0; // o menor escolhido, em caso de empate, será o de menor índice
    double menor = coefs[0][0];

    for(int i = 0; i < 1; i++){ // checar na primeira linha, da função Objetivo

        for(int j = 1; j < colunas; j++){

            if(coefs[i][j] < menor){

                menor = coefs[i][j];
                best_j = j;
            }
        }
    }

    colunaPivo = best_j;

    return;

}

void Simplex::acharLinhaPivo(){

    int best_i = 0;
    int numNegativos = 0;
    double menor = 9999999999999.9;
    double resultado;

    for(int i = 1; i < linhas; i++){

        if(coefs[i][colunaPivo] < 0.000000000001){ // coeficiente negativo ou 0 não deve ser considerado
            
            numNegativos++;
            continue;
        }

        resultado = valores[i] / coefs[i][colunaPivo];

        if(resultado < menor){

            menor = resultado;
            best_i = i;
            continue;

        }

        if(resultado == menor){
            
            // em caso de empate na saida: escolher o de menor indice das VB para evitar loop || escolher variavel artificial no empate
            if(VB[i] < VB[best_i] || contains(VA, VB[i])){
                
                if(contains(VA, VB[best_i]) && VB[i] > VB[best_i]) // se o best e o i forem VA e o indice i nao for menor que o best
                    continue;

                if(contains(VA, VB[best_i]) && !contains(VA, VB[i])) // se o best for VA e o i nao for
                    continue;

                menor = resultado;
                best_i = i;
                
            }

        }
        
    }

    if(numNegativos == linhas-1)
        ilimitada = true;

    linhaPivo = best_i;

    return;

}

void Simplex::atualizarTabela(){

    //std::cout << "Variavel que entrou: X" << VT[colunaPivo] << std::endl;
    //std::cout << "Variavel que saiu: X" << VB[linhaPivo] << std::endl;

    double elementoPivo = coefs[linhaPivo][colunaPivo];

    for(int j = 0; j < colunas; j++){ // colocar o elemento pivo = 1, da coluna pivo

        coefs[linhaPivo][j] = coefs[linhaPivo][j] / elementoPivo;
    }
    valores[linhaPivo] = valores[linhaPivo] / elementoPivo;


    double valorAux = valores[linhaPivo];

    for(int i = 0; i < linhas; i++){ // zerar todos os outros elementos da coluna pivo
        
        if(coefs[i][colunaPivo] == 0)
            continue;

        if(i == linhaPivo)
            continue;

        double multiplicar = -(coefs[i][colunaPivo]); // fator de multiplicidade

        for(int j = 0; j < colunas; j++){

            coefs[i][j] = (double)(multiplicar*coefs[linhaPivo][j]) + coefs[i][j];
        }
        valores[i] = multiplicar*valorAux + valores[i];

    }

    auxPivo = VB[linhaPivo];
    VB[linhaPivo] = VT[colunaPivo];

    for(int i = 0; i < VNB.size(); i++){

        if(VNB[i] == VT[colunaPivo]){

            VNB.erase(VNB.begin() + i);
            VNB.insert(VNB.begin() + i, auxPivo);
            break;

        }

    }


}

void Simplex::printarTabela(){

    puts("");

    for(int i = 0; i < linhas; i++){

        std::cout << "X" << VB[i] << " | ";
        
        for(int j = 0; j < colunas; j++){
            
            printf("%.2lf  ", (double)coefs[i][j]);
            //std::cout << coefs[i][j] << " ";
        }

        std::cout << " | " << valores[i] << std::endl;

    }

    puts("");

    return;
}

void Simplex::printarResultado(){


    std::cout << "\n/============ RESULTADO FINAL ============/\n" << std::endl;

    if(ilimitada){

        std::cout << "\nErro : Solucao Ilimitada\n" << std::endl;
        return;

    }

    if(naoViavel){

        std::cout << "\nErro : Solucao nao-viavel\n" << std::endl;
        return;

    }

    if(degenerada){

        std::cout << "\n(Solucao Degenerada)" << std::endl;

    }

    int indexValor = 0;
    bool checarVO = false;

    for(int n = 0; n < VO.size(); n++){ // Mostrar o valor das variaveis da função objetivo

        for(int i = 1; i < linhas; i++){

            if(VO[n] == VB[i]){

                indexValor = i;
                checarVO = true;
                break;
            }

        }

        if(checarVO)
            std::cout << "Variavel X" << VO[n] << " : " << valores[indexValor] << std::endl;
        else
            std::cout << "Variavel X" << VO[n] << " : 0" << std::endl;

        checarVO = false;

    }

    if(min)
        std::cout << "\n\nValor minimo da funcao objetivo : "<< -valores[0] << std::endl;
    else
        std::cout << "\n\nValor maximo da funcao objetivo : "<< valores[0] << std::endl;


    rhsAnalysis();

    std::cout << "\n/=========================================/\n" << std::endl;


    return;
}

void Simplex::rhsAnalysis(){

    int RHS = valores.size()-1;
    bool infMax = true;
    bool infMin = true;
    double range[2];
    double maxRange = 99999999999999999;
    double minRange = -99999999999999999;

    std::vector<std::vector<double> > Matbase;

    obterMatBase(Matbase);

    for(int j = 0; j < RHS; j++){

        for(int i = 0; i < RHS; i++){

            if(Matbase[i][j] < 0.0000000000000000001 && Matbase[i][j] > -0.0000000000000000001)
                continue;

            range[0] = Matbase[i][j]; // coeficiente multiplicando DeltaB_i
            range[1] = valores[i+1]; // valor somando na inequação

            if(!(range[1] < 0.0000000000000000001 && range[1] > -0.0000000000000000001))
                range[1] = -range[1]; // passar para o outro lado da inequação

            if(!(range[1] < 0.0000000000000000001 && range[1] > -0.0000000000000000001))
                range[1] = range[1] / range[0]; // dividir pelo valor que está multiplicando DeltaB_i


            if(range[0] < 0.0000000000000000001){ // coeficiente negativo, passar dividindo muda o sinal da inequação

                infMax = false;

                if(range[1] < maxRange)
                    maxRange = range[1]; // definir o limite de incremento

            }else{
                
                infMin = false;

                if(range[1] > minRange)
                    minRange = range[1]; // definir o limite de decremento

            }

        }

        if(!(minRange < 0.0000000000000000001 && minRange > -0.0000000000000000001))
            minRange = -minRange;

        std::cout << "\n\nRestricao #" << j+1;
        std::cout << "\nIncremento maximo permitido: ";
            
        if(infMax)
            printf("INFINITO\n");
        else
            printf("%.2lf\n", maxRange);
        

        std::cout << "Decremento maximo permitido: ";
        
        if(infMin)
            printf("INFINITO\n");
        else
            printf("%.2lf\n", minRange);


        infMax = true;
        infMin = true;
        maxRange = 999999999999999999;
        minRange = -999999999999999999;
    
    }

    return;
}

void Simplex::obterMatBase(std::vector<std::vector<double> >& Matbasis){

    std::vector<double> a;
    for(int j = 0; j < varBase.size(); j++){
        a.push_back(0.0);
    }

    for(int i = 0; i < varBase.size(); i++){
        Matbasis.push_back(a);
    }

    //printf("\nmatBase:\n");
    for(int i = 1; i < linhas; i++){
        
        for(int j = 0; j < varBase.size(); j++){

            Matbasis[i-1][j] = (double) coefs[i][varBase[j]-1];

            //printf("%.2lf ", (double) Matbasis[i-1][j]);
            
        }

        //puts("");

    }

}

bool Simplex::contains(std::vector<int>& array, int i){

    for(int n = 0; n < array.size(); n++){

        if(array[n] == i)
            return true;

    }

    return false;
}

bool Simplex::comparar(double* num, double floor, double ceil){

    double cmp1 = *num - floor;
    double cmp2 = ceil - *num;
    
    if(cmp1 < 0.000000001 && cmp1 > -0.000000001){

        *num = floor;
        return true;

    }

    if(cmp2 < 0.000000001 && cmp2 > -0.000000001){

        *num = ceil;
        return true;

    }
    
    return false;

}