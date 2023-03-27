# Serie 6: projet - L'équation de la chaleur

Le but de ce projet **noté** est de procéder à une implémentation simple d'un [code stencil](https://en.wikipedia.org/wiki/Stencil_code).
Ceci dans le but de résoudre l'équation de la chaleur sur un domaine carré en deux dimensions. 
L'implémentation parallèle se fera avec MPI en C afin que vous puissiez exécuter votre code sur Baobab ou Yggdrasil.

L'algorithme décrit dans cet énoncé est admis sans démonstration.
Il n'est donc pas nécessaire de comprendre les mathématiques associées à la résolution de l'équation de la chaleur, ni de comprendre l'équation elle-même.
Ceci pour des raisons de temps et du champs convert par le cours de HPC.
Néanmoins, la "traduction" de l'approche en algorithme devra être claire pour l'étudiant.

Toutefois, les étudiants curieux peuvent se référer à: [équation de la chaleur](https://en.wikipedia.org/wiki/Heat_equation), ainsi qu'aux concepts suivants pour comprendre la description du problème:
- description du [flux](https://en.wikipedia.org/wiki/Flux),
- le [théorème de la divergence](https://en.wikipedia.org/wiki/Divergence_theorem),
- et l'[opérateur de Laplace](https://en.wikipedia.org/wiki/Laplace_operator),
- ou encore l'[équation de Laplace](https://en.wikipedia.org/wiki/Laplace%27s_equation).

Ensuite pour la résolution numérique du problème: 
- la [série de Taylor](https://en.wikipedia.org/wiki/Taylor_series),
- les [différences fines](https://en.wikipedia.org/wiki/Finite_difference).

Les articles proposés sont en anglais. 
Les articles francophones correspondant peuvent exister et devraient être accessibles via les articles anglophones, si la traduction existe.

## Simulation de la diffusion de la chaleur

Le problème que nous cherchons à résoudre est le suivant:
- on a une plaque rectangulaire,
- on ne s'intéresse pas à la température initiale de la plaque,
- on applique une source de chaleur constante sur les bords de la plaque,
- la chaleur va se propager depuis/vers les bords de la plaque,
- au bout d'un certain temps, la plaque va atteindre un état stable.

On dit que la chaleur diffuse dans le domaine depuis/vers les bords. 
La diffusion se fait du chaud vers le froid, selon les lois de la thermodynamique.
A l'état stable, il n'y a plus de propagation, donc plus de changement de température au cours du temps: la température est constante en tout point de la plaque.

Ce phénomène se décrit bien mathématiquement par une équation aux dérivées partielles, dont une solution analytique a été proposée par [Jean-Baptiste Joseph Fourier](https://en.wikipedia.org/wiki/Joseph_Fourier).
Bien que cette solution existe, nous n'allons pas chercher à résoudre analytiquement l'équation de la chaleur, mais nous allons appliquer un algorithme numérique pour simuler la diffusion de la chaleur.
En simulant ce processus suffisamment longtemps, on atteint l'état stable.

## Diffusion numérique sur un plaque rectangulaire

Tout d'abord, on ne peut pas considérer un domaine continu. 
Notre plaque sera donc composée d'un nombre de points fini.
C'est-à-dire qu'il faut considérer un domaine discret, dénoté par $`U`$.
Ceci est inhérent à l'utilisation d'un ordinateur pour représenter le domaine.
Le domaine s'apparente donc à une grille sur laquelles nous allons uniquement approximer les valeurs de chaleur (càd. la température) aux coordonnées $`(i,j)`$ de la grille $`U`$:

![dom](fig/domain.png)

Les points rouges sont ceux sur lesquels on approxime la chaleur.
Les points bleues sont les valeurs aux bords et restent constantes au court du temps.
Il n'est donc pas nécessaire de les "résoudre", étant donné qu'elles sont connues.

Il est intéressant de noter que :
- chaque point du bord peut avoir une valeur de température au cours du temps différente des autres points du bord;
- ce sont ces valeurs qui vont définir l'état stable de la  plaque. Des valeurs au bords différentes impliquent une solution différente.

Les points de grilles (dénotés $`u_{ij}`$) sont équidistants les uns des autres.
Ceci même si la figure précédente ne l'illustre pas parfaitement.
Ce qui implique qu'on peut facilement représenter ce domaine à l'aide d'un tableau en deux dimensions composé de valeurs réelles.

Une fois le domaine discrétisé et les valeurs aux bords imposées, la résolution se fait à l'aide de simples moyennes successives appliquées sur tous les points du domaine de manière synchrone.

En effet, une solution au problème consiste à calculer l'évolution de la température d'un point $`u_{ij}`$ à l'itération $`t`$, comme étant la température moyenne de ses voisins de l'itération précédente $`t-1`$.
Ce calcul s'applique du temps initial, noté $`t_0`$, jusqu'à l'itération où le système converge, noté $`t_n`$.
C'est-à-dire que les températures des tous les points $`u_{ij}`$ du domaine ne changent plus en appliquant la moyenne.

Faire une telle opération se nomme une application de stencil.
Un stencil est un pattern ou motif qui décrit quels voisins sont utilisés pour calculer la valeur d'un point $`u_{ij}`$.
Il est appliqué en même temps à tous les points de l'espace considéré. 
C'est ce qu'on appelle une application synchrone sur la grille. 

Le stencil qui nous intéresse peut être schématisé comme:

![sten](fig/stencil.png)

donc dans notre cas on utilise les quatre voisins (points blancs) pour calculer la valeur d'un point du domaine (point noir, notre $`u_{ij}`$).

Plus formellement, om calcule la moyenne des quatre voisins à chaque application du stencil au cours des itération et pour chaque point du domaine:

$` u_{i,j}^{t+1} = \frac{1}{4} \cdot (u_{i-1,j}^{t} + u_{i+1,j}^{t} + u_{i,j-1}^{t} + u_{i,j+1}^{t} )`$

où $`u_{i,j}^{t}`$ est la valeur (la chaleur) à la coordonnée $`(i,j)`$ du domaine $`U`$ à l'itération $`t`$.

Pour atteindre l'état stationnaire, on applique itérativement le stencil sur tous les points $`(i,j)`$ du domaine et on produit successivement les domaines: $`U^0`$ (état inital, à l'itération $`t_0`$), $`U^1, U^2, \ldots, U^n`$ (à l'itération $`t_n`$).
Ceci jusqu'à ce que les valeurs convergent. 
Numériquement cela implique que toutes les points points $`(i,j)`$ de $`U`$ ne changent plus suffisamment d'une itération à l'autre.

Mais pour des questions de confort, spécialement pour les mesures de performance du code, nous utiliserons un nombre d'itération fix $`n`$ à appliquer.

### Pseudo-code séquentiel des boucles temporelle et spatiale

Le pseudo-code suivant illustre l'algorithme approximant la solution de l'équation de Laplace pour un nombre d'itérations fixe `max_T`, pour un domaine discret composé de `max_I` $`\times`$ `max_J` points:

```c
U = init() // intialise U et assigne les valeurs au bords
for (int t = 0; t < max_T; t++) 
{
    for (int i = 0; i < max_I; i++)
    {
        for (int j = 0; j < max_J; j++)
        {
            U_temp[i][j] = 0.25 * (
                U[i-1][j] + U[i+1][j] + 
                U[i][j-1] + U[i][j+1]
            )
        }
    } 
    U = U_temp
}
```

Pour l'état initiale (i.e. `init()`) on peut choisir de mettre à 0 les bords du bas et de gauche, et à 1 les bords du haut et de droite.
La valeur à l'intérieur du domaine peut être de 0, mais ces valeurs importent peu:

```c
U = 0
for (int i = 0; i < max_I; i++)
{
    U[i][0] = 0.0 // première colonne
    U[i][max_J - 1] = 1.0 // dernière colonne
}

for (int j = 0; i < max_J; j++)
{
    U[0][j] = 1.0 // première rangée
    U[max_i - 1][j] = 0.0 // dernière rangée
}
```

Mais un code pour approximer cette équation doit fonctionner avec des conditions aux bords arbitraires.

Voici un exemple de taille $`7 \times 7`$ d'un état initial $`U^0`$:

```
1.0 1.0 1.0 1.0 1.0 1.0 1.0
0.0 0.0 0.0 0.0 0.0 0.0 1.0
0.0 0.0 0.0 0.0 0.0 0.0 1.0
0.0 0.0 0.0 0.0 0.0 0.0 1.0
0.0 0.0 0.0 0.0 0.0 0.0 1.0
0.0 0.0 0.0 0.0 0.0 0.0 1.0
0.0 0.0 0.0 0.0 0.0 0.0 0.0
```

et après application d'une itération nous obtenons $`U^1`$:

```
1.0  1.0  1.0  1.0  1.0  1.0  1.0
0.0  0.25 0.25 0.25 0.25 0.5  1.0
0.0  0.0  0.0  0.0  0.0  0.25 1.0
0.0  0.0  0.0  0.0  0.0  0.25 1.0
0.0  0.0  0.0  0.0  0.0  0.25 1.0
0.0  0.0  0.0  0.0  0.0  0.25 1.0
0.0  0.0  0.0  0.0  0.0  0.0  0.0
```

## Parallélisation du code

Pour paralléliser, on découpe simplement le domaine en bande (horizontales ou verticales) et attribue le calcul de chaque bande à un processus MPI. 
Vous aurez donc autant de rangs ou tâches MPI qu'il y aura de bandes.

Evidemment le nombre de bande doit correspondre aux ressources demandées sur la machine parallèle et les bandes doivent être de tailles égales dans la mesure du possible (selon si le nombre de tâches divise le nombre de points sur l'axe $`i`$ ou $`j`$).

Par exemple, avec trois tâches MPI à disposition (et donc autant de ressource de calcul), voici un exemple de découpage:

![pardom](fig/paradom.png)

Ensuite on applique les itérations successives.
On se rend compte qu'il y a un peu de réflexion à faire, parce que toute l'information n'est pas présente dans une bande pour calculer les moyennes de chaque point.

Pour vous aider, notez que de l'information pour les points de grille sur les bords des bandes est manquante.
Il faut donc, en plus de la bande, communiquer les voisins de ces points et les mettre à jour à chaque fin d'itération.
Ce qui va impliquer une opération de "synchronisation" des bords à la fin de chaque itération.

## Travail à réaliser

Il y a trois partie:
1. implémentation avec C et MPI d'une version parallèle de l'algorithme de résolution de l'équation de la chaleur;
2. mesures de performance de votre implémentation, avec calcul de speedup et une analyse et présentation des résultats dans un rapport;
3. présentation de votre implémentation ou résultat (au choix des enseignants) lors d'une session orale de 20 minutes.

### 1. Partie implémentation

Vous devez implémenter la résolution de l'équation de Laplace en parallèle avec MPI en C.
Votre code doit pouvoir être **compilé** et s'exécuter sur la machine **Baobab** (ou Yggdrasil).
Sans quoi votre travail ne sera pas évalué et sanctionné par la note minimale.

Votre exécutable devra être nommé `laplace.out` et prendre à la ligne de commande les trois paramètres suivants :
- nombre de lignes du domaine (dénoté ici par `N`)
- nombre de colonnes du domaine (dénoté ici par `M`)
- nombre d'itérations à appliquer

Et on considère les conditions aux bords suivantes:
- haut = 1.0
- bas = 0.0
- gauche = 0.0
- droite = 1.0

Par exemple, la commande:
```
$ laplace.out 10 12 300
```
produira un domaine composé de 120 points de grille sur lesquels on appliquera 300 itérations du stencil introduit précédemment avec les conditions aux bords décrites ci-dessus.
On note donc que les valeurs au bords sont incluses dans la taille de domaine fournie.

Après 300 itérations, la solution ressemblera à:

![dom](fig/mini_heat.png)

L'exécution doit produire un fichier text nommé `laplace.txt` là où se trouve l'exécutable. 
Evidement on ne va écrire le domaine uniquement à la fin des itérations.

Le format représente une simple matrice, avec une ligne par rangée, où chaque valeur est séparée par un espace.

Lors de l'évaluation de votre travail, les modules chargés seront ceux du module foss:
```
$ module load foss
```
qui correspond au module par défaut `foss/2022b` et charge les dépendances suivantes:
```
Currently Loaded Modules:
  1) GCCcore/12.2.0   6) XZ/5.2.7           11) libevent/2.1.12   16) OpenMPI/4.1.4    21) ScaLAPACK/2.2.0-fb
  2) zlib/1.2.12      7) libxml2/2.10.3     12) UCX/1.13.1        17) OpenBLAS/0.3.21  22) foss/2022b
  3) binutils/2.39    8) libpciaccess/0.17  13) libfabric/1.16.1  18) FlexiBLAS/3.2.1
  4) GCC/12.2.0       9) hwloc/2.8.0        14) PMIx/4.2.2        19) FFTW/3.3.10
  5) numactl/2.0.16  10) OpenSSL/1.1        15) UCC/1.1.0         20) FFTW.MPI/3.3.10
```

Vous devez aussi fournir un `Makefile` contenant les cibles suivantes:
- `clean`: qui efface tous les fichiers produit par l'operation de compilation
- `build`: qui compile votre application et produit l'exécutable `laplace.out` à la racine du projet
- `all`: qui applique un clean build (càd un clean suivit d'un build)

Vous pouvez considérer que les modules seront chargés avant l'appel à `make`.

Vous créerez dans votre dépôt un dossier `src` dans lequel vous y déposerez :
- votre code source, càd tous les fichiers nécessaires à la compilation,
- votre `Makefile`.

Votre code sera évalué sur la compilation (simple à comprendre), l'exécution, p. ex. est-ce qu'il fonctionne toujours selon les différents paramètres choisis (simple à comprendre), et sa performance.

Pour la performance, vous ne devez pas forcément produire un code qui produit un speedup idéal, mais il ne faudrait pas non plus avoir un [code en C qui s'exécute aussi lentement que du Python](https://benchmarksgame-team.pages.debian.net/benchmarksgame/fastest/python3-gcc.html).

Pensez à tester votre code:
- sur un domaine petit, et que quelques itéreations, de manière à ce que l'exécution soit rapide,
- comparez les résultats avec une exécution séquentielle, le code est relativement trivial à écrire (attention si vous [comparez des nombres en virgules flottante](https://stackoverflow.com/questions/4915462/how-should-i-do-floating-point-comparison)),
- testez votre code sur un noeud, et plusieurs noeuds sans nécessairement utiliser les noeuds en entier,
- il est possible que pour des tests multi-noeuds, `shared-cpu` soit plus disponible que `debug-cpu`, en particulier si tous les étudiants font leur tests en même temps,
- tester aussi votre code avec un nombre de tâches qui ne divise pas le nombre de lignes (ou colones) de votre domaine discret.

### Partie mesure de performance

Vous devez également faire des mesures de performance de votre code et produire une courbe de speedup de votre application parallèle.
Ceci afin d'observer si vous être bien en mesure d'accélérer efficacement votre code sur un superordinateur.

**ATTENTION: lors de vos mesures, n'écrivez PAS le résultat dans le fichier `laplace.txt` !!** Désactivez cette partie, il ne fait pas de sens de mesurer cet I/O dans vos benchmarks.

Si votre code est bien testé, soumettez vos mesures en batch. 
Vous pouvez par exemple utilser les [jobs arrays SLURM](https://slurm.schedmd.com/job_array.html).
La machine travaille pour vous et il y a potentiellement de la place sur la machine, vos différentes mesures peuvent s'exécuter en même temps.
Donnez assez de travail pour que l'exécution la plus rapide soit quand même de l'ordre de la minute et pas trop de manière à ce que l'exécution séquentielle ne prennent pas plus de 60 minutes.
Faites des tests pour estimer les temps que l'exécution peut prendre.
Vous avez trois axes sur lesquels vous pouvez jouer pour ajouter du travail, mais ils peuvent influencer les performances de manière différentes. Il s'agit du nombre de rangées, du nombre de colonnes, et du nombre d'itérations.

Pensez bien à la forme de la sortie de vos mesures de temps, de manière à pouvoir les parser facilement lorsque toutes vos mesures sont terminées.

Faites plusieurs mesures pour des paramètres donnés et calculez une moyenne.

Vous présenterez tous vos résultats sous forme d'un rapport au format Markdown `rapport.md` que vous déposerez dans un répertoire ``doc` de votre dépot.

Le rapport contientdra:
1. une description de la stratégie employée pour paralléliser votre code.
2. Une présentation de vos mesures, p.ex. sous forme de tableau ainsi que des courbes de speedup.
3. Une discussion/analyse de vos mesures.

Votre rapport sera évalué sur :
- la description de votre stratégie,
- la pertinance des mesures faites,
- la clareté de la présentation de ces mesures,
- la discussion, si nécessaire, pour expliquer les (mauvaises) performances de votre code. Si vous n'êtes pas dans un cas idéal de speedup (ce qui sera probablement le cas). Pensez à ce qui a été étudié lors de la partie théorique du cours.

Dans votre rapport, veuillez ne pas:
- réexpliquer l'algorithme, on le connait, il est dans cet énoncé même;
- dire que c'est passionnant et intéressant, l'état émotionel de l'étudiant n'apporte pas de point supplémentaire;
- les conclusions vide: on voit que le code est plus rapide avec plus de CPUs,
- les graphiques sans description/explication, sans légende et sans axes nommés.

### Délai et rendu

Vos travaux seront rendu sur un dépôt Git déjà existant à votre nom dans le groupe suivant (à réaliser):
https://githepia.hesge.ch/hpc_2223_projet

C'est-à-dir si votre mail est `berk.prout@etu.hesge.ch`, votre projet assigné sera `berk.prout`.

Si vous ne voyez pas votre projet, ou que vous n'avez pas les droits d'écritures, signalez-le au plus vite.
N'attendez pas la dernière minute pour tester vos accès, faites ceci dès que vous aurez terminé la lecture de cet énoncé.

Vérifiez que, en plus de vous même sont, l'enseignant ainsi que Pierre Künzli ont accès au dépôt dans la liste des membres.
Vous pouvez vérifier cela via `project information -> members` dans le menu de gauche de l'interface GitLab.

Les excuses de type: "je suis en retard car je n'ai pas pu pusher mon travail" ne seront pas acceptées à moins que vous ayez signaler votre problème assez tôt.
C'est-à-dire dans les 24/48h qui suivent la distribution de cet énoncé.

L'entièreté du travail, partie implémentation et le raport doivent être rendue au plus tard le 
**14 AVRIL 2023**.

