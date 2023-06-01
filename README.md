# codingame_spring2023


STEPS FOR THIS CHALLENGE :
- Make a mapping for every cell to determine the distance between two points
- Make a function that generate a path with all the cells on the way with priority options
- Make a function to generate the cells that should be prioritized

-- PATH PRIORITY --

My algorithm is linking point to point, each time by chossing the closest one that is not yet linked

In addition, here is my rules when many cell could be a valide path :

- Prioritize cell that are already beaconed
- Prioritize cell that contains a ressource
- Prioritize cell with the highest number of neighboor

28/05 update
Algorithm now generate a list of point from nearest to fathest from bases, then for each point, it selects
the closest point that in the list of connected point or the nearest base if better.

-- CELL PRIORITY --

What could determine the value of a cell ?

- Type of ressource
- Distance from a base, more interesting if closer
- Quantity of that ressource compared to the total on the map
- How to determine if egg or crystal is priority ?
- Max target = ants / 10 ?

30/05
- Distance from each base :
	equal distance to both base = High priority as it medium to defend but higly impact the game
	nearer from one of my base = Easy to defend
	nearer from ennemy base = Hard to defend
- Quantity of eggs : when half harvested, 


Piste d'améliorations :
- Calculer la combinaisons de cellules precises pour gagner
- Avant de prendre une nouvelle target, checker si les cellules que l'on a est suffisant pour win 
- Pathfinding sans indice -> Choisir la cellule qui est la plus proche d'une autre cellule

seed=4957399276813445000
albert_998

-> retenir tous les points de passages potentiels
-> refaire tous les paths possible
-> compter les ressources sur chaque path

-> Ameliorer repartition des fourmis sur un path

seed=5763675996854108000
valgrowth