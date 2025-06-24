CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g # Flags de compilation: tous les warnings, C11, symboles de debug
LDFLAGS = # Flags pour l'éditeur de liens (si besoin plus tard)

# Nom de l'exécutable final
TARGET = gestion_medicale

# Fichiers sources (.c)
# Pour l'instant, seulement ceux qui existent:
SRCS = main.c patient.c medicament.c personnel.c consultation.c csv_utils.c user_roles.c utilisateur.c

# Fichiers objets (.o) générés à partir des sources
OBJS = $(SRCS:.c=.o)

# Règle principale: pour créer l'exécutable
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Règle pour compiler les fichiers .c en .o
# $< est le nom du premier prérequis (le .c)
# $@ est le nom de la cible (le .o)
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Si les fichiers d'en-tête sont modifiés, recompiler les .o qui en dépendent.
# Cette règle est implicite avec la dépendance %.h dans la règle %.o: %.c %.h
# mais on peut la rendre plus explicite si on a des dépendances complexes.
# Pour l'instant, patient.o dépend de patient.h et gestion_medicale.h
# main.o dépend de gestion_medicale.h (qui inclut les autres .h)

# Cibles "PHONY" (cibles qui ne sont pas des noms de fichiers)
.PHONY: all clean run

# Règle pour nettoyer les fichiers compilés
clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe # .exe pour Windows

# Règle pour exécuter le programme (optionnel, pratique)
run: $(TARGET)
	./$(TARGET)

# Dépendances spécifiques (optionnel, make peut souvent les inférer)
# main.o: main.c gestion_medicale.h patient.h medicament.h personnel.h consultation.h
# patient.o: patient.c patient.h gestion_medicale.h
# medicament.o: medicament.c medicament.h gestion_medicale.h
# etc.
# Pour l'instant, la règle générique %.o: %.c %.h suffira, car nos .c incluent les .h nécessaires.
# Si gestion_medicale.h est modifié, tous les .c qui l'incluent seront recompilés.
# Si patient.h est modifié, patient.c et tous les .c qui incluent patient.h (ou gestion_medicale.h) seront recompilés.

# Pour que la règle %.o: %.c %.h fonctionne bien, on s'assure que les .h sont dans le même répertoire
# ou alors on ajoute des chemins d'inclusion avec -I<path> dans CFLAGS.
# Pour ce projet simple, on garde tout à plat pour l'instant.

# Ajout pour que make recompile si un .h est modifié.
# Cela liste tous les fichiers .h comme prérequis pour les objets.
# Ce n'est pas la manière la plus fine (recompile tout si un .h change) mais simple pour débuter.
# Une meilleure approche générerait des dépendances par fichier.
HEADERS = $(wildcard *.h)
$(OBJS): $(HEADERS)
