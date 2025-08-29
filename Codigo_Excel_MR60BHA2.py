# Instrucoes de como usar:
# 1 - Instale a biblioteca "pyserial", abrindo o terminal e digitando: "pip install pyserial"
# 2 - Depois que do codigo estar pronto para rodar, ligue o arduino com o codigo e o sensor carregados nele
# 3 - Rode o codigo no VScode
# 4 - Nao e necessario abrir o Serial Plotter ou o Serial Monitor
# 5 - O arquivo para Excel vai ficar salvo na pasta aonde voce selecionou na linha 19
# 6 - Para parar o codigo e salvar a coleta aperte Ctrl + C no terminal que esta saindo os dados no compilador

import serial

import csv

from datetime import datetime

porta = "COM4" # Colocar a porta que aparece quando conecta o sensor no arduino no meu caso e "COM4"

baud_rate = 9600 # Colocar o mesmo baud rate que colocou no codigo do arduino e no Serial Monitor

arquivo_csv = r"D:\Arduino_BHA2\Excel_MR60BHA2\dados_sensor_MR60BHA2_B06.csv" # Nome da pasta que os dados serao salvos

ser = serial.Serial(porta, baud_rate, timeout=1) # Conecta na porta serial

print(f"Lendo dados da porta {porta} e salvando em {arquivo_csv}...")

# Abrindo o arquivo csv
with open(arquivo_csv, mode = "w", newline = "") as file:
    writer = csv.writer(file)
    
    # Cabecalho opcional , ajustar confome os sensores que esta usando, nesse caso estou usando apenas um sensor
    # Timestamp serve para mostrar uma string vazia caso o codigo nao detecte nada por um segundo
    writer.writerow(["Timestamp", "Sensor1"])
    
    try:
        while True:
            linha = ser.readline().decode("utf-8").strip()
            
            if linha:
                # Divide pelos separadores de virgula
                valores = linha.split(",")
                
                # Adiciona timestamp
                timestamp = datetime.now().strftime("%H:%M:%S")
                
                # Escreve no CSV
                writer.writerow([timestamp] + valores)
                
                print(timestamp, valores)
    except KeyboardInterrupt:
        print("\nColeta encerrada pelo usuário.")
        ser.close()