# Projet - L'équation de la chaleur

### Stratégie de parallélisation

<img src="figures/tab1.png" alt="Alt Text" style="zoom: 80%;" />

Nous avons divisé l’espace en lignes par le nombre de processus, lorsque p ne divise pas n, les lignes restantes sont distribuées à partir du rang 0.

Le tableau ci-dessus démontre une divison d’un domaine non divisible par p par trois processus représenté par le chiffre correspondant.

L'application du stencil dans notre problème nécessite les valeurs adjacentes Nord, Sud, Est Ouest. Pour cela, lors de la division de l'espace pour chaque processus (scatterv), nous avons dupliqué les lignes adjacentes au processus voisins.

Voici le découpage par processus concerné:
Processus 0:
<img src="figures/proc0.png" alt="Alt Text" style="zoom: 80%;" />

Processus 1:
<img src="figures/proc1.png" alt="Alt Text" style="zoom: 80%;" />

Processus 2:
<img src="figures/proc2.png" alt="Alt Text" style="zoom: 80%;" />

Après application du stencil, il est nécessaire d'envoyer les bords adjacents pour les calculs suivants (mpi_send, mpi:_recv). Une barrière entre ces deux étapes permet de synchroniser les données.

Après le nombre d'itérations atteint T, les données à jour sont ainsi collectées dans l'ordre dans une matrice de même taille du domaine avec gatherv.

### Résultats

#### Méthode:

Nous avons fait des mesures pour le temps séquentiel (1cpu), ainsi que 2, 4, 8, 16 et 32 cpus pour les temps parallèles. Nous avons quintuplé les mesures selon de tailles de domaines et des temps discrets différents. Les moyennes de ces 5 mesures sont représentées dans ces graphes.

Nous avons choisis un même volume de données de 10¹¹  (N * M * T). Cela permet d'être plus rigoureux lors de la comparaison des paramètres modifiant le nombre de données car le volume est identique. De plus, nous obtenons des temps de calculs entre un et trente minutes, ce qui est un bon compromis entre précision et praticité.


###### Figure 1

<img src="figures/Data_N10.png" alt="Alt Text" style="zoom: 80%;" />
###### Figure 2
<img src="figures/Data_M10.png" alt="Alt Text" style="zoom: 80%;" />

###### Figure 3
<img src="figures/Data_T10.png" alt="Alt Text" style="zoom:80%;" />

#### Analyse:

Nous observons un speedup *(Ts/Tp)* performant, s'approchant du speedup ideal pour les *figures 1 et 2*. 

Le speedup de la *figure 3* dépasse le speedup idéal pour des valeurs CPU de 4 et 8. C'est étrange car la loi d'Amdahl définit le speedup ideal théorique comme une limite et par ce fait infranchissable physiquement.

Cependant, le speedup diverge avec l'augmentation des CPU. Cela est sans doute expliquable par l'overhead de communication. Augmenter le nombre de CPUs augmente le nombre de communications inter-processus.

Nous pouvons observer un infléchissement plus important avec la *figure 3*. En effet, augmenter le nombre d'itérations *T*, revient à multiplier le nombre de communications inter-processus par *T*. Ce n'est pas le cas lorsque l'on augmente la taille du domaine avec *M* ou *N*.

