
# Numo 0-9 Programming Language

Numo 0-9 est un langage de programmation révolutionnaire dont la syntaxe est uniquement composée de chiffres de 0 à 9.

## Syntaxe

- **0, 1** : Code binaire (langage machine)
- **2** : Marqueur de fin de programme binaire / Boucles et itérations
- **3** : Créer une variable numérique (entiers/décimaux)
- **4** : Créer une variable de texte (chaînes de caractères)
- **5** : Créer une variable booléenne
- **6** : Exécution conditionnelle et contrôle de flux
- **7** : Opérations d'entrée/sortie
- **8** : Opérations mathématiques et calculs
- **9** : Opérations sur fichiers et persistance des données

## Utilisation

```bash
make
./main example.num
./main hello_world.num -d  # Mode debug
```

## Exemples

### Hello World
```
0100100001100101011011000110110001101111001000000010000000100000010101110110111101110010011011000110010000100001230348579
```

### Programme simple
```
0110100001100101011011000110110001101111001000003456789
```

## Compilation

Le langage est interprété par un programme C. Pour compiler :

```bash
make
```

## Fonctionnalités

- Interprétation de code binaire réel
- Gestion de variables typées (numériques, texte, booléennes)
- Opérations conditionnelles et contrôle de flux
- Boucles et itérations automatiques
- Entrées/sorties avancées
- Calculs mathématiques complexes
- Manipulation de fichiers et persistance
- Gestion de tableaux et structures de données
- Simulation de communication réseau
- Mode debug pour traçage d'exécution

## Nouvelles Fonctionnalités

### Variables Corrigées
- **3** : Variables numériques (pour les chiffres et calculs)
- **4** : Variables de texte (pour les chaînes de caractères)
- **5** : Variables booléennes (vrai/faux)

### Fonctionnalités Avancées
- **Boucles automatiques** : Le chiffre 2 peut aussi créer des boucles
- **Tableaux** : Création automatique d'arrays avec les opérations
- **Communication** : Simulation de transmission de données
- **Calculs complexes** : Opérations mathématiques étendues

## Philosophie

Numo 0-9 démontre qu'un langage de programmation peut être Turing-complet avec seulement 10 symboles, permettant théoriquement de créer n'importe quel programme avec la bonne séquence de chiffres.
