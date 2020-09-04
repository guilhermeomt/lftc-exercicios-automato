/*=====================================
UENF-CCT-LCMAT-Ciência da Computação
Aluno: Guilherme Oliveira M. Tavares
Data: 22/08/2020
Disciplina: Linguagens Formais e Teoria da Computação
Professor: Annabell Del Real Tamariz
Lista 2 - Exercício 4
=======================================*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <utility>
#include <algorithm>

using namespace std;

class DFA {
public:
    int estadoInicial;
    vector<int> aceitacao;
    vector<char> alfabeto;
    int numeroEstados;
    int **transicoes;

    DFA() {
    }

    DFA(fstream &arq) {
        string linha;

        // Obter o estado inicial na 1a linha do arquivo
        getline(arq, linha);
        int pos = linha.find(':');
        string estadoInicialStr = linha.substr(pos + 1);
        this->estadoInicial = stoi(estadoInicialStr);

        // Obter o estados de aceitacao na 2a linha
        getline(arq, linha);
        pos = linha.find(':');
        string aceitacaoStr = linha.substr(pos + 2);

        for (char estadoStr : aceitacaoStr) {
            if (estadoStr == ',' || estadoStr == '}')
                continue;
            else {
                int estado = (int) estadoStr - 48;
                this->aceitacao.push_back(estado);
            }
        }

        // Obter o número de estados na 3a linha
        getline(arq, linha);
        pos = linha.find(':');
        string numeroEstadosStr = linha.substr(pos + 1);
        this->numeroEstados = stoi(numeroEstadosStr);

        // Linha do inicio da tabela
        getline(arq, linha);

        // Linha do alfabeto
        getline(arq, linha);
        linha.erase(remove(linha.begin(), linha.end(), ' '), linha.end()); /* faz o trim
                                                                                        (remove espacos em branco) */
        for (char simbolo : linha)
            this->alfabeto.push_back(simbolo);

        // Obter as transicoes
        this->transicoes = new int *[numeroEstados];
        for (int i = 0; i < numeroEstados; i++) {
            this->transicoes[i] = new int[this->alfabeto.size()]; // entender isso 😜
        }
        getline(arq, linha);
        while (linha.compare("FIM")) { // Otimizar pelo numero de estados
            linha.erase(remove(linha.begin(), linha.end(), ' '), linha.end());

            int estado = (int) linha[0] - 48;
            string tscStr = linha.substr(1);
            int j = 0; // Coluna da matriz (tamanho do alfabeto)
            for (char transicao : tscStr) {
                this->transicoes[estado][j] = (int) transicao - 48;
                j++;
            }

            getline(arq, linha);
        }
    }

    void executar(string entrada) {
        int posicao = 0;
        int estadoAtual = this->estadoInicial;

        while (posicao < entrada.size()) {
            this->imprimeCI(entrada, estadoAtual, posicao);
            int elemento = (int) entrada.at(posicao) - 48;
            estadoAtual = this->transicoes[estadoAtual][elemento];
            posicao++;
        }
        this->imprimeCI(entrada, estadoAtual, posicao);

        bool aceita = false;
        for (int estado : this->aceitacao) {
            if (estadoAtual == estado)
                aceita = true;
        }

        if (aceita)
            cout << "Aceita" << endl;
        else
            cout << "Rejeita" << endl;

    }


   /* DFA minimizarCerto() {
        DFA min; // DFA minimizado
        int **tabela_min; // tabela de minimizacao


        // Alocar espaco de memoria para a tabela min
        tabela_min = new int

    } */


    DFA minimizar() {
        DFA min;
        min.estadoInicial = this->estadoInicial;

        int **tabela_min;
        int tamanho = this->numeroEstados - 1;

        // Alocando memória para a tabela de minimização
        tabela_min = new int *[tamanho];
        int i = 0;
        for (int j = 1; j <= tamanho; j++) {
            tabela_min[i] = new int[j];
            i++;
        }

        // Inicializando a tabela com 0`s (tabela vazia)
        for (int i = 1; i <= tamanho; i++) {
            for (int j = 0; j < i; j++) {
                tabela_min[i - 1][j] = 0;
            }
        }

        // "Assinala" as celulas na tabela dos estados de aceitacao (com o valor de 1)
        queue<pair<int, int>> proximosEstados;
        for (int estado : this->aceitacao) {
            if (estado == 0) {  // verifica se é o extremo inferior
                for (int i = 0; i < tamanho; i++) {
                    if (tabela_min[i][0] == 1) {
                        tabela_min[i][0] = 0;
                        continue;
                    }
                    tabela_min[i][0] = 1;
                }
            } else if (estado == tamanho) { // extremo superior
                for (int j = 0; j < estado; j++) {
                    if (tabela_min[estado - 1][j] == 1) {
                        tabela_min[estado - 1][j] = 0;
                        continue;
                    }
                    tabela_min[estado - 1][j] = 1;
                }
            } else { // estados intermediarios
                // percorre as colunas
                for (int j = 0; j < estado; j++) {
                    if (tabela_min[estado - 1][j] == 1) {
                        tabela_min[estado - 1][j] = 0;
                        continue;
                    }
                    tabela_min[estado - 1][j] = 1;
                }
                // percorre as linhas
                for (int i = estado; i < tamanho; i++) {
                    if (tabela_min[i][estado] == 1) {
                        tabela_min[i][estado] = 0;
                        continue;
                    }
                    tabela_min[i][estado] = 1;
                }

            }
        }

       // Coloca numa fila os estados marcados
       for (int i = 1; i <= tamanho; i++) {
           for (int j = 0; j < i; j++) {
               if(tabela_min[i - 1][j] == 1) {
                   proximosEstados.push({i, j});
                //   cout << i << " - " << j << endl;
               }
           }
       }

        //1. assinalar os pares encontrados e adiciona-los na fila
        //2. verificar se a tabela esta toda preenchida
        vector<pair<int, int>> novosPares;
        while(!proximosEstados.empty()) {
             pair<int, int> par = proximosEstados.front();
             novosPares = testaPar(par);

            int i = par.first;
            int j = par.second;
             if(i > j) {
                 tabela_min[par.first - 1][par.second] = 2; // Destacar o estado na tabela
             } else if(par.first == 0) {
                 tabela_min[par.second - 1][0] = 2;
             } else {
                 tabela_min[par.second - 1][par.first] = 2;
             }

            // Marca os novos pares encontrados
             for (pair<int, int> par : novosPares) {
                 int i = par.first;
                 int j = par.second;

                 if(i > j && tabela_min[i - 1][j] == 0) { // Problema: zero na esquerda 🤣
                     proximosEstados.push(par); // Adiciona os novos pares na fila
                     tabela_min[i - 1][j] = 1;
                 } else if(j > 0 && tabela_min[j - 1][i] == 0) {
                     proximosEstados.push(par);
                     tabela_min[j - 1][i] = 1;
                 }
             }

            this->imprimeTabelaMin(tabela_min, tamanho);
            cout << endl;
            proximosEstados.pop();
        }


        return min;
    }

    void imprimeTabela() {
        cout << '\t';
        for (char simbolo : this->alfabeto) {
            cout << simbolo << '\t';
        }
        cout << endl;
        for (int i = 0; i < this->numeroEstados; i++) {
            cout << 'q' << i << '\t';
            for (int j = 0; j < this->alfabeto.size(); j++) {
                cout << this->transicoes[i][j];
                cout << '\t';
            }
            cout << endl;
        }

        cout << endl;
    }

