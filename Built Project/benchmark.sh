#!/usr/bin/env bash

PROGRAM="./TFG-VictorGonzalez"   # ruta a tu ejecutable
TOTAL_TIME=30
WARMUP_TIME=10
OUTPUT_FILE="average_result.txt"  # archivo donde se guardará la media

TMP_FILE=$(mktemp)

# Ejecutar el programa y capturar stdout durante 45s
timeout ${TOTAL_TIME}s $PROGRAM > "$TMP_FILE"

# Filtrar los últimos 30 segundos y calcular la media
awk -v warmup="$WARMUP_TIME" '
{
    values[NR] = $1
}
END {
    start = int(warmup)
    sum = 0
    count = 0
    for (i = start + 1; i <= NR; i++) {
        sum += values[i]
        count++
    }
    if (count > 0)
        print sum / count
    else
        print "No hay datos suficientes"
}
' "$TMP_FILE" > "$OUTPUT_FILE"

rm "$TMP_FILE"

echo "La media de los últimos 30s se ha guardado en $OUTPUT_FILE"
