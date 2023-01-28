  #include <stdio.h>
  #include <stdlib.h>
  #include <stdbool.h>
  #include <string.h>

  // Structure pour représenter un état de l'AFND.
  typedef struct {
    int id;         // ID de l'état.
    bool is_final;  // Indique si l'état est final ou non.
  } State;

  typedef struct {
    int num_states;   // Nombre d'états dans l'automate.
    State *states;    // Tableau des états de l'automate.
    int num_symbols;  // Nombre de symboles de l'alphabet.
    char *symbols;    // Tableau des symboles de l'alphabet.
    int **transitions; // Matrice de transitions.
                        // transitions[i][j] contient l'ID de l'état cible
                        // pour la transition depuis l'état i avec le symbole j.
  } Automaton;
  typedef struct {
    int size;
    int *elements;
  } Set;





  // Crée un ensemble vide.
  Set *create_set(int size) {
    Set *set = malloc(sizeof(Set));
    set->size = 0;
    set->elements = malloc(sizeof(int) * size);
    return set;
  }

  // Retourne l'index du symbole dans l'alphabet de l'automate, ou -1 s'il n'appartient pas à l'alphabet.
  int get_symbol_index(Automaton automaton, char symbol) {
    for (int i = 0; i < automaton.num_symbols; i++) {
      if (automaton.symbols[i] == symbol) {
        return i;
      }
    }
    return -1;
  }

  // Vérifie si un élément est dans un ensemble.

  bool is_in_set(Set *set, int element) {
    for (int i = 0; i < set->size; i++) {
      if (set->elements[i] == element) {
        return true;
      }
    }
    return false;
  }
  // Ajoute un élément à un ensemble.
  void add_to_set(Set *set, int element) {
    // Vérifie si l'élément est déjà dans l'ensemble.
    if (is_in_set(set, element)) {
      return;
    }

    // Ajoute l'élément à l'ensemble.
    set->elements[set->size] = element;
    set->size++;
  }

  // Libère la mémoire allouée pour un ensemble.
  void destroy_set(Set *set) {
    free(set->elements);
    free(set);
  }
  // Vérifie si deux ensembles sont égaux.
  bool sets_are_equal(Set *set1, Set *set2) {
    if (set1->size != set2->size) {
      return false;
    }
    for (int i = 0; i < set1->size; i++) {
      if (!is_in_set(set2, set1->elements[i])) {
        return false;
      }
    }
    return true;
  }
  void free_set(Set *set) {
    free(set->elements);
    free(set);
  }

  // Fonction pour lire un AFND à partir d'un fichier.
  void print_automaton(Automaton automaton) {
    printf("Nombre d'états : %d\n", automaton.num_states);
    printf("États accepteurs : ");
    for (int i = 0; i < automaton.num_states; i++) {
      if (automaton.states[i].is_final) {
        printf("%d ", automaton.states[i].id);
      }
    }
    printf("\n");
    printf("Alphabet : ");
    for (int i = 0; i < automaton.num_symbols; i++) {
      printf("%c ", automaton.symbols[i]);
    }
    printf("\n");
    printf("Transitions :\n");
    for (int i = 0; i < automaton.num_states; i++) {
      for (int j = 0; j < automaton.num_symbols; j++) {
        printf("%d %c %d\n", i, automaton.symbols[j], automaton.transitions[i][j]);
      }
    }
  }

  void destroy_automaton(Automaton automaton) {
    free(automaton.states);
    free(automaton.symbols);
    for (int i = 0; i < automaton.num_states; i++) {
      free(automaton.transitions[i]);
    }
    free(automaton.transitions);
  }

  Automaton read_automaton(char *filename) {
    Automaton automaton;

    // Ouvre le fichier en lecture.
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
      printf("Erreur lors de l'ouverture du fichier %s\n", filename);
      exit(EXIT_FAILURE);
    }

    // Lit le nombre d'états.
    fscanf(fp, "%d", &automaton.num_states);

    // Alloue de la mémoire pour le tableau des états.
    automaton.states = malloc(sizeof(State) * automaton.num_states);

    // Lit les états accepteurs.
    int num_accepting_states;
    fscanf(fp, "%d", &num_accepting_states);
    for (int i = 0; i < num_accepting_states; i++) {
      int accepting_state;
      fscanf(fp, "%d", &accepting_state);
      automaton.states[accepting_state].is_final = true;
    }

    // Lit le nombre de symboles de l'alphabet.
    automaton.num_symbols = 0;

    // Alloue de la mémoire pour le tableau des symboles de l'alphabet.
    // La taille maximale de l'alphabet est de 95 caractères ASCII imprimables.
    automaton.symbols = malloc(sizeof(char) * 95);

    // Alloue de la mémoire pour la matrice de transitions.
    automaton.transitions = malloc(sizeof(int*) * automaton.num_states);
    for (int i = 0; i < automaton.num_states; i++) {
      automaton.transitions[i] = malloc(sizeof(int) * 95);
    }

  
    // Lit les transitions.
    int state , next_state;
    char symbol, line[100];

    
    fgets(line, sizeof(line), fp);
    while (fscanf(fp, "%d %c %d", &state, &symbol, &next_state) == 3) {
      printf("state: %d \n, symbol: %c \n, next_state: %d\n", state, symbol, next_state);
      // Ajoute le symbole à l'alphabet s'il n'y est pas déjà.
      int symbol_index = -1;
      for (int i = 0; i < automaton.num_symbols; i++) {
        printf("add symbol: %c\n", symbol);
        if (automaton.symbols[i] == symbol) {
          symbol_index = i;
          break;
        }
      }
      if (symbol_index == -1) {
        //printf("add symbol :%d %c %d\n", state, symbol, next_state);
        automaton.symbols[automaton.num_symbols] = symbol;
        symbol_index = automaton.num_symbols;
        automaton.num_symbols++;
      }

      // Ajoute la transition à la matrice.
      automaton.transitions[state][symbol_index] = next_state;
    }

    // Ferme le fichier.
    fclose(fp);
  print_automaton(automaton);


  return automaton;
  }

  Automaton determinize(Automaton automaton) {
    // Crée l'AFD résultant.
    Automaton determined_automaton;
    determined_automaton.num_states = 0;
    determined_automaton.states = malloc(sizeof(State));
    determined_automaton.num_symbols = automaton.num_symbols;
    determined_automaton.symbols = automaton.symbols;
    determined_automaton.transitions = malloc(sizeof(int*));
    determined_automaton.transitions[0] = malloc(sizeof(int) * automaton.num_symbols);

    // Crée une liste de sets d'états, où chaque set représente un état de l'AFD.
    // Le set de l'état initial de l'AFD contient l'état initial de l'AFN.
    Set **sets = malloc(sizeof(Set*));
    sets[0] = create_set(1);
    add_to_set(sets[0], 0);

    // Crée une liste d'états tels qu'ils sont rencontrés dans l'algorithme de déterminisation.
    int *determined_states = malloc(sizeof(int));
    determined_states[0] = 0;
    determined_automaton.num_states = 1;

    // Répète jusqu'à ce qu'il n'y ait plus de nouveaux états à ajouter.
    bool changed = true;
    printf(" before while changed  ... \n");
    while (changed) {
      changed = false;
      printf("num_states : %d \n", determined_automaton.num_states);
      printf("num_symbols : %d \n",determined_automaton.num_symbols);
      printf("symbols : %s \n ",determined_automaton.symbols);
      // Parcours chaque état de l'AFD.
      for (int i = 0; i < determined_automaton.num_states; i++) {
        Set *set = sets[i];

        // Parcours chaque symbole de l'alphabet.
        for (int j = 0; j < determined_automaton.num_symbols; j++) {
          // Crée un nouveau set d'états en faisant la réunion de tous les états accessibles
          // depuis l'état courant de l'AFD avec le symbole courant.
          Set *new_set = create_set(automaton.num_states);
          for (int k = 0; k < set->size; k++) {
            int state = set->elements[k];
            int next_state = automaton.transitions[state][j];
            add_to_set(new_set, next_state);
          }

          // Vérifie si le nouveau set a déjà été rencontré dans l'algorithme.
          bool found = false;
          int index = -1;
          for (int k = 0; k < determined_automaton.num_states; k++) {
            if (sets_are_equal(new_set, sets[k])) {
              found = true;
              index = k;
                break;
            }
          }

              if (found) {
        // Si le nouveau set a déjà été rencontré, ajoute simplement une transition depuis l'état courant
        // de l'AFD vers l'état correspondant dans l'AFD.
        determined_automaton.transitions[i][j] = determined_states[index];
        destroy_set(new_set);
      } else {
              // Si le nouveau set n'a pas encore été rencontré, ajoute un nouvel état dans l'AFD et une transition
              // depuis l'état courant vers ce nouvel état.
              determined_automaton.num_states++;
              determined_automaton.states = realloc(determined_automaton.states, sizeof(State) * determined_automaton.num_states);
              determined_automaton.transitions = realloc(determined_automaton.transitions, sizeof(int*) * determined_automaton.num_states);
              determined_automaton.transitions[determined_automaton.num_states - 1] = malloc(sizeof(int) * determined_automaton.num_symbols);
              sets = realloc(sets, sizeof(Set*) * determined_automaton.num_states);
              determined_states = realloc(determined_states, sizeof(int) * determined_automaton.num_states);
              determined_states[determined_automaton.num_states - 1] = determined_automaton.num_states - 1;
              sets[determined_automaton.num_states - 1] = new_set;
              determined_automaton.transitions[i][j] = determined_automaton.num_states - 1;
              determined_automaton.states[determined_automaton.num_states - 1].id = determined_automaton.num_states - 1;
              determined_automaton.states[determined_automaton.num_states - 1].is_final = false;
              for (int k = 0; k < new_set->size; k++) {
                if (automaton.states[new_set->elements[k]].is_final) {
                  determined_automaton.states[determined_automaton.num_states - 1].is_final = true;
                  break;
                }
              }
              changed = true;
            }
      }
    }
    
    }
    
  // Retourne l'AFD obtenu.

  // Détermine les états accepteurs de l'AFD.
    for (int i = 0; i < determined_automaton.num_states; i++) {
      Set *set = sets[i];
      determined_automaton.states[i].id = i;
      determined_automaton.states[i].is_final = false;
      for (int j = 0; j < set->size; j++) {
        int state = set->elements[j];
        
        if (automaton.states[state].is_final) {
          determined_automaton.states[i].is_final = true;
          break;
        }
      }
    }

  for (int i = 0; i < determined_automaton.num_states; i++) {
    destroy_set(sets[i]);
  }
  free(sets);
  free(determined_states);


  return determined_automaton;
  } 
      

  Automaton minimize(Automaton automaton) {
    // Crée l'AFD minimisé.
    int j;
    Automaton minimized_automaton;
    minimized_automaton.num_states = 0;
    minimized_automaton.states = malloc(sizeof(State));
    minimized_automaton.num_symbols = automaton.num_symbols;
    minimized_automaton.symbols = automaton.symbols;
    minimized_automaton.transitions = malloc(sizeof(int*));
    minimized_automaton.transitions[0] = malloc(sizeof(int) * automaton.num_symbols);

    // Crée une liste de sets d'états, où chaque set représente un état de l'AFD minimisé.
    // Le set de l'état initial de l'AFD minimisé contient l'état initial de l'AFD.
    Set **sets = malloc(sizeof(Set*));
    sets[0] = create_set(1);
    add_to_set(sets[0], 0);

    // Crée une liste d'états tels qu'ils sont rencontrés dans l'algorithme de minimisation.
    int *minimized_states = malloc(sizeof(int));
    minimized_states[0] = 0;
    minimized_automaton.num_states = 1;

    // Répète jusqu'à ce qu'il n'y ait plus de nouveaux états à ajouter.
    bool changed = true;
    while (changed) {
      changed = false;

      // Parcours chaque état de l'AFD minimisé.
      for (int i = 0; i < minimized_automaton.num_states; i++) {
        Set *set = sets[i];

        // Crée un set pour les états finaux et un set pour les états non finaux.
        Set *final_states = create_set(automaton.num_states);
        Set *non_final_states = create_set(automaton.num_states);
        for (j = 0; j < set->size; j++) {
          int state = set->elements[j];
          if (automaton.states[state].is_final) {
            add_to_set(final_states, state);
          } else {
            add_to_set(non_final_states, state);
          }
        }

        // Si tous les états du set sont finaux ou non finaux, ajoute simplement une transition depuis
        // l'état courant de l'AFD minimisé vers lui-même pour chaque symbole de l'alphabet.
        if (final_states->size == 0 || non_final_states->size == 0) {
          for ( j = 0; j < minimized_automaton.num_symbols; j++) {
            minimized_automaton.transitions[i][j] = i;
          } 
          }else {
            // Si tous les états du set ne sont pas tous finaux ou tous non finaux, alors il faut créer
            // deux nouveaux états dans l'AFD minimisé et ajouter des transitions depuis l'état courant
            // de l'AFD minimisé vers ces nouveaux états.
            minimized_automaton.num_states += 2;
            minimized_automaton.states = realloc(minimized_automaton.states, sizeof(State) * minimized_automaton.num_states);
            minimized_automaton.transitions = realloc(minimized_automaton.transitions, sizeof(int*) * minimized_automaton.num_states);
            minimized_automaton.transitions[minimized_automaton.num_states - 2] = malloc(sizeof(int) * minimized_automaton.num_symbols);
            minimized_automaton.transitions[minimized_automaton.num_states - 1] = malloc(sizeof(int) * minimized_automaton.num_symbols);
            minimized_states = realloc(minimized_states, sizeof(int) * minimized_automaton.num_states);
            sets = realloc(sets, sizeof(Set*) * minimized_automaton.num_states);

            // Crée un nouveau set pour chaque nouvel état et ajoute les états finaux et non finaux
            // respectivement à chaque set.
            sets[minimized_automaton.num_states - 2] = final_states;
            sets[minimized_automaton.num_states - 1] = non_final_states;
            minimized_states[minimized_automaton.num_states - 2] = minimized_automaton.num_states - 2;
            minimized_states[minimized_automaton.num_states - 1] = minimized_automaton.num_states - 1;

            // Ajoute une transition depuis l'état courant de l'AFD minimisé vers les nouveaux états pour le symbole courant.
            minimized_automaton.transitions[i][j] = minimized_automaton.num_states - 2;
            minimized_automaton.transitions[minimized_automaton.num_states - 2][j] = minimized_automaton.num_states - 2;
            minimized_automaton.transitions[minimized_automaton.num_states - 1][j] = minimized_automaton.num_states - 1;
            changed = true;
          }
        }
      }
    

    // Marque les nouveaux états finaux de l'AFD minimisé.
    for (int i = 0; i < minimized_automaton.num_states; i++) {
      for (int i = 0; i < minimized_automaton.num_states; i++) {
          Set *set = sets[i];
          bool is_final = true;
          for ( j = 0; j < set->size; j++) {
          if (!automaton.states[set->elements[j]].is_final) {
  is_final = false;
  break;
  }
  }
  minimized_automaton.states[i].is_final = is_final;
  }
  }



  // Libère la mémoire allouée pour les sets.
  for (int i = 0; i < minimized_automaton.num_states; i++) {
  free_set(sets[i]);
  }
  free(sets);
  free(minimized_states);

  return minimized_automaton;

  }

  bool run(Automaton automaton, char *input) {
    int state = 0;
    for (int i = 0; input[i] != '\0'; i++) {
      char symbol = input[i];
      int symbol_index = get_symbol_index(automaton, symbol);
      if (symbol_index == -1) {
        return false;
      }
      state = automaton.transitions[state][symbol_index];
    }
    return automaton.states[state].is_final;
  }
  void print_results(Automaton automaton, char **inputs, int num_inputs) {
    printf(" PRINT RESULT \n");
    for (int i = 0; i < num_inputs; i++) {
      char *input = inputs[i];
      bool result = run(automaton, input);
      printf("%s: %s\n", input, result ? "ACCEPTED  OK" : "REJECTED  KO");
    }
    free(inputs);
  }

  int main(int argc, char *argv[]) {
    // Vérifie qu'un fichier d'entrée et au moins une chaîne de caractères ont été fournis en paramètres.
    if (argc < 3) {
      printf("Usage: %s <fichier d'entrée> <chaîne 1> [<chaîne 2>...]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  
   printf("\n***********LECTURE D AUTOMATE *********** \n"); 

    // Lit l'automate fini non-déterministe à partir du fichier d'entrée.
    Automaton automaton = read_automaton(argv[1]);
   
    printf("\n***********DETERMINISATION *********** \n"); 
    // Détermine l'automate.
    Automaton determined_automaton = determinize(automaton);
      
      
    printf("\n***********MINIMISATION *********** \n"); 


    // Minimise l'AFD obtenu.
    Automaton minimized_automaton = minimize(determined_automaton);
    
    // Exécute l'AFD minimisé sur chaque chaîne de caractères passée en paramètre.
    bool results[argc - 2];
    for (int i = 2; i < argc; i++) {
      results[i - 2] = run(minimized_automaton, argv[i]);
    }

    // Affiche les résultats de l'exécution de l'AFN et de l'AFD minimisé sur les chaînes de caractères,
    // ainsi que les informations sur l'AFD déterminisé et minimisé.
    printf("\n*********** RESULT *********** \n"); 

    print_results(automaton, argv + 2, argc - 2);
    destroy_automaton(automaton);

    return 0;
  }