private:

    vector<pair<int, int>> testaPar(pair<int, int> par) {
        vector<int> vect1;
        vector<int> vect2;
        vector<pair<int, int>> novosPares;
        cout << par.first << " - " << par.second << endl;

        for (int j = 0; j < this->alfabeto.size(); j++) {
            for (int i = 0; i < this->numeroEstados; i++) {
                if (this->transicoes[i][j] == par.first) {
                    vect1.push_back(i);
                }

                if (this->transicoes[i][j] == par.second) {
                    vect2.push_back(i);
                }
            }
            for (int el1 : vect1) {
                for (int el2 : vect2) {
                    cout << "novo par:" << el1 << " - " << el2 << endl;
                    novosPares.push_back({el1, el2});
                }
            }

            vect1.clear();
            vect2.clear();
        }

        return novosPares;
    }

    void imprimeTabelaMin(int **tabela, int tamanho) {
        for (int i = 1; i <= tamanho; i++) {
            cout << i << '\t';
            for (int j = 0; j < i; j++) {
                cout << tabela[i - 1][j];
                cout << '\t';
            }
            cout << endl;
        }
        cout << endl << '\t';
        for (int i = 0; i < tamanho; i++) {
            cout << i << '\t';
        }
    }

    void imprimeCI(string entrada, int estado, int posicao) {
        cout << entrada.substr(0, posicao);
        cout << "[q" << estado << "]";
        cout << entrada.substr(posicao) << endl;
    }
};


int main(int argc, char *argv[]) {
    fstream arqMaquina;

    if (argv[1]) {
        string arquivoStr(argv[1]);
        arqMaquina.open(arquivoStr, ios_base::in);

        if (!arqMaquina) {
            cout << "Arquivo nao encontrado...";
            exit(1);
        }
    } else {
        cout << "Informe o nome do arquivo de texto da maquina de estados (com a extensao .txt)";
        exit(1);
    }

    DFA automato(arqMaquina);
    arqMaquina.close();
    automato.imprimeTabela();
    string entrada;
    cout << "Digite uma entrada para o automato acima: ";
    cout << endl;
    // cin >> entrada;
    automato.minimizar();
    //automato.executar(entrada);
    return 0;
}

