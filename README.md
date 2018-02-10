# Trabalho 2 - Sistemas Operacionais 2017/2
Prof. Maurício Lima Pilla - http://lups.inf.ufpel.edu.br/~pilla

## Dados do aluno

Declaro que o presente trabalho contém código desenvolvido exclusivamente por mim e que não foi compartilhado de nenhuma forma com terceiros a não ser o professor da disciplina. Compreendo que qualquer violação destes princípios será punida rigorosamente de acordo com o Regimento da UFPEL.

(Preencha com seus dados)

- Nome completo: Matheus Ianzer Hertzog 
- Username do Github: Matheusih	
- Email @inf: mihertzog@inf.ufpel.edu.br

## Descrição

Neste trabalho, os alunos irão desenvolver um programa em C ou C++ para simular o funcionamento do gerenciamento de memória virtual em um sistema operacional.

Cada processo começa com o mesmo número de quadros (total de quadros dividido número de processos arredondado para baixo), mas os processos devem ter seus números de quadros recalculados proporcionalmente ao tamanho de seus _working sets_ após cada intervalo especificado na entrada.

O algoritmo para escolha de página vítima é o algoritmo de segunda chance (ou do relógio). Para cada processo, o algoritmo começa sempre pela primeira página acessada pelo processo. Novas páginas que não estavam em memória são incluídas na lista de páginas sempre antes da próxima página a ser avaliada. **Não esqueçam de manter um bit de referência para cada página**. 

Os arquivos _.c_ e _.h_ fornecidos pelo professor **não devem ser modificados**, no máximo renomeados para _.cpp_. Qualquer modificação não autorizada levará à redução da nota final. Nenhum diretório deverá ser criado no repositório. A convenção de nomes de arquivos de traços dos alunos será _t01.txt_, _t02.txt_, ... Não adicionem traços muito grandes ao repositório no Github (i.e. cuidem para não ultrapassar o total de 5 MiB para todos os traços).

**IMPORTANTE**: 

- Desabilitem o Travis na interface dele enquanto não estiverem com o trabalho pronto; 
- Habilitem o Travis para a entrega da versão final;
- Usem o Github para manter as versões intermediárias do trabalho (ou seja, não façam apenas _upload_ da versão final). Trabalhos que não usarem o Github desta forma poderão ser **desconsiderados**.


### Entradas

A entrada do simulador consistirá de:

- Número de processos
- Número total de quadros (_frames_)
- Um arquivo de traço com as páginas que são referenciadas por cada processo no formato "PID PAGE\n" (uma referência por linha)
- Intervalo para o cálculo de _working set_ (em número de referências a páginas)

Um exemplo de arquivo de traço é:

        1 1
        1 5
        2 4
        3 1

Onde o processo _1_ acessa as páginas _1_ e _5_, o processo _2_ acessa a página _4_ e o processo _3_ acessa sua página _1_. O PID começa em _0_ e termina no número de processos _N-1_ especificado na entrada.

### Saídas 

A saída do simulador conterá:

- Número de referências por processo, em ordem crescente de PID
- Número de _page faults_ por processo, em ordem crescente de PID
- Taxa de _page faults_ por processo, em ordem crescente de PID
- Tamanho médio de _working set_ de toda a simulação
- Taxa de _page faults_ de toda a simulação

As estruturas de dados e assinatura da função a ser implementada encontram-se no arquivo _memvirt.h_.

## Detalhamento do cálculo do total de frames de um processo

1. Calcule o _working set_ de cada um dos processos
2. Cada processo recebe pelo menos dois _frames_  (a execução deve retornar NULL se não houver _frames_ suficientes para esta condição)
3. O total de _frames_ que um processo recebe no máximo é proporcional ao seu _working set_ calculado pela fórmula _(uint32_t) ((float) ws/total) * (total de frames)_
    - O somatório do total de _frames_ máximos dos processos pode ser maior que o número de _frames_ do sistema (arredondamentos)
4. Caso um processo tenha menos _frames_ que seu máximo total, no próximo _page fault_ ele irá receber:
    - Um _frame_ dos livres ou
    - Um _frame_ de outro processo (buscando sempre a partir do menor PID) ou
    - Um _frame_ seu (caso todos os processos tenham número de _frames_ menor ou igual ao determinado pelos seus _working sets_)
5. Caso o processo tenha mais ou o mesmo número de _frames_ que seu máximo total, ele irá buscar uma página vítima dentre as suas

## Detalhamento do algoritmo do relógio

Cada nova página inserida na tabela de páginas de um processo vai sempre ser colocada na lista do algoritmo de vítima na última posição do _buffer_ circular, com o bit de referência ligado.

## Testes

Cada trabalho deverá fornecer exemplos de testes no arquivo *teste.c* usando a biblioteca Simplegrade. Uma função *main()* deverá chamar as funções de teste. 

## Produtos

* Implementação (*memvirt.c*) **8 pontos**
* Casos de teste documentados (*teste.c*) **até 2 pontos** (proporcional à nota da implementação)

## Cronograma

* Envio dos casos de teste: _07/12/2017_
* Envio da versão final: _14/12/2017_ 

