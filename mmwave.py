import pandas as pd
import matplotlib.pyplot as plt

# 1) Lê o CSV e pula a primeira linha (cabeçalho)  
df = pd.read_csv('dados_mmwave.csv', delimiter=',', skiprows=1, header=None, names=['Tempo','Fase'])

print(df.columns)   # ['Tempo', 'Fase']
print(df.head())    # diagnóstico rápido, coloquei para ver se abriu corretamente o arquivo csv

# 2) Vetores para o gráfico
tempo = df['Tempo'].astype(str).str.strip()     
fase  = df['Fase'].astype(float)                 # valores do Sensor mmWave

# 3) Usa o índice das amostras no eixo X e mostra alguns rótulos de tempo
x = range(len(df))

plt.xlabel('Tempo')
plt.ylabel('Fase de respiração')
plt.title('Grafico mmWave')
plt.plot(x, fase)                                # gráfico de linha
plt.grid(True)

# Coloca ~10 rótulos de tempo para não poluir
if len(df) > 0:
    passo = max(len(df)//10, 1)
    idx = list(range(0, len(df), passo))
    plt.xticks(idx, tempo.iloc[idx], rotation=45, ha='right')

#saida do gráfico
plt.tight_layout()
plt.show()
